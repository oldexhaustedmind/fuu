
#ifndef dialog_h__
#define dialog_h__

#pragma once
#include "types.h"
#include <commctrl.h>
#include <strsafe.h>

#include "resource.h"
#include "unpacker.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "strsafe.lib")

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0'" \
						"processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

class cDialog
{
public:
	enum { iMax = 255 };

private:
	static char	lFile[MAX_PATH], lTitle[iMax], lContent[iMax];
	static HWND lDialog;

public:
	cDialog(HINSTANCE pInstance, pchar pTitle, pchar pAbout);
	void Log(const pchar pFormat, ...);

private:
	cDialog();

	static int __stdcall Handler(HWND pDialog, int pMessage, int pwParam, int plParam);
};

#endif // dialog_h__