// myUnpacker.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "myUnpacker.h"
#include "sdk\SDK.h"

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0'" \
						"processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"") 

#define UE_ACCESS_READ 0
#define UE_ACCESS_WRITE 1
#define UE_ACCESS_ALL 2
#define ID_TIMEREVENT 101
// Global Variables:
HWND BoxHandle;
HWND WindowHandle;
long fdImageBase = NULL;
long fdLoadedBase = NULL;
long fdEntryPoint = NULL;
long fdSizeOfImage = NULL;
bool UnpackerRunning = false;
char GlobalBuffer[1024] = {};
char GlobalBackBuffer[1024] = {};
char GlobalTempBuffer[1024] = {};
char UnpackFileNameBuffer[1024] = {};
char GlobalUnpackerFolderBuffer[1024] = {};
HMODULE hInstance = GetModuleHandleA(NULL);
LPPROCESS_INFORMATION fdProcessInfo = NULL;
bool dtSecondSnapShootOnEP = false;
void* cbInitCallBack = NULL;

// Global Functions:
void GetFileDialog();
void InitializeUnpacker(char* szFileName);

// Unpacker Data:
bool MapFileEx(char* szFileName, DWORD ReadOrWrite, LPHANDLE FileHandle, LPDWORD FileSize, LPHANDLE FileMap, LPDWORD FileMapVA, DWORD SizeModifier){

	HANDLE hFile = 0;
	DWORD FileAccess = 0;
	DWORD FileMapType = 0;
	DWORD FileMapViewType = 0;
	DWORD mfFileSize = 0;
	HANDLE mfFileMap = 0;
	LPVOID mfFileMapVA = 0;

	if(ReadOrWrite == UE_ACCESS_READ){
		FileAccess = GENERIC_READ;
		FileMapType = 2;
		FileMapViewType = 4;
	}else if(ReadOrWrite == UE_ACCESS_WRITE){
		FileAccess = GENERIC_WRITE;
		FileMapType = 4;
		FileMapViewType = 2;
	}else if(ReadOrWrite == UE_ACCESS_ALL){
		FileAccess = GENERIC_READ+GENERIC_WRITE;
		FileMapType = 4;
		FileMapViewType = 2;
	}else{
		FileAccess = GENERIC_READ+GENERIC_WRITE;
		FileMapType = 4;
		FileMapViewType = 2;
	}

	hFile = CreateFileA(szFileName, FileAccess, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE){
		*FileHandle = hFile;
		mfFileSize = GetFileSize(hFile,NULL);
		mfFileSize = mfFileSize + SizeModifier;
		*FileSize = mfFileSize;
		mfFileMap = CreateFileMappingA(hFile, NULL, FileMapType, NULL, mfFileSize, NULL);
		if(mfFileMap != NULL){
			*FileMap = mfFileMap;
			mfFileMapVA = MapViewOfFile(mfFileMap, FileMapViewType, NULL, NULL, NULL);
			if(mfFileMapVA != NULL){
				*FileMapVA = (DWORD)mfFileMapVA;
				return(true);
			}
		}
		*FileMapVA = NULL;
		*FileHandle = NULL;
		*FileSize = NULL;
		CloseHandle(hFile);
	}else{
		*FileMapVA = NULL;
	}
	return(false);
}
void UnMapFileEx(HANDLE FileHandle, DWORD FileSize, HANDLE FileMap, DWORD FileMapVA){

	LPVOID ufFileMapVA = (LPDWORD)FileMapVA;

	UnmapViewOfFile(ufFileMapVA);
	CloseHandle(FileMap);
	SetFilePointer(FileHandle,FileSize,NULL,FILE_BEGIN);
	SetEndOfFile(FileHandle);
	CloseHandle(FileHandle);
}
// Forward declarations of functions included in this code module:
INT_PTR CALLBACK WndProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){

	char szDlgTitle[] = "[AHPack Unpacker]";
	char szAboutText[] = "RL!deAHPack unpacker \r\n\r\n Visit Reversing Labs at http://www.reversinglabs.com \r\n\r\n  Minimum engine version needed:\r\n- TitanEngine 2.0.2 by RevLabs\r\n\r\nUnpacker coded by Reversing Labs";
	char szAboutTitle[] = "[ About ]";

	if(uMsg == WM_INITDIALOG){
		SendMessageA(hwndDlg, WM_SETTEXT, NULL, (LPARAM)&szDlgTitle);
		SendMessageA(hwndDlg, WM_SETICON, NULL, (LPARAM)LoadIconA((HINSTANCE)hInstance, MAKEINTRESOURCEA(IDI_ICON1)));
		SetDlgItemTextA(hwndDlg, IDC_FILENAME, "filename.exe");
		CheckDlgButton(hwndDlg, IDC_REALING, 1);
		WindowHandle = hwndDlg;
	}else if(uMsg == WM_DROPFILES){
		DragQueryFileA((HDROP)wParam, NULL, GlobalBuffer, 1024);
		SetDlgItemTextA(hwndDlg, IDC_FILENAME, GlobalBuffer);
	}else if(uMsg == WM_CLOSE){
		EndDialog(hwndDlg, NULL);
	}else if(uMsg == WM_COMMAND){
		if(wParam == IDC_UNPACK){
			if(!UnpackerRunning){
				UnpackerRunning = true;
				BoxHandle = GetDlgItem(hwndDlg, IDC_LISTBOX);
				SendMessageA(BoxHandle, LB_RESETCONTENT, NULL, NULL);
				InitializeUnpacker(GlobalBuffer);
				UnpackerRunning = false;
			}
		}else if(wParam == IDC_BROWSE){
			GetFileDialog();
			if(GlobalBuffer[0] != 0x00){
				SetDlgItemTextA(hwndDlg, IDC_FILENAME, GlobalBuffer);
			}
		}else if(wParam == IDC_ABOUT){
			MessageBoxA(hwndDlg, szAboutText, szAboutTitle, MB_ICONASTERISK);
		}else if(wParam == IDC_EXIT){
			EndDialog(hwndDlg, NULL);
		}
	}
	return(NULL);
}
void GetUnpackerFolder(){

	int i;

	if(GetModuleFileNameA((HMODULE)hInstance, GlobalUnpackerFolderBuffer, 1024) > NULL){
		i = lstrlenA(GlobalUnpackerFolderBuffer);
		while(GlobalUnpackerFolderBuffer[i] != 0x5C){
			GlobalUnpackerFolderBuffer[i] = 0x00;
			i--;
		}
	}
}

void GetFileDialog(){

	OPENFILENAMEA sOpenFileName;
	char szFilterString[] = "All Files \0*.*\0\0";
	char szDialogTitle[] = "RL!deAHPack from Reversing Labs";

	RtlZeroMemory(&sOpenFileName, sizeof(OPENFILENAMEA)); 
	sOpenFileName.lStructSize = sizeof(OPENFILENAMEA);
	sOpenFileName.lpstrFilter = &szFilterString[0];
	sOpenFileName.lpstrFile = &GlobalBuffer[0];
	sOpenFileName.nMaxFile = 1024;
	sOpenFileName.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_HIDEREADONLY;
	sOpenFileName.lpstrTitle = &szDialogTitle[0];
	if(!GetOpenFileNameA(&sOpenFileName)){
		RtlZeroMemory(&GlobalBuffer[0], 1024);
	}
}
void AddLogMessage(char* szLogMessage){

	int cSelect;

	SendMessageA(BoxHandle, LB_ADDSTRING, NULL, (LPARAM)szLogMessage);
	cSelect = (int)SendMessageA(BoxHandle, LB_GETCOUNT, NULL, NULL);
	cSelect--;
	SendMessageA(BoxHandle, LB_SETCURSEL, (WPARAM)cSelect, NULL);
}
void InitializeUnpacker(char* szFileName){

	int i = NULL;
	int j = NULL;
	int k = NULL;
	bool Critical = false;
	long uSectionNumber;
	long OriginalFileSize;
	long uFirstSectionDelta;
	long uFirstSectionSize;
	long uFirstSectionRawSize;
	long uFileSectionAlignment;
	long uFirstSectionDataPointer;
	PIMAGE_IMPORT_DESCRIPTOR ImportIID;
	FILE_STATUS_INFO inFileStatus = {};
	void* DecompressedMemory;
	void* ResizeFileMemory;
	ULONG_PTR uReadLocation;
	ULONG_PTR uEPLocation;
	DWORD uIATLocation;
	DWORD uOEPLocation;
	ULONG_PTR FileMapVA;
	HANDLE FileHandle;
	DWORD FileSize;
	HANDLE FileMap;

	BYTE uIATPattern[] = {0x61,0xB9,0x00,0x00,0x00,0x00,0x8B,0x1C,0x08,0x89,0x99,0x00,0x00,0x00,0x00,0xE2,0xF5,0x90,0x90,0xBA,0x00,0x00,0x00,0x00,0xBE,0x00,0x00,0x00,0x00,0x01,0xD6};
	BYTE uOEPPattern[] = {0x8B,0x15,0x00,0x00,0x00,0x00,0x52,0xFF,0xD0,0x61};

	if(szFileName != NULL){
		AddLogMessage("-> Unpacking started...");
		if(IsPE32FileValidEx(szFileName, UE_DEPTH_DEEP, &inFileStatus)){
			fdImageBase = (long)GetPE32Data(szFileName, NULL, UE_IMAGEBASE);
			fdEntryPoint = (long)GetPE32Data(szFileName, NULL, UE_OEP);
			fdSizeOfImage = (long)GetPE32Data(szFileName, NULL, UE_SIZEOFIMAGE);

			lstrcpyA(GlobalBackBuffer, GlobalBuffer);
			i = lstrlenA(GlobalBackBuffer);
			while(GlobalBackBuffer[i] != 0x2E){
				i--;
			}
			GlobalBackBuffer[i] = 0x00;
			j = i + 1;
			while(GlobalBackBuffer[i] != 0x5C){
				i--;
			}
			i++;
			wsprintfA(GlobalTempBuffer, "%s.unpacked.%s", &GlobalBackBuffer[i], &GlobalBackBuffer[j]);
			GlobalBackBuffer[i] = 0x00;
			lstrcpyA(UnpackFileNameBuffer, GlobalBackBuffer);
			lstrcatA(UnpackFileNameBuffer, GlobalTempBuffer);
			
			if(CopyFileA(szFileName, UnpackFileNameBuffer, false)){
				if(StaticFileLoad(UnpackFileNameBuffer, UE_ACCESS_READ, false, &FileHandle, &FileSize, &FileMap, &FileMapVA)){
					uSectionNumber = (long)GetPE32DataFromMappedFile(FileMapVA, NULL, UE_SECTIONNUMBER);
					uFirstSectionSize = (long)GetPE32DataFromMappedFile(FileMapVA, NULL, UE_SECTIONVIRTUALSIZE);
					uFirstSectionRawSize = (long)GetPE32DataFromMappedFile(FileMapVA, NULL, UE_SECTIONRAWSIZE);
					uFirstSectionDataPointer = (long)GetPE32DataFromMappedFile(FileMapVA, NULL, UE_SECTIONRAWOFFSET);
					uFileSectionAlignment = (long)GetPE32DataFromMappedFile(FileMapVA, NULL, UE_SECTIONALIGNMENT);
					if(uFirstSectionSize % uFileSectionAlignment != 0){
						uFirstSectionSize = ((uFirstSectionSize / uFileSectionAlignment) + 1) * uFileSectionAlignment;
					}
					OriginalFileSize = FileSize;
					uFirstSectionDelta = uFirstSectionSize - uFirstSectionRawSize;
					DecompressedMemory = VirtualAlloc(NULL, (SIZE_T)uFirstSectionSize, MEM_COMMIT, PAGE_READWRITE);
					if(DecompressedMemory != NULL){
						if(StaticMemoryDecompress((void*)(uFirstSectionDataPointer + FileMapVA), (DWORD)uFirstSectionRawSize, DecompressedMemory, (DWORD)uFirstSectionSize, UE_STATIC_APLIB)){
							AddLogMessage("[+] Section memory has been decompressed");
							StaticFileUnload(UnpackFileNameBuffer, false, FileHandle, FileSize, FileMap, FileMapVA);
							if(MapFileEx(UnpackFileNameBuffer, UE_ACCESS_ALL, &FileHandle, &FileSize, &FileMap, &FileMapVA, uFirstSectionDelta)){
								ResizeFileMemory = VirtualAlloc(NULL, (SIZE_T)FileSize, MEM_COMMIT, PAGE_READWRITE);
								if(ResizeFileMemory != NULL){
									RtlMoveMemory(ResizeFileMemory, (void*)(FileMapVA + uFirstSectionDataPointer + uFirstSectionRawSize), OriginalFileSize - (uFirstSectionDataPointer + uFirstSectionRawSize));
									RtlZeroMemory((void*)(FileMapVA + uFirstSectionDataPointer + uFirstSectionRawSize), OriginalFileSize - (uFirstSectionDataPointer + uFirstSectionRawSize));
									RtlMoveMemory((void*)(FileMapVA + uFirstSectionDataPointer + uFirstSectionRawSize + uFirstSectionDelta), ResizeFileMemory, OriginalFileSize - (uFirstSectionDataPointer + uFirstSectionRawSize));
									RtlMoveMemory((void*)(FileMapVA + uFirstSectionDataPointer), DecompressedMemory, uFirstSectionSize);
									AddLogMessage("[+] Section memory has been moved to original location");
									for(int n = 0; n < uSectionNumber; n++){
										if(n == 0){
											SetPE32DataForMappedFile(FileMapVA, n, UE_SECTIONRAWSIZE, uFirstSectionSize);
										}else{
											SetPE32DataForMappedFile(FileMapVA, n, UE_SECTIONRAWOFFSET, (ULONG_PTR)(GetPE32DataFromMappedFile(FileMapVA, n, UE_SECTIONRAWOFFSET) + uFirstSectionDelta));
										}
									}
									AddLogMessage("[+] Section header has been realigned");
									VirtualFree(ResizeFileMemory, NULL, MEM_RELEASE);
									uEPLocation = (ULONG_PTR)ConvertVAtoFileOffset(FileMapVA, fdEntryPoint + fdImageBase, true);
									if(uEPLocation != NULL){
										uReadLocation = (ULONG_PTR)FindEx(GetCurrentProcess(), (void*)uEPLocation, 0x200, &uIATPattern[0], sizeof uIATPattern / sizeof uIATPattern[0], NULL);
										if(uReadLocation != NULL){
											k++;
											uReadLocation = uReadLocation + 0x19;
											RtlMoveMemory(&uIATLocation, (void*)uReadLocation, sizeof uIATLocation);
											ImportIID = (PIMAGE_IMPORT_DESCRIPTOR)ConvertVAtoFileOffset(FileMapVA, uIATLocation + fdImageBase, true);
											while(ImportIID->FirstThunk != NULL){
												ImportIID = (PIMAGE_IMPORT_DESCRIPTOR)((ULONG_PTR)ImportIID + sizeof IMAGE_IMPORT_DESCRIPTOR);
												k = k + sizeof IMAGE_IMPORT_DESCRIPTOR;
											}
											SetPE32DataForMappedFile(FileMapVA, NULL, UE_IMPORTTABLEADDRESS, uIATLocation);
											SetPE32DataForMappedFile(FileMapVA, NULL, UE_IMPORTTABLESIZE, k);
											AddLogMessage("[+] Import table has been fixed");
										}else{
											AddLogMessage("[Error] Could not get find pattern #1!");
											Critical = true;
										}
										uReadLocation = (ULONG_PTR)FindEx(GetCurrentProcess(), (void*)uEPLocation, 0x200, &uOEPPattern[0], sizeof uOEPPattern / sizeof uOEPPattern[0], NULL);
										if(uReadLocation != NULL){
											uReadLocation = uReadLocation + (sizeof uOEPPattern / sizeof uOEPPattern[0]) + 1;
											RtlMoveMemory(&uOEPLocation, (void*)uReadLocation, sizeof uOEPLocation);
											uOEPLocation = uOEPLocation - fdImageBase;
											SetPE32DataForMappedFile(FileMapVA, NULL, UE_OEP, uOEPLocation);
											AddLogMessage("[+] Original entry point has been set");
										}else{
											AddLogMessage("[Error] Could not get find pattern #2!");
											Critical = true;
										}
									}else{
										AddLogMessage("[Error] Could not get entry point address!");
										Critical = true;
									}
								}else{
									AddLogMessage("[Error] Could not allocate needed memory!");
									Critical = true;
								}
								VirtualFree(DecompressedMemory, NULL, MEM_RELEASE);
								UnMapFileEx(FileHandle, FileSize, FileMap, FileMapVA);
								if(!DeleteLastSection(UnpackFileNameBuffer)){
									AddLogMessage("[Error] Could not delete last section!");
									Critical = true;
								}else if(!Critical){
									AddLogMessage("[+] File has been unpacked!");
								}
							}
						}else{
							AddLogMessage("[Error] Could not decompress memory!");
							Critical = true;
						}
					}else{
						StaticFileUnload(UnpackFileNameBuffer, false, FileHandle, FileSize, FileMap, FileMapVA);
						AddLogMessage("[Error] Could not allocate needed memory!");
						Critical = true;
					}
				}else{
					AddLogMessage("[Error] Could not find the selected file!");
				}
			}else{
				AddLogMessage("[Error] Could not find the selected file!");
			}
		}else{
			AddLogMessage("[Error] Selected file is not a valid PE32 file!");
		}
		if(Critical){
			DeleteFileA(UnpackFileNameBuffer);
		}
		AddLogMessage("-> Unpack ended...");
	}
}
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow){

	GetUnpackerFolder();
	DialogBoxParamA(hInstance, MAKEINTRESOURCEA(IDD_MAINWINDOW), NULL, (DLGPROC)WndProc, NULL);
	ExitProcess(NULL);
}