// ASMExtractor.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "sdk\sdk.h"

// Global.constants:
#define PLUGIN_MAJOR_VERSION 1
#define PLUGIN_MINOR_VERSION 0
#define AE_PLUGIN_DATA_TYPE_MEMORY_PTR 0
#define AE_PLUGIN_REASON_CODE_ACTION 1
#define AE_NORMAL 0
#define AE_WITH_LABELS 1

// ThisPlugin.Options.Structure:
typedef struct{
	int ExportType;
	bool EmitVersionInfo;
	int LabelIncrement;
	char* szLableName;
	wchar_t* szSaveToFile;
}PluginOptions, *PPluginOptions;

// Global.variables:
HMODULE uePluginBase;
PluginOptions myOptions;
wchar_t uniFileName[MAX_PATH] = {};
char temp[255] = {0};

void StringReplace(char* source, char* search, char* replace){

	char *ptr1 = 0;
	char *ptr2 = 0;
	bool Replaced = false;
	RtlZeroMemory(&temp, 255);
	// find the search string in the source string
	ptr1 = source;
	while(*ptr1){ 
		if(*ptr1 == search[0]){
			   int i = 0, j = 0;
			   while(search[i] && ptr1[j] == search[i]){
				   i++;
				   j++;
			   }
			   if(search[i] == 0){
				   *ptr1++ = 0; // truncate source string at point of search string
				   lstrcpyA(temp, source); // copy 1st part of source string
				   lstrcatA(temp, replace);  // add the replace string
				   ptr1 += lstrlenA(search); // advance past end of the search string
				   lstrcatA(temp, ptr1); // add rest of source string
				   Replaced = true;
			   }
		  }
		  ptr1++;
	}
	if(!Replaced){
		lstrcpyA(temp, source);
	}
}
bool aeGrabData(int CallReason, int Identifier, int DataType, LPVOID DataBuffer, DWORD DataSize){

	int j;
	int x = NULL;
	HANDLE hFile;
	HANDLE hProcess;
	int JunkId = NULL;
	int cInstrLen = NULL;
	int i = int(DataSize);
	LPVOID cpMemory = NULL;
	DWORD dwNumOfBytes = NULL;
	ULONG_PTR deNumOfBytes = NULL;
	LPVOID inspectDataBuffer = DataBuffer;
	LPVOID memBuffer = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);
	LPVOID tempBuffer = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);
	LPVOID SearchBuffer = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);
	LPVOID ReplaceBuffer = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);
	LPPROCESS_INFORMATION pProcessInfo = (LPPROCESS_INFORMATION)GetProcessInformation();
	ULONG_PTR inspectCurrentAddress = (ULONG_PTR)DataBuffer;
	ULONG_PTR CurrentAddress = (ULONG_PTR)DataBuffer;
	ULONG_PTR StartAddress = (ULONG_PTR)DataBuffer;
	ULONG_PTR CurrentJumpAddress = NULL;
	char szDefaultLabel[5] = "LBL-";
	char* szDisassmInstruction;
	ULONG_PTR LabelList[1000];
	int LabelListFirstEmpty;
	bool LabelAddress = false;

	if(pProcessInfo != NULL){
		hProcess = pProcessInfo->hProcess;
		if(CallReason == AE_PLUGIN_REASON_CODE_ACTION){
			for(j = 0; j < 1000; j++){
				LabelList[j] = NULL;
			}
			hFile = CreateFile(myOptions.szSaveToFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile != INVALID_HANDLE_VALUE){
				if(DataType == AE_PLUGIN_DATA_TYPE_MEMORY_PTR){
					cpMemory = VirtualAlloc(NULL, DataSize, MEM_COMMIT, PAGE_READWRITE);
					if(!ReadProcessMemory(hProcess, DataBuffer, cpMemory, DataSize, &deNumOfBytes)){
						VirtualFree(cpMemory, NULL, MEM_RELEASE);
						return(false);
					}
					DataBuffer = cpMemory;
					inspectDataBuffer = DataBuffer;
				}
				if(myOptions.EmitVersionInfo){
					wsprintfA((LPSTR)memBuffer, "; UnpackEngine.ASMExtractor %i.%02i\r\n\r\n", PLUGIN_MAJOR_VERSION, PLUGIN_MINOR_VERSION);
					WriteFile(hFile, (LPCVOID)memBuffer, lstrlenA((LPCSTR)memBuffer), &dwNumOfBytes, NULL);
					RtlZeroMemory(memBuffer, 0x1000);
				}
				if(myOptions.szLableName == NULL){
					myOptions.szLableName = (char*)szDefaultLabel;
				}
				if(myOptions.ExportType == AE_WITH_LABELS){
					while(i > NULL){
						cInstrLen = StaticLengthDisassemble(inspectDataBuffer);
						szDisassmInstruction = (char*)StaticDisassembleEx(inspectCurrentAddress, inspectDataBuffer);
						CurrentJumpAddress = (ULONG_PTR)GetJumpDestinationEx(GetCurrentProcess(), (ULONG_PTR)inspectDataBuffer, false);
						if(CurrentJumpAddress != NULL){
							CurrentJumpAddress = StartAddress + (CurrentJumpAddress - (ULONG_PTR)cpMemory);
							LabelListFirstEmpty = NULL;
							for(j = 0; j < 1000; j++){
								if(LabelListFirstEmpty == NULL && LabelList[j] == NULL){
									LabelListFirstEmpty = j;
								}else if(LabelList[j] == CurrentJumpAddress){
									LabelListFirstEmpty = -1;
								}
							}
							if(LabelListFirstEmpty != -1 && CurrentJumpAddress != NULL && (CurrentJumpAddress >= StartAddress && CurrentJumpAddress <= StartAddress + DataSize)){
								LabelList[LabelListFirstEmpty] = CurrentJumpAddress;
								x++;
							}
						}
						inspectDataBuffer = (LPVOID)((ULONG_PTR)inspectDataBuffer + cInstrLen);
						inspectCurrentAddress = inspectCurrentAddress + cInstrLen;
						i = i - cInstrLen;
					}
				}
				int i = int(DataSize);
				while(i > NULL){
					cInstrLen = StaticLengthDisassemble(DataBuffer);
					szDisassmInstruction = (char*)StaticDisassembleEx(CurrentAddress, DataBuffer);
					CurrentJumpAddress = (ULONG_PTR)GetJumpDestinationEx(GetCurrentProcess(), (ULONG_PTR)DataBuffer, false);
					if(CurrentJumpAddress != NULL){
						CurrentJumpAddress = StartAddress + (CurrentJumpAddress - (ULONG_PTR)cpMemory);
					}
					if(myOptions.ExportType == AE_NORMAL){
						if(sizeof HANDLE == 4){
							wsprintfA((LPSTR)memBuffer, " 0x%08X: %s\r\n", CurrentAddress, (LPCSTR)szDisassmInstruction);
						}else{
							wsprintfA((LPSTR)memBuffer, " 0x%016I64X: %s\r\n", CurrentAddress, (LPCSTR)szDisassmInstruction);
						}
					}else if(myOptions.ExportType == AE_WITH_LABELS){
						LabelAddress = false;
						for(j = 0; j < x; j++){
							if(LabelList[j] == CurrentAddress){
								LabelAddress = true;
								break;
							}
						}
						if(LabelAddress){
							wsprintfA((LPSTR)memBuffer, " %s%03i: %s\r\n", myOptions.szLableName, j + myOptions.LabelIncrement, (LPCSTR)szDisassmInstruction);
						}else{
							RtlZeroMemory(tempBuffer, 0x1000);
							lstrcpyA((LPSTR)tempBuffer, (LPCSTR)szDisassmInstruction);
							for(j = 1; j < x; j++){
								RtlZeroMemory(SearchBuffer, 0x1000);
								RtlZeroMemory(ReplaceBuffer, 0x1000);
								wsprintfA((LPSTR)SearchBuffer, "0x%x", LabelList[j]);
								wsprintfA((LPSTR)ReplaceBuffer, "%s%03i", myOptions.szLableName, j + myOptions.LabelIncrement);
								StringReplace((char*)tempBuffer, (char*)SearchBuffer, (char*)ReplaceBuffer);
								lstrcpyA((LPSTR)tempBuffer, (LPCSTR)temp);
							}
							wsprintfA((LPSTR)memBuffer, "          %s\r\n", (LPCSTR)temp);
						}
					}
					WriteFile(hFile, (LPCVOID)memBuffer, lstrlenA((LPCSTR)memBuffer), &dwNumOfBytes, NULL);
					DataBuffer = (LPVOID)((ULONG_PTR)DataBuffer + cInstrLen);
					CurrentAddress = CurrentAddress + cInstrLen;
					i = i - cInstrLen;
				}
				if(cpMemory != NULL){
					VirtualFree(cpMemory, NULL, MEM_RELEASE);
				}
				CloseHandle(hFile);
				VirtualFree(memBuffer, NULL, MEM_RELEASE);
				VirtualFree(tempBuffer, NULL, MEM_RELEASE);
				VirtualFree(SearchBuffer, NULL, MEM_RELEASE);
				VirtualFree(ReplaceBuffer, NULL, MEM_RELEASE);
				return(true);
			}else{
				VirtualFree(memBuffer, NULL, MEM_RELEASE);
				VirtualFree(tempBuffer, NULL, MEM_RELEASE);
				VirtualFree(SearchBuffer, NULL, MEM_RELEASE);
				VirtualFree(ReplaceBuffer, NULL, MEM_RELEASE);
				return(false);
			}
		}
	}
	VirtualFree(memBuffer, NULL, MEM_RELEASE);
	VirtualFree(tempBuffer, NULL, MEM_RELEASE);
	VirtualFree(SearchBuffer, NULL, MEM_RELEASE);
	VirtualFree(ReplaceBuffer, NULL, MEM_RELEASE);
	return(false);
}
/*

	TitanEngine callback communication

*/
__declspec(dllexport) void __stdcall TitanDebuggingCallBack(LPDEBUG_EVENT debugEvent, int CallReason){

}
__declspec(dllexport) bool __stdcall TitanRegisterPlugin(char* szPluginName, LPDWORD titanPluginMajorVersion, LPDWORD titanPluginMinorVersion){

	if(titanPluginMajorVersion != NULL && titanPluginMinorVersion != NULL){
		*titanPluginMajorVersion = PLUGIN_MAJOR_VERSION;
		*titanPluginMinorVersion = PLUGIN_MINOR_VERSION;
		lstrcpyA(szPluginName, "ASMExtractor");
		return(true);
	}else{
		return(false);
	}
}
__declspec(dllexport) void __stdcall TitanResetPlugin(){

}
__declspec(dllexport) void __stdcall TitanReleasePlugin(){

}
// Global.Plugin.Export:
__declspec(dllexport) bool __stdcall ASMExtractorRipDataW(LPVOID DataBuffer, DWORD DataSize, int ExportType, int LabelIncrement, char* szLableName, wchar_t* szSaveFileName){

	RtlZeroMemory(&myOptions, sizeof PluginOptions);
	myOptions.EmitVersionInfo = true;
	myOptions.ExportType = ExportType;
	myOptions.LabelIncrement = LabelIncrement;
	myOptions.szLableName = szLableName;
	myOptions.szSaveToFile = szSaveFileName;
	return(aeGrabData(AE_PLUGIN_REASON_CODE_ACTION, NULL, AE_PLUGIN_DATA_TYPE_MEMORY_PTR, DataBuffer, DataSize));
}
__declspec(dllexport) bool __stdcall ASMExtractorRipData(LPVOID DataBuffer, DWORD DataSize, int ExportType, int LabelIncrement, char* szLableName, char* szSaveFileName){

	RtlZeroMemory(&uniFileName[0], sizeof uniFileName);	
	RtlZeroMemory(&myOptions, sizeof PluginOptions);
	myOptions.EmitVersionInfo = true;
	myOptions.ExportType = ExportType;
	myOptions.LabelIncrement = LabelIncrement;
	myOptions.szLableName = szLableName;
	myOptions.szSaveToFile = &uniFileName[0];
	MultiByteToWideChar(CP_ACP, NULL, szSaveFileName, lstrlenA(szSaveFileName) + 1, uniFileName, sizeof(uniFileName)/(sizeof(uniFileName[0])));
	return(aeGrabData(AE_PLUGIN_REASON_CODE_ACTION, NULL, AE_PLUGIN_DATA_TYPE_MEMORY_PTR, DataBuffer, DataSize));
}
// Global.Plugin.Entry:
bool APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		uePluginBase = hModule;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}