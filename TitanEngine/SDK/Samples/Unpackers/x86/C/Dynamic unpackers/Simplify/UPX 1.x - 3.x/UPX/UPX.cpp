// UPX.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SDK\SDK.h"

// Unpacker Data:
BYTE upxPattern1[] = {0x50,0x83,0xC7,0x08,0xFF}; //LoadLibrary callback
BYTE upxPattern2[] = {0x50,0x47,0x00,0x57,0x48,0xF2,0xAE}; //GetProcAddress callback
BYTE upxPattern3[] = {0x57,0x48,0xF2,0xAE,0x00,0xFF}; //GetProcAddress callback
BYTE upxPattern4[] = {0x89,0xF9,0x57,0x48,0xF2,0xAE,0x52,0xFF}; //GetProcAddress callback
BYTE upxPattern5[] = {0x61,0xE9}; // EP jump
BYTE upxPattern6[] = {0x83,0xEC,0x00,0xE9}; // EP jump
BYTE upxPattern7[] = {0x31,0xC0,0x8A,0x07,0x47,0x09,0xC0,0x74,0x22,0x3C,0xEF,0x77,0x11,0x01,\
					   0xC3,0x8B,0x03,0x86,0xC4,0xC1,0xC0,0x10,0x86,0xC4,0x01,0xF0,0x89,0x03,\
					   0xEB,0xE2,0x24,0x0F,0xC1,0xE0,0x10,0x66,0x8B,0x07,0x83,0xC7,0x02,0xEB,0xE2};

ULONG_PTR SnapshootMemoryStartRVA;
ULONG_PTR SnapshootMemorySize;

void UnpackingError(){
	
	EngineAddUnpackerWindowLogMessage("-> Unpack ended...");
	StopDebug();
}

void FinishUnpacking(){

	LPPROCESS_INFORMATION DebugInfo = (LPPROCESS_INFORMATION)GetProcessInformation();

	EngineUnpackerSetEntryPointAddress((ULONG_PTR)GetJumpDestination(DebugInfo->hProcess, (ULONG_PTR)GetContextData(UE_CIP)));
	EngineUnpackerFinalizeUnpacking();
}

void StartUnpacking(){

	if(!EngineUnpackerSetBreakCondition((void*)UE_UNPACKER_CONDITION_SEARCH_FROM_EP, NULL, &upxPattern1[0], sizeof upxPattern1, NULL, UE_UNPACKER_CONDITION_LOADLIBRARY, false, UE_EAX, -1)){
		EngineAddUnpackerWindowLogMessage("[Error] File is not packed with UPX 1.x - 3.x");
		UnpackingError();
		return;
	}
	EngineUnpackerSetBreakCondition((void*)UE_UNPACKER_CONDITION_SEARCH_FROM_EP, NULL, &upxPattern2[0], sizeof upxPattern2, NULL, UE_UNPACKER_CONDITION_GETPROCADDRESS, false, UE_EAX, UE_EBX);
	if(!EngineUnpackerSetBreakCondition((void*)UE_UNPACKER_CONDITION_SEARCH_FROM_EP, NULL, &upxPattern3[0], sizeof upxPattern3, NULL, UE_UNPACKER_CONDITION_GETPROCADDRESS, false, UE_EDI, UE_EBX)){
		EngineAddUnpackerWindowLogMessage("[Error] File is not packed with UPX 1.x - 3.x");
		UnpackingError();
		return;
	}
	EngineUnpackerSetBreakCondition((void*)UE_UNPACKER_CONDITION_SEARCH_FROM_EP, NULL, &upxPattern4[0], sizeof upxPattern4, 2, UE_UNPACKER_CONDITION_GETPROCADDRESS, false, UE_EDI, UE_EBX);
	if(!EngineUnpackerSetBreakCondition((void*)UE_UNPACKER_CONDITION_SEARCH_FROM_EP, NULL, &upxPattern5[0], sizeof upxPattern5, 1, (ULONG_PTR)&FinishUnpacking, false, NULL, NULL)){
		if(!EngineUnpackerSetBreakCondition((void*)UE_UNPACKER_CONDITION_SEARCH_FROM_EP, NULL, &upxPattern6[0], sizeof upxPattern6, 3, (ULONG_PTR)&FinishUnpacking, false, NULL, NULL)){
			EngineAddUnpackerWindowLogMessage("[Error] File is not packed with UPX 1.x - 3.x");
			UnpackingError();
			return;
		}
	}
	EngineUnpackerSetBreakCondition((void*)UE_UNPACKER_CONDITION_SEARCH_FROM_EP, NULL, &upxPattern7[0], sizeof upxPattern7, -3, UE_UNPACKER_CONDITION_RELOCSNAPSHOT1, false, SnapshootMemoryStartRVA, SnapshootMemorySize);
}

void __stdcall InitializeUnpacking(char* szInputFile, bool RealignFile, bool CopyOverlay){

	SnapshootMemoryStartRVA = (ULONG_PTR)GetPE32Data(szInputFile, NULL, UE_SECTIONVIRTUALOFFSET);
	SnapshootMemorySize = (ULONG_PTR)GetPE32Data(szInputFile, NULL, UE_OEP) - SnapshootMemoryStartRVA;

	EngineUnpackerInitialize(szInputFile, NULL, true, RealignFile, CopyOverlay, &StartUnpacking);
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow){

	EngineCreateUnpackerWindow("UPX 1.x - 3.x", "Unpacker for UPX packed files", "RL!deUPX 1.x - 3.x", "ReversingLabs Corporation", &InitializeUnpacking);
	return 0;
}
