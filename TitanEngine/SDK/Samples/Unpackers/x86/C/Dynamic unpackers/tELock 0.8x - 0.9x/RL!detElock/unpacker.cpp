
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

		lSize = ulong(GetPE32Data(lPath, 0, UE_SIZEOFIMAGE));
		if(lSize && IsPE32FileValidEx(lPath, UE_DEPTH_DEEP, &Status) && Status.OveralEvaluation == UE_RESULT_FILE_OK && !Status.FileIsDLL)
		{
			bool Success = false;
			lTlsSuccess  = TLSBackupData(pPath);

			Success = InitDebugEx(lPath, 0, 0, pvoid(&cUnpacker::OnEntry)) != 0;
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
ulong					cUnpacker::lPageSize, cUnpacker::lBase, cUnpacker::lSize, cUnpacker::lOEPJmp;
bool					cUnpacker::lRealign,  cUnpacker::lCopyOverlay, cUnpacker::lIsDll, cUnpacker::lTlsSuccess;

void __stdcall cUnpacker::OnEntry()
{
	lProcess	= PPROCESS_INFORMATION(GetProcessInformation());
	lBase		= ulong(GetDebuggedFileBaseAddress());

	ulong EP = ulong(GetContextData(UE_EIP)), IO;
	lOEPJmp  = 0;

	uchar EPByte = 0;
	if(ReadProcessMemory(lProcess->hProcess, pvoid(EP), &EPByte, sizeof EPByte, &IO) && IO == sizeof EPByte)
	{
		if(EPByte == 0xe9)
		{
			lOEPJmp = ulong(GetJumpDestination(lProcess->hProcess, EP));
			if(!lOEPJmp) Abort();
		}
		else lOEPJmp = EP;
	}
	else Abort();

	enum
	{
		iMaxInitSize		= 0x10000,
		iOepPointerOff		= 5
	};

	Log("[x] Debuggee base: %08X.", lBase);

	if(!SetAPIBreakPoint((GetModuleHandleA("kernelbase") > 0) ? "kernelbase" : "kernel32", "GetModuleHandleA", UE_BREAKPOINT, UE_APIEND, pvoid(&cUnpacker::OnGetModuleHandleA)))
		Abort();
}

void __stdcall cUnpacker::OnGetModuleHandleA()
{
	ulong IO, Return;
	Return = ulong(GetContextData(UE_ESP));

	if(Return && ReadProcessMemory(lProcess->hProcess, pvoid(Return), &Return, sizeof Return, &IO) && IO == sizeof Return)
	{
		uchar Wildcard			= 0xcc;
		uchar ImportPattern[]	= "\x85\xc0\x0f\x85\xCC\xCC\xCC\xCC\xCC\xff\x95\xCC\xCC\xCC\xCC\x85\xc0\x0f\x85";
		ulong ImportPatternLen	= sizeof ImportPattern / sizeof *ImportPattern - 1;

		if(Find(pvoid(Return), ImportPatternLen, ImportPattern, ImportPatternLen, &Wildcard) == Return)
		{
			Log("[x] Import routine found.");
			DeleteBPX(ulong(GetContextData(UE_EIP)));

			uchar RedirPattern[]	= "\x2b\xc0\x40\x83\x3f\x00\x8d\x7f\x04\x75\xf7\x48";
			ulong RedirPatternLen	= sizeof RedirPattern / sizeof *RedirPattern - 1;

			ulong Decision;
			if((Decision = ulong(Find(pvoid(Return), 0x200, RedirPattern, RedirPatternLen, &Wildcard))))
			{
				uchar Zero = 0;
				Decision += 10;

				Log("[x] Patching byte at %08X to zero.", Decision);
				if(VirtualProtectEx(lProcess->hProcess, pvoid(Decision), sizeof Zero, PAGE_EXECUTE_READWRITE, &IO) &&
					WriteProcessMemory(lProcess->hProcess, pvoid(Decision), &Zero, sizeof Zero, &IO) && IO == sizeof Zero)
				{
					Decision += 2;
					Decision = ulong(GetJumpDestination(lProcess->hProcess, Decision));
					if(Decision)
					{
						Decision += 6;

						Log("[x] Patching byte at %08X to zero.", Decision);
						if(VirtualProtectEx(lProcess->hProcess, pvoid(Decision), sizeof Zero, PAGE_EXECUTE_READWRITE, &IO) &&
							WriteProcessMemory(lProcess->hProcess, pvoid(Decision), &Zero, sizeof Zero, &IO) && IO == sizeof Zero)
						{
							if(!SetHardwareBreakPoint(lOEPJmp + 1, 0, UE_HARDWARE_WRITE, UE_HARDWARE_SIZE_1, pvoid(&cUnpacker::OnOEPWrite)))
							{
								Log("[x] Could not set hardware breakpoint.");
								Abort();
							}
							else Log("[x] Set hardware breakpoint at %08X.", lOEPJmp + 1);

							return;
						}
					}
				}
			}
		}
		else return;
	}

	Log("[x] Error while reading/writing debuggee memory.");
	Abort();
}

void __stdcall cUnpacker::OnOEPWrite()
{
	static bool IgnoreFirst = true;
	if(!IgnoreFirst)
	{
		IgnoreFirst = true;
		RemoveAllBreakPoints(UE_OPTION_REMOVEALL);

		ulong OEP = ulong(GetJumpDestination(lProcess->hProcess, lOEPJmp));
		if(OEP && SetBPX(OEP, UE_SINGLESHOOT, pvoid(&cUnpacker::OnOEPArrival)))
			return;

		Log("[x] Could not set OEP breakpoint.");
		Abort();
	}

	IgnoreFirst = false;
}

void __stdcall cUnpacker::OnOEPArrival()
{
	RemoveAllBreakPoints(UE_OPTION_REMOVEALL);
	ulong OEP = ulong(GetContextData(UE_EIP));

	Log("[x] Breakpoint triggered at %08X (OEP).", OEP);
	PastePEHeader(lProcess->hProcess, pvoid(lBase), lPath);

	if(lTlsSuccess)
	{
		Log("[x] TLS directory restored, keeping packer section.");
		TLSRestoreData();
	}

	if(DumpProcess(lProcess->hProcess, pvoid(lBase), lOut, OEP))
	{
		Log("[x] Process dumped.");

		ulong Index = ulong(GetPE32Data(lOut, 0,	 UE_SECTIONNUMBER));
		ulong Begin = ulong(GetPE32Data(lOut, Index, UE_SECTIONVIRTUALOFFSET));

		ulong Reloc = ulong(GetPE32Data(lOut, 0,	 UE_RELOCATIONTABLEADDRESS));
		ulong Resou = ulong(GetPE32Data(lOut, 0,	 UE_RESOURCETABLEADDRESS));

		MakeAllSectionsRWE(lOut);
		if(Reloc < Begin && Resou < Begin)
		{
			if(!lTlsSuccess)
			{
				Log("[x] No TLS directory present, deleting packer section.");
				DeleteLastSection(lOut);
			}
		}
		else Log("[x] Last section contains resource or relocation directory.");

		ulong IatStart = 0, IatSize = 0;
		ImporterAutoSearchIAT(lProcess->hProcess, lOut, lBase, lBase, lSize, &IatStart, &IatSize);

		if(IatStart && IatSize && !ImporterAutoFixIATEx(lProcess->hProcess, lOut, ".revlabs", false, false, OEP, lBase, IatStart, IatSize, sizeof(ulong), true, false, 0))
		{
			Log("[Error] Cannot fix imports.");
			ImporterCleanup();

			Abort();
			return;
		}
		else Log("[x] Imports fixed (%08X to %08X).", IatStart, IatStart + IatSize);

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
