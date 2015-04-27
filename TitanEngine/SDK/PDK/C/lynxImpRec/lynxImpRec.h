#ifndef TITANENGINE_LYNXIMPREC
#define TITANENGINE_LYNXIMPREC

#if _MSC_VER > 1000
    #pragma once
#endif

#pragma pack(push, 1)
#pragma comment(lib, "sdk\\lynxImpRec.lib")

__declspec(dllimport) bool __stdcall lynxImpRecExportTree(char* szOriginalFileName, char* szSaveFileName, DWORD OEPAddressRVA, DWORD IATRva, DWORD IATSize);
__declspec(dllimport) bool __stdcall lynxImpRecExportTreeW(char* szOriginalFileName, wchar_t* szSaveFileName, DWORD OEPAddressRVA, DWORD IATRva, DWORD IATSize);
__declspec(dllimport) bool __stdcall lynxImpRecLoadTree(char* szOriginalFileName, char* szLoadFileName);
__declspec(dllimport) bool __stdcall lynxImpRecLoadTreeW(char* szOriginalFileName, wchar_t* szLoadFileName);

#pragma pack(pop)

#endif /*TITANENGINE_LYNXIMPREC*/