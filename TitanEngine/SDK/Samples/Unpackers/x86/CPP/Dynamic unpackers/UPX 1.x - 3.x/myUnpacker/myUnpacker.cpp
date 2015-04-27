// myUnpacker.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "SDK.hpp"

using namespace TE;

// Global Variables:
HINSTANCE hInstance;
HWND WindowHandle;
HWND BoxHandle;
bool fdFileIsDll = false;
long fdImageBase = NULL;
long fdLoadedBase = NULL;
long fdEntryPoint = NULL;
long fdSizeOfImage = NULL;
long SnapshootMemorySize = NULL;
long SnapshootMemoryStartRVA = NULL;
bool UnpackerRunning = false;
TCHAR SnapShoot1[MAX_PATH] = {};
TCHAR SnapShoot2[MAX_PATH] = {};
TCHAR FileName[MAX_PATH] = {};
TCHAR UnpackFileName[MAX_PATH] = {};
char szReadStringData[MAX_PATH] = {};
TCHAR PrintBuffer[1024] = {};

const PROCESS_INFORMATION* fdProcessInfo = NULL;
Debugger::fBreakPointCallback cbInitCallBack = NULL;

// Global Functions:

INT_PTR CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

bool MapFileEx(TCHAR*, DWORD, LPHANDLE, LPDWORD, LPHANDLE, LPVOID*, DWORD);
void UnMapFileEx(HANDLE, DWORD, HANDLE, LPCVOID);
void cbCreateProcess(void*);
void InitializeUnpacker(TCHAR*, Debugger::fBreakPointCallback);
void AddLogMessage(const TCHAR*);
bool GetFileDialog(TCHAR[MAX_PATH]);
void cbLoadLibrary();
void cbGetProcAddress();
void cbEntryPoint();
void cbMakeSnapShoot1();
void cbFindPatterns();

// Unpacker Data:
BYTE glWildCard = 0x00;
BYTE dtPattern1[] = {0x50, 0x83, 0xC7, 0x08, 0xFF};
Debugger::fBreakPointCallback dtPattern1CallBack = &cbLoadLibrary;
ULONG_PTR dtPattern1BPXAddress = NULL;

BYTE dtPattern2[] = {0x50, 0x47, 0x00, 0x57, 0x48, 0xF2, 0xAE};
Debugger::fBreakPointCallback dtPattern2CallBack = &cbGetProcAddress;
ULONG_PTR dtPattern2BPXAddress = NULL;

BYTE dtPattern3[] = {0x57, 0x48, 0xF2, 0xAE, 0x00, 0xFF};
Debugger::fBreakPointCallback dtPattern3CallBack = &cbGetProcAddress;
ULONG_PTR dtPattern3BPXAddress = NULL;

BYTE dtPattern4[] = {0x89, 0xF9, 0x57, 0x48, 0xF2, 0xAE, 0x52, 0xFF};
Debugger::fBreakPointCallback dtPattern4CallBack = &cbGetProcAddress;
ULONG_PTR dtPattern4BPXAddress = NULL;

BYTE dtPattern5[] = {0x61, 0xE9};
Debugger::fBreakPointCallback dtPattern5CallBack = &cbEntryPoint;
ULONG_PTR dtPattern5BPXAddress = NULL;

BYTE dtPattern51[] = {0x83, 0xEC, 0x00, 0xE9};
Debugger::fBreakPointCallback dtPattern51CallBack = &cbEntryPoint;
ULONG_PTR dtPattern51BPXAddress = NULL;

BYTE dtPattern6[] = {0x31, 0xC0, 0x8A, 0x07, 0x47, 0x09, 0xC0, 0x74, 0x22, 0x3C, 0xEF, 0x77, 0x11, 0x01, 0xC3,
					 0x8B, 0x03, 0x86, 0xC4, 0xC1, 0xC0, 0x10, 0x86, 0xC4, 0x01, 0xF0, 0x89, 0x03, 0xEB, 0xE2,
					 0x24, 0x0F, 0xC1, 0xE0, 0x10, 0x66, 0x8B, 0x07, 0x83, 0xC7, 0x02, 0xEB, 0xE2};
Debugger::fBreakPointCallback dtPattern6CallBack = &cbMakeSnapShoot1;
ULONG_PTR dtPattern6BPXAddress = NULL;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	::hInstance = hInstance;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINWINDOW), NULL, &WndProc);
	ExitProcess(NULL);
}

INT_PTR CALLBACK WndProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const TCHAR   szDlgTitle[] = _T("[UPX 1.x - 3.x Unpacker]");
	const TCHAR  szAboutText[] = _T("RL!deUPX 1.x - 3.x unpacker\r\n\r\nVisit Reversing Labs at http://www.reversinglabs.com\r\n\r\nMinimum engine version needed:\r\n- TitanEngine 2.0 by RevLabs\r\n\r\nUnpacker coded by Reversing Labs");
	const TCHAR szAboutTitle[] = _T("[ About ]");

	switch(uMsg)
	{
	case WM_INITDIALOG:
		WindowHandle = hwndDlg;
		BoxHandle = GetDlgItem(hwndDlg, IDC_LISTBOX);
		SendMessage(hwndDlg, WM_SETTEXT, NULL, (LPARAM)szDlgTitle);
		SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)));
		SetDlgItemText(hwndDlg, IDC_FILENAME, _T("filename.exe"));
		CheckDlgButton(hwndDlg, IDC_REALING, BST_CHECKED);
		break;

	case WM_DROPFILES:
		DragQueryFile((HDROP)wParam, NULL, FileName, _countof(FileName));
		if(!(GetFileAttributes(FileName) & FILE_ATTRIBUTE_DIRECTORY))
		{
			SetDlgItemText(hwndDlg, IDC_FILENAME, FileName);
		}
		DragFinish((HDROP)wParam);
		break;

	case WM_CLOSE:
		EndDialog(hwndDlg, NULL);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_UNPACK:
			if(!UnpackerRunning)
			{
				UnpackerRunning = true;
				SendMessage(BoxHandle, LB_RESETCONTENT, NULL, NULL);
				InitializeUnpacker(FileName, &cbFindPatterns);
				UnpackerRunning = false;
			}
			break;
		case IDC_BROWSE:
			if(GetFileDialog(FileName))
			{
				SetDlgItemText(hwndDlg, IDC_FILENAME, FileName);
			}
			break;
		case IDC_ABOUT:
			MessageBox(hwndDlg, szAboutText, szAboutTitle, MB_OK | MB_ICONINFORMATION);
			break;

		case IDC_EXIT:
			EndDialog(hwndDlg, NULL);
			break;
		}
		break;

	default:
		return false;
	}

	return true;
}

bool GetFileDialog(TCHAR Buffer[MAX_PATH])
{
	OPENFILENAME sOpenFileName = {0};
	const TCHAR szFilterString[] = _T("All Files \0*.*\0\0");
	const TCHAR  szDialogTitle[] = _T("RL!deUPX 1.x - 3.x from Reversing Labs");

	Buffer[0] = 0;

	sOpenFileName.lStructSize = sizeof(sOpenFileName);
	sOpenFileName.lpstrFilter = szFilterString;
	sOpenFileName.lpstrFile = Buffer;
	sOpenFileName.nMaxFile = MAX_PATH;
	sOpenFileName.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_HIDEREADONLY;
	sOpenFileName.lpstrTitle = szDialogTitle;

	return (TRUE == GetOpenFileName(&sOpenFileName));
}

void AddLogMessage(const TCHAR* szLogMessage)
{
	LRESULT cSelect = SendMessage(BoxHandle, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)szLogMessage);
	SendMessage(BoxHandle, LB_SETCURSEL, cSelect, NULL);
}

void InitializeUnpacker(TCHAR* szFileName, Debugger::fBreakPointCallback CallBack)
{
	Realigner::FILE_STATUS_INFO inFileStatus = {};

	AddLogMessage(_T("-> Unpack started..."));
	if(Realigner::IsPE32FileValidEx(szFileName, UE_DEPTH_DEEP, &inFileStatus))
	{
		cbInitCallBack = CallBack;
		fdImageBase = (long)Dumper::GetPE32Data(szFileName, NULL, UE_IMAGEBASE);
		fdEntryPoint = (long)Dumper::GetPE32Data(szFileName, NULL, UE_OEP);
		fdSizeOfImage = (long)Dumper::GetPE32Data(szFileName, NULL, UE_SIZEOFIMAGE);

		SnapshootMemoryStartRVA = (long)Dumper::GetPE32Data(szFileName, 0, UE_SECTIONVIRTUALOFFSET);
		SnapshootMemorySize = fdEntryPoint - SnapshootMemoryStartRVA;

		TCHAR* FilePart = PathFindFileName(FileName);
		TCHAR* Extension = PathFindExtension(FilePart);

		TCHAR Bak = *Extension;
		*Extension = 0;
		lstrcpy(UnpackFileName, FileName);
		*Extension = Bak;
		lstrcat(UnpackFileName, _T(".dumped"));
		lstrcat(UnpackFileName, Extension);

		fdFileIsDll = inFileStatus.FileIsDLL;
		if(!fdFileIsDll)
		{
			fdProcessInfo = Debugger::InitDebug(szFileName, NULL, NULL);
		}
		else
		{
			GetTempPath(_countof(SnapShoot1), SnapShoot1);
			lstrcpy(SnapShoot2, SnapShoot1);
			lstrcat(SnapShoot1, _T("snapshoot.1"));
			lstrcat(SnapShoot2, _T("snapshoot.2"));
			fdProcessInfo = Debugger::InitDLLDebug(szFileName, true, NULL, NULL, CallBack);
		}
		if(fdProcessInfo)
		{
			if(!fdFileIsDll)
			{
				Debugger::SetCustomHandler(UE_CH_CREATEPROCESS, &cbCreateProcess);
			}
			Debugger::DebugLoop();
		}
		else
		{
			AddLogMessage(_T("[Error]"));
			AddLogMessage(_T("-> Unpack ended..."));
		}
	}
	else
	{
		AddLogMessage(_T("[Error] Selected file is not a valid PE32 file!"));
		AddLogMessage(_T("-> Unpack ended..."));
	}
}

void cbCreateProcess(void* lpCreateProcInfo)
{
	fdLoadedBase = (long)((CREATE_PROCESS_DEBUG_INFO*)lpCreateProcInfo)->lpBaseOfImage;
	Debugger::SetCustomHandler(UE_CH_CREATEPROCESS, NULL);
	Debugger::SetBPX(fdLoadedBase + fdEntryPoint, UE_BREAKPOINT, cbInitCallBack);
	Importer::Init(50 * 1024, fdLoadedBase);
}

void cbLoadLibrary()
{
	long cAddress;
	long cReadSize;
	long cTargetAddress;
	SIZE_T NumberOfBytes;
	MEMORY_BASIC_INFORMATION MemInfo;

	cAddress = Debugger::GetContextData(UE_EIP);
	if(dtPattern1BPXAddress == cAddress)
	{
		cTargetAddress = Debugger::GetContextData(UE_EAX);
	}
	if(cTargetAddress > fdLoadedBase)
	{
		VirtualQueryEx(fdProcessInfo->hProcess, (void*)cTargetAddress, &MemInfo, sizeof(MemInfo));
		cReadSize = (long)MemInfo.BaseAddress + MemInfo.RegionSize;
		VirtualQueryEx(fdProcessInfo->hProcess, (void*)((long)MemInfo.BaseAddress + MemInfo.RegionSize), &MemInfo, sizeof(MEMORY_BASIC_INFORMATION));
		cReadSize = cReadSize + MemInfo.RegionSize;
		cReadSize = cReadSize - cAddress;
		if(cReadSize > 256)
		{
			cReadSize = 256;
		}
		if(ReadProcessMemory(fdProcessInfo->hProcess, (void*)cTargetAddress, &szReadStringData, cReadSize, &NumberOfBytes))
		{
			Importer::AddNewDll(szReadStringData, NULL);
			wsprintf(PrintBuffer, _T("[x] LoadLibrary BPX -> %hs"), szReadStringData);
			AddLogMessage(PrintBuffer);
		}
	}
}

void cbGetProcAddress()
{
	long cThunk;
	long cAddress;
	long cReadSize;
	long cTargetAddress;
	SIZE_T NumberOfBytes;
	MEMORY_BASIC_INFORMATION MemInfo;

	cAddress = Debugger::GetContextData(UE_EIP);
	if(dtPattern2BPXAddress == cAddress)
	{
		cTargetAddress = Debugger::GetContextData(UE_EAX);
		cThunk = Debugger::GetContextData(UE_EBX);
	}
	else if(dtPattern3BPXAddress == cAddress)
	{
		cTargetAddress = Debugger::GetContextData(UE_EDI);
		cThunk = Debugger::GetContextData(UE_EBX);
	}
	else if(dtPattern4BPXAddress == cAddress)
	{
		cTargetAddress = Debugger::GetContextData(UE_EDI);
		cThunk = Debugger::GetContextData(UE_EBX);
	}
	if(cTargetAddress > fdLoadedBase)
	{
		VirtualQueryEx(fdProcessInfo->hProcess, (void*)cTargetAddress, &MemInfo, sizeof(MEMORY_BASIC_INFORMATION));
		cReadSize = (long)MemInfo.BaseAddress + MemInfo.RegionSize;
		VirtualQueryEx(fdProcessInfo->hProcess, (void*)((long)MemInfo.BaseAddress + MemInfo.RegionSize), &MemInfo, sizeof(MEMORY_BASIC_INFORMATION));
		cReadSize = cReadSize + MemInfo.RegionSize;
		cReadSize = cReadSize - cAddress;
		if(cReadSize > 256){
			cReadSize = 256;
		}
		if(ReadProcessMemory(fdProcessInfo->hProcess, (void*)cTargetAddress, &szReadStringData, cReadSize, &NumberOfBytes))
		{
			Importer::AddNewAPI(szReadStringData, cThunk);
			wsprintf(PrintBuffer, _T("[x] GetProcAddress BPX -> %hs"),szReadStringData);
			AddLogMessage(PrintBuffer);
		}
	}
	else
	{
		Importer::AddNewAPI((char*)cTargetAddress, cThunk);
		wsprintf(PrintBuffer, _T("[x] GetProcAddress BPX -> %08X"), cTargetAddress);
		AddLogMessage(PrintBuffer);
	}
}
void cbEntryPoint()
{
	int i;
	long cAddress;	
	int UnpackedOEP;
	HANDLE FileHandle;
	DWORD FileSize;
	HANDLE FileMap;
	LPVOID FileMapVA;
	SIZE_T NumberOfBytes;
	long mImportTableOffset;
	long mRelocTableOffset;
	DWORD pOverlayStart;
	DWORD pOverlaySize;

	cAddress = Debugger::GetContextData(UE_EIP);
	__try
	{
		if(ReadProcessMemory(fdProcessInfo->hProcess, (void*)(cAddress + 1), &UnpackedOEP, sizeof(UnpackedOEP), &NumberOfBytes))
		{
			UnpackedOEP = UnpackedOEP + cAddress + 5;
		}
		wsprintf(PrintBuffer, _T("[x] Entry Point at: %08X"), UnpackedOEP);
		AddLogMessage(PrintBuffer);

		if(!fdFileIsDll)
		{
			Dumper::PastePEHeader(fdProcessInfo->hProcess, (void*)fdImageBase, FileName);
			AddLogMessage(_T("[x] Paste PE32 header!"));
		}
		else
		{
			Relocater::MakeSnapshot(fdProcessInfo->hProcess, SnapShoot2, (void*)(SnapshootMemoryStartRVA + fdLoadedBase), SnapshootMemorySize);
			Relocater::CompareTwoSnapshots(fdProcessInfo->hProcess, fdLoadedBase, fdSizeOfImage, SnapShoot1, SnapShoot2, SnapshootMemoryStartRVA + fdLoadedBase);
			DeleteFile(SnapShoot1);
			DeleteFile(SnapShoot2);
		}
		Dumper::DumpProcess(fdProcessInfo->hProcess, (void*)fdLoadedBase, UnpackFileName, UnpackedOEP);
		AddLogMessage(_T("[x] Process dumped!"));
		Debugger::StopDebug();
		mImportTableOffset = Dumper::AddNewSection(UnpackFileName, "TEv20", Importer::EstimatedSize() + 200) + fdLoadedBase;
		if(fdFileIsDll)
		{
			mRelocTableOffset = Dumper::AddNewSection(UnpackFileName, "TEv20", Relocater::EstimatedSize() + 200);
		}
		if(MapFileEx(UnpackFileName, UE_ACCESS_ALL, &FileHandle, &FileSize, &FileMap, &FileMapVA, NULL))
		{
			Importer::ExportIAT(Dumper::ConvertVAtoFileOffset((ULONG_PTR)FileMapVA, mImportTableOffset, true), (ULONG_PTR)FileMapVA);
			AddLogMessage(_T("[x] IAT has been fixed!"));
			if(fdFileIsDll)
			{
				Relocater::ExportRelocation(Dumper::ConvertVAtoFileOffset((ULONG_PTR)FileMapVA, mRelocTableOffset + fdLoadedBase, true), mRelocTableOffset, (ULONG_PTR)FileMapVA);
				AddLogMessage(_T("[x] Exporting relocations!"));
			}
			if(BST_CHECKED == IsDlgButtonChecked(WindowHandle, IDC_REALING))
			{
				FileSize = Realigner::RealignPE((ULONG_PTR)FileMapVA, FileSize, 2);
				AddLogMessage(_T("[x] Realigning file!"));
			}
			UnMapFileEx(FileHandle, FileSize, FileMap, FileMapVA);
			Dumper::MakeAllSectionsRWE(UnpackFileName);
			if(fdFileIsDll)
			{
				Relocater::ChangeFileBase(UnpackFileName, fdImageBase);
				AddLogMessage(_T("[x] Rebase file image!"));
			}
			if(Dumper::FindOverlay(FileName, &pOverlayStart, &pOverlaySize))
			{
				AddLogMessage(_T("[x] Moving overlay to unpacked file!"));
				Dumper::CopyOverlay(FileName, UnpackFileName);
			}
			AddLogMessage(_T("[Success] File has been unpacked!"));

			i = lstrlen(UnpackFileName);
			while(UnpackFileName[i] != '\\')
			{
				i--;
			}
			i++;
			wsprintf(PrintBuffer, _T("[x] File has been unpacked to: %s"), &UnpackFileName[i]);
			AddLogMessage(PrintBuffer);
			AddLogMessage(_T("-> Unpack ended..."));
		}
		else
		{
			AddLogMessage(_T("[Fatal Unpacking Error] Please mail file you tried to unpack to Reversing Labs!"));
			AddLogMessage(_T("-> Unpack ended..."));
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		Debugger::ForceClose();
		Importer::Cleanup();
		if(FileMapVA > NULL)
		{
			UnMapFileEx(FileHandle, FileSize, FileMap, FileMapVA);
		}
		DeleteFile(UnpackFileName);
		AddLogMessage(_T("[Fatal Unpacking Error] Please mail file you tried to unpack to Reversing Labs!"));
		AddLogMessage(_T("-> Unpack ended..."));
	}
}

void cbMakeSnapShoot1()
{
	Relocater::MakeSnapshot(fdProcessInfo->hProcess, SnapShoot1, (void*)(SnapshootMemoryStartRVA + fdLoadedBase), SnapshootMemorySize);
}

void cbFindPatterns()
{
	bool DontLog = false;
	long cReadSize = NULL;

	if(fdFileIsDll)
	{
		fdLoadedBase = (long)Debugger::GetDebuggedDLLBaseAddress();
		Importer::Init(50 * 1024, fdLoadedBase);
		Relocater::Init(100 * 1024, fdImageBase, fdLoadedBase);
	}
	cReadSize = fdSizeOfImage - fdEntryPoint;

	dtPattern1BPXAddress = Debugger::Find((void*)(fdLoadedBase + fdEntryPoint), cReadSize, dtPattern1, sizeof(dtPattern1), &glWildCard);
	if(dtPattern1BPXAddress)
	{
		Debugger::SetBPX(dtPattern1BPXAddress, UE_BREAKPOINT, dtPattern1CallBack);
	}
	else if(!DontLog)
	{
		AddLogMessage(_T("[Error] File is not packed with UPX 1.x - 3.x"));
		AddLogMessage(_T("-> Unpack ended..."));
		Debugger::StopDebug();
		DontLog = true;
	}

	dtPattern2BPXAddress = Debugger::Find((void*)(fdLoadedBase + fdEntryPoint), cReadSize, dtPattern2, sizeof(dtPattern2), &glWildCard);
	if(dtPattern2BPXAddress)
	{
		Debugger::SetBPX(dtPattern2BPXAddress, UE_BREAKPOINT, dtPattern2CallBack);
	}

	dtPattern3BPXAddress = Debugger::Find((void*)(fdLoadedBase + fdEntryPoint), cReadSize, dtPattern3, sizeof(dtPattern3), &glWildCard);
	if(dtPattern3BPXAddress)
	{
		Debugger::SetBPX(dtPattern3BPXAddress, UE_BREAKPOINT, dtPattern3CallBack);
	}
	else if(!DontLog)
	{
			AddLogMessage(_T("[Error] File is not packed with UPX 1.x - 3.x"));
			AddLogMessage(_T("-> Unpack ended..."));
			Debugger::StopDebug();
			DontLog = true;
	}

	dtPattern4BPXAddress = Debugger::Find((void*)(fdLoadedBase + fdEntryPoint), cReadSize, dtPattern4, sizeof(dtPattern4), &glWildCard);
	if(dtPattern4BPXAddress)
	{
		dtPattern4BPXAddress += 2;;
		Debugger::SetBPX(dtPattern4BPXAddress, UE_BREAKPOINT, dtPattern4CallBack);
	}

	dtPattern5BPXAddress = Debugger::Find((void*)(fdLoadedBase + fdEntryPoint), cReadSize, dtPattern5, sizeof(dtPattern5), &glWildCard);
	if(dtPattern5BPXAddress)
	{
		dtPattern5BPXAddress++;
		Debugger::SetBPX(dtPattern5BPXAddress, UE_BREAKPOINT, dtPattern5CallBack);
	}
	else
	{
		dtPattern51BPXAddress = Debugger::Find((void*)(fdLoadedBase + fdEntryPoint), cReadSize, dtPattern51, sizeof(dtPattern51), &glWildCard);
		if(dtPattern51BPXAddress)
		{
			dtPattern51BPXAddress += 3;
			Debugger::SetBPX(dtPattern51BPXAddress, UE_BREAKPOINT, dtPattern51CallBack);
		}
		else if(!DontLog)
		{
			AddLogMessage(_T("[Error] File is not packed with UPX 1.x - 3.x"));
			AddLogMessage(_T("-> Unpack ended..."));
			Debugger::StopDebug();
			DontLog = true;
		}
	}

	if(fdFileIsDll)
	{
		dtPattern6BPXAddress = Debugger::Find((void*)(fdLoadedBase + fdEntryPoint), cReadSize, dtPattern6, sizeof(dtPattern6), &glWildCard);
		if(dtPattern6BPXAddress)
		{
			dtPattern6BPXAddress -= 3;
			Debugger::SetBPX(dtPattern6BPXAddress, UE_BREAKPOINT, dtPattern6CallBack);
		}
		else if(!DontLog)
		{
				AddLogMessage(_T("[Error] File is not packed with UPX 1.x - 3.x"));
				AddLogMessage(_T("-> Unpack ended..."));
				Debugger::StopDebug();
				DontLog = true;
		}
	}
}

bool MapFileEx(TCHAR* szFileName, DWORD ReadOrWrite, LPHANDLE FileHandle, LPDWORD FileSize, LPHANDLE FileMap, LPVOID* FileMapVA, DWORD SizeModifier)
{
	HANDLE hFile = 0;
	DWORD FileAccess = 0;
	DWORD FileMapType = 0;
	DWORD FileMapViewType = 0;
	DWORD mfFileSize = 0;
	HANDLE mfFileMap = 0;
	LPVOID mfFileMapVA = 0;

	switch(ReadOrWrite)
	{
	case UE_ACCESS_READ:
		FileAccess = GENERIC_READ;
		FileMapType = 2;
		FileMapViewType = 4;
		break;
	case UE_ACCESS_WRITE:
		FileAccess = GENERIC_WRITE;
		FileMapType = 4;
		FileMapViewType = 2;
		break;
	case UE_ACCESS_ALL:
	default:
		FileAccess = GENERIC_READ | GENERIC_WRITE;
		FileMapType = 4;
		FileMapViewType = 2;
		break;
	}

	hFile = CreateFile(szFileName, FileAccess, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		*FileHandle = hFile;
		mfFileSize = GetFileSize(hFile, NULL);
		mfFileSize = mfFileSize + SizeModifier;
		*FileSize = mfFileSize;
		mfFileMap = CreateFileMapping(hFile, NULL, FileMapType, NULL, mfFileSize, NULL);
		if(mfFileMap != NULL)
		{
			*FileMap = mfFileMap;
			mfFileMapVA = MapViewOfFile(mfFileMap, FileMapViewType, NULL, NULL, NULL);
			if(mfFileMapVA != NULL)
			{
				*FileMapVA = mfFileMapVA;
				return true;
			}
		}
		*FileMapVA = NULL;
		*FileHandle = NULL;
		*FileSize = NULL;
		CloseHandle(hFile);
	}
	else
	{
		*FileMapVA = NULL;
	}
	return false;
}

void UnMapFileEx(HANDLE FileHandle, DWORD FileSize, HANDLE FileMap, LPCVOID FileMapVA)
{
	UnmapViewOfFile(FileMapVA);
	CloseHandle(FileMap);
	SetFilePointer(FileHandle, FileSize, NULL, FILE_BEGIN);
	SetEndOfFile(FileHandle);
	CloseHandle(FileHandle);
}
