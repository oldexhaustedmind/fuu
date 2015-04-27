// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>


// TODO: reference additional headers your program requires here

#if !defined TEE_HOOK_NRM_JUMP
	#define TEE_HOOK_NRM_JUMP 1
	#define TEE_HOOK_NRM_CALL 3
#endif

// Global.functions:
__declspec(dllexport) void __stdcall TitanResetPlugin();
__declspec(dllexport) void __stdcall TitanReleasePlugin();