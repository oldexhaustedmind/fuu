// Spy.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "sdk\sdk.h"

#if defined (_WIN64)
	#define cWINAPI __fastcall
#else
	#define cWINAPI __stdcall
#endif

typedef struct RemoteData{
	HANDLE ConsoleHandle;
}RemoteData, *PRemoteData;

RemoteData myRemoteData;
void* hkAddrGPA;
void* hkAddrGMHA;
void* hkAddrLLA;

HMODULE cWINAPI hookedLoadLibraryA(LPCSTR lpModuleName){

	HMODULE hModule;
	DWORD NumberOfBytes;
	char szOutputBuffer[MAX_PATH] = {};
	typedef HMODULE(cWINAPI *fLoadLibraryA)(LPCSTR lpModuleName);
	fLoadLibraryA cLoadLibraryA = (fLoadLibraryA)HooksGetTrampolineAddress(hkAddrLLA);

	// Finally execute real LoadLibraryA
	hModule = cLoadLibraryA(lpModuleName);
	if(lpModuleName != NULL){	
		wsprintfA(szOutputBuffer, "LoadLibraryA call to %s (%x)\r\n", lpModuleName, hModule);
		WriteFile(myRemoteData.ConsoleHandle, szOutputBuffer, lstrlenA(szOutputBuffer), &NumberOfBytes, NULL);
	}
	return(hModule);
}
HMODULE cWINAPI hookedGetModuleHandleA(LPCSTR lpModuleName){

	HMODULE hModule;
	DWORD NumberOfBytes;
	char szOutputBuffer[MAX_PATH] = {};
	typedef HMODULE(cWINAPI *fGetModuleHandleA)(LPCSTR lpModuleName);
	fGetModuleHandleA cGetModuleHandleA = (fGetModuleHandleA)HooksGetTrampolineAddress(hkAddrGMHA);

	// Finally execute real LoadLibraryA
	hModule = cGetModuleHandleA(lpModuleName);
	if(lpModuleName != NULL){	
		wsprintfA(szOutputBuffer, "GetModuleHandleA call %s (%x)\r\n", lpModuleName, hModule);
		WriteFile(myRemoteData.ConsoleHandle, szOutputBuffer, lstrlenA(szOutputBuffer), &NumberOfBytes, NULL);
	}
	return(hModule);
}
void* cWINAPI hookedGetProcAddress(HMODULE hModule, LPCSTR lpProcName){

	DWORD NumberOfBytes;
	char szModuleName[MAX_PATH] = {};
	char szOutputBuffer[MAX_PATH] = {};
	typedef void*(cWINAPI *fGetProcAddress)(HMODULE hModule, LPCSTR lpProcName);
	fGetProcAddress cGetProcAddress = (fGetProcAddress)HooksGetTrampolineAddress(hkAddrGPA);

	if(GetModuleFileNameA(hModule, szModuleName, MAX_PATH) > NULL){
		wsprintfA(szOutputBuffer, "GetProcAddress call to %s!%s\r\n", szModuleName, lpProcName);
		WriteFile(myRemoteData.ConsoleHandle, szOutputBuffer, lstrlenA(szOutputBuffer), &NumberOfBytes, NULL);
	}
	// Finally execute real GetProcAddress
	return(cGetProcAddress(hModule, lpProcName));
}

void InitHooks(){

	// Hook all APIs
	hkAddrGMHA = (void*)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetModuleHandleA");
	hkAddrLLA = (void*)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	hkAddrGPA = (void*)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetProcAddress");

	HooksInsertNewRedirection(hkAddrGMHA, &hookedGetModuleHandleA, TEE_HOOK_NRM_JUMP);
	HooksInsertNewRedirection(hkAddrLLA, &hookedLoadLibraryA, TEE_HOOK_NRM_JUMP);	
	HooksInsertNewRedirection(hkAddrGPA, &hookedGetProcAddress, TEE_HOOK_NRM_JUMP);
}

void RemoveHooks(){

	// First we pause all the running threads except this one
	HooksSafeTransition(NULL, true);
	HooksRemoveRedirection(NULL, true);
	HooksSafeTransition(NULL, false);
}

__declspec(dllexport) bool __stdcall SpyProcess(){

	AllocConsole();
	myRemoteData.ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	InitHooks();
	return(true);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved){

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if(!SpyProcess()){
			return FALSE;
		}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		if(lpReserved != NULL){
			RemoveHooks();
		}
		break;
	}
	return TRUE;
}