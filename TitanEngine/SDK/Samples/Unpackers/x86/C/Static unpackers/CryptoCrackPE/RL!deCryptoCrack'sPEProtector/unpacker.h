
#ifndef unpacker_h__
#define unpacker_h__

#pragma once
#include "types.h"
#include <strsafe.h>

#include "sdk/sdk.h"
#pragma comment(lib, "strsafe.lib")

class cUnpacker
{
private:
	enum { iMax = 512 };

	static HWND						lLog;
	static char						lPath[MAX_PATH], lOut[MAX_PATH];

	static ulong					lMap, lKeyA, lKeyB, lFirstByte;
	static puchar					lDecryption;

public:
	void SetLogHandle(HWND pLog) { lLog = pLog; }
	bool Unpack(pchar pPath);

	static void Log(const pchar pFormat, ...);
	static void Abort();

private:
	static bool __stdcall CustomDecryption(pvoid pStart, int);
	static bool			  CurrentDecryption(pvoid pStart);
};

#endif // unpacker_h__