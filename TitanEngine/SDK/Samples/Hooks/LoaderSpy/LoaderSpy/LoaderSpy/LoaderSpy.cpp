// LoaderSpy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "sdk\sdk.h"
#include "psapi.h"


LPPROCESS_INFORMATION lpProcessInformation;

void __stdcall epBreakPoint(){

	if(RemoteLoadLibrary(lpProcessInformation->hProcess, "Spy.dll", true)){
		printf("Injection Success!\r\n");
		DetachDebuggerEx(lpProcessInformation->dwProcessId);
	}else{
		StopDebug();
		printf("Error while injecting data into new process!\r\n");
	}
}

int __cdecl main(int argc, char* argv[]){

	char szFileName[MAX_PATH];

	lstrcpyA(szFileName, argv[1]);
	printf("LoaderSpy 1.0\r\nReversingLabs Corporation\r\nUsage: loaderspy filename.exe\r\n\r\n");
	lpProcessInformation = (LPPROCESS_INFORMATION)InitDebugEx(szFileName, NULL, NULL, &epBreakPoint);
	if(lpProcessInformation != NULL){
		DebugLoop();
	}else{
		printf("Error while creating process!\r\n");
	}
	return(NULL);
}

