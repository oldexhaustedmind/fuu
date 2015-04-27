
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

	static bool					lRealign, lCopyOverlay, lIs10;
	static ulong				lBase, lPageSize;

public:
	cUnpacker();
	void SetLogHandle(HWND pLog) { lLog = pLog; }
	bool Unpack(pchar pPath, bool pRealign, bool pCopyOverlay);

	static void Log(const pchar pFormat, ...);
	static void Abort();

private:
	static bool OEPLayer(ulong pStart = 0);

	static void __stdcall OnEntry();
	static void __stdcall OnOEPLayer();
	static void __stdcall OnOEP();
};

#endif // unpacker_h__