
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
			if(InitDebugEx(lPath, 0, 0, pvoid(&cUnpacker::OnEntry)))	DebugLoop();
			else														Log("[Error] Engine initialization failed!");
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
ulong					cUnpacker::lPageSize, cUnpacker::lBase;
bool					cUnpacker::lRealign,  cUnpacker::lCopyOverlay, cUnpacker::lIs10;

void __stdcall cUnpacker::OnEntry()
{
	ulong  IO, Layer, Eip = ulong(GetContextData(UE_EIP));
	ushort MEW10;

	enum { iMEW10Indicator = 0xc033 };

	lProcess	= PPROCESS_INFORMATION(GetProcessInformation());
	lBase		= ulong(GetDebuggedFileBaseAddress());
	
	Log("[x] Debuggee base: %08X.", lBase);

	if(ReadProcessMemory(lProcess->hProcess, pvoid(Eip), &MEW10, sizeof ushort, &IO) && IO == sizeof ushort)
	{
		if((lIs10 = MEW10 == iMEW10Indicator))
		{
			Log("[x] MEW 10 detected.");

			if(ReadProcessMemory(lProcess->hProcess, pvoid(Eip - sizeof ulong), &Layer, sizeof Layer, &IO) && IO == sizeof Layer)
			{
				if(SetHardwareBreakPoint(Layer, 0, UE_HARDWARE_EXECUTE, UE_HARDWARE_SIZE_1, &cUnpacker::OnOEPLayer))
					return;
			}
			else Log("[*] Could not read process memory.");
		}
		else
		{
			Log("[x] MEW 11 or newer detected.");

			Layer = ulong(GetJumpDestination(lProcess->hProcess, Eip));
			if(Layer && OEPLayer(Layer)) return;
		}
	}
	else Log("[*] Could not read process memory.");

	Log("[Error] Failed to unpack, probably not packed with a supported version?");
	Abort();
}

void __stdcall cUnpacker::OnOEPLayer()
{
	Log("[x] MEW 10 layer processed.");
	if(OEPLayer()) return;

	Log("[Error] Pattern missing, probably not packed with a supported version?");
	Abort();
}

bool cUnpacker::OEPLayer(ulong pStart)
{
	enum { iOEPIndex = 1, iLength = 5 };

	Log("[x] OEP layer reached.");

	uchar Buffer[iLength];
	ulong IO, OEP;

	if(ReadProcessMemory(lProcess->hProcess, pvoid(pStart ? pStart : GetContextData(UE_EIP)), Buffer, iLength, &IO) && IO == iLength)
	{
		OEP = *pulong(Buffer + 1) + (iOEPIndex + ulong(lIs10)) * sizeof ulong;

		if(ReadProcessMemory(lProcess->hProcess, pvoid(OEP), &OEP, sizeof OEP, &IO) && IO == sizeof OEP)
		{
			if(SetHardwareBreakPoint(OEP, 0, UE_HARDWARE_EXECUTE, UE_HARDWARE_SIZE_1, &cUnpacker::OnOEP))
				return true;
		}
		else Log("[*] Could not read process memory.");
	}
	else Log("[*] Could not read process memory.");

	return false;
}

void __stdcall cUnpacker::OnOEP()
{
	RemoveAllBreakPoints(UE_OPTION_REMOVEALL);
	ulong OEP = ulong(GetContextData(UE_EIP));

	Log("[x] OEP found: %08X.", OEP);

	PastePEHeader(lProcess->hProcess, pvoid(lBase), lPath);
	if(DumpProcess(lProcess->hProcess, pvoid(lBase), lOut, OEP))
	{
		Log("[x] Process dumped.");
		MakeAllSectionsRWE(lOut);

		ulong IatStart, IatSize;
		ImporterAutoSearchIAT(lProcess->hProcess, lOut, lBase, lBase, ulong(GetPE32Data(lPath, 0, UE_SIZEOFIMAGE)), &IatStart, &IatSize);

		if(!ImporterAutoFixIATEx(lProcess->hProcess, lOut, ".revlabs", false, lRealign, OEP, lBase, IatStart, IatSize, sizeof(ulong), false, false, 0))
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
