#include "dialog.h"

#include <cstring>
#include "resource.h"
#include "unpacker.h"

HINSTANCE cDialog::lInstance;
HWND cDialog::lDialog;
char cDialog::lTitle[cDialog::iMax], cDialog::lContent[cDialog::iMax];

cDialog::cDialog(HINSTANCE pInstance, const char* pTitle, const char* pAbout)
{
	lInstance = pInstance;
	strncpy(lTitle, pTitle, sizeof lTitle - 1);
	lTitle[sizeof lTitle - 1] = '\0';
	strncpy(lContent, pAbout, sizeof lContent-1);
	lContent[sizeof lContent - 1] = '\0';
	DialogBoxA(lInstance, MAKEINTRESOURCEA(rlDialog), NULL, &cDialog::Handler);
}

INT_PTR CALLBACK cDialog::Handler(HWND pDialog, UINT pMessage, WPARAM pwParam, LPARAM plParam)
{
static cUnpacker& Unpack = cUnpacker::Instance();
static char	lFile[MAX_PATH];
static HICON MainIcon;

	switch(pMessage)
	{
	case WM_INITDIALOG:
		lDialog = pDialog;

		MainIcon = LoadIconA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(rlIcon));
		// ICON_BIG automatically sets the small icon if there is none
		SendMessageA(pDialog, WM_SETICON, ICON_BIG, LPARAM(MainIcon));

		SetWindowTextA(pDialog, lTitle);
		SetDlgItemTextA(pDialog, rlFilename, "Drag file here or browse for it...");
		CheckDlgButton(pDialog, rlRealign, true);
		CheckDlgButton(pDialog, rlOverlay, true);
		EnableWindow(GetDlgItem(pDialog, rlUnpack), false);
		return true;

	case WM_CLOSE:
		DestroyIcon(MainIcon);
		EndDialog(pDialog, 0);
		return true;

	case WM_DROPFILES:
		if(DragQueryFileA((HDROP)pwParam, 0, lFile, sizeof lFile))
		{
			SetDlgItemTextA(pDialog, rlFilename, lFile);
		}
		DragFinish((HDROP)pwParam);
		break;

	case WM_COMMAND:
		switch(LOWORD(pwParam))
		{
		case rlExit:
			SendMessageA(pDialog, WM_CLOSE, 0, 0);
			break;

		case rlUnpack:
			SendDlgItemMessageA(pDialog, rlLog, LB_RESETCONTENT, 0, 0);
			if(GetDlgItemTextA(pDialog, rlFilename, lFile, sizeof lFile))
			{
				bool bRealign = BST_CHECKED == IsDlgButtonChecked(pDialog, rlRealign);
				bool bCopyOverlay = BST_CHECKED == IsDlgButtonChecked(pDialog, rlOverlay);
				Unpack.Window = pDialog;
				Unpack.LogCallback = &LogCallback;
				Unpack.Unpack(lFile, bRealign, bCopyOverlay);
			}
			break;

		case rlFilename:
			if(HIWORD(pwParam) == EN_UPDATE)
			{
				bool Enable = 0 != GetWindowTextLengthA(GetDlgItem(pDialog, rlFilename));
				EnableWindow(GetDlgItem(pDialog, rlUnpack), Enable);
			}
			break;

		case rlAbout:
			{
			MSGBOXPARAMSA MsgBoxInfo = { 0 };

			MsgBoxInfo.cbSize = sizeof MsgBoxInfo;
			MsgBoxInfo.hwndOwner = pDialog;
			MsgBoxInfo.hInstance = lInstance;
			MsgBoxInfo.lpszText = lContent;
			MsgBoxInfo.lpszCaption = "[ About ]";
			MsgBoxInfo.dwStyle = MB_USERICON;
			MsgBoxInfo.lpszIcon = MAKEINTRESOURCEA(rlIcon);

			MessageBoxIndirectA(&MsgBoxInfo);
			}
			break;

		case rlBrowse:
			{
			OPENFILENAMEA File = { 0 };

			File.lStructSize = sizeof File;
			File.hwndOwner   = pDialog;
			File.lpstrFilter = "Portable Executable (x86, *.exe) \0*.exe\0Portable Executable (x86, *.dll)\0*.dll\0\0";
			File.lpstrFile   = lFile;
			File.nMaxFile    = sizeof lFile;
			File.Flags       = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER;

			if(GetOpenFileNameA(&File)) SetDlgItemTextA(pDialog, rlFilename, lFile);
			}
			break;
		}
		break;
	}

	return false;
}

void cDialog::LogCallback(const char* pText)
{
	SendDlgItemMessageA(lDialog, rlLog, LB_ADDSTRING, 0, (LPARAM)pText);
	WPARAM Index = SendDlgItemMessageA(lDialog, rlLog, LB_GETCOUNT, 0, 0) - 1;
	SendDlgItemMessageA(lDialog, rlLog, LB_SETCURSEL, Index, 0);
}
