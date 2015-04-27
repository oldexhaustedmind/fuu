#include "unpacker.h"

#include <cstdio>
#include <cstring>
#include <cstdarg>

cUnpacker cUnpacker::lInstance;

HWND cUnpacker::Window = NULL;
void (*cUnpacker::LogCallback)(const char*) = &cUnpacker::DefLogCallback;
char cUnpacker::lPath[MAX_PATH], cUnpacker::lOut[MAX_PATH];

PROCESS_INFORMATION* cUnpacker::lProcess;

bool cUnpacker::lIsDLL;
bool cUnpacker::lRealign, cUnpacker::lCopyOverlay;

PE32Struct cUnpacker::lPEInfo;
ULONG_PTR cUnpacker::lBase;
ULONG_PTR cUnpacker::lEP;

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

	if(GetPE32DataEx(lPath, &lPEInfo))
	{
		lIsDLL = (lPEInfo.Characteristics & IMAGE_FILE_DLL) != 0; //IsFileDLL(lPath, NULL);

		if(lIsDLL)
		{
			Log("[i] File is a DLL, using DLL loader.");
			lProcess = (PROCESS_INFORMATION*)InitDLLDebug(lPath, true, 0, 0, &OnEp);
		}
		else
			lProcess = (PROCESS_INFORMATION*)InitDebugEx(lPath, 0, 0, &OnEp);

		if(lProcess)
		{
			DebugLoop();

			ImporterCleanup();
			if(lIsDLL)
			{
				RelocaterCleanup();
				DeleteFileA("snapshot0");
				DeleteFileA("snapshot1");
			}

			Return = true;
		}
		else Log("[Error] Engine initialization failed!");
	}
	else Log("[Error] Selected file is not a valid PE32 file!");

	Log("-> Unpack ended...");
	return Return;
}

void __stdcall cUnpacker::OnEp()
{
const BYTE EpPattern[] = {0x60, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x5B, 0x8D, 0x5B, 0xC6, 0x01, 0x1B, 0x8B, 0x13, 0x8D, 0x73, 0x14, 0x6A, 0x08, 0x59, 0x01, 0x16, 0xAD, 0x49, 0x75, 0xFA, 0x8B, 0xE8, 0xC6, 0x06, 0xE9, 0x8B, 0x43, 0x0C, 0x89, 0x46, 0x01, 0x6A, 0x04, 0x68, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x73, 0x08, 0x51, 0xFF, 0x55, 0x08};

/*
PUSHAD
CALL @F
@@:
POP EBX
LEA EBX,DWORD PTR DS:[EBX-3A]
ADD DWORD PTR DS:[EBX],EBX
MOV EDX,DWORD PTR DS:[EBX]
LEA ESI,DWORD PTR DS:[EBX+14]
PUSH 8
POP ECX
@@:
ADD DWORD PTR DS:[ESI],EDX
LODS DWORD PTR DS:[ESI]
DEC ECX
JNZ @B
MOV EBP,EAX
MOV BYTE PTR DS:[ESI],0E9
MOV EAX,DWORD PTR DS:[EBX+C]
MOV DWORD PTR DS:[ESI+1],EAX
PUSH 4
PUSH 1000
PUSH DWORD PTR DS:[EBX+8]
PUSH ECX
CALL DWORD PTR SS:[EBP+8] ; VirtualAlloc
*/

const BYTE ImportsPattern[] = {0x03, 0x03, 0x50, 0xFF, 0x55, 0x00, 0x8B, 0x3E, 0x03, 0x3B, 0xEB, 0x12, 0x0F, 0xBA, 0xF1, 0x1F, 0x72, 0x04, 0x03, 0x0B, 0x41, 0x41, 0x50, 0x51, 0x50, 0xFF, 0x55, 0x04, 0xAB, 0x58, 0x8B, 0x0F, 0x85, 0xC9, 0x75, 0xE8, 0x83, 0xC6, 0x10, 0xAD, 0x85, 0xC0, 0x75, 0xD4};

/*
@NEXT_DLL:
ADD EAX,DWORD PTR DS:[EBX]
PUSH EAX
CALL DWORD PTR SS:[EBP]
MOV EDI,DWORD PTR DS:[ESI]
ADD EDI,DWORD PTR DS:[EBX]
JMP @CHECK_API_END
@NEXT_API:
BTR ECX,1F
JB @SKIP_NAME
ADD ECX,DWORD PTR DS:[EBX]
INC ECX
INC ECX
@SKIP_NAME:
PUSH EAX
PUSH ECX
PUSH EAX
CALL DWORD PTR SS:[EBP+4]
STOS DWORD PTR ES:[EDI]
POP EAX           
@CHECK_API_END:
MOV ECX,DWORD PTR DS:[EDI]
TEST ECX,ECX
JNZ @NEXT_API
ADD ESI,10
LODS DWORD PTR DS:[ESI]
TEST EAX,EAX
JNZ @NEXT_DLL
*/

const size_t ImportsOffsetGetModuleHandleA = 3;
const size_t ImportsOffsetGetProcAddress = 25;

const BYTE RelocPattern[] = {0x8B, 0x4B, 0x2C, 0x0B, 0xC9, 0x74, 0x25, 0x8B, 0x7B, 0x24, 0xEB, 0x14, 0x33, 0xC0, 0x66, 0xAD, 0x0B, 0xC0, 0x74, 0x08, 0x80, 0xE4, 0x0F, 0x03, 0x03, 0x01, 0x0C, 0x02, 0x3B, 0xF7, 0x75, 0xEC, 0x8B, 0x17, 0x8D, 0x77, 0x08, 0x03, 0x7F, 0x04, 0x85, 0xD2, 0x75, 0xF0};

/*
MOV ECX,DWORD PTR DS:[EBX+2C]
OR ECX,ECX
JE @SKIP_RELOC
MOV EDI,DWORD PTR DS:[EBX+24]
JMP @NEXT_BLOCK
@NEXT_ITEM:
XOR EAX,EAX
LODS WORD PTR DS:[ESI]
OR EAX,EAX
JE @CHECK_BLOCK_END
AND AH,0F
ADD EAX,DWORD PTR DS:[EBX]
ADD DWORD PTR DS:[EDX+EAX],ECX
@CHECK_BLOCK_END:
CMP ESI,EDI
JNZ @NEXT_ITEM
@NEXT_BLOCK:
MOV EDX,DWORD PTR DS:[EDI]
LEA ESI,DWORD PTR DS:[EDI+8]
ADD EDI,DWORD PTR DS:[EDI+4]
TEST EDX,EDX
JNZ @CHECK_BLOCK_END
@SKIP_RELOC:
*/

	lBase = lIsDLL ? GetDebuggedDLLBaseAddress() : GetDebuggedFileBaseAddress();
	lEP = lBase + lPEInfo.OriginalEntryPoint;

	ImporterInit(iImporterSize, lBase);
	if(lIsDLL)
	{
		RelocaterInit(iRelocaterSize, lPEInfo.ImageBase, lBase);
	}

	if(lEP != Find((void*)lEP, sizeof EpPattern, (void*)EpPattern, sizeof EpPattern, NULL))
	{
		const char Title[] = "[ WARNING ]";
		const char Warning[] = "This file doesn't seem to be packed with a known version of Packman!\r\n"
		                       "Do you want to continue unpacking?";
		Log("[Warning] File doesn't seem to be packed with Packman!");
		if(IDNO == MessageBoxA(Window, Warning, Title, MB_YESNO | MB_ICONWARNING))
		{
			Log("[x] Aborted by user.");
			Abort();
			return;
		}
	}

	ULONG_PTR ImportsCode = Find((void*)lEP, 0x200, (void*)ImportsPattern, sizeof ImportsPattern, NULL);
	if(!ImportsCode)
	{
		Log("[Error] Cannot find imports handling code, probably not packed with a supported version?");
		Abort();
		return;
	}
	SetBPX(ImportsCode+ImportsOffsetGetModuleHandleA, UE_BREAKPOINT, &OnGetModuleHandleACall);
	SetBPX(ImportsCode+ImportsOffsetGetProcAddress,   UE_BREAKPOINT, &OnGetProcAddressCall);

	if(lIsDLL)
	{
		ULONG_PTR RelocCode = Find((void*)lEP, 0x200, (void*)RelocPattern, sizeof RelocPattern, NULL);
		if(!RelocCode)
		{
			Log("[Error] Cannot find relocation code, probably not packed with a supported version?");
			Abort();
			return;
		}
		SetBPX(RelocCode, UE_SINGLESHOOT, &OnBeforeRelocation);
	}

	// Step one instruction so we can set a hardware breakpoint on the EP w/o triggering it right away
	StepInto(OnEpStep);
}

void __stdcall cUnpacker::OnEpStep()
{
	if(!SetHardwareBreakPoint(lEP, NULL, UE_HARDWARE_EXECUTE, UE_HARDWARE_SIZE_1, &OnOepJump))
	{
		Log("[Error] Unable to set breakpoint on jump to OEP.");
		Abort();
	}
}

void __stdcall cUnpacker::OnGetModuleHandleACall()
{
	/*
	We're currently on the call to GetModuleHandleA
	To get the parameter pushed to the API, we need to decrease the index we pass to GetFunctionParameter by 1
	This is because the return address has not yet been pushed to the stack and parameters are set off by one DWORD
	*/

	// Get name param pushed to GetModuleHandleA
	char* DLLName = (char*)GetFunctionParameter(lProcess->hProcess, UE_FUNCTION_STDCALL, 0, UE_PARAMETER_STRING);

	// Add Dll (next call to ImporterAddNew[Ordinal]API sets first thunk)
	ImporterAddNewDll(DLLName, NULL);
	Log("[i] Imported DLL: %s", DLLName);
}

void __stdcall cUnpacker::OnGetProcAddressCall()
{
char APIName[iMaxString];

	// Get address of IAT thunk for this API
	ULONG_PTR ThunkAddr = GetContextData(UE_EDI);

	/*
	Get second parameter pushed to GetProcAddress (API name or ordinal)
	See OnGetModuleHandleACall as to why we use 1 instead of 2 for the index
	*/
	ULONG_PTR APIParam = GetFunctionParameter(lProcess->hProcess, UE_FUNCTION_STDCALL, 1, UE_PARAMETER_DWORD);

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

void __stdcall cUnpacker::OnBeforeRelocation()
{
	/*
	- Create first module snapshot right before Packman relocates code/data
	- The second snapshot is taken once we reach the OEP
	*/
	RelocaterMakeSnapshot(lProcess->hProcess, "snapshot0", (void*)lBase, lPEInfo.NtSizeOfImage);
}

void __stdcall cUnpacker::OnOepJump()
{
	RemoveAllBreakPoints(UE_OPTION_REMOVEALL);

	ULONG_PTR OEP = GetJumpDestination(lProcess->hProcess, lEP);

	Log("[x] Reached OEP: %08X.", OEP);

	// Dump the unpacked module to disk
	if(!DumpProcess(lProcess->hProcess, (void*)lBase, lOut, OEP))
	{
		Log("[Error] Cannot dump process.");
		Abort();
		return;
	}
	Log("[x] Dumping process.");

	if(lIsDLL)
	{
		/*
		- Create second memory snapshot after code and data have been relocated
		- Gather relocation data by comparing differences between the two snapshots
		- Export data into a new relocation table
		*/
		RelocaterMakeSnapshot(lProcess->hProcess, "snapshot1", (void*)lBase, lPEInfo.NtSizeOfImage);
		RelocaterCompareTwoSnapshots(lProcess->hProcess, lBase, lPEInfo.NtSizeOfImage, "snapshot0", "snapshot1", lBase);
		
		if(!RelocaterExportRelocationEx(lOut, ".revlabs"))
		{
			Log("[Error] Cannot fix relocations.");
			Abort();
			return;
		}
		Log("[x] Fixing relocations.");
	}

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
