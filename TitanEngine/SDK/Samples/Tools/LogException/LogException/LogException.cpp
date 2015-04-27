// LogException.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "SDK\SDK.h"

bool bHideDebugger = false;
LPPROCESS_INFORMATION ProcessInfo;

void cbCustomHandler(void* ExceptionData){

	PLIBRARY_ITEM_DATA LibraryInfo;
	LPDEBUG_EVENT DbgEvent = (LPDEBUG_EVENT)GetDebugData();

	if(DbgEvent->dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT){
		printf(" CreateProcess: %08X, LoadBase: %08X, StartAddress: %08X\r\n", DbgEvent->dwProcessId, DbgEvent->u.CreateProcessInfo.hProcess, DbgEvent->u.CreateProcessInfo.lpStartAddress);
		if(bHideDebugger){
			if(HideDebugger(ProcessInfo->hProcess, NULL)){
				printf(" Debugging of the process has been hidden!\r\n");
			}
			bHideDebugger = false;
		}
	}else if(DbgEvent->dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT){
		printf(" ExitProcess: %08X, ExitCode: %08X\r\n", DbgEvent->dwProcessId, DbgEvent->u.ExitProcess.dwExitCode);
	}else if(DbgEvent->dwDebugEventCode == CREATE_THREAD_DEBUG_EVENT){
		printf(" CreateThread: Process(%08X) Id:%08X, StartAddress: %08X\r\n", DbgEvent->dwProcessId, DbgEvent->dwThreadId, DbgEvent->u.CreateThread.lpStartAddress);
	}else if(DbgEvent->dwDebugEventCode == EXIT_THREAD_DEBUG_EVENT){
		printf(" ExitThread: Process(%08X) Id:%08X, ExitCode: %08X\r\n", DbgEvent->dwProcessId, DbgEvent->dwThreadId, DbgEvent->u.ExitThread.dwExitCode);
	}else if(DbgEvent->dwDebugEventCode == LOAD_DLL_DEBUG_EVENT){
		LibraryInfo = (PLIBRARY_ITEM_DATA)LibrarianGetLibraryInfoEx(DbgEvent->u.LoadDll.lpBaseOfDll);
		printf("  LoadLibrary: Process(%08X), LoadBase: %08X, Name: %s\r\n", DbgEvent->dwProcessId, DbgEvent->u.LoadDll.lpBaseOfDll, LibraryInfo->szLibraryName);
	}else if(DbgEvent->dwDebugEventCode == UNLOAD_DLL_DEBUG_EVENT){
		LibraryInfo = (PLIBRARY_ITEM_DATA)LibrarianGetLibraryInfoEx(DbgEvent->u.LoadDll.lpBaseOfDll);
		printf("  UnloadLibrary: Process(%08X), LoadBase: %08X, Name: %s\r\n", DbgEvent->dwProcessId, DbgEvent->u.UnloadDll.lpBaseOfDll, LibraryInfo->szLibraryName);
	}else if(DbgEvent->dwDebugEventCode == EXCEPTION_DEBUG_EVENT){
		printf("   Exception in Process(%08X) %08X at %08X\r\n", DbgEvent->dwProcessId, DbgEvent->u.Exception.ExceptionRecord.ExceptionCode, DbgEvent->u.Exception.ExceptionRecord.ExceptionAddress);
	}
}

int _tmain(int argc, _TCHAR* argv[]){

	printf("LogException 1.0\r\nReversingLabs Corporation - www.reversinglabs.com\r\n\r\n");
	if(argc > 2){
		if(argv[1][0] == 'h'){
			bHideDebugger = true;
		}
		if(IsFileDLLW(argv[2], NULL)){
			ProcessInfo = (LPPROCESS_INFORMATION)InitDLLDebugW(argv[2], false, NULL, NULL, NULL);
		}else{
			ProcessInfo = (LPPROCESS_INFORMATION)InitDebugW(argv[2], NULL, NULL);
		}
		if(ProcessInfo != NULL){
			SetCustomHandler(UE_CH_EVERYTHINGELSE, &cbCustomHandler);
			SetCustomHandler(UE_CH_CREATETHREAD, &cbCustomHandler);
			SetCustomHandler(UE_CH_EXITTHREAD, &cbCustomHandler);
			SetCustomHandler(UE_CH_CREATEPROCESS, &cbCustomHandler);
			SetCustomHandler(UE_CH_EXITPROCESS, &cbCustomHandler);
			SetCustomHandler(UE_CH_LOADDLL, &cbCustomHandler);
			SetCustomHandler(UE_CH_UNLOADDLL, &cbCustomHandler);
			DebugLoop();
		}else{
			printf(" Could not start process for debugging!\r\n");
		}
	}else{
		printf(" Usage: LogException [r|h] inputFileName.ext\r\n Options: r - Run; h - HideDebugger\r\n\r\n");
	}
	return 0;
}

