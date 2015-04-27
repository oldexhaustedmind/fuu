#include "unpacker.h"

#include <cstdio>
#include <cstring>
#include <cstdarg>

cUnpacker cUnpacker::lInstance;

HWND cUnpacker::Window = NULL;
void (*cUnpacker::LogCallback)(const char*) = &cUnpacker::DefLogCallback;
char cUnpacker::lPath[MAX_PATH], cUnpacker::lOut[MAX_PATH];

PROCESS_INFORMATION* cUnpacker::lProcess;

bool cUnpacker::lRealign, cUnpacker::lCopyOverlay;

PE32Struct cUnpacker::lPEInfo;
ULONG_PTR cUnpacker::lEP;
ULONG_PTR cUnpacker::SFXSectionVA;
DWORD cUnpacker::SFXSectionSize;

void cUnpacker::Log(const char* pFormat, ...)
{
va_list ArgList;
char Buffer[iMaxString];

	va_start(ArgList, pFormat);
	vsprintf_s(Buffer, sizeof Buffer, pFormat, ArgList);
	va_end(ArgList);

	LogCallback(Buffer);
}

void cUnpacker::Abort()
{
	StopDebug();
	Log("[Fatal Error] Unpacking has been aborted.");
}

bool cUnpacker::Unpack(const char* pPath, bool pRealign, bool pCopyOverlay)
{
HANDLE HFile, HMap;
DWORD FileSize;
ULONG_PTR MapVA;
bool Return = false;

	lRealign = pRealign;
	lCopyOverlay = pCopyOverlay;

	strncpy(lPath, pPath, sizeof lPath - 1);
	lPath[sizeof lPath - 1] = '\0';

	char* ExtChar = strrchr(lPath, '.');
	if(ExtChar)
	{
		*ExtChar = '\0';
		sprintf_s(lOut, MAX_PATH, "%s.unpacked.%s", lPath, ExtChar+1);
		*ExtChar = '.';
	}
	else
	{
		sprintf_s(lOut, MAX_PATH, "%s.unpacked", lPath);
	}

	DeleteFileA(lOut);

	Log("-> Unpack started...");

	if(StaticFileLoad(lPath, UE_ACCESS_READ, false, &HFile, &FileSize, &HMap, &MapVA))
	{
		if(GetPE32DataFromMappedFileEx(MapVA, &lPEInfo))
		{
			lEP = lPEInfo.ImageBase + lPEInfo.OriginalEntryPoint;

			WORD SFXSectionIndex = GetPE32SectionNumberFromVA(MapVA, lEP);
			SFXSectionVA = lPEInfo.ImageBase + GetPE32DataFromMappedFile(MapVA, SFXSectionIndex, UE_SECTIONVIRTUALOFFSET);
			SFXSectionSize = GetPE32DataFromMappedFile(MapVA, SFXSectionIndex, UE_SECTIONVIRTUALSIZE);

			lProcess = (PROCESS_INFORMATION*)InitDebugEx(lPath, 0, 0, &OnEp);
			if(lProcess)
			{
				DebugLoop();
				ImporterCleanup();
				Return = true;
			}
			else Log("[Error] Engine initialization failed!");
		}
		else Log("[Error] Selected file is not a valid PE32 file!");

		StaticFileUnload(lPath, false, HFile, FileSize, HMap, MapVA);
	}
	else Log("[Error] Can't open selected file!");

	Log("-> Unpack ended...");
	return Return;
}

void __stdcall cUnpacker::OnEp()
{
BYTE WildCard = 0xFF;

const BYTE DecryptPattern[] = {0x90, 0xEB, 0x04, 0x01, 0x07, 0x01, 0x07, 0xBB, 0xFF, 0xFF, 0xFF, 0xFF, 0xB9, 0xC8, 0x03, 0x00, 0x00, 0xB0, 0xFF, 0x30, 0x04, 0x0B, 0x8A, 0x04, 0x0B, 0xE2, 0xF8};

/*
NOP
JMP @SKIP
ADD DWORD PTR DS:[EDI],EAX
ADD DWORD PTR DS:[EDI],EAX
SKIP:
MOV EBX, XXXXXXXX ; code after LOOPD
MOV ECX, 3C8 ; constant value
MOV AL, XX
@DECRYPT:
XOR BYTE PTR DS:[EBX+ECX],AL
MOV AL,BYTE PTR DS:[EBX+ECX]
LOOPD DECRYPT
*/

	ImporterInit(iImporterSize, lPEInfo.ImageBase);

	ULONG_PTR DecryptCode = Find((void*)lEP, SFXSectionVA+SFXSectionSize-lEP, (void*)DecryptPattern, sizeof DecryptPattern, &WildCard);
	if(!DecryptCode)
	{
		Log("[Error] Cannot find decryption code, probably not packed with a supported version?");
		Abort();
		return;
	}
	if(!SetHardwareBreakPoint(DecryptCode + sizeof DecryptPattern, NULL, UE_HARDWARE_EXECUTE, UE_HARDWARE_SIZE_1, &OnDecrypted))
	{
		Log("[Error] Unable to set breakpoint on decryption code.");
		Abort();
	}
}

void __stdcall cUnpacker::OnDecrypted()
{
BYTE WildCard = 0xEE;

const BYTE LoadLibraryAPattern[] = {0xB9, 0xEE, 0xEE, 0xEE, 0xEE, 0x01, 0xE9, 0x83, 0x79, 0x0C, 0x00, 0x0F, 0x84, 0xEE, 0xEE, 0xEE, 0xEE, 0x8B, 0x59, 0x0C, 0x01, 0xEB, 0x51, 0x53, 0xFF, 0xD7, 0x59, 0x85, 0xC0};

/*
MOV ECX, XXXXXXXX
ADD ECX,EBP
CMP DWORD PTR DS:[ECX+C],0
JE XXXXXXXX
MOV EBX,DWORD PTR DS:[ECX+C]
0ADD EBX,EBP
PUSH ECX
PUSH EBX
CALL EDI
POP ECX
TEST EAX,EAX
*/

const BYTE GetProcAddressPattern[] = {0x8B, 0x07, 0x83, 0xC7, 0x04, 0xA9, 0x00, 0x00, 0x00, 0x80, 0x74, 0x08, 0x25, 0xFF, 0xFF, 0x00, 0x00, 0x50, 0xEB, 0x06, 0x01, 0xE8, 0x83, 0xC0, 0x02, 0x50, 0x53, 0xFF, 0xD6, 0x5A, 0x59, 0x85, 0xC0};

/*
MOV EAX,DWORD PTR DS:[EDI]
ADD EDI,4
TEST EAX,80000000
JE SHORT Nag_exeF.00404491
AND EAX,0FFFF
PUSH EAX
JMP SHORT Nag_exeF.00404497
ADD EAX,EBP
ADD EAX,2
PUSH EAX
PUSH EBX
CALL ESI
POP EDX
POP ECX
TEST EAX,EAX
*/

const size_t LoadLibraryABreakpointOffset = 24;
const size_t GetProcAddressBreakpointOffset = 27;

const BYTE OepPattern[] = {0x01, 0x2C, 0x24, 0xC3, 0x13, 0x13, 0x13, 0x13};

/*
ADD DWORD PTR SS:[ESP],EBP
RETN
ADC EDX,DWORD PTR DS:[EBX]
ADC EDX,DWORD PTR DS:[EBX]
*/

	ULONG_PTR DecryptedCode = GetContextData(UE_CIP);

	ULONG_PTR LoadLibraryACode   = Find((void*)DecryptedCode, SFXSectionVA+SFXSectionSize-DecryptedCode, (void*)LoadLibraryAPattern, sizeof LoadLibraryAPattern, &WildCard);
	ULONG_PTR GetProcAddressCode = Find((void*)DecryptedCode, SFXSectionVA+SFXSectionSize-DecryptedCode, (void*)GetProcAddressPattern, sizeof GetProcAddressPattern, NULL);
	if(!LoadLibraryACode || !GetProcAddressCode)
	{
		Log("[Error] Cannot find imports handling code, probably not packed with a supported version?");
		Abort();
		return;
	}
	SetBPX(LoadLibraryACode   + LoadLibraryABreakpointOffset,   UE_BREAKPOINT, &OnLoadLibraryACall);
	SetBPX(GetProcAddressCode + GetProcAddressBreakpointOffset, UE_BREAKPOINT, &OnGetProcAddressCall);

	ULONG_PTR OepCode = Find((void*)DecryptedCode, SFXSectionVA+SFXSectionSize-DecryptedCode, (void*)OepPattern, sizeof OepPattern, NULL);
	if(!OepCode)
	{
		Log("[Error] Cannot find OEP code, probably not packed with a supported version?");
		Abort();
		return;
	}
	SetBPX(OepCode, UE_BREAKPOINT, &OnOepCode);
}

void __stdcall cUnpacker::OnLoadLibraryACall()
{
	/*
	We're currently on the call to LoadLibraryA
	To get the parameter pushed to the API, we need to decrease the index we pass to GetFunctionParameter by 1
	This is because the return address has not yet been pushed to the stack and parameters are set off by one DWORD
	*/

	// Get name param pushed to LoadLibraryA
	char* DLLName = (char*)GetFunctionParameter(lProcess->hProcess, UE_FUNCTION_STDCALL, 0, UE_PARAMETER_STRING);

	// Add Dll (next call to ImporterAddNew[Ordinal]API sets first thunk)
	ImporterAddNewDll(DLLName, NULL);
	Log("[i] Imported DLL: %s", DLLName);
}

void __stdcall cUnpacker::OnGetProcAddressCall()
{
char APIName[iMaxString];

	/*
	Get second parameter pushed to GetProcAddress (API name or ordinal)
	See OnGetModuleHandleACall as to why we use 1 instead of 2 for the index
	*/
	ULONG_PTR APIParam = GetFunctionParameter(lProcess->hProcess, UE_FUNCTION_STDCALL, 1, UE_PARAMETER_DWORD);

	// Get address of IAT thunk for this API
	// It is located at [ESP+8], we can use GetFunctionParameter to easily get its value
	// since effectively it's the 3rd param
	ULONG_PTR ThunkAddr = GetFunctionParameter(lProcess->hProcess, UE_FUNCTION_STDCALL, 2, UE_PARAMETER_DWORD);

	if(!HIWORD(APIParam)) // Is this an ordinal API?
	{
		// Add ordinal API (pushed value = ordinal #)
		ImporterAddNewOrdinalAPI(APIParam, ThunkAddr);
		Log("  + [%08X]: #%d", ThunkAddr, APIParam);
	}
	else
	{
		// Add API name (pushed value = address to string)
		GetRemoteString(lProcess->hProcess, (void*)APIParam, APIName, sizeof APIName);
		ImporterAddNewAPI(APIName, ThunkAddr);
		Log("  + [%08X]: %s", ThunkAddr, APIName);
	}
}

void __stdcall cUnpacker::OnOepCode()
{
	RemoveAllBreakPoints(UE_OPTION_REMOVEALL);

	// Lazy man's way of getting the value at [ESP]
	ULONG_PTR OEP = lPEInfo.ImageBase + GetFunctionParameter(lProcess->hProcess, UE_FUNCTION_STDCALL, 0, UE_PARAMETER_DWORD);

	Log("[x] Reached OEP: %08X.", OEP);

	// Dump the unpacked module to disk
	if(!DumpProcess(lProcess->hProcess, (void*)lPEInfo.ImageBase, lOut, OEP))
	{
		Log("[Error] Cannot dump process.");
		Abort();
		return;
	}
	Log("[x] Dumping process.");

	// Create a new import table from the data added through ImporterAddNewDll + ImporterAddNew[Ordinal]API
	if(!ImporterExportIATEx(lOut, ".revlabs"))
	{
		Log("[Error] Cannot fix imports.");
		Abort();
		return;
	}
	Log("[x] Fixing imports.");

	if(lRealign) 
	{
		// Realign sections, shrink file size
		if(!RealignPEEx(lOut, NULL, NULL))
		{
			Log("[Error] Realigning failed.");
			Abort();
			return;
		}
		Log("[x] Realigning file.");
	}

	if(lCopyOverlay)
	{
		// Copy overlay from the original file to the dump
		if(CopyOverlay(lPath, lOut))
			Log("[x] Copying overlay.");
		else
			Log("[x] No overlay found.");
	}

	StopDebug();
	Log("[x] File successfully unpacked to %s.", lOut);
}
