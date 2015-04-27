#ifndef TITANIUMHOOKS
#define TITANIUMHOOKS

#ifdef _MSC_VER
	#pragma once
#endif

// Engine.Libs:
#define TitanEngineSubFolderSDK  // Comment out this line to have SDK in default folder!

#ifdef TitanEngineSubFolderSDK
	#if defined(_WIN64)
		#pragma comment(lib, "sdk\\TitaniumHooks_x64.lib")
	#else
		#pragma comment(lib, "sdk\\TitaniumHooks_x86.lib")
	#endif
#else
	#if defined(_WIN64)
		#pragma comment(lib, "TitaniumHooks_x64.lib")
	#else
		#pragma comment(lib, "TitaniumHooks_x86.lib")
	#endif
#endif

#define TEE_HOOK_NRM_JUMP 1
#define TEE_HOOK_NRM_CALL 2
#define TEE_HOOK_IAT 3

// Global.Engine.Hook.functions:
__declspec(dllexport) bool __stdcall HooksSafeTransitionEx(LPVOID HookAddressArray, int NumberOfHooks, bool TransitionStart);
__declspec(dllexport) bool __stdcall HooksSafeTransition(LPVOID HookAddress, bool TransitionStart);
__declspec(dllexport) bool __stdcall HooksIsAddressRedirected(LPVOID HookAddress);
__declspec(dllexport) void* __stdcall HooksGetTrampolineAddress(LPVOID HookAddress);
__declspec(dllexport) void* __stdcall HooksGetHookEntryDetails(LPVOID HookAddress);
__declspec(dllexport) bool __stdcall HooksInsertNewRedirection(LPVOID HookAddress, LPVOID RedirectTo, int HookType);
__declspec(dllexport) bool __stdcall HooksInsertNewIATRedirectionEx(ULONG_PTR FileMapVA, ULONG_PTR LoadedModuleBase, char* szHookFunction, LPVOID RedirectTo);
__declspec(dllexport) bool __stdcall HooksInsertNewIATRedirection(char* szModuleName, char* szHookFunction, LPVOID RedirectTo);
__declspec(dllexport) bool __stdcall HooksRemoveRedirection(LPVOID HookAddress, bool RemoveAll);
__declspec(dllexport) bool __stdcall HooksRemoveRedirectionsForModule(HMODULE ModuleBase);
__declspec(dllexport) bool __stdcall HooksRemoveIATRedirection(char* szModuleName, char* szHookFunction, bool RemoveAll);
__declspec(dllexport) bool __stdcall HooksDisableRedirection(LPVOID HookAddress, bool DisableAll);
__declspec(dllexport) bool __stdcall HooksDisableRedirectionsForModule(HMODULE ModuleBase);
__declspec(dllexport) bool __stdcall HooksDisableIATRedirection(char* szModuleName, char* szHookFunction, bool DisableAll);
__declspec(dllexport) bool __stdcall HooksEnableRedirection(LPVOID HookAddress, bool EnableAll);
__declspec(dllexport) bool __stdcall HooksEnableRedirectionsForModule(HMODULE ModuleBase);
__declspec(dllexport) bool __stdcall HooksEnableIATRedirection(char* szModuleName, char* szHookFunction, bool EnableAll);
__declspec(dllexport) void __stdcall HooksScanModuleMemory(HMODULE ModuleBase, LPVOID CallBack);
__declspec(dllexport) void __stdcall HooksScanEntireProcessMemory(LPVOID CallBack);
__declspec(dllexport) void __stdcall HooksScanEntireProcessMemoryEx();

#endif /*TITANIUMHOOKS*/