// FSG.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SDK\SDK.h"

BYTE fsgPattern1[] = {0xAD,0x50,0xFF,0x53,0x10,0x95}; // LoadLibary pattern; +1
BYTE fsgPattern2[] = {0x50,0x55,0xFF,0x53,0x14,0xAB}; // GetProcAddress pattern; +0
BYTE fsgPattern3[] = {0x78,0xF3,0x75,0x03,0xFF,0x63,0x0C}; // EP pattern; +4

void FinalizeUnpacking(){

	LPPROCESS_INFORMATION lpProcessInfo = (LPPROCESS_INFORMATION)GetProcessInformation();

	EngineUnpackerSetEntryPointAddress((ULONG_PTR)GetJumpDestination(lpProcessInfo->hProcess, (ULONG_PTR)GetContextData(UE_EIP)));
	EngineUnpackerFinalizeUnpacking();
}

void StartUnpacking(){

	if(!EngineUnpackerSetBreakCondition((void*)UE_UNPACKER_CONDITION_SEARCH_FROM_EP, 0xA0, &fsgPattern1, sizeof fsgPattern1, 1, UE_UNPACKER_CONDITION_LOADLIBRARY, false, UE_EAX, NULL)){
		EngineAddUnpackerWindowLogMessage("[Error] File isn't packed with FSG 2.0!");
		StopDebug();
		return;
	}
	if(!EngineUnpackerSetBreakCondition((void*)UE_UNPACKER_CONDITION_SEARCH_FROM_EP, 0xA0, &fsgPattern2, sizeof fsgPattern2, NULL, UE_UNPACKER_CONDITION_GETPROCADDRESS, false, UE_EAX, UE_EDI)){
		EngineAddUnpackerWindowLogMessage("[Error] File isn't packed with FSG 2.0!");
		StopDebug();
		return;
	}
	if(!EngineUnpackerSetBreakCondition((void*)UE_UNPACKER_CONDITION_SEARCH_FROM_EP, 0xA0, &fsgPattern3, sizeof fsgPattern3, 4, (ULONG_PTR)&FinalizeUnpacking, false, NULL, NULL)){
		EngineAddUnpackerWindowLogMessage("[Error] File isn't packed with FSG 2.0!");
		StopDebug();
		return;
	}
}

void InitUnpacker(char* szFileName, bool DoRealign, bool DoMoveOverlay){
	
	EngineUnpackerInitialize(szFileName, NULL, true, DoRealign, DoMoveOverlay, &StartUnpacking);
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow){

	EngineCreateUnpackerWindow("FSG 2.0", "Unpacker for FSG 2.0 packed files", "RL!deFSG 2.0", "ReversingLabs Corporation", &InitUnpacker);
	return 0;
}

