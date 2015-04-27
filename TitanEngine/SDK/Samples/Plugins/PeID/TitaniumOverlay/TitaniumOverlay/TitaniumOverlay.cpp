// TitaniumOverlay.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "sdk\sdk.h"
#include "resource.h"

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0'" \
						"processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"") 

char* szMainFileName;
char AdditionOverlay[1024] = {};
char GlobalBuffer[1024] = {};
HMODULE hInstance;

bool GetFileDialog(HWND hwndDlg, char* szDialogTitle, bool SaveFile){

	OPENFILENAMEA sOpenFileName;
	char szFilterString[] = "All Files \0*.*\0\0";

	RtlZeroMemory(&GlobalBuffer, sizeof(GlobalBuffer)); 	
	RtlZeroMemory(&sOpenFileName, sizeof(OPENFILENAMEA)); 
	sOpenFileName.lStructSize = sizeof(OPENFILENAMEA);
	sOpenFileName.lpstrFilter = &szFilterString[0];
	sOpenFileName.lpstrFile = &GlobalBuffer[0];
	sOpenFileName.nMaxFile = 1024;
	sOpenFileName.Flags = OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_HIDEREADONLY;
	sOpenFileName.lpstrTitle = &szDialogTitle[0];
	sOpenFileName.hwndOwner = hwndDlg;
	if(SaveFile){
		if(!GetSaveFileNameA(&sOpenFileName)){
			RtlZeroMemory(&GlobalBuffer[0], 1024);
		}
	}else{
		if(!GetOpenFileNameA(&sOpenFileName)){
			RtlZeroMemory(&GlobalBuffer[0], 1024);
		}
	}
	if(GlobalBuffer[0] == 0x00){
		return(false);
	}else{
		return(true);
	}
}
void ScanOverlay(HWND hwndDlg){

	DWORD OverlayStart = NULL;
	DWORD OverlaySize = NULL;
	char LocalBuffer[1024] = {};

	FindOverlay(szMainFileName, &OverlayStart, &OverlaySize);
	wsprintfA(LocalBuffer, "%08X", OverlayStart);
	SetDlgItemTextA(hwndDlg, IDC_EDIT2, LocalBuffer);
	wsprintfA(LocalBuffer, "%08X", OverlaySize);
	SetDlgItemTextA(hwndDlg, IDC_EDIT3, LocalBuffer);
}
// Forward declarations of functions included in this code module:
INT_PTR CALLBACK WndProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){

	char szDlgTitle[] = "[TitaniumOverlay]";
	char szAboutText[] = "TitaniumOverlay 1.0 \r\n\r\n Visit Reversing Labs at http://www.reversinglabs.com \r\n\r\n  Minimum engine version needed:\r\n- TitanEngine 2.0.2 by RevLabs\r\n\r\nPlugin coded by Reversing Labs";
	char szAboutTitle[] = "[ About ]";

	if(uMsg == WM_INITDIALOG){
		SendMessageA(hwndDlg, WM_SETTEXT, NULL, (LPARAM)&szDlgTitle);
		SendMessageA(hwndDlg, WM_SETICON, NULL, (LPARAM)LoadIconA((HINSTANCE)hInstance, MAKEINTRESOURCEA(IDI_ICON1)));
		CheckRadioButton(hwndDlg, IDC_RADIO1, IDC_RADIO4, IDC_RADIO1);
		ScanOverlay(hwndDlg);
	}else if(uMsg == WM_CLOSE){
		EndDialog(hwndDlg, NULL);
	}else if(uMsg == WM_DROPFILES){
		DragQueryFileA((HDROP)wParam, NULL, GlobalBuffer, 1024);
		SetDlgItemTextA(hwndDlg, IDC_EDIT1, GlobalBuffer);
		if(GlobalBuffer[0] != 0x00){
			lstrcpyA(AdditionOverlay, GlobalBuffer);
		}else{
			RtlZeroMemory(AdditionOverlay, sizeof AdditionOverlay);
		}
	}else if(uMsg == WM_COMMAND){
		if(wParam == IDC_CLOSE){
			EndDialog(hwndDlg, NULL);
		}else if(wParam == IDC_BROWSE){	
			if(GetFileDialog(hwndDlg, "Open file...", false)){
				SetDlgItemTextA(hwndDlg, IDC_EDIT1, GlobalBuffer);
				lstrcpyA(AdditionOverlay, GlobalBuffer);
			}else{
				RtlZeroMemory(AdditionOverlay, sizeof AdditionOverlay);
				SetDlgItemTextA(hwndDlg, IDC_EDIT1, AdditionOverlay);
			}
		}else if(wParam == IDC_ACTION){		
			if(IsDlgButtonChecked(hwndDlg, IDC_RADIO1)){
				if(GetFileDialog(hwndDlg, "Extract overlay...", true)){
					if(ExtractOverlay(szMainFileName, GlobalBuffer)){
						MessageBoxA(hwndDlg, "Overlay has been extracted!", "TitaniumOverlay:", MB_ICONASTERISK);
					}else{
						MessageBoxA(hwndDlg, "Overlay has NOT been extracted!", "TitaniumOverlay:", MB_ICONEXCLAMATION);
					}
				}
			}else if(IsDlgButtonChecked(hwndDlg, IDC_RADIO2)){
				if(AdditionOverlay[0] != 0x00){
					if(CopyOverlay(szMainFileName, AdditionOverlay)){
						MessageBoxA(hwndDlg, "Overlay has been copied to selected file!", "TitaniumOverlay:", MB_ICONASTERISK);
					}else{
						MessageBoxA(hwndDlg, "Overlay has NOT been copied to selected file", "TitaniumOverlay:", MB_ICONEXCLAMATION);
					}
				}else{
					MessageBoxA(hwndDlg, "You must select the file which will receive the overlay from the main file!", "TitaniumOverlay:", MB_ICONEXCLAMATION);
				}
			}else if(IsDlgButtonChecked(hwndDlg, IDC_RADIO3)){
				if(RemoveOverlay(szMainFileName)){
					MessageBoxA(hwndDlg, "Overlay has been removed from the selected file!", "TitaniumOverlay:", MB_ICONASTERISK);
					ScanOverlay(hwndDlg);
				}else{
					MessageBoxA(hwndDlg, "Overlay has NOT been removed from the selected file", "TitaniumOverlay:", MB_ICONEXCLAMATION);
				}
			}else if(IsDlgButtonChecked(hwndDlg, IDC_RADIO4)){
				if(AdditionOverlay[0] != 0x00){
					if(AddOverlay(szMainFileName, AdditionOverlay)){
						MessageBoxA(hwndDlg, "Overlay has been added to selected file!", "TitaniumOverlay:", MB_ICONASTERISK);
						ScanOverlay(hwndDlg);
					}else{
						MessageBoxA(hwndDlg, "Overlay has NOT been added to selected file", "TitaniumOverlay:", MB_ICONEXCLAMATION);
					}
				}else{
					MessageBoxA(hwndDlg, "You must select the file which will be added as overlay to main file!", "TitaniumOverlay:", MB_ICONEXCLAMATION);
				}
			}
		}else if(wParam == IDC_ABOUT){
			MessageBoxA(hwndDlg, szAboutText, szAboutTitle, MB_ICONASTERISK);
		}
	}
	return(NULL);
}

DWORD __cdecl DoMyJob(HWND hMainDlg, char *szFname, DWORD lpReserved, LPVOID lpParam){
	
	//hMainDlg: HWND of PEiD window
	//szFname: Filename
	//lpReserved: PEiD passes 'PEiD' as the value
	//lpParam: NULL passed, for future use
 
	// Write your main code here

	szMainFileName = szFname;
	DialogBoxParamA(hInstance, MAKEINTRESOURCEA(IDD_DIALOG1), hMainDlg, (DLGPROC)WndProc, NULL);
	return 1;
}

LPSTR __cdecl LoadDll(){
	return "TitaniumOverlay";
}

BOOL APIENTRY myDLLMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved){

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hInstance = hModule;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}