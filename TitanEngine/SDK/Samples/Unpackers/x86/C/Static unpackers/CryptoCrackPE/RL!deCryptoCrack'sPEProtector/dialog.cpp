
#include "dialog.h"

char cDialog::lFile[MAX_PATH], cDialog::lTitle[cDialog::iMax], cDialog::lContent[cDialog::iMax];
HWND cDialog::lDialog;

cDialog::cDialog(HINSTANCE pInstance, pchar pTitle, pchar pAbout)
{
	StringCbCopyA(lTitle,	iMax, pTitle);
	StringCbCopyA(lContent,	iMax, pAbout);
	DialogBoxParamA(pInstance, MAKEINTRESOURCEA(rlDialog), 0, DLGPROC(cDialog::Handler), 0);
}

int __stdcall cDialog::Handler(HWND pDialog, int pMessage, int pwParam, int)
{
	static cUnpacker Unpack;

	switch(pMessage)
	{
	case WM_INITDIALOG:
		lDialog = pDialog;

		SendMessageA(pDialog, WM_SETICON, 0, LPARAM(LoadIconA(GetModuleHandleA(0), MAKEINTRESOURCEA(rlIcon))));
		SetDlgItemTextA(pDialog, rlFilename, "filename.exe");

		CheckDlgButton(pDialog, rlRealign, true);
		CheckDlgButton(pDialog, rlOverlay, true);
		return true;

	case WM_CLOSE:
		EndDialog(pDialog, 0);
		return true;

	case WM_DROPFILES:
		if(DragQueryFileA(HDROP(pwParam), 0, lFile, MAX_PATH - 1))
		{
			SetDlgItemTextA(pDialog, rlFilename, lFile);

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

			Unpack.SetLogHandle(GetDlgItem(pDialog, rlLog));
			if(lFile) Unpack.Unpack(lFile);

			break;

		case rlAbout:
			MessageBoxA(pDialog, lContent, "[ About ]", MB_ICONASTERISK);
			break;

		case rlBrowse:
			OPENFILENAMEA File;
			ZeroMemory(&File, sizeof File);

			File.lStructSize	= sizeof File;
			File.lpstrFilter	= "Portable Executable (x86, *.exe) \0*.exe\0Portable Executable (x86, *.dll)\0*.dll\0\0";
			File.lpstrFile		= lFile;
			File.nMaxFile		= MAX_PATH - 1;
			File.Flags			= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_HIDEREADONLY;
			File.lpstrTitle		= lTitle;

			if(GetOpenFileNameA(&File)) SetDlgItemTextA(pDialog, rlFilename, lFile);
			break;
		}
		break;
	}

	return false;
}