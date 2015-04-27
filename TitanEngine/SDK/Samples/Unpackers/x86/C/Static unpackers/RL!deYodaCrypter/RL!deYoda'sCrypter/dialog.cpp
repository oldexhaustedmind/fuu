
#include "dialog.h"
#include "static.h"

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

HWND gDialog = 0, gLog = 0;
char gFile[MAX_PATH];

int __stdcall Handler(HWND pDialog, int pMessage, int pwParam, int)
{
	switch(pMessage)
	{
	case WM_INITDIALOG:
		gDialog = pDialog;
		gLog	= GetDlgItem(gDialog, rlLog);

		SendMessageA(pDialog, WM_SETICON, 0, LPARAM(LoadIconA(GetModuleHandleA(0), MAKEINTRESOURCEA(rlIcon))));
		SetDlgItemTextA(pDialog, rlFilename, "filename.exe");

		CheckDlgButton(pDialog, rlRealign, true);
		CheckDlgButton(pDialog, rlOverlay, true);
		return true;

	case WM_CLOSE:
		EndDialog(pDialog, 0);
		return true;

	case WM_DROPFILES:
		if(DragQueryFileA(HDROP(pwParam), 0, gFile, MAX_PATH - 1))
		{
			SetDlgItemTextA(pDialog, rlFilename, gFile);

			SendMessageA(GetDlgItem(pDialog, rlLog), LB_RESETCONTENT, 0, 0);
			UpdateWindow(GetDlgItem(pDialog, rlLog));
		}
		break;

	case WM_COMMAND:
		switch(pwParam)
		{
		case rlExit:
			SendMessageA(pDialog, WM_CLOSE, 0, 0);
			break;

		case rlUnpack:
			SendMessageA(GetDlgItem(pDialog, rlLog), LB_RESETCONTENT, 0, 0);
			UpdateWindow(GetDlgItem(pDialog, rlLog));

			Unpack(gFile);
			break;

		case rlAbout:
			MessageBoxA(pDialog, iAbout, "[ About ]", MB_ICONASTERISK);
			break;

		case rlBrowse:
			OPENFILENAMEA File;
			ZeroMemory(&File, sizeof File);

			File.lStructSize	= sizeof File;
			File.lpstrFilter	= "Portable Executable (x86, *.exe) \0*.exe\0Portable Executable (x86, *.dll)\0*.dll\0\0";
			File.lpstrFile		= gFile;
			File.nMaxFile		= MAX_PATH - 1;
			File.Flags			= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_HIDEREADONLY;
			File.lpstrTitle		= iTitle;

			if(GetOpenFileNameA(&File)) SetDlgItemTextA(pDialog, rlFilename, gFile);
			break;
		}
		break;
	}

	return false;
}