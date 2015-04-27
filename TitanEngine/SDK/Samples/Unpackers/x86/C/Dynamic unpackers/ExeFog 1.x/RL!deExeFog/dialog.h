#ifndef dialog_h__
#define dialog_h__

#pragma once

#include <windows.h>

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

class cDialog
{
private:
	static const size_t iMax = 256;

	static HINSTANCE lInstance;
	static HWND lDialog;
	static char	lTitle[iMax], lContent[iMax];

public:
	cDialog(HINSTANCE pInstance, const char* pTitle, const char* pAbout);

private:
	static INT_PTR CALLBACK Handler(HWND pDialog, UINT pMessage, WPARAM pwParam, LPARAM plParam);
	static void LogCallback(const char* pText);
};

#endif // dialog_h__
