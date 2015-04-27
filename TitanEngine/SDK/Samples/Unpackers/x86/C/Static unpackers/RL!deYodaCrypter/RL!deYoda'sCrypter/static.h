
#ifndef static_h__
#define static_h__

#pragma once
#include "types.h"
#include <strsafe.h>

#include "signatures.h"
#include "dialog.h"

#pragma comment(lib, "strsafe.lib")

enum eVersion
{
	iNone,
	iyC11,
	iyC12,
	iyC13
};

enum
{
	iMax			= 512,
	iVersionBase	= 10,
};

bool			Unpack(pchar pPath);

bool __stdcall	DecryptStub(pvoid pCurrent, int pKeySize);
bool			FetchDeltaValue(ulong &pDelta);
ulong			FetchStubData(ulong &pStub, ulong &pStubSize, eVersion &pVersion);

#endif // static_h__