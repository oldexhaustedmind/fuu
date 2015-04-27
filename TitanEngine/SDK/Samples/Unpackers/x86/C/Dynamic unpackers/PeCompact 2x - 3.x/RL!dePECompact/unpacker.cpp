
#include "unpacker.h"

cUnpacker::cUnpacker()
{
	SYSTEM_INFO Info = { 0 };
	GetSystemInfo(&Info);

	lPageSize = Info.dwPageSize;
}

void cUnpacker::Log(const pchar pFormat, ...)
{
	va_list ArgList;
	va_start(ArgList, pFormat);

	char Buffer[iMax];
	StringCbVPrintfA(Buffer, iMax, pFormat, ArgList);

	SendMessageA(lLog, LB_ADDSTRING, 0, LPARAM(Buffer));
	SendMessageA(lLog, LB_SETCURSEL, WPARAM(SendMessageA(lLog, LB_GETCOUNT, 0, 0) - 1), 0);

	va_end(ArgList);
}

bool cUnpacker::Unpack(pchar pPath, bool pRealign, bool pCopyOverlay)
{
	bool Return = false;

	lRealign = pRealign;
	lCopyOverlay = pCopyOverlay;

	FILE_STATUS_INFO Status;
	ZeroMemory(&Status, sizeof Status);

	StringCbCopyA(lPath, MAX_PATH, pPath);

	ulong ExtIndex = lstrlenA(lPath) - 1;
	while(ExtIndex > 0 && lPath[ExtIndex] != '.') ExtIndex--;

	if(ExtIndex)
	{
		lPath[ExtIndex] = 0;
		ExtIndex++;

		StringCbPrintfA(lOut, MAX_PATH, "%s.unpacked.%s", lPath, &lPath[ExtIndex]);
		lPath[ExtIndex - 1] = '.';

		DeleteFileA(lOut);
		Log("-> Unpack started...");

		if(IsPE32FileValidEx(lPath, UE_DEPTH_DEEP, &Status))
		{
			lIsDLL			= Status.FileIsDLL;
			lIsRelocInit	= false;

			pvoid Handle, Map;
			ulong Size, MapVA;

			lFileBase	= ulong(GetPE32Data(lPath, 0, UE_IMAGEBASE));
			lImgSize	= ulong(GetPE32Data(lPath, 0, UE_SIZEOFIMAGE));

			bool ValidOEP = false;
			if(StaticFileLoad(lPath, UE_ACCESS_READ, false, &Handle, &Size, &Map, &MapVA))
			{
				puchar OEP = puchar(ConvertVAtoFileOffset(MapVA, lFileBase + ulong(GetPE32DataFromMappedFile(MapVA, 0, UE_OEP)), true));

				ValidOEP = *OEP == 0xb8 && OEP[5] == 0x50;
				StaticFileUnload(lPath, false, Handle, Size, Map, MapVA);
			}

			if(ValidOEP)
			{
				Log("[x] EP pattern match (beware of fake signatures though).");

				DeleteFileA("snapshot0");
				DeleteFileA("snapshot1");

				bool Success = false;

				if(lIsDLL)	Success = InitDLLDebug(lPath, true, 0, 0, pvoid(&cUnpacker::OnEntry)) != 0;
				else		Success = InitDebugEx(lPath, 0, 0, pvoid(&cUnpacker::OnEntry)) != 0;

				if(!Success)	Log("[Error] Engine initialization failed!");
				else			DebugLoop();
			}
			else Log("[Error] File doesn't seem to be packed with PECompact!");
		}
		else Log("[Error] Selected file is not a valid PE32 file!");
	}

	Log("-> Unpack ended...");
	return Return;
}

void cUnpacker::Abort()
{
	StopDebug();
	if(lIsRelocInit) RelocaterCleanup();

	Log("[Fatal Error] Unpacking has been aborted.");
}

HWND cUnpacker::lLog;
char cUnpacker::lPath[MAX_PATH], cUnpacker::lOut[MAX_PATH];

ulong					cUnpacker::lPageSize;
PPROCESS_INFORMATION	cUnpacker::lProcess;

bool					cUnpacker::lIsDLL;
bool					cUnpacker::lIsRelocInit;

bool					cUnpacker::lRealign;
bool					cUnpacker::lCopyOverlay;

ulong					cUnpacker::lBase;
ulong					cUnpacker::lFileBase;

ulong					cUnpacker::lImgSize;
uchar					cUnpacker::lWildCard;

void __stdcall cUnpacker::OnEntry()
{
	lProcess	= PPROCESS_INFORMATION(GetProcessInformation());
	lBase		= ulong(lIsDLL ? GetDebuggedDLLBaseAddress() : GetDebuggedFileBaseAddress());
	
	Log("[x] Debuggee base: %08X.", lBase);
	StepInto(&cUnpacker::OnSectionEax);
}

void __stdcall cUnpacker::OnSectionEax()
{
	const char					OepPattern[]	= "\xff\xe0\x00\x00";
	const ulong					OepLength		= sizeof OepPattern / sizeof *OepPattern - 1;

	const char					DynPattern[]	= "\x89\xcc\xcc\x8b\xcc\xcc\x89\xcc\xcc\xff\xcc\x89\xcc\xcc\xcc\xcc\xcc\x8b\xcc\x8b\xcc\xcc\xcc\xeb";
	const ulong					DynLength		= sizeof DynPattern / sizeof *DynPattern - 1;
	const ulong					DynOffset		= 9;

	MEMORY_BASIC_INFORMATION	Info			= { 0 };

	ulong Eax = ulong(GetContextData(UE_EAX));
	if(VirtualQuery(pvoid(Eax), &Info, sizeof Info))
	{
		lWildCard = 0xcc;
		ulong VA  = 0;

		const pvoid Start	= pvoid(Eax);
		const ulong Length	= ulong(Info.BaseAddress) + lPageSize - Eax;

		if((VA += ulong(Find(Start, Length, pvoid(OepPattern), OepLength, &lWildCard))))
		{
			SetBPX(VA, UE_SINGLESHOOT, &cUnpacker::OnOEPJump);
			
			if(lFileBase == lBase)
			{
				Log("[x] File is loaded at default base.");
				return;
			}

			VA = DynOffset;
			if((VA += ulong(Find(Start, Length, pvoid(DynPattern), DynLength, &lWildCard))) != DynOffset)
			{
				SetBPX(VA, UE_SINGLESHOOT, &cUnpacker::OnDynAllocationCall);
			}
			else if(lIsDLL)
			{
				Log("[Error] Pattern missing, probably not packed with a supported version?");
				Abort();
			}
		}
		else
		{
			Log("[Error] Pattern missing, probably not packed with a supported version?");
			Abort();
		}
	}
	else
	{
		Log("[Error] Cannot query page.");
		Abort();
	}
}

void __stdcall cUnpacker::OnDynAllocationCall()
{
	StepInto(&cUnpacker::OnDynAllocation);
}

void __stdcall cUnpacker::OnDynAllocation()
{
	const char		RelocPattern[]	= "\x6a\x40\x68\x00\x10\x00\x00\xcc\x6a\x00\xff\x95\xcc\xcc\xcc\xcc\x89\x85\xcc\xcc\xcc\xcc\x56\xe8\xcc\xcc\xcc\xcc\x8d\xcc" \
									  "\xcc\xcc\xcc\xcc\x85\xc0\x0f\x85\xcc\xcc\xcc\xcc\x56\xe8\xcc\xcc\xcc\xcc\x56\xe8\xcc\xcc\xcc\xcc\x56\xe8";
	const ulong		RelocLength		= sizeof RelocPattern / sizeof *RelocPattern - 1;
	
	lWildCard = 0xcc;
	ulong VA  = RelocLength - 1;

	MEMORY_BASIC_INFORMATION Info	= { 0 };
	ulong Eip = ulong(GetContextData(UE_EIP));

	if(VirtualQuery(pvoid(Eip), &Info, sizeof Info))
	{
		if((VA += ulong(Find(pvoid(Eip), ulong(Info.BaseAddress) + lPageSize - Eip, pvoid(RelocPattern), RelocLength, &lWildCard))) != RelocLength)
		{
			Log("[x] Relocation handling present.");
			SetBPX(VA, UE_SINGLESHOOT, &cUnpacker::OnFixRelocation);

			RelocaterInit(0x10000, lFileBase, lBase);
			lIsRelocInit = true;
		}
		else if(lIsDLL)
		{
			Log("[Error] Pattern missing, probably not packed with a supported version?");
			Abort();
		}
	}
	else
	{
		Log("[Error] Cannot query page.");
		Abort();
	}
}

void __stdcall cUnpacker::OnFixRelocation()
{
	Log("[x] First snapshot created.");

	RelocaterMakeSnapshot(lProcess->hProcess, "snapshot0", pvoid(lBase), lImgSize);
	StepOver(&cUnpacker::OnRelocationFixed);
}

void __stdcall cUnpacker::OnRelocationFixed()
{
	Log("[x] Second snapshot created.");

	RelocaterMakeSnapshot(lProcess->hProcess, "snapshot1", pvoid(lBase), lImgSize);
	RelocaterCompareTwoSnapshots(lProcess->hProcess, lBase, lImgSize, "snapshot0", "snapshot1", lBase);

	DeleteFileA("snapshot0");
	DeleteFileA("snapshot1");
}

pvoid __stdcall cUnpacker::FixRedirection(pvoid pIatPointer)
{
	static uchar Buffer[7];

	ulong IO = 0;
	if(ReadProcessMemory(lProcess->hProcess, pIatPointer, Buffer, 7, &IO) && IO == 7)
		if(*Buffer = 0xb8 && *pushort(Buffer + 5) == 0xe0ff)
		{
			ulong API = *pulong(Buffer + 1);

			if(ImporterGetAPIName(API))
				return pvoid(API);

			if(ReadProcessMemory(lProcess->hProcess, pvoid(API), Buffer, 7, &IO) && IO == 7)
				if(*Buffer == 0x55 && *pushort(Buffer + 1) == 0xec8b)
					return pvoid(ImporterGetRemoteAPIAddress(lProcess->hProcess, ulongptr(&GetProcAddress)));
		}

	return 0;
}

void __stdcall cUnpacker::OnOEPJump()
{
	RemoveAllBreakPoints(UE_OPTION_REMOVEALL);
	ulong OEP = ulong(GetContextData(UE_EAX));

	Log("[x] OEP found: %08X.", OEP);

	PastePEHeader(lProcess->hProcess, pvoid(lBase), lPath);
	if(DumpProcess(lProcess->hProcess, pvoid(lBase), lOut, OEP))
	{
		Log("[x] Process dumped.");
		MakeAllSectionsRWE(lOut);

		ulong IatStart, IatSize;
		if(lIsRelocInit)
		{
			if(!RelocaterExportRelocationEx(lOut, ".revlabs"))
			{
				Log("[Error] Cannot fix relocations.");

				Abort();
				return;
			}
			else Log("[x] Relocations fixed.");
		}

		ImporterAutoSearchIAT(lProcess->hProcess, lOut, lBase, lBase, lImgSize, &IatStart, &IatSize);
		if(!ImporterAutoFixIATEx(lProcess->hProcess, lOut, ".revlabs", false, lRealign, OEP, lBase, IatStart, IatSize, sizeof(ulong), false, false, &cUnpacker::FixRedirection))
		{
			Log("[Error] Cannot fix imports.");

			Abort();
			return;
		}
		else
		{
			if(lRealign) Log("[x] File has been realigned.");
			Log("[x] Imports fixed.");
		}

		if(lCopyOverlay) Log(CopyOverlay(lPath, lOut) ? "[x] Moving overlay to unpacked file." : "[x] No overlay found.");

		StopDebug();
		Log("[x] File successfully unpacked to %s.", lOut);
	}
	else
	{
		Log("[Error] Cannot dump process.");
		Abort();
	}
}
