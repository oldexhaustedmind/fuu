
#include <windows.h>

typedef BYTE					uchar;
typedef ULONG					ulong;
typedef void*					pvoid;
typedef char*					pchar;
typedef BYTE*					puchar;
typedef ULONG*					pulong;

#include "sdk/sdk.h"
#include "resource.h"

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0'" \
						"processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"") 

HINSTANCE					gInstance				= 0;
HWND						gWindow					= 0;

bool						gOverlay				= 0;

char						gFile[MAX_PATH]			= { 0 };
char						gPath[MAX_PATH]			= { 0 };
char						gDone[MAX_PATH]			= { 0 };

FILE_STATUS_INFO			gStatus					= { 0 };
PPROCESS_INFORMATION		gProcess				= 0;

MEMORY_BASIC_INFORMATION	gEPBase					= { 0 };
uchar						gWildCard				= 0;

pvoid						gBase					= 0;

void __stdcall OnLLACall();
void __stdcall OnGPACall();
void __stdcall OnOEPJmp();

void AddLog(const char pFormat[], ...);

enum
{
	iLLAPattern,
	iGPAPattern,
	iOEPPattern,

	iPatternCount,
	iSignatureOffset		= 0x14,
	iImporterMax			= 0x10000
};

ulong						gPatternLocation[iPatternCount] = { 0 };
const ulong					gInitialPatternOffset[iPatternCount] =
{
	7,
	2,
	2
};

const pchar					gPattern[] =
{
	"\xEB\x00\x00\xAD\x00\xAD\x50\xFF\x53\x10\x95",
	"\x50\x55\xFF\x53\x14\xAB\xEB\x00\x33\x00\xFF",
	"\x75\x03\xFF\x63\x0C"
};

const ulong					gPatternLen[] =
{
	sizeof gPattern[iLLAPattern] / sizeof *gPattern[iLLAPattern],
	sizeof gPattern[iGPAPattern] / sizeof *gPattern[iGPAPattern],
	sizeof gPattern[iOEPPattern] / sizeof *gPattern[iOEPPattern],
};

const pvoid					gPatternHandler[] =
{
	OnLLACall,
	OnGPACall,
	OnOEPJmp
};

void __stdcall OnLLACall()
{
	if(ulong(GetContextData(UE_EIP)) != gPatternLocation[iLLAPattern])
		return;

	static char Library[MAX_PATH] = { 0 };
	if(GetRemoteString(gProcess->hProcess, pvoid(GetContextData(UE_EAX)), &Library, MAX_PATH - 1))
		ImporterAddNewDll(Library, 0);
}

void __stdcall OnGPACall()
{
	if(ulong(GetContextData(UE_EIP)) != gPatternLocation[iGPAPattern])
		return;

	static char	Function[MAX_PATH]	= { 0 };
	ulong		Thunk				= ulong(GetContextData(UE_EDI));
	pvoid		Ordinal				= pvoid(GetContextData(UE_EAX));

	ImporterAddNewAPI(GetRemoteString(gProcess->hProcess, Ordinal, &Function, MAX_PATH - 1) ? Function : pchar(Ordinal), Thunk);
}

void __stdcall OnOEP()
{
	ulong OEP = ulong(GetContextData(UE_EIP));
	AddLog("[x] Entry Point at: %08X.", OEP);

	bool IsFatal = false;

	__try
	{
		DeleteFileA(gDone);
		if(PastePEHeader(gProcess->hProcess, gBase, gFile) &&
		   DumpProcess(gProcess->hProcess, gBase, gDone, OEP))
		{
			AddLog("[x] Paste PE32 header.");
			AddLog("[x] Process dumped.");

			pvoid Handle, Map;
			ulong Size, MapVA;

			if(ImporterExportIATEx(gDone, ".revlabs") && StaticFileLoad(gDone, UE_ACCESS_ALL, false, &Handle, &Size, &Map, &MapVA))
			{
				AddLog("[x] IAT has been fixed.");

				*pulong(MapVA + iSignatureOffset) = 'LveR';

				if(IsDlgButtonChecked(gWindow, rlRealign))
				{
					Size = RealignPE(MapVA, Size, 0);
					AddLog("[x] Realigning file.");
				}

				StaticFileUnload(gDone, true, Handle, Size, Map, MapVA);

				MakeAllSectionsRWE(gDone);
				ResortFileSections(gDone);

				if(gOverlay)
				{
					if(IsDlgButtonChecked(gWindow, rlOverlay))
					{
						CopyOverlay(gFile, gDone);
						AddLog("[x] Moving overlay to unpacked file.");
					}
					else AddLog("[x] Overlay has been ignored.");
				}

				AddLog("[Success] File has been unpacked.");
				AddLog("[x] File has been unpacked to: %s.", gDone);
				AddLog("-> Unpack ended...");
			}
			else
			{
				IsFatal = true;
				AddLog("[Error] Cannot export IAT!");
			}
		}
		else
		{
			IsFatal = true;
			AddLog("[Error] Cannot dump process!");
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		IsFatal = true;
	}

	if(IsFatal)
	{
		ImporterCleanup();
		ForceClose();

		AddLog("[Fatal Unpacking Error] Please mail file you tried to unpack to Reversing Labs!");
		AddLog("-> Unpack ended...");
	}

	StopDebug();
}

void __stdcall OnOEPJmp()
{
	if(ulong(GetContextData(UE_EIP)) != gPatternLocation[iOEPPattern])
		return;

	StepInto(OnOEP);
}

ulong Verify()
{
	if(gEPBase.AllocationBase != pvoid(GetPE32Data(gFile, 0, UE_IMAGEBASE)))
		return 0;

	ulong FSGMagic;
	ulong IO;
	if(ReadProcessMemory(gProcess->hProcess, pvoid(ulong(gEPBase.AllocationBase) + iSignatureOffset), &FSGMagic, sizeof FSGMagic, &IO) && IO == sizeof FSGMagic)
	{
		if(FSGMagic == '!GSF')
			return 1;
		else
			return 2;
	}

	return true;
}

void __stdcall OnEntry()
{
	pvoid EP = pvoid(GetContextData(UE_CIP));

	if(VirtualQueryEx(gProcess->hProcess, EP, &gEPBase, sizeof gEPBase))
	{
		ulong Length = ulong(gEPBase.AllocationBase) + gEPBase.RegionSize - ulong(EP);

		CopyMemory(gPatternLocation, gInitialPatternOffset, sizeof gInitialPatternOffset);

		bool Error = false;
		for(ulong i = 0; i < iPatternCount && !Error; i++)
		{
			Error = !(gPatternLocation[i] += ulong(Find(EP, Length, pvoid(gPattern[i]), gPatternLen[i], &gWildCard)));
			if(!Error) SetBPX(gPatternLocation[i], UE_BREAKPOINT, gPatternHandler[i]);
		}

		if(!Error)
		{
			ulong Result = Verify();
			Error = !Result;

			if(Result == 2)
			{
				if(!IsDlgButtonChecked(gWindow, rlIgnoreSig))
				{
					AddLog("[Abort] File unpacking has been aborted by user (signature mismatch).");
					AddLog("-> Unpack ended...");

					StopDebug();
					return;
				}
				else
					AddLog("[X] Ignoring signature mismatch.");
			}
		}

		if(!Error)
		{
			gBase = gEPBase.AllocationBase;
			ImporterInit(iImporterMax, ulong(gBase));
		}
		else
		{
			AddLog("[Error] File is not packed with FSG 2.0!");
			AddLog("-> Unpack ended...");

			StopDebug();
		}
	}
}

void InitializeUnpacker()
{
	AddLog("-> Unpack started...");
	if(IsPE32FileValidEx(gFile, UE_DEPTH_DEEP, &gStatus) && GetModuleFileNameA(gInstance, gPath, MAX_PATH - 1))
	{
		ulong SlashIndex = lstrlenA(gPath) - 1;

		while(SlashIndex > 0 && gPath[SlashIndex] != '\\') SlashIndex--;
		gPath[SlashIndex + 1] = 0;

		ulong ExtIndex = lstrlenA(gFile) - 1;
		while(ExtIndex > 0 && gFile[ExtIndex] != '.') ExtIndex--;

		gFile[ExtIndex] = 0;
		SlashIndex = ExtIndex;

		while(SlashIndex > 0 && gFile[SlashIndex] != '\\') SlashIndex--;
		SlashIndex++;
		ExtIndex++;

		wsprintfA(gDone, "%s%s.unpacked.%s", gPath, &gFile[SlashIndex], &gFile[ExtIndex]);
		gFile[ExtIndex - 1] = '.';

		gProcess = PPROCESS_INFORMATION(InitDebugEx(gFile, 0, 0, OnEntry));
		if(gProcess)
		{
			DebugLoop();
		}
		else
		{
			AddLog("[Error] Engine initialization failed!");
			AddLog("-> Unpack ended...");

			StopDebug();
		}
	}
	else
	{
		AddLog("[Error] Selected file is not a valid PE32 file!");
		AddLog("-> Unpack ended...");
	}
}

void AddLog(const char pFormat[], ...)
{
	va_list ArgList;
	va_start(ArgList, pFormat);

	static HWND Log = 0;
	if(!Log)	Log = GetDlgItem(gWindow, rlLog);

	char Buffer[512];
	wvsprintfA(Buffer, pFormat, ArgList);

	SendMessageA(Log, LB_ADDSTRING, 0, LPARAM(Buffer));
	SendMessageA(Log, LB_SETCURSEL, WPARAM(SendMessageA(Log, LB_GETCOUNT, 0, 0) - 1), 0);

	va_end(ArgList);
}

int __stdcall Handler(HWND pDialog, int pMessage, int pwParam, int plParam)
{
	static bool UnpackerRunning = false;

	switch(pMessage)
	{
	case WM_INITDIALOG:
		SendMessageA(pDialog, WM_SETICON, 0, LPARAM(LoadIconA(gInstance, MAKEINTRESOURCEA(rlIcon))));
		SetDlgItemTextA(pDialog, rlFilename, "filename.exe");
		
		CheckDlgButton(pDialog, rlRealign, true);
		gWindow = pDialog;
		break;

	case WM_DROPFILES:
		if(DragQueryFileA(HDROP(pwParam), 0, gFile, MAX_PATH - 1))
		{
			ulong Start, Size;
			gOverlay = FindOverlay(gFile, &Start, &Size);

			CheckDlgButton(pDialog, rlOverlay, gOverlay);
			EnableWindow(GetDlgItem(pDialog, rlOverlay), gOverlay);
			
			SetDlgItemTextA(pDialog, rlFilename, gFile);
		}
		return true;

	case WM_COMMAND:
		switch(pwParam)
		{
		case rlExit:
			SendMessageA(pDialog, WM_CLOSE, 0, 0);
			break;

		case rlUnpack:
			if(!UnpackerRunning)
			{
				UnpackerRunning = true;
				SendMessageA(GetDlgItem(pDialog, rlLog), LB_RESETCONTENT, 0, 0);

				InitializeUnpacker();
				UnpackerRunning = false;
			}
			break;

		case rlBrowse:
			OPENFILENAMEA File;
			ZeroMemory(&File, sizeof File);

			File.lStructSize	= sizeof File;
			File.lpstrFilter	= "Portable Executable (x86, *.exe) \0*.exe\0\0";
			File.lpstrFile		= gFile;
			File.nMaxFile		= MAX_PATH - 1;
			File.Flags			= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_HIDEREADONLY;
			File.lpstrTitle		= "RL!deFSG 2.0 from Reversing Labs";

			if(GetOpenFileNameA(&File))
			{
				ulong Start, Size;
				gOverlay = FindOverlay(gFile, &Start, &Size);

				CheckDlgButton(pDialog, rlOverlay, gOverlay);
				EnableWindow(GetDlgItem(pDialog, rlOverlay), gOverlay);

				SetDlgItemTextA(pDialog, rlFilename, gFile);
			}
			break;

		case rlAbout:
			const pchar Content = "RL!deFSG 2.0 unpacker \r\n\r\n" \
				" Visit Reversing Labs at http://www.reversinglabs.com \r\n\r\n" \
				"  Minimum engine version needed:\r\n" \
				"- TitanEngine 2.0.1 by RevLabs\r\n\r\n" \
				"Unpacker coded by Reversing Labs.";

			MessageBoxA(pDialog, Content, "[ About ]", MB_ICONASTERISK);
			break;
		}
		return true;

	case WM_CLOSE:
		EndDialog(pDialog, 0);
		break;
	}

	return 0;
}

long __stdcall ExceptionFilter(PEXCEPTION_POINTERS pInfo)
{
	char Buffer[512];
	wsprintfA(Buffer, "[Fatal Application Error]\r\nPlease report the error to Reversing Labs!\r\n\r\nCode 0%08X at 0%08X.", pInfo->ExceptionRecord->ExceptionCode,
		pInfo->ExceptionRecord->ExceptionAddress);

	MessageBoxA(gWindow, Buffer, "[ Error ]", MB_ICONERROR);

	ImporterCleanup();
	ForceClose();
	ExitProcess(0);
}

int __stdcall WinMain(HINSTANCE pInstance, HINSTANCE, pchar, int)
{
	gInstance = pInstance;
	SetUnhandledExceptionFilter(ExceptionFilter);

	return DialogBoxParamA(pInstance, MAKEINTRESOURCEA(rlDialog), 0, DLGPROC(Handler), 0);
}