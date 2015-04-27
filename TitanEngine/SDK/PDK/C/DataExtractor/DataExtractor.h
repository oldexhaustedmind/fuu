#ifndef TITANENGINE_DATAEXTRACTOR
#define TITANENGINE_DATAEXTRACTOR

#if _MSC_VER > 1000
    #pragma once
#endif

#pragma pack(push, 1)

#if defined(_WIN64)
	#pragma comment(lib, "sdk\\DataExtractor_x64.lib")
#else
	#pragma comment(lib, "sdk\\DataExtractor_x86.lib")
#endif

#define DE_ASM 0
#define DE_C 1
#define DE_DELPHI 2
#define DE_VB 3

__declspec(dllimport) bool __stdcall DataExtractorRipData(LPVOID DataBuffer, DWORD DataSize, int ExportType, int RipSize, int BytesPerLine, int TableStart, char* szTableName, char* szSaveFileName);
__declspec(dllimport) bool __stdcall DataExtractorRipDataW(LPVOID DataBuffer, DWORD DataSize, int ExportType, int RipSize, int BytesPerLine, int TableStart, char* szTableName, wchar_t* szSaveFileName);

#pragma pack(pop)

#endif /*TITANENGINE_DATAEXTRACTOR*/