// LibraryLoader.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "LibraryLoader.h"

wchar_t szLibraryName[512];
wchar_t szFileName[512];

void ReserveImageBase(){

	HANDLE hFile;

	lstrcpyW((LPWSTR)szFileName, (LPCWSTR)szLibraryName);
	lstrcatW((LPWSTR)szFileName, L".module");
	hFile = CreateFileW((LPCWSTR)szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE){
		CloseHandle(hFile);
		//MessageBoxW(NULL, szFileName, szLibraryName, 0x40);
		LoadLibraryW((LPCWSTR)szFileName);
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow){

	ReserveImageBase();
	if(LoadLibraryW((LPCWSTR)szLibraryName) == NULL){
		//DeleteFileA((LPCSTR)szFileName);
		ExitProcess(0x61703078);
	}else{
		//DeleteFileA((LPCSTR)szFileName);
		ExitProcess(NULL);
	}
}