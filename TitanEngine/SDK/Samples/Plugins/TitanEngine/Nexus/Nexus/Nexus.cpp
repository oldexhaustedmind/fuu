// Nexus.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "sdk\sdk.h"

#define BUILD_FILE_FIXING


// Global.constants:
#define PLUGIN_MAJOR_VERSION 1
#define PLUGIN_MINOR_VERSION 0

// Global.variables:
void* hookInitDebug;
void* hookInitDebugW;
void* hookInitDebugEx;
void* hookInitDebugExW;
void* hookInitDLLDebug;
void* hookInitDLLDebugW;
void* hookAutoDebugEx;
void* hookAutoDebugExW;
#if defined BUILD_FILE_FIXING
	void* hookDumpProcessW;
	void* hookIsPE32FileValidExW;
#endif
bool NexusDoProcHooking = true;
HMODULE uePluginBase;

FILE_STATUS_INFO myFileStatus = {};

#if defined BUILD_FILE_FIXING
FILE_FIX_INFO myFileFixStatus = {};

bool BackupFile(char* szFileName){

	char szBackupFile[MAX_PATH] = {};

	lstrcpyA(szBackupFile, szFileName);
	lstrcatA(szBackupFile, ".bak");
	CopyFileA(szFileName, szBackupFile, FALSE);
	return(true);
}
bool BackupFileW(wchar_t* szFileName){

	wchar_t szBackupFile[MAX_PATH] = {};

	lstrcpyW(szBackupFile, szFileName);
	lstrcatW(szBackupFile, L".bak");
	CopyFileW(szFileName, szBackupFile, FALSE);
	return(true);
}
#endif
void EngineExtractPath(char* szFileName, char* StringStorage, int StringSize){

	RtlZeroMemory(StringStorage, StringSize);
	lstrcpyA(StringStorage, szFileName);
	int i = lstrlenA(StringStorage);
	while(i >= 0 && StringStorage[i] != 0x5C){
		StringStorage[i] = 0x00;
		i--;
	}
	if(i < 0){
		GetCurrentDirectoryA(StringSize, StringStorage);
	}
}
void EngineExtractPathW(wchar_t* szFileName, wchar_t* StringStorage, int StringSize){

	RtlZeroMemory(StringStorage, StringSize);
	lstrcpyW(StringStorage, szFileName);
	int i = lstrlenW(StringStorage);
	while(i >= 0 && StringStorage[i] != 0x5C){
		StringStorage[i] = 0x00;
		i--;
	}
	if(i < 0){
		GetCurrentDirectoryW(StringSize, StringStorage);
	}
}
bool NexusInitialize(){

	HMODULE hTitanEngine = GetModuleHandleA("TitanEngine.dll");

	if(hTitanEngine != NULL){	
		hookInitDebug = (void*)GetProcAddress(hTitanEngine, "InitDebug");
		hookInitDebugW = (void*)GetProcAddress(hTitanEngine, "InitDebugW");
		hookInitDebugEx = (void*)GetProcAddress(hTitanEngine, "InitDebugEx");
		hookInitDebugExW = (void*)GetProcAddress(hTitanEngine, "InitDebugExW");
		hookInitDLLDebug = (void*)GetProcAddress(hTitanEngine, "InitDLLDebug");
		hookInitDLLDebugW = (void*)GetProcAddress(hTitanEngine, "InitDLLDebugW");
		hookAutoDebugEx = (void*)GetProcAddress(hTitanEngine, "AutoDebugEx");
		hookAutoDebugExW = (void*)GetProcAddress(hTitanEngine, "AutoDebugExW");
#if defined BUILD_FILE_FIXING
		hookDumpProcessW = (void*)GetProcAddress(hTitanEngine, "DumpProcessW");
		hookIsPE32FileValidExW = (void*)GetProcAddress(hTitanEngine, "IsPE32FileValidExW");
#endif
		return(true);
	}else{
		TitanReleasePlugin();
		return(false);
	}
}
void NexusDebugInitialize(){
	NexusDoProcHooking = true;
}
/*

	Hooked functions

*/
#if defined BUILD_FILE_FIXING
bool __stdcall hookedIsPE32FileValidExW(wchar_t* szFileName, DWORD CheckDepth, LPVOID FileStatusInfo){

	typedef bool(__stdcall *fIsPE32FileValidExW)(wchar_t* szFileName, DWORD CheckDepth, LPVOID FileStatusInfo);
	fIsPE32FileValidExW cIsPE32FileValidExW = (fIsPE32FileValidExW)HooksGetTrampolineAddress(hookIsPE32FileValidExW);
	PFILE_STATUS_INFO checkFileStatus = (PFILE_STATUS_INFO)FileStatusInfo;

	bool fReturn = cIsPE32FileValidExW(szFileName, CheckDepth, FileStatusInfo);
	if(checkFileStatus->OveralEvaluation == UE_RESULT_FILE_INVALID_BUT_FIXABLE){
		return(true);
	}else if(fReturn){
		return(true);
	}else{
		return(false);
	}
}
bool __stdcall hookedDumpProcessW(HANDLE hProcess, LPVOID ImageBase, wchar_t* szDumpFileName, ULONG_PTR EntryPoint){

	typedef bool(__stdcall *fDumpProcessW)(HANDLE hProcess, LPVOID ImageBase, wchar_t* szDumpFileName, ULONG_PTR EntryPoint);
	fDumpProcessW cDumpProcessW = (fDumpProcessW)HooksGetTrampolineAddress(hookDumpProcessW);

	bool fReturn = cDumpProcessW(hProcess, ImageBase, szDumpFileName, EntryPoint);
	if(myFileFixStatus.FileFixPerformed){
		FixBrokenPE32FileExW(szDumpFileName, &myFileStatus, &myFileFixStatus);
	}
	if(fReturn){
		return(true);
	}else{
		return(false);
	}
}
#endif
void* __stdcall hookedInitDebug(char* szFileName, char* szCommandLine, char* szCurrentFolder){

#if defined BUILD_FILE_FIXING
	typedef bool(__stdcall *fIsPE32FileValidExW)(wchar_t* szFileName, DWORD CheckDepth, LPVOID FileStatusInfo);
	fIsPE32FileValidExW cIsPE32FileValidExW = (fIsPE32FileValidExW)HooksGetTrampolineAddress(hookIsPE32FileValidExW);
#endif
	typedef void*(__stdcall *fInitDebug)(char* szFileName, char* szCommandLine, char* szCurrentFolder);
	fInitDebug cInitDebug = (fInitDebug)HooksGetTrampolineAddress(hookInitDebug);
#if defined BUILD_FILE_FIXING
	wchar_t uniFileName[MAX_PATH] = {};
#endif
	char szFolderPath[MAX_PATH] = {};

	NexusDebugInitialize();
	RtlZeroMemory(&myFileStatus, sizeof FILE_STATUS_INFO);
#if defined BUILD_FILE_FIXING
	MultiByteToWideChar(CP_ACP, NULL, szFileName, lstrlenA(szFileName)+1, uniFileName, sizeof(uniFileName)/(sizeof(uniFileName[0])));
	if(cIsPE32FileValidExW(uniFileName, UE_DEPTH_DEEP, &myFileStatus)){
#else
	if(IsPE32FileValidEx(szFileName, UE_DEPTH_DEEP, &myFileStatus)){	
#endif
		if(myFileStatus.MissingDependencies){
			EngineExtractPath(szFileName, szFolderPath, sizeof szFolderPath);
			EngineCreateMissingDependencies(szFileName, szFolderPath, true);
		}
		return(cInitDebug(szFileName, szCommandLine, szCurrentFolder));
	}else{
#if defined BUILD_FILE_FIXING
		if(myFileStatus.OveralEvaluation == UE_RESULT_FILE_INVALID_BUT_FIXABLE){
			if(BackupFile(szFileName) && FixBrokenPE32FileEx(szFileName, &myFileStatus, &myFileFixStatus) && myFileFixStatus.FileFixPerformed){
				if(myFileStatus.MissingDependencies){
					EngineExtractPath(szFileName, szFolderPath, sizeof szFolderPath);
					EngineCreateMissingDependencies(szFileName, szFolderPath, true);
				}
				return(cInitDebug(szFileName, szCommandLine, szCurrentFolder));
			}
		}
#endif
		return(NULL);
	}
}
void* __stdcall hookedInitDebugW(wchar_t* szFileName, wchar_t* szCommandLine, wchar_t* szCurrentFolder){

#if defined BUILD_FILE_FIXING
	typedef bool(__stdcall *fIsPE32FileValidExW)(wchar_t* szFileName, DWORD CheckDepth, LPVOID FileStatusInfo);
	fIsPE32FileValidExW cIsPE32FileValidExW = (fIsPE32FileValidExW)HooksGetTrampolineAddress(hookIsPE32FileValidExW);
#endif
	typedef void*(__stdcall *fInitDebugW)(wchar_t* szFileName, wchar_t* szCommandLine, wchar_t* szCurrentFolder);
	fInitDebugW cInitDebugW = (fInitDebugW)HooksGetTrampolineAddress(hookInitDebugW);
	wchar_t szFolderPath[MAX_PATH] = {};

	NexusDebugInitialize();
	RtlZeroMemory(&myFileStatus, sizeof FILE_STATUS_INFO);
#if defined BUILD_FILE_FIXING
	if(cIsPE32FileValidExW(szFileName, UE_DEPTH_DEEP, &myFileStatus)){
#else
	if(IsPE32FileValidExW(szFileName, UE_DEPTH_DEEP, &myFileStatus)){
#endif
		if(myFileStatus.MissingDependencies){
			EngineExtractPathW(szFileName, szFolderPath, sizeof szFolderPath);
			EngineCreateMissingDependenciesW(szFileName, szFolderPath, true);
		}
		return(cInitDebugW(szFileName, szCommandLine, szCurrentFolder));
	}else{
#if defined BUILD_FILE_FIXING
		if(myFileStatus.OveralEvaluation == UE_RESULT_FILE_INVALID_BUT_FIXABLE){
			if(BackupFileW(szFileName) && FixBrokenPE32FileExW(szFileName, &myFileStatus, &myFileFixStatus) && myFileFixStatus.FileFixPerformed){
				if(myFileStatus.MissingDependencies){
					EngineExtractPathW(szFileName, szFolderPath, sizeof szFolderPath);
					EngineCreateMissingDependenciesW(szFileName, szFolderPath, true);
				}
				return(cInitDebugW(szFileName, szCommandLine, szCurrentFolder));
			}
		}
#endif
		return(NULL);
	}
}
void* __stdcall hookedInitDebugEx(char* szFileName, char* szCommandLine, char* szCurrentFolder, LPVOID EntryCallBack){

#if defined BUILD_FILE_FIXING
	typedef bool(__stdcall *fIsPE32FileValidExW)(wchar_t* szFileName, DWORD CheckDepth, LPVOID FileStatusInfo);
	fIsPE32FileValidExW cIsPE32FileValidExW = (fIsPE32FileValidExW)HooksGetTrampolineAddress(hookIsPE32FileValidExW);
#endif
	typedef void*(__stdcall *fInitDebugEx)(char* szFileName, char* szCommandLine, char* szCurrentFolder, LPVOID EntryCallBack);
	fInitDebugEx cInitDebugEx = (fInitDebugEx)HooksGetTrampolineAddress(hookInitDebugEx);
#if defined BUILD_FILE_FIXING
	wchar_t uniFileName[MAX_PATH] = {};
#endif
	char szFolderPath[MAX_PATH] = {};

	NexusDebugInitialize();
	RtlZeroMemory(&myFileStatus, sizeof FILE_STATUS_INFO);
#if defined BUILD_FILE_FIXING
	MultiByteToWideChar(CP_ACP, NULL, szFileName, lstrlenA(szFileName)+1, uniFileName, sizeof(uniFileName)/(sizeof(uniFileName[0])));
	if(cIsPE32FileValidExW(uniFileName, UE_DEPTH_DEEP, &myFileStatus)){
#else
	if(IsPE32FileValidEx(szFileName, UE_DEPTH_DEEP, &myFileStatus)){
#endif
		if(myFileStatus.MissingDependencies){
			EngineExtractPath(szFileName, szFolderPath, sizeof szFolderPath);
			EngineCreateMissingDependencies(szFileName, szFolderPath, true);
		}
		return(cInitDebugEx(szFileName, szCommandLine, szCurrentFolder, EntryCallBack));
	}else{
#if defined BUILD_FILE_FIXING
		if(myFileStatus.OveralEvaluation == UE_RESULT_FILE_INVALID_BUT_FIXABLE){
			if(BackupFile(szFileName) && FixBrokenPE32FileEx(szFileName, &myFileStatus, &myFileFixStatus) && myFileFixStatus.FileFixPerformed){
				if(myFileStatus.MissingDependencies){
					EngineExtractPath(szFileName, szFolderPath, sizeof szFolderPath);
					EngineCreateMissingDependencies(szFileName, szFolderPath, true);
				}
				return(cInitDebugEx(szFileName, szCommandLine, szCurrentFolder, EntryCallBack));
			}
		}
#endif
		return(NULL);
	}
}
void* __stdcall hookedInitDebugExW(wchar_t* szFileName, wchar_t* szCommandLine, wchar_t* szCurrentFolder, LPVOID EntryCallBack){

#if defined BUILD_FILE_FIXING
	typedef bool(__stdcall *fIsPE32FileValidExW)(wchar_t* szFileName, DWORD CheckDepth, LPVOID FileStatusInfo);
	fIsPE32FileValidExW cIsPE32FileValidExW = (fIsPE32FileValidExW)HooksGetTrampolineAddress(hookIsPE32FileValidExW);
#endif
	typedef void*(__stdcall *fInitDebugExW)(wchar_t* szFileName, wchar_t* szCommandLine, wchar_t* szCurrentFolder, LPVOID EntryCallBack);
	fInitDebugExW cInitDebugExW = (fInitDebugExW)HooksGetTrampolineAddress(hookInitDebugExW);
	wchar_t szFolderPath[MAX_PATH] = {};

	NexusDebugInitialize();
	RtlZeroMemory(&myFileStatus, sizeof FILE_STATUS_INFO);
#if defined BUILD_FILE_FIXING
	if(cIsPE32FileValidExW(szFileName, UE_DEPTH_DEEP, &myFileStatus)){
#else
	if(IsPE32FileValidExW(szFileName, UE_DEPTH_DEEP, &myFileStatus)){
#endif
		if(myFileStatus.MissingDependencies){
			EngineExtractPathW(szFileName, szFolderPath, sizeof szFolderPath);
			EngineCreateMissingDependenciesW(szFileName, szFolderPath, true);
		}
		return(cInitDebugExW(szFileName, szCommandLine, szCurrentFolder, EntryCallBack));
	}else{
#if defined BUILD_FILE_FIXING		
		if(myFileStatus.OveralEvaluation == UE_RESULT_FILE_INVALID_BUT_FIXABLE){
			if(BackupFileW(szFileName) && FixBrokenPE32FileExW(szFileName, &myFileStatus, &myFileFixStatus) && myFileFixStatus.FileFixPerformed){
				if(myFileStatus.MissingDependencies){
					EngineExtractPathW(szFileName, szFolderPath, sizeof szFolderPath);
					EngineCreateMissingDependenciesW(szFileName, szFolderPath, true);
				}
				return(cInitDebugExW(szFileName, szCommandLine, szCurrentFolder, EntryCallBack));
			}
		}
#endif
		return(NULL);
	}
}
void* __stdcall hookedInitDLLDebug(char* szFileName, bool ReserveModuleBase, char* szCommandLine, char* szCurrentFolder, LPVOID EntryCallBack){

#if defined BUILD_FILE_FIXING	
	typedef bool(__stdcall *fIsPE32FileValidExW)(wchar_t* szFileName, DWORD CheckDepth, LPVOID FileStatusInfo);
	fIsPE32FileValidExW cIsPE32FileValidExW = (fIsPE32FileValidExW)HooksGetTrampolineAddress(hookIsPE32FileValidExW);
#endif
	typedef void*(__stdcall *fInitDLLDebug)(char* szFileName, bool ReserveModuleBase, char* szCommandLine, char* szCurrentFolder, LPVOID EntryCallBack);
	fInitDLLDebug cInitDLLDebug = (fInitDLLDebug)HooksGetTrampolineAddress(hookInitDLLDebug);
#if defined BUILD_FILE_FIXING
	wchar_t uniFileName[MAX_PATH] = {};
#endif
	char szFolderPath[MAX_PATH] = {};

	NexusDebugInitialize();
	RtlZeroMemory(&myFileStatus, sizeof FILE_STATUS_INFO);
#if defined BUILD_FILE_FIXING
	MultiByteToWideChar(CP_ACP, NULL, szFileName, lstrlenA(szFileName)+1, uniFileName, sizeof(uniFileName)/(sizeof(uniFileName[0])));
	if(cIsPE32FileValidExW(uniFileName, UE_DEPTH_DEEP, &myFileStatus)){
#else
	if(IsPE32FileValidEx(szFileName, UE_DEPTH_DEEP, &myFileStatus)){
#endif
		if(myFileStatus.MissingDependencies){
			EngineExtractPath(szFileName, szFolderPath, sizeof szFolderPath);
			EngineCreateMissingDependencies(szFileName, szFolderPath, true);
		}
		return(cInitDLLDebug(szFileName, ReserveModuleBase, szCommandLine, szCurrentFolder, EntryCallBack));
	}else{
#if defined BUILD_FILE_FIXING
		if(myFileStatus.OveralEvaluation == UE_RESULT_FILE_INVALID_BUT_FIXABLE){
			if(BackupFile(szFileName) && FixBrokenPE32FileEx(szFileName, &myFileStatus, &myFileFixStatus) && myFileFixStatus.FileFixPerformed){
				if(myFileStatus.MissingDependencies){
					EngineExtractPath(szFileName, szFolderPath, sizeof szFolderPath);
					EngineCreateMissingDependencies(szFileName, szFolderPath, true);
				}
				return(cInitDLLDebug(szFileName, ReserveModuleBase, szCommandLine, szCurrentFolder, EntryCallBack));
			}
		}
#endif
		return(NULL);
	}
}
void* __stdcall hookedInitDLLDebugW(wchar_t* szFileName, bool ReserveModuleBase, wchar_t* szCommandLine, wchar_t* szCurrentFolder, LPVOID EntryCallBack){

#if defined BUILD_FILE_FIXING
	typedef bool(__stdcall *fIsPE32FileValidExW)(wchar_t* szFileName, DWORD CheckDepth, LPVOID FileStatusInfo);
	fIsPE32FileValidExW cIsPE32FileValidExW = (fIsPE32FileValidExW)HooksGetTrampolineAddress(hookIsPE32FileValidExW);
#endif
	typedef void*(__stdcall *fInitDLLDebugW)(wchar_t* szFileName, bool ReserveModuleBase, wchar_t* szCommandLine, wchar_t* szCurrentFolder, LPVOID EntryCallBack);
	fInitDLLDebugW cInitDLLDebugW = (fInitDLLDebugW)HooksGetTrampolineAddress(hookInitDLLDebugW);
	wchar_t szFolderPath[MAX_PATH] = {};

	NexusDebugInitialize();
	RtlZeroMemory(&myFileStatus, sizeof FILE_STATUS_INFO);
#if defined BUILD_FILE_FIXING
	if(cIsPE32FileValidExW(szFileName, UE_DEPTH_DEEP, &myFileStatus)){
#else
	if(IsPE32FileValidExW(szFileName, UE_DEPTH_DEEP, &myFileStatus)){
#endif
		if(myFileStatus.MissingDependencies){
			EngineExtractPathW(szFileName, szFolderPath, sizeof szFolderPath);
			EngineCreateMissingDependenciesW(szFileName, szFolderPath, true);
		}
		return(cInitDLLDebugW(szFileName, ReserveModuleBase, szCommandLine, szCurrentFolder, EntryCallBack));
	}else{
#if defined BUILD_FILE_FIXING
		if(myFileStatus.OveralEvaluation == UE_RESULT_FILE_INVALID_BUT_FIXABLE){
			if(BackupFileW(szFileName) && FixBrokenPE32FileExW(szFileName, &myFileStatus, &myFileFixStatus) && myFileFixStatus.FileFixPerformed){
				if(myFileStatus.MissingDependencies){
					EngineExtractPathW(szFileName, szFolderPath, sizeof szFolderPath);
					EngineCreateMissingDependenciesW(szFileName, szFolderPath, true);
				}
				return(cInitDLLDebugW(szFileName, ReserveModuleBase, szCommandLine, szCurrentFolder, EntryCallBack));
			}
		}
#endif
		return(NULL);
	}
}
void __stdcall hookedAutoDebugEx(char* szFileName, bool ReserveModuleBase, char* szCommandLine, char* szCurrentFolder, DWORD TimeOut, LPVOID EntryCallBack){

#if defined BUILD_FILE_FIXING
	typedef bool(__stdcall *fIsPE32FileValidExW)(wchar_t* szFileName, DWORD CheckDepth, LPVOID FileStatusInfo);
	fIsPE32FileValidExW cIsPE32FileValidExW = (fIsPE32FileValidExW)HooksGetTrampolineAddress(hookIsPE32FileValidExW);
#endif
	typedef void(__stdcall *fAutoDebugEx)(char* szFileName, bool ReserveModuleBase, char* szCommandLine, char* szCurrentFolder, DWORD TimeOut, LPVOID EntryCallBack);
	fAutoDebugEx cAutoDebugEx = (fAutoDebugEx)HooksGetTrampolineAddress(hookAutoDebugEx);
#if defined BUILD_FILE_FIXING
	wchar_t uniFileName[MAX_PATH] = {};
#endif
	char szFolderPath[MAX_PATH] = {};

	NexusDebugInitialize();
	RtlZeroMemory(&myFileStatus, sizeof FILE_STATUS_INFO);
#if defined BUILD_FILE_FIXING
	MultiByteToWideChar(CP_ACP, NULL, szFileName, lstrlenA(szFileName)+1, uniFileName, sizeof(uniFileName)/(sizeof(uniFileName[0])));
	if(cIsPE32FileValidExW(uniFileName, UE_DEPTH_DEEP, &myFileStatus)){
#else
	if(IsPE32FileValidEx(szFileName, UE_DEPTH_DEEP, &myFileStatus)){
#endif
		if(myFileStatus.MissingDependencies){
			EngineExtractPath(szFileName, szFolderPath, sizeof szFolderPath);
			EngineCreateMissingDependencies(szFileName, szFolderPath, true);
		}
		cAutoDebugEx(szFileName, ReserveModuleBase, szCommandLine, szCurrentFolder, TimeOut, EntryCallBack);
	}
}
void __stdcall hookedAutoDebugExW(wchar_t* szFileName, bool ReserveModuleBase, wchar_t* szCommandLine, wchar_t* szCurrentFolder, DWORD TimeOut, LPVOID EntryCallBack){

#if defined BUILD_FILE_FIXING
	typedef bool(__stdcall *fIsPE32FileValidExW)(wchar_t* szFileName, DWORD CheckDepth, LPVOID FileStatusInfo);
	fIsPE32FileValidExW cIsPE32FileValidExW = (fIsPE32FileValidExW)HooksGetTrampolineAddress(hookIsPE32FileValidExW);
#endif
	typedef void(__stdcall *fAutoDebugExW)(wchar_t* szFileName, bool ReserveModuleBase, wchar_t* szCommandLine, wchar_t* szCurrentFolder, DWORD TimeOut, LPVOID EntryCallBack);
	fAutoDebugExW cAutoDebugExW = (fAutoDebugExW)HooksGetTrampolineAddress(hookAutoDebugExW);
	wchar_t szFolderPath[MAX_PATH] = {};

	NexusDebugInitialize();
	RtlZeroMemory(&myFileStatus, sizeof FILE_STATUS_INFO);
#if defined BUILD_FILE_FIXING
	if(cIsPE32FileValidExW(szFileName, UE_DEPTH_DEEP, &myFileStatus)){
#else
	if(IsPE32FileValidExW(szFileName, UE_DEPTH_DEEP, &myFileStatus)){
#endif
		if(myFileStatus.MissingDependencies){
			EngineExtractPathW(szFileName, szFolderPath, sizeof szFolderPath);
			EngineCreateMissingDependenciesW(szFileName, szFolderPath, true);
		}
		cAutoDebugExW(szFileName, ReserveModuleBase, szCommandLine, szCurrentFolder, TimeOut, EntryCallBack);
	}
}
/*

	TitanEngine callback communication

*/
__declspec(dllexport) void __stdcall TitanDebuggingCallBack(LPDEBUG_EVENT debugEvent, int CallReason){

	if(CallReason == UE_PLUGIN_CALL_REASON_PREDEBUG){
		if(NexusDoProcHooking){
			LPPROCESS_INFORMATION procInfo = (LPPROCESS_INFORMATION)GetProcessInformation();
			if(procInfo->hProcess != NULL){
				EngineFakeMissingDependencies(procInfo->hProcess);
				NexusDoProcHooking = false;
			}
		}
	}else if(CallReason == UE_PLUGIN_CALL_REASON_POSTDEBUG){
		EngineDeleteCreatedDependencies();
		NexusDoProcHooking = true;
	}
}
__declspec(dllexport) bool __stdcall TitanRegisterPlugin(char* szPluginName, LPDWORD titanPluginMajorVersion, LPDWORD titanPluginMinorVersion){

	if(titanPluginMajorVersion != NULL && titanPluginMinorVersion != NULL){
		*titanPluginMajorVersion = PLUGIN_MAJOR_VERSION;
		*titanPluginMinorVersion = PLUGIN_MINOR_VERSION;
		lstrcpyA(szPluginName, "Nexus");
		if(NexusInitialize()){
			TitanResetPlugin();
			return(true);
		}else{
			return(false);
		}
	}else{
		return(false);
	}
}
__declspec(dllexport) void __stdcall TitanResetPlugin(){

	if(!HooksIsAddressRedirected(hookInitDebug)){
#if defined BUILD_FILE_FIXING
		if(!HooksInsertNewRedirection(hookDumpProcessW, &hookedDumpProcessW, TEE_HOOK_NRM_JUMP)){
			TitanReleasePlugin();
		}
#endif
		if(!HooksInsertNewRedirection(hookInitDebug, &hookedInitDebug, TEE_HOOK_NRM_JUMP)){
			TitanReleasePlugin();
		}
		if(!HooksInsertNewRedirection(hookInitDebugW, &hookedInitDebugW, TEE_HOOK_NRM_JUMP)){
			TitanReleasePlugin();
		}
		if(!HooksInsertNewRedirection(hookInitDebugEx, &hookedInitDebugEx, TEE_HOOK_NRM_JUMP)){
			TitanReleasePlugin();
		}
		if(!HooksInsertNewRedirection(hookInitDebugExW, &hookedInitDebugExW, TEE_HOOK_NRM_JUMP)){
			TitanReleasePlugin();
		}
		if(!HooksInsertNewRedirection(hookInitDLLDebug, &hookedInitDLLDebug, TEE_HOOK_NRM_JUMP)){
			TitanReleasePlugin();
		}
		if(!HooksInsertNewRedirection(hookInitDLLDebugW, &hookedInitDLLDebugW, TEE_HOOK_NRM_JUMP)){
			TitanReleasePlugin();
		}
		if(!HooksInsertNewRedirection(hookAutoDebugEx, &hookedAutoDebugEx, TEE_HOOK_NRM_JUMP)){
			TitanReleasePlugin();
		}
		if(!HooksInsertNewRedirection(hookAutoDebugExW, &hookedAutoDebugExW, TEE_HOOK_NRM_JUMP)){
			TitanReleasePlugin();
		}
#if defined BUILD_FILE_FIXING
		if(!HooksInsertNewRedirection(hookIsPE32FileValidExW, &hookedIsPE32FileValidExW, TEE_HOOK_NRM_JUMP)){
			TitanReleasePlugin();
		}
#endif
	}
}
__declspec(dllexport) void __stdcall TitanReleasePlugin(){

	if(!HooksIsAddressRedirected(hookInitDebug)){
		HooksSafeTransition(NULL, true);
		HooksRemoveRedirection(NULL, true);
		HooksSafeTransition(NULL, false);
	}
}
// Global.Plugin.Export:

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