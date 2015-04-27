// DataExtractor.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "sdk\sdk.h"

// Global.constants:
#define PLUGIN_MAJOR_VERSION 1
#define PLUGIN_MINOR_VERSION 0
#define DE_PLUGIN_DATA_TYPE_MEMORY_PTR 0
#define DE_PLUGIN_REASON_CODE_ACTION 1
#define DE_ASM 0
#define DE_C 1
#define DE_DELPHI 2
#define DE_VB 3

// ThisPlugin.Options.Structure:
typedef struct{
	int ExportType;			// 0 - ASM, 1 - C/C++, 2 - Delphi, 3 - VB
	int RipSize;
	int BytesPerLine;
	int TableStart;
	bool EmitVersionInfo;
	wchar_t* szSaveToFile;
	char* szTableName;
}PluginOptions, *PPluginOptions;

// Global.variables:
HMODULE uePluginBase;
PluginOptions myOptions;
wchar_t uniFileName[MAX_PATH] = {};

bool deGrabData(int CallReason, int Identifier, int DataType, LPVOID DataBuffer, DWORD DataSize){

	int i;
	int j = NULL;
	int x = NULL;
	HANDLE hFile;
	HANDLE hProcess;
	BYTE ReadByte[8] = {0,0,0,0,0,0,0,0};
	LPVOID cpMemory = NULL;
	ULONG_PTR deNumOfBytes = NULL;
	DWORD dwNumOfBytes = NULL;
	LPVOID memBuffer = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);
	LPVOID tempBuffer = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);
	LPPROCESS_INFORMATION pProcessInfo = (LPPROCESS_INFORMATION)GetProcessInformation();
	char szTabName[10] = "TableData";
	bool OnlyFirst = false;

	if(pProcessInfo != NULL){
		hProcess = pProcessInfo->hProcess;
		if(CallReason == DE_PLUGIN_REASON_CODE_ACTION){
			hFile = CreateFileW(myOptions.szSaveToFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile != INVALID_HANDLE_VALUE){
				if(DataType == DE_PLUGIN_DATA_TYPE_MEMORY_PTR){
					cpMemory = VirtualAlloc(NULL, DataSize, MEM_COMMIT, PAGE_READWRITE);
					if(!ReadProcessMemory(hProcess, DataBuffer, cpMemory, DataSize, &deNumOfBytes)){
						VirtualFree(cpMemory, NULL, MEM_RELEASE);
						return(false);
					}
					DataBuffer = cpMemory;
				}
				if(myOptions.EmitVersionInfo){
					wsprintfA((LPSTR)memBuffer, "; UnpackEngine.DataExtractor %i.%02i\r\n\r\n", PLUGIN_MAJOR_VERSION, PLUGIN_MINOR_VERSION);
					WriteFile(hFile, (LPCVOID)memBuffer, lstrlenA((LPCSTR)memBuffer), &dwNumOfBytes, NULL);
					RtlZeroMemory(memBuffer, 0x1000);
				}
				if(myOptions.szTableName == NULL){
					myOptions.szTableName = (char*)szTabName;
				}
				if(myOptions.RipSize == NULL || myOptions.RipSize == 3){
					myOptions.RipSize++;
				}
				if(myOptions.RipSize > 4){
					myOptions.RipSize = 8;
				}
				if(myOptions.RipSize > 4 && myOptions.ExportType >= DE_DELPHI){
					myOptions.RipSize = 4;
				}
				myOptions.BytesPerLine--;
				for(i = 0; i < ((int)DataSize / myOptions.RipSize); i++){
					for(x = 0; x < 8; x++){
						ReadByte[x] = NULL;
					}
					for(x = 0; x < myOptions.RipSize; x++){
						RtlMoveMemory(&ReadByte[x], (LPVOID)((ULONG_PTR)DataBuffer + x), 1);
					}
					if(myOptions.ExportType == DE_ASM){
						if(j == NULL){
							if(myOptions.RipSize == 1){
								wsprintfA((LPSTR)tempBuffer, " db %03Xh", ReadByte[0]);
							}else if(myOptions.RipSize == 2){
								wsprintfA((LPSTR)tempBuffer, " dw %03X%02Xh", ReadByte[1], ReadByte[0]);
							}else if(myOptions.RipSize == 4){
								wsprintfA((LPSTR)tempBuffer, " dd %03X%02X%02X%02Xh", ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
							}else if(myOptions.RipSize == 8){
								wsprintfA((LPSTR)tempBuffer, " dq %03X%02X%02X%02X%02X%02X%02X%02Xh", ReadByte[7], ReadByte[6], ReadByte[5], ReadByte[4], ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
							}
						}else if(j != myOptions.BytesPerLine){
							if(myOptions.RipSize == 1){
								wsprintfA((LPSTR)tempBuffer, ",%03Xh", ReadByte[0]);
							}else if(myOptions.RipSize == 2){
								wsprintfA((LPSTR)tempBuffer, ",%03X%02Xh", ReadByte[1], ReadByte[0]);
							}else if(myOptions.RipSize == 4){
								wsprintfA((LPSTR)tempBuffer, ",%03X%02X%02X%02Xh", ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
							}else if(myOptions.RipSize == 8){
								wsprintfA((LPSTR)tempBuffer, ",%03X%02X%02X%02X%02X%02X%02X%02Xh", ReadByte[7], ReadByte[6], ReadByte[5], ReadByte[4], ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
							}
						}else{
							if(myOptions.RipSize == 1){
								wsprintfA((LPSTR)tempBuffer, ",%03Xh\r\n", ReadByte[0]);
							}else if(myOptions.RipSize == 2){
								wsprintfA((LPSTR)tempBuffer, ",%03X%02Xh\r\n", ReadByte[1], ReadByte[0]);
							}else if(myOptions.RipSize == 4){
								wsprintfA((LPSTR)tempBuffer, ",%03X%02X%02X%02Xh\r\n", ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
							}else if(myOptions.RipSize == 8){
								wsprintfA((LPSTR)tempBuffer, ",%03X%02X%02X%02X%02X%02X%02X%02Xh\r\n", ReadByte[7], ReadByte[6], ReadByte[5], ReadByte[4], ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
							}
							j = -1;
						}
					}else if(myOptions.ExportType == DE_C){
						if(j == NULL && OnlyFirst == false){
							if(myOptions.RipSize == 1){
								wsprintfA((LPSTR)tempBuffer, " BYTE %s[%i] = { 0x%03X", myOptions.szTableName, ((int)DataSize / myOptions.RipSize), ReadByte[0]);
							}else if(myOptions.RipSize == 2){
								wsprintfA((LPSTR)tempBuffer, " WORD %s[%i] = { 0x%03X%02X", myOptions.szTableName, ((int)DataSize / myOptions.RipSize), ReadByte[1], ReadByte[0]);
							}else if(myOptions.RipSize == 4){
								wsprintfA((LPSTR)tempBuffer, " DWORD %s[%i] = { 0x%03X%02X%02X%02X", myOptions.szTableName, ((int)DataSize / myOptions.RipSize), ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
							}else if(myOptions.RipSize == 8){
								wsprintfA((LPSTR)tempBuffer, " DWORD64 %s[%i] = { 0x%03X%02X%02X%02X%02X%02X%02X%02X", myOptions.szTableName, ((int)DataSize / myOptions.RipSize), ReadByte[7], ReadByte[6], ReadByte[5], ReadByte[4], ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
							}
							OnlyFirst = true;
						}else if(j == NULL && OnlyFirst == true){
							if(myOptions.RipSize == 1){
								wsprintfA((LPSTR)tempBuffer, " 0x%03X", ReadByte[0]);
							}else if(myOptions.RipSize == 2){
								wsprintfA((LPSTR)tempBuffer, " 0x%03X%02X", ReadByte[1], ReadByte[0]);
							}else if(myOptions.RipSize == 4){
								wsprintfA((LPSTR)tempBuffer, " 0x%03X%02X%02X%02X", ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
							}else if(myOptions.RipSize == 8){
								wsprintfA((LPSTR)tempBuffer, " 0x%03X%02X%02X%02X%02X%02X%02X%02X", ReadByte[7], ReadByte[6], ReadByte[5], ReadByte[4], ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
							}
						}else if(j != myOptions.BytesPerLine){
							if(myOptions.RipSize == 1){
								wsprintfA((LPSTR)tempBuffer, ",0x%03X", ReadByte[0]);
							}else if(myOptions.RipSize == 2){
								wsprintfA((LPSTR)tempBuffer, ",0x%03X%02X", ReadByte[1], ReadByte[0]);
							}else if(myOptions.RipSize == 4){
								wsprintfA((LPSTR)tempBuffer, ",0x%03X%02X%02X%02X", ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
							}else if(myOptions.RipSize == 8){
								wsprintfA((LPSTR)tempBuffer, ",0x%03X%02X%02X%02X%02X%02X%02X%02X", ReadByte[7], ReadByte[6], ReadByte[5], ReadByte[4], ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
							}
						}else{
							if(i != ((int)DataSize / myOptions.RipSize) - 1){
								if(myOptions.RipSize == 1){
									wsprintfA((LPSTR)tempBuffer, ",0x%03X,\r\n", ReadByte[0]);
								}else if(myOptions.RipSize == 2){
									wsprintfA((LPSTR)tempBuffer, ",0x%03X%02X,\r\n", ReadByte[1], ReadByte[0]);
								}else if(myOptions.RipSize == 4){
									wsprintfA((LPSTR)tempBuffer, ",0x%03X%02X%02X%02X,\r\n", ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
								}else if(myOptions.RipSize == 8){
									wsprintfA((LPSTR)tempBuffer, ",0x%03X%02X%02X%02X%02X%02X%02X%02X,\r\n", ReadByte[7], ReadByte[6], ReadByte[5], ReadByte[4], ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
								}
							}else{
								if(myOptions.RipSize == 1){
									wsprintfA((LPSTR)tempBuffer, ",0x%03X};\r\n", ReadByte[0]);
								}else if(myOptions.RipSize == 2){
									wsprintfA((LPSTR)tempBuffer, ",0x%03X%02X};\r\n", ReadByte[1], ReadByte[0]);
								}else if(myOptions.RipSize == 4){
									wsprintfA((LPSTR)tempBuffer, ",0x%03X%02X%02X%02X};\r\n", ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
								}else if(myOptions.RipSize == 8){
									wsprintfA((LPSTR)tempBuffer, ",0x%03X%02X%02X%02X%02X%02X%02X%02X};\r\n", ReadByte[7], ReadByte[6], ReadByte[5], ReadByte[4], ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
								}
							}
							j = -1;
						}
						if(i == ((int)DataSize / myOptions.RipSize) - 1){
							lstrcatA((LPSTR)tempBuffer, "};");
						}
					}else if(myOptions.ExportType == DE_DELPHI){
						if(myOptions.RipSize == 1){
							wsprintfA((LPSTR)tempBuffer, "%s[%i] = $%03X;\r\n", myOptions.szTableName, i + myOptions.TableStart, ReadByte[0]);
						}else if(myOptions.RipSize == 2){
							wsprintfA((LPSTR)tempBuffer, "%s[%i] = $%02X%02X;\r\n", myOptions.szTableName, i + myOptions.TableStart, ReadByte[1], ReadByte[0]);					
						}else if(myOptions.RipSize == 4){
							wsprintfA((LPSTR)tempBuffer, "%s[%i] = $%02X%02X%02X%02X;\r\n", myOptions.szTableName, i + myOptions.TableStart, ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);					
						}
						j = -1;
					}else if(myOptions.ExportType == DE_VB){
						if(myOptions.RipSize == 1){
							wsprintfA((LPSTR)tempBuffer, "%s(%i) = &H%03X\r\n", myOptions.szTableName, i + myOptions.TableStart, ReadByte[0]);
						}else if(myOptions.RipSize == 2){
							wsprintfA((LPSTR)tempBuffer, "%s(%i) = &H%02X%02X\r\n", myOptions.szTableName, i + myOptions.TableStart, ReadByte[1], ReadByte[0]);
						}else if(myOptions.RipSize == 4){
							wsprintfA((LPSTR)tempBuffer, "%s(%i) = &H%02X%02X%02X%02X\r\n", myOptions.szTableName, i + myOptions.TableStart, ReadByte[3], ReadByte[2], ReadByte[1], ReadByte[0]);
						}
						j = -1;
					}
					if(j == -1 || i == ((int)DataSize / myOptions.RipSize) - 1){
						lstrcatA((LPSTR)memBuffer, (LPCSTR)tempBuffer);
						WriteFile(hFile, (LPCVOID)memBuffer, lstrlenA((LPCSTR)memBuffer), &dwNumOfBytes, NULL);
						RtlZeroMemory(memBuffer, 0x1000);
					}else{
						lstrcatA((LPSTR)memBuffer, (LPCSTR)tempBuffer);
					}
					DataBuffer = (LPVOID)((ULONG_PTR)DataBuffer + myOptions.RipSize);
					j++;
				}
				myOptions.BytesPerLine++;
				if(cpMemory != NULL){
					VirtualFree(cpMemory, NULL, MEM_RELEASE);
				}
				CloseHandle(hFile);
				VirtualFree(memBuffer, NULL, MEM_RELEASE);
				VirtualFree(tempBuffer, NULL, MEM_RELEASE);
				return(true);
			}else{
				VirtualFree(memBuffer, NULL, MEM_RELEASE);
				VirtualFree(tempBuffer, NULL, MEM_RELEASE);
				return(false);
			}
		}
	}
	VirtualFree(memBuffer, NULL, MEM_RELEASE);
	VirtualFree(tempBuffer, NULL, MEM_RELEASE);
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
		lstrcpyA(szPluginName, "DataExtractor");
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
__declspec(dllexport) bool __stdcall DataExtractorRipData(LPVOID DataBuffer, DWORD DataSize, int ExportType, int RipSize, int BytesPerLine, int TableStart, char* szTableName, char* szSaveFileName){

	RtlZeroMemory(&uniFileName[0], sizeof uniFileName);
	RtlZeroMemory(&myOptions, sizeof PluginOptions);
	myOptions.EmitVersionInfo = true;
	myOptions.ExportType = ExportType;
	myOptions.RipSize = RipSize;
	myOptions.BytesPerLine = BytesPerLine;
	myOptions.TableStart = TableStart;
	myOptions.szTableName = szTableName;
	myOptions.szSaveToFile = &uniFileName[0];
	MultiByteToWideChar(CP_ACP, NULL, szSaveFileName, lstrlenA(szSaveFileName) + 1, uniFileName, sizeof(uniFileName)/(sizeof(uniFileName[0])));
	return(deGrabData(DE_PLUGIN_REASON_CODE_ACTION, NULL, DE_PLUGIN_DATA_TYPE_MEMORY_PTR, DataBuffer, DataSize));
}
__declspec(dllexport) bool __stdcall DataExtractorRipDataW(LPVOID DataBuffer, DWORD DataSize, int ExportType, int RipSize, int BytesPerLine, int TableStart, char* szTableName, wchar_t* szSaveFileName){

	RtlZeroMemory(&myOptions, sizeof PluginOptions);
	myOptions.EmitVersionInfo = true;
	myOptions.ExportType = ExportType;
	myOptions.RipSize = RipSize;
	myOptions.BytesPerLine = BytesPerLine;
	myOptions.TableStart = TableStart;
	myOptions.szTableName = szTableName;
	myOptions.szSaveToFile = szSaveFileName;
	return(deGrabData(DE_PLUGIN_REASON_CODE_ACTION, NULL, DE_PLUGIN_DATA_TYPE_MEMORY_PTR, DataBuffer, DataSize));
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