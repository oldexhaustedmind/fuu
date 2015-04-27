// lynxImpRec.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <stdlib.h>
#include "sdk\sdk.h"

// Global.constants:
#define PLUGIN_MAJOR_VERSION 1
#define PLUGIN_MINOR_VERSION 0
#define LYNX_PLUGIN_DATA_TYPE_MEMORY_PTR 0
#define LYNX_PLUGIN_REASON_CODE_ACTION 1
#define LYNX_IMPREC_SAVEFILE 1
#define LYNX_IMPREC_LOADFILE 2

// ThisPlugin.Options.Structure:
typedef struct{
	int ExportType;
	DWORD OEPAddress;
	DWORD IATRva;
	DWORD IATSize;
	char* szIatForFile;
	wchar_t* szSaveToFile;
	wchar_t* szLoadFromFile;
}PluginOptions, *PPluginOptions;

typedef struct{
	BYTE DataByte[50];
}MEMORY_CMP_HANDLER, *PMEMORY_CMP_HANDLER;

// Global.variables:
HANDLE hFile;
HMODULE uePluginBase;
PluginOptions myOptions;
LPVOID memBuffer = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);
wchar_t uniFileName[MAX_PATH] = {};

void lynxImpRecAddDetail(PImportEnumData myImportEnumData){

	DWORD NumberOfBytes = NULL;

	if(myImportEnumData->NewDll){
		wsprintfA((LPSTR)memBuffer, "\r\nFThunk: %08X\tNbFunc: %08X\r\n", (ULONG_PTR)(myImportEnumData->BaseImportThunk - myImportEnumData->ImageBase), myImportEnumData->NumberOfImports);
		WriteFile(hFile, (LPCVOID)memBuffer, lstrlenA((LPCSTR)memBuffer), &NumberOfBytes, NULL);
	}
	wsprintfA((LPSTR)memBuffer, "1\t%08X\t%s\t0000\t%s\r\n", (ULONG_PTR)(myImportEnumData->ImportThunk - myImportEnumData->ImageBase), myImportEnumData->DLLName, myImportEnumData->APIName);
	WriteFile(hFile, (LPCVOID)memBuffer, lstrlenA((LPCSTR)memBuffer), &NumberOfBytes, NULL);
}
bool lynxImpRecAction(int CallReason, int Identifier, int DataType, LPVOID DataBuffer, DWORD DataSize){
	
	int i;
	int j;
	int lineCount;
	DWORD fSize = NULL;
	DWORD NumberOfBytes = NULL;
	PMEMORY_CMP_HANDLER cmpMemory;
	ULONG_PTR ThunkAddress = NULL;
	ULONG_PTR ImageBase = NULL;
	char DLLName[128];
	char APIName[128];
	char LastDllName[256];
	LPVOID fileBuffer;
	char* pEnd;

	if(CallReason == LYNX_PLUGIN_REASON_CODE_ACTION){
		if(Identifier == LYNX_IMPREC_SAVEFILE){
			if(ImporterGetAddedDllCount() > NULL){
				hFile = CreateFileW(myOptions.szSaveToFile, GENERIC_READ+GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if(hFile != INVALID_HANDLE_VALUE){
					wsprintfA((LPSTR)memBuffer, "Target: %s\r\n", myOptions.szIatForFile);
					WriteFile(hFile, (LPCVOID)memBuffer, lstrlenA((LPCSTR)memBuffer), &NumberOfBytes, NULL);
					wsprintfA((LPSTR)memBuffer, "OEP: %08X\tIATRVA: %08X\tIATSize: %08X\r\n", myOptions.OEPAddress, myOptions.IATRva, myOptions.IATSize);
					WriteFile(hFile, (LPCVOID)memBuffer, lstrlenA((LPCSTR)memBuffer), &NumberOfBytes, NULL);
					ImporterEnumAddedData((LPVOID)&lynxImpRecAddDetail);
					CloseHandle(hFile);
				}
			}
		}else if(Identifier == LYNX_IMPREC_LOADFILE){
			if(myOptions.szLoadFromFile != NULL){
				RtlZeroMemory(&LastDllName, 256);
				if(ImporterGetAddedDllCount() > NULL){
					ImporterCleanup();
				}
				ImageBase = (ULONG_PTR)GetPE32Data(myOptions.szIatForFile, NULL, UE_IMAGEBASE);
				if(ImageBase != NULL){
					ImporterInit(30 * 1024, ImageBase);
					hFile = CreateFileW(myOptions.szLoadFromFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if(hFile != INVALID_HANDLE_VALUE){
						__try{
							fSize = GetFileSize(hFile, NULL);
							fileBuffer = VirtualAlloc(NULL, fSize, MEM_COMMIT, PAGE_READWRITE);
							if(ReadFile(hFile, fileBuffer, fSize, &NumberOfBytes, NULL)){
								lineCount = NULL;
								cmpMemory = (PMEMORY_CMP_HANDLER)fileBuffer;
								for(i = 0; i < (int)fSize; i++){
									if(cmpMemory->DataByte[0] == 13 || cmpMemory->DataByte[0] == 10){
										if(cmpMemory->DataByte[0] == 13){
											lineCount++;
										}
										cmpMemory->DataByte[0] = 0x00;
									}
									cmpMemory = (PMEMORY_CMP_HANDLER)((ULONG_PTR)cmpMemory + 1);
								}
								cmpMemory = (PMEMORY_CMP_HANDLER)fileBuffer;
								for(i = 0; i < lineCount; i++){
									if(cmpMemory->DataByte[0] == 0x3B){
										cmpMemory = (PMEMORY_CMP_HANDLER)((ULONG_PTR)cmpMemory + lstrlenA((LPCSTR)cmpMemory));
									}else if(cmpMemory->DataByte[0] == 0x46 && cmpMemory->DataByte[6] == 0x3A){
										cmpMemory = (PMEMORY_CMP_HANDLER)((ULONG_PTR)cmpMemory + lstrlenA((LPCSTR)cmpMemory));
									}else if(cmpMemory->DataByte[0] == 0x31){
										j = NULL;
										ThunkAddress = (ULONG_PTR)strtol((char*)&cmpMemory->DataByte[2], &pEnd, 16) + ImageBase;
										cmpMemory = (PMEMORY_CMP_HANDLER)((ULONG_PTR)cmpMemory + 11);
										while(cmpMemory->DataByte[0] != 0x09){
											DLLName[j] = (char)(cmpMemory->DataByte[0]);
											cmpMemory = (PMEMORY_CMP_HANDLER)((ULONG_PTR)cmpMemory + 1);
											j++;
										}
										DLLName[j] = (char)(0x00);
										j = NULL;
										cmpMemory = (PMEMORY_CMP_HANDLER)((ULONG_PTR)cmpMemory + 6);
										while(cmpMemory->DataByte[0] != 0x00){
											APIName[j] = (char)(cmpMemory->DataByte[0]);
											cmpMemory = (PMEMORY_CMP_HANDLER)((ULONG_PTR)cmpMemory + 1);
											j++;
										}
										APIName[j] = (char)(0x00);
										if(lstrcmpiA((LPCSTR)LastDllName, DLLName) != NULL){
											lstrcpyA((LPSTR)LastDllName, DLLName);
											ImporterAddNewDll((char*)LastDllName, ThunkAddress);
											ImporterAddNewAPI((char*)APIName, ThunkAddress);
										}else{
											ImporterAddNewAPI((char*)APIName, ThunkAddress);
										}
									}else{
										cmpMemory = (PMEMORY_CMP_HANDLER)((ULONG_PTR)cmpMemory + lstrlenA((LPCSTR)cmpMemory));
									}
									if(cmpMemory->DataByte[0] == NULL && cmpMemory->DataByte[1] == NULL){
										cmpMemory = (PMEMORY_CMP_HANDLER)((ULONG_PTR)cmpMemory + 2);
									}else if(cmpMemory->DataByte[0] == NULL){
										cmpMemory = (PMEMORY_CMP_HANDLER)((ULONG_PTR)cmpMemory + 1);
									}
								}
								VirtualFree(fileBuffer, NULL, MEM_RELEASE);
							}else{
								CloseHandle(hFile);
								return(false);
							}
						}__except(EXCEPTION_EXECUTE_HANDLER){
							ImporterCleanup();
							VirtualFree(fileBuffer, NULL, MEM_RELEASE);
							CloseHandle(hFile);
							return(false);
						}
						CloseHandle(hFile);
						return(true);
					}
				}
			}
		}
	}
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
		lstrcpyA(szPluginName, "lynxImpRec");
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
__declspec(dllexport) bool __stdcall lynxImpRecExportTree(char* szOriginalFileName, char* szSaveFileName, DWORD OEPAddressRVA, DWORD IATRva, DWORD IATSize){

	RtlZeroMemory(&uniFileName[0], sizeof uniFileName);
	RtlZeroMemory(&myOptions, sizeof PluginOptions);
	myOptions.OEPAddress = OEPAddressRVA;
	myOptions.IATRva = IATRva;
	myOptions.IATSize = IATSize;
	myOptions.szIatForFile = szOriginalFileName;
	myOptions.szSaveToFile = &uniFileName[0];
	MultiByteToWideChar(CP_ACP, NULL, szSaveFileName, lstrlenA(szSaveFileName) + 1, uniFileName, sizeof(uniFileName)/(sizeof(uniFileName[0])));
	return(lynxImpRecAction(LYNX_PLUGIN_REASON_CODE_ACTION, LYNX_IMPREC_SAVEFILE, NULL, NULL, NULL));
}
__declspec(dllexport) bool __stdcall lynxImpRecExportTreeW(char* szOriginalFileName, wchar_t* szSaveFileName, DWORD OEPAddressRVA, DWORD IATRva, DWORD IATSize){

	RtlZeroMemory(&myOptions, sizeof PluginOptions);
	myOptions.OEPAddress = OEPAddressRVA;
	myOptions.IATRva = IATRva;
	myOptions.IATSize = IATSize;
	myOptions.szIatForFile = szOriginalFileName;
	myOptions.szSaveToFile = szSaveFileName;
	return(lynxImpRecAction(LYNX_PLUGIN_REASON_CODE_ACTION, LYNX_IMPREC_SAVEFILE, NULL, NULL, NULL));
}
__declspec(dllexport) bool __stdcall lynxImpRecLoadTree(char* szOriginalFileName, char* szLoadFileName){

	RtlZeroMemory(&uniFileName[0], sizeof uniFileName);
	RtlZeroMemory(&myOptions, sizeof PluginOptions);
	myOptions.szIatForFile = szOriginalFileName;
	myOptions.szLoadFromFile = &uniFileName[0];
	MultiByteToWideChar(CP_ACP, NULL, szLoadFileName, lstrlenA(szLoadFileName) + 1, uniFileName, sizeof(uniFileName)/(sizeof(uniFileName[0])));
	return(lynxImpRecAction(LYNX_PLUGIN_REASON_CODE_ACTION, LYNX_IMPREC_LOADFILE, NULL, NULL, NULL));
}

__declspec(dllexport) bool __stdcall lynxImpRecLoadTreeW(char* szOriginalFileName, wchar_t* szLoadFileName){

	RtlZeroMemory(&myOptions, sizeof PluginOptions);
	myOptions.szIatForFile = szOriginalFileName;
	myOptions.szLoadFromFile = szLoadFileName;
	return(lynxImpRecAction(LYNX_PLUGIN_REASON_CODE_ACTION, LYNX_IMPREC_LOADFILE, NULL, NULL, NULL));
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