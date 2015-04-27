#ifndef unpacker_h__
#define unpacker_h__

#pragma once

#include <windows.h>
#include "Sdk/sdk.h"

class cUnpacker
{
public:

	static HWND Window;
	static void (*LogCallback)(const char*);

	static cUnpacker& Instance() { return lInstance; }

	bool Unpack(const char* pPath, bool pRealign, bool pCopyOverlay);
	static void Abort();
	static void Log(const char* pFormat, ...);

private:

	static cUnpacker lInstance;

	static const size_t iMaxString = 512;
	static const size_t iImporterSize  =  40*1024;
	static const size_t iRelocaterSize = 100*1024;

	static char lPath[MAX_PATH], lOut[MAX_PATH];

	static PROCESS_INFORMATION* lProcess;

	static bool lIsDLL;
	static bool lRealign, lCopyOverlay;

	static PE32Struct lPEInfo;
	static ULONG_PTR lBase;
	static ULONG_PTR lEP;

	cUnpacker() { }
	cUnpacker(const cUnpacker&);
	cUnpacker& operator=(const cUnpacker&);

	static void DefLogCallback(const char*) { }

	static void __stdcall OnEp();
	static void __stdcall OnEpStep();

	static void __stdcall OnGetModuleHandleACall();
	static void __stdcall OnGetProcAddressCall();
	static void __stdcall OnBeforeRelocation();

	static void __stdcall OnOepJump();

};

#endif // unpacker_h__
