
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

	static HWND lLog;
	static char lPath[MAX_PATH], lOut[MAX_PATH];

	static ulong				lPageSize;
	static PPROCESS_INFORMATION lProcess;

	static bool					lIsDLL;
	static bool					lIsRelocInit;

	static bool					lRealign;
	static bool					lCopyOverlay;

	static ulong				lBase;
	static ulong				lFileBase;

	static ulong				lImgSize;
	static uchar				lWildCard;

public:
	cUnpacker();
	void SetLogHandle(HWND pLog) { lLog = pLog; }
	bool Unpack(pchar pPath, bool pRealign, bool pCopyOverlay);

	static void Log(const pchar pFormat, ...);
	static void Abort();

private:
	static void __stdcall OnEntry();

	static void __stdcall OnSectionEax();
	static void __stdcall OnOEPJump();

	static void __stdcall OnDynAllocationCall();
	static void __stdcall OnDynAllocation();

	static void __stdcall OnFixRelocation();
	static void __stdcall OnRelocationFixed();

	static pvoid __stdcall FixRedirection(pvoid pIatPointer);
};

#endif // unpacker_h__