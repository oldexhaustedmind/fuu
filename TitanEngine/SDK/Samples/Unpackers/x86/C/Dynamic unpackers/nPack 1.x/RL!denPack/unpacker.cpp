
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

		lRelocBase	= ulong(GetPE32Data(lPath, 0, UE_IMAGEBASE));
		lSize		= ulong(GetPE32Data(lPath, 0, UE_SIZEOFIMAGE));

		DeleteFileA("snapshot0");
		DeleteFileA("snapshot1");

		if(lRelocBase && lSize && IsPE32FileValidEx(lPath, UE_DEPTH_DEEP, &Status))
		{
			bool Success = false;
			lTlsSuccess  = TLSBackupData(pPath);

			if((lIsDll = Status.FileIsDLL))	Success = InitDLLDebug(lPath, true, 0, 0, pvoid(&cUnpacker::OnEntry))	!= 0;
			else							Success = InitDebugEx(lPath, 0, 0, pvoid(&cUnpacker::OnEntry))			!= 0;

			Success ? DebugLoop() : Log("[Error] Engine initialization failed!");
		}
		else Log("[Error] Selected file is not a valid PE32 file!");
	}

	Log("-> Unpack ended...");
	return Return;
}

void cUnpacker::Abort()
{
	StopDebug();
	Log("[Fatal Error] Unpacking has been aborted.");
}

HWND cUnpacker::lLog;
char cUnpacker::lPath[MAX_PATH], cUnpacker::lOut[MAX_PATH];


PPROCESS_INFORMATION	cUnpacker::lProcess;
ulong					cUnpacker::lPageSize, cUnpacker::lOepPtr, cUnpacker::lBase, cUnpacker::lSize, cUnpacker::lRelocBase;
bool					cUnpacker::lRealign,  cUnpacker::lCopyOverlay, cUnpacker::lIsDll, cUnpacker::lTlsSuccess;

void __stdcall cUnpacker::OnEntry()
{
	ulong Eip = ulong(GetContextData(UE_EIP));

	lProcess	= PPROCESS_INFORMATION(GetProcessInformation());
	lBase		= ulong(lIsDll ? GetDebuggedDLLBaseAddress() : GetDebuggedFileBaseAddress());
	
	enum
	{
		iMaxInitSize		= 0x100,
		iOepPointerOff		= 5
	};

	uchar WildCard = 0xCC;

	uchar EpLayout[]					= "\x83\x3d\xCC\xCC\xCC\xCC\x00\x75\x05\xe9\x01\x00\x00\x00\xc3\xe8";
	const ulong EpLayoutLen				= sizeof EpLayout / sizeof *EpLayout - 1;

	uchar RelocAndOep[]					= "\xE8\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\xA1\xCC\xCC\xCC\xCC\xC7\x05\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x01\x05\xCC\xCC\xCC\xCC\xFF\x35\xCC\xCC\xCC\xCC\xC3";
	const ulong RelocAndOepLen			= sizeof RelocAndOep / sizeof *RelocAndOep - 1;

	Log("[x] Debuggee base: %08X.", lBase);
	bool RelocNecessary = !(lBase == lRelocBase);

	if(!RelocNecessary) Log("[x] File is loaded at default base.");
	else				RelocaterInit(0x10000, lRelocBase, lBase);

	if(ulong(Find(pvoid(Eip), EpLayoutLen, EpLayout, EpLayoutLen, &WildCard)) == Eip)
	{
		ulong OepRef, IO;
		if((OepRef = ulong(Find(pvoid(Eip), iMaxInitSize, RelocAndOep, RelocAndOepLen, &WildCard))))
			if(ReadProcessMemory(lProcess->hProcess, pvoid(OepRef + RelocAndOepLen - iOepPointerOff), &lOepPtr, sizeof ulong, &IO) && IO == sizeof ulong)
				if(SetBPX(OepRef, UE_SINGLESHOOT, &cUnpacker::OnRelocInit))
					return;

		Log("[Error] Cannot read debuggee's memory.");
	}
	else Log("[Error] Pattern missing, probably not packed with a supported version?");

	Abort();
}

void __stdcall cUnpacker::OnDecompressedData()
{
	ulong Oep, IO;
	if(ReadProcessMemory(lProcess->hProcess, pvoid(lOepPtr), &Oep, sizeof ulong, &IO) && IO == sizeof ulong)
	{
		if(SetBPX(Oep + lBase, UE_SINGLESHOOT, &cUnpacker::OnOEP))
			return;
	}

	Log("[Error] Cannot set OEP breakpoint.");
	Abort();
}

void __stdcall cUnpacker::OnOEP()
{
	RemoveAllBreakPoints(UE_OPTION_REMOVEALL);
	ulong OEP = ulong(GetContextData(UE_EIP));

	Log("[x] OEP found: %08X.", OEP);

	if(lTlsSuccess)
	{
		Log("[x] TLS directory restored, keeping packer section.");
		TLSRestoreData();
	}

	PastePEHeader(lProcess->hProcess, pvoid(lBase), lPath);

	if(DumpProcess(lProcess->hProcess, pvoid(lBase), lOut, OEP))
	{
		Log("[x] Process dumped.");
		MakeAllSectionsRWE(lOut);

		if(!lTlsSuccess)
		{
			Log("[x] No TLS directory present, deleting packer section.");
			DeleteLastSection(lOut);
		}

		if(lRelocBase != lBase)
		{
			if(!RelocaterExportRelocationEx(lOut, ".revlabs"))
			{
				Log("[Error] Cannot fix relocations.");

				Abort();
				return;
			}
			else Log("[x] Relocations fixed.");
		}

		ulong IatStart, IatSize;
		ImporterAutoSearchIAT(lProcess->hProcess, lOut, lBase, lBase, lSize, &IatStart, &IatSize);

		if(IatStart && IatSize && !ImporterAutoFixIATEx(lProcess->hProcess, lOut, ".revlabs", false, lRealign, OEP, lBase, IatStart, IatSize, sizeof(ulong), false, false, 0))
		{
			Log("[Error] Cannot fix imports.");

			Abort();
			return;
		}
		else Log("[x] Imports fixed.");

		if(lCopyOverlay)	Log(CopyOverlay(lPath, lOut) ? "[x] Moving overlay to unpacked file." : "[x] No overlay found.");
		if(lRealign)		Log(RealignPEEx(lOut, 0, 0)	 ? "[x] File has been realigned."		  : "[x] File realigning failed.");

		StopDebug();
		Log("[x] File successfully unpacked to %s.", lOut);
	}
	else
	{
		Log("[Error] Cannot dump process.");
		Abort();
	}
}

void __stdcall cUnpacker::OnRelocInit()
{
	if(lRelocBase != lBase)
	{
		Log("[x] First snapshot created.");
		RelocaterMakeSnapshot(lProcess->hProcess, "snapshot0", pvoid(lBase), lSize);

		StepOver(&cUnpacker::OnRelocDone);
		return;
	}

	OnRelocDone();
}

void __stdcall cUnpacker::OnRelocDone()
{
	if(lRelocBase != lBase)
	{
		Log("[x] Second snapshot created.");

		RelocaterMakeSnapshot(lProcess->hProcess, "snapshot1", pvoid(lBase), lSize);
		RelocaterCompareTwoSnapshots(lProcess->hProcess, lBase, lSize, "snapshot0", "snapshot1", lBase);

		DeleteFileA("snapshot0");
		DeleteFileA("snapshot1");
	}

	OnDecompressedData();
}
