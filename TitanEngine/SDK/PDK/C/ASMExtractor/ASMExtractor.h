#ifndef TITANENGINE_ASMEXTRACTOR
#define TITANENGINE_ASMEXTRACTOR

#if _MSC_VER > 1000
    #pragma once
#endif

#pragma pack(push, 1)

#if defined(_WIN64)
	#pragma comment(lib, "sdk\\ASMExtractor_x64.lib")
#else
	#pragma comment(lib, "sdk\\ASMExtractor_x86.lib")
#endif

#define AE_NORMAL 0
#define AE_WITH_LABELS 1

__declspec(dllimport) bool __stdcall ASMExtractorRipData(LPVOID DataBuffer, DWORD DataSize, int ExportType, int LabelIncrement, char* szLableName, char* szSaveFileName);
__declspec(dllimport) bool __stdcall ASMExtractorRipDataW(LPVOID DataBuffer, DWORD DataSize, int ExportType, int LabelIncrement, char* szLableName, wchar_t* szSaveFileName);

#pragma pack(pop)

#endif /*TITANENGINE_ASMEXTRACTOR*/