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

	static bool Unpack(const char* pPath, bool pRealign, bool pCopyOverlay);
	static void Abort();
	static void Log(const char* pFormat, ...);

private:

	static cUnpacker lInstance;

	static const size_t iMaxString = 512;
	static const size_t iImporterSize  =  40*1024;

	static char lPath[MAX_PATH], lOut[MAX_PATH];

	static PROCESS_INFORMATION* lProcess;

	static bool lRealign, lCopyOverlay;

	static PE32Struct lPEInfo;
	static ULONG_PTR lEP;
	static ULONG_PTR SFXSectionVA;
	static DWORD SFXSectionSize;

	cUnpacker() { }
	cUnpacker(const cUnpacker&);
	cUnpacker& operator=(const cUnpacker&);

	static void DefLogCallback(const char*) { }

	static void __stdcall OnEp();
	static void __stdcall OnDecrypted();

	static void __stdcall OnLoadLibraryACall();
	static void __stdcall OnGetProcAddressCall();

	static void __stdcall OnOepCode();

};

#endif // unpacker_h__
