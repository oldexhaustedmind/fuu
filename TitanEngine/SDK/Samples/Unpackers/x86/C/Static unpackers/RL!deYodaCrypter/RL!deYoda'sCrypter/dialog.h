
#ifndef dialog_h__
#define dialog_h__

#pragma once

#include "types.h"
#include <strsafe.h>

#include "resource.h"

#pragma comment(lib, "strsafe.lib")

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0'" \
						"processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

const pchar iAbout =	"RL!deYoda'sCrypter 1.1 - 1.3 static unpacker \r\n\r\n" \
						" Visit Reversing Labs at http://www.reversinglabs.com \r\n\r\n" \
						"Minimum engine version needed:\r\n" \
						"- TitanEngine 2.0.2 by RevLabs\r\n\r\n" \
						"Unpacker coded by Reversing Labs.",
			iTitle =	"RL!deYoda'sCrypter from Reversing Labs";

extern HWND gDialog, gLog;
extern char gFile[MAX_PATH];

int __stdcall Handler(HWND pDialog, int pMessage, int pwParam, int);

#endif // dialog_h__
