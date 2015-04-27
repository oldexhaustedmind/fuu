
#ifndef unpacker_h__
#define unpacker_h__

#pragma once
#include "types.h"
#include <strsafe.h>

#include "Sdk/sdk.h"
#pragma comment(lib, "strsafe.lib")

class cUnpacker
{
private:
	enum { iMax = 512 };

	static HWND					lLog;
	static char					lPath[MAX_PATH], lOut[MAX_PATH];

	static PPROCESS_INFORMATION lProcess;

	static bool					lRealign, lCopyOverlay, lIsDll, lTlsSuccess;
	static ulong				lRelocBase, lBase, lSize, lPageSize, lOEPJmp;

public:
	cUnpacker();
	void SetLogHandle(HWND pLog) { lLog = pLog; }
	bool Unpack(pchar pPath, bool pRealign, bool pCopyOverlay);

	static void Log(const pchar pFormat, ...);
	static void Abort();

private:
	static void __stdcall OnEntry();
	static void __stdcall OnGetModuleHandleA();
	
	static void __stdcall OnOEPWrite();
	static void __stdcall OnOEPArrival();
};

#endif // unpacker_h__