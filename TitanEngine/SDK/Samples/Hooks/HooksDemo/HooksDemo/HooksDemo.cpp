// HooksDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#if defined(HOOKING_USES_TE)
	#include "sdk\sdk.h"
#else
	#include "sdk\TH.h"
#endif
#include <stdlib.h>

void* hkAddrGPA;
void* hkAddrGMHA;
int GlobalCounter = NULL;
char szMainModuleName[MAX_PATH] = {};

#if defined (_WIN64)
	#define cWINAPI __fastcall
#else
	#define cWINAPI __stdcall
#endif

// Ignore this code <start>
#if !defined (_WIN64)
__declspec(naked) long RelocateTest(){
	__asm{
		NOP
		JMP label
			XOR EAX,EAX
label:
		INC EAX
		RET
	}
}

long hookedRelocateTest(){

	typedef long(cWINAPI *fRelocateTest)();
	fRelocateTest cRelocateTest = (fRelocateTest)HooksGetTrampolineAddress(&RelocateTest);

	printf("Relocation test!\r\n");
	
	return(cRelocateTest());
}
#endif
// Ignore this code <stop>

long SecondThread(LPVOID InputParameter){

	__try{
		while(GlobalCounter < 10){
			GetProcAddress(GetModuleHandleA("kernel32.dll"), "ExitProcess");
			GlobalCounter++;
			Sleep(100);
		}
	}__except(EXCEPTION_EXECUTE_HANDLER){
		printf("ERROR!\r\n");
	}
	return(NULL);
}
HMODULE cWINAPI hookedGetModuleHandleA(LPCSTR lpModuleName){

	typedef HMODULE(cWINAPI *fGetModuleHandleA)(LPCSTR lpModuleName);
	fGetModuleHandleA cGetModuleHandleA = (fGetModuleHandleA)HooksGetTrampolineAddress(hkAddrGMHA);

	printf("GetModuleHandleA call to %s\r\n", lpModuleName);
	
	// Fix pointer if IAT is hooked
	if(cGetModuleHandleA == NULL){
		cGetModuleHandleA = (fGetModuleHandleA)hkAddrGMHA;
	}
	// Finally execute real GetModuleHandleA
	return(cGetModuleHandleA(lpModuleName));
}
void* cWINAPI hookedGetProcAddress(HMODULE hModule, LPCSTR lpProcName){

	char szModuleName[MAX_PATH] = {};
	typedef void*(cWINAPI *fGetProcAddress)(HMODULE hModule, LPCSTR lpProcName);
	fGetProcAddress cGetProcAddress = (fGetProcAddress)HooksGetTrampolineAddress(hkAddrGPA);

	// Determine hook type, no trampoline means IAT hook
	if(cGetProcAddress != NULL){
		// Pause execution to disable hook since printf calls GetProcAddress in its call tree
		HooksSafeTransition(NULL, true);
		HooksDisableRedirection(hkAddrGPA, false);
		
		if(GetModuleFileNameA(hModule, szModuleName, MAX_PATH) > NULL){
			printf("GetProcAddress call to %s!%s\r\n", szModuleName, lpProcName);
		}
		
		// Enable hook and resume execution
		HooksEnableRedirection(hkAddrGPA, false);
		HooksSafeTransition(NULL, false);
	}else{
		// Pause execution to disable hook since printf calls GetProcAddress in its call tree
		HooksSafeTransition(NULL, true);
		HooksDisableIATRedirection(szMainModuleName, "GetProcAddress", false);

		if(GetModuleFileNameA(hModule, szModuleName, MAX_PATH) > NULL){
			printf("GetProcAddress call to %s!%s\r\n", szModuleName, lpProcName);
		}

		// Enable hook and resume execution
		HooksEnableIATRedirection(szMainModuleName, "GetProcAddress", false);
		HooksSafeTransition(NULL, false);
	}

	// Fix pointer if IAT is hooked
	if(cGetProcAddress == NULL){
		cGetProcAddress = (fGetProcAddress)hkAddrGPA;
	}
	// Finally execute real GetProcAddress
	return(cGetProcAddress(hModule, lpProcName));
}
int __cdecl _tmain(int argc, _TCHAR* argv[]){

	DWORD ThreadId;
	HANDLE hThread;

	/*
		Demo
	*/
	printf("ReversingLabs Hooks demo\r\n");
	printf("Demo #1: Hooking APIs\r\n");
	GetModuleFileNameA(GetModuleHandleA(NULL), szMainModuleName, MAX_PATH);

	// Create a second thread for testing purposes
	hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&SecondThread, NULL, NULL, &ThreadId);
	// First we pause all the running threads except this one
	HooksSafeTransition(NULL, true);

	// Hook all APIs
	hkAddrGMHA = (void*)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetModuleHandleA");
	hkAddrGPA = (void*)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetProcAddress");

// Ignore this code <start>
#if !defined (_WIN64)
	// Relocation test is perfomed by debugging
	HooksInsertNewRedirection(&RelocateTest, &hookedRelocateTest, TEE_HOOK_NRM_JUMP);
	RelocateTest();
#endif
// Ignore this code <stop>

	/*
		ACTUAL HOOKING CODE, LOOK HERE!
	*/
	HooksInsertNewRedirection(hkAddrGMHA, &hookedGetModuleHandleA, TEE_HOOK_NRM_JUMP);
	HooksInsertNewRedirection(hkAddrGPA, &hookedGetProcAddress, TEE_HOOK_NRM_JUMP);

// Ignore this code <start>
#if !defined (_WIN64)
	RelocateTest();
#endif
// Ignore this code <stop>

	// Resume execution
	HooksSafeTransition(NULL, false);

	// Execute hooks 10 times
	WaitForSingleObject(hThread, INFINITE);

	printf("Demo #2: Hooking IAT\r\n");
	GlobalCounter = NULL;
	// Pause execution to remove all hooks
	HooksSafeTransition(NULL, true);
	HooksRemoveRedirection(NULL, true);

	HooksInsertNewIATRedirection(szMainModuleName, "GetModuleHandleA", &hookedGetModuleHandleA);
	HooksInsertNewIATRedirection(szMainModuleName, "GetProcAddress", &hookedGetProcAddress);

	// Resume execution
	HooksSafeTransition(NULL, false);

	// Create a second thread for testing purposes
	hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&SecondThread, NULL, NULL, &ThreadId);
	// Execute hooks 10 times
	WaitForSingleObject(hThread, INFINITE);

	printf("Demo has terminated successfully!\r\n");
	return 0;
}

