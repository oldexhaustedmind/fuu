// myUnpacker.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "myUnpacker.h"
#include "sdk\SDK.h"
#include <vector>

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0'" \
						"processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"") 

#define UE_ACCESS_READ 0
#define UE_ACCESS_WRITE 1
#define UE_ACCESS_ALL 2
#define ID_TIMEREVENT 101
// Global Variables:
HWND BoxHandle;
HWND WindowHandle;
bool UnpackerRunning = false;
char GlobalBuffer[MAX_PATH] = {};
HMODULE hInstance = GetModuleHandleA(NULL);
LPPROCESS_INFORMATION fdProcessInfo = NULL;
bool dtSecondSnapShootOnEP = false;
void* cbInitCallBack = NULL;

typedef struct MEMORY_COMPARE_HANDLER{
	union {
		BYTE bArrayEntry[1];		
		WORD wArrayEntry[1];
		DWORD dwArrayEntry[1];
		DWORD64 qwArrayEntry[1];
	} Array;
}MEMORY_COMPARE_HANDLER, *PMEMORY_COMPARE_HANDLER;

typedef struct AlexProt_SectionData{
	DWORD SectionVirtualOffset;
	DWORD SectionVirtualSize;
}AlexProt_SectionData, *PAlexProt_SectionData;

typedef struct AlexProt_API_Entry{
	char* szAPIName;
	DWORD EliminatedAddress;
}AlexProt_API_Entry, *PAlexProt_API_Entry;

// Global Functions:
void InitializeUnpacker(char* szFileName, void* CallBack);
void AddLogMessage(char*);
void GetFileDialog();

// Unpacker Data:
std::vector<AlexProt_API_Entry> apiEntry;

long hlpHashString(char* szStringToHash){

	int i = NULL;
	DWORD HashValue = NULL;

	if(szStringToHash != NULL){
		for(i = 0; i < lstrlenA(szStringToHash); i++){
			HashValue = (((HashValue << 7) | (HashValue >> (32 - 7))) ^ szStringToHash[i]);
		}
	}
	return(HashValue);
}
// Forward declarations of functions included in this code module:
INT_PTR CALLBACK WndProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){

	char szDlgTitle[] = "[AlexProtector Unpacker]";
	char szAboutText[] = "RL!deAlexProtector 1.0 unpacker \r\n\r\n Visit Reversing Labs at http://www.reversinglabs.com \r\n\r\n  Minimum engine version needed:\r\n- TitanEngine 2.0 by RevLabs\r\n\r\nUnpacker coded by Reversing Labs";
	char szAboutTitle[] = "[ About ]";

	if(uMsg == WM_INITDIALOG){
		SendMessageA(hwndDlg, WM_SETTEXT, NULL, (LPARAM)&szDlgTitle);
		SendMessageA(hwndDlg, WM_SETICON, NULL, (LPARAM)LoadIconA((HINSTANCE)hInstance, MAKEINTRESOURCEA(IDI_ICON1)));
		SetDlgItemTextA(hwndDlg, IDC_FILENAME, "filename.exe");
		CheckDlgButton(hwndDlg, IDC_REALING, 1);
		WindowHandle = hwndDlg;
	}else if(uMsg == WM_DROPFILES){
		DragQueryFileA((HDROP)wParam, NULL, GlobalBuffer, MAX_PATH);
		SetDlgItemTextA(hwndDlg, IDC_FILENAME, GlobalBuffer);
	}else if(uMsg == WM_CLOSE){
		EndDialog(hwndDlg, NULL);
	}else if(uMsg == WM_COMMAND){
		if(wParam == IDC_UNPACK){
			if(!UnpackerRunning){
				UnpackerRunning = true;
				BoxHandle = GetDlgItem(hwndDlg, IDC_LISTBOX);
				SendMessageA(BoxHandle, LB_RESETCONTENT, NULL, NULL);
				InitializeUnpacker(GlobalBuffer, NULL);
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

void GetFileDialog(){

	OPENFILENAMEA sOpenFileName;
	char szFilterString[] = "All Files \0*.*\0\0";
	char szDialogTitle[] = "RL!deAlexProtector 1.0 from Reversing Labs";

	RtlZeroMemory(&sOpenFileName, sizeof(OPENFILENAMEA)); 
	sOpenFileName.lStructSize = sizeof(OPENFILENAMEA);
	sOpenFileName.lpstrFilter = &szFilterString[0];
	sOpenFileName.lpstrFile = &GlobalBuffer[0];
	sOpenFileName.nMaxFile = MAX_PATH;
	sOpenFileName.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_HIDEREADONLY;
	sOpenFileName.lpstrTitle = &szDialogTitle[0];
	if(!GetOpenFileNameA(&sOpenFileName)){
		RtlZeroMemory(&GlobalBuffer[0], MAX_PATH);
	}
}
void AddLogMessage(char* szLogMessage){

	int cSelect;

	SendMessageA(BoxHandle, LB_ADDSTRING, NULL, (LPARAM)szLogMessage);
	cSelect = (int)SendMessageA(BoxHandle, LB_GETCOUNT, NULL, NULL);
	cSelect--;
	SendMessageA(BoxHandle, LB_SETCURSEL, (WPARAM)cSelect, NULL);
}
void InitializeUnpacker(char* szFileName, void* CallBack){

	int i;
	DWORD FileSize;
	HANDLE FileMap;
	HANDLE FileHandle;
	ULONG_PTR FileMapVA;
	DWORD ItemTableSize;
	DWORD NewSectionSize;
	DWORD ItemStringSize;
	DWORD ItemWriteAddress;
	DWORD AlexProtectorDelta;
	DWORD AlexProtectorEPAddress;
	void* AlexProtectorSectionData;
	DWORD AlexProtectorDecryptSize;
	DWORD AlexProtectorDecryptedSize;
	DWORD AlexProtectorVirtualIAT = -4;
	ULONG_PTR AlexProtectorCompressedData;
	char szUnpackedFileName[MAX_PATH];
	PMEMORY_COMPARE_HANDLER myMemoryDecryptor;
	PAlexProt_SectionData myAlexSection;
	FILE_STATUS_INFO myFileStatus = {};
	AlexProt_API_Entry myAPIEntry = {};
	char LoggingBuffer[1024] = {};
	PE32Struct myPEData = {};

	apiEntry.clear();
	if(szFileName != NULL){
		AddLogMessage("-> Unpack started...");
		lstrcpyA(szUnpackedFileName, szFileName);
		lstrcatA(szUnpackedFileName, "_unpacked.exe");
		if(IsPE32FileValidEx(szFileName, UE_DEPTH_DEEP, &myFileStatus)){
			if(StaticFileLoad(szFileName, UE_ACCESS_READ, true, &FileHandle, &FileSize, &FileMap, &FileMapVA) && GetPE32DataEx(szFileName, &myPEData)){
				if(myPEData.NtSizeOfImage % myPEData.SectionAligment != NULL){
					myPEData.NtSizeOfImage = (((myPEData.NtSizeOfImage / myPEData.SectionAligment) + 1) * myPEData.SectionAligment);
				}
				DumpMemory(GetCurrentProcess(), (void*)FileMapVA, myPEData.NtSizeOfImage, szUnpackedFileName);
				StaticFileUnload(szFileName, false, FileHandle, FileSize, FileMap, FileMapVA);
				AddLogMessage("[x] Image has been resized and realigned");
				if(StaticFileLoad(szUnpackedFileName, UE_ACCESS_ALL, false, &FileHandle, &FileSize, &FileMap, &FileMapVA)){
					SetPE32DataForMappedFile(FileMapVA, NULL, UE_SIZEOFIMAGE, myPEData.NtSizeOfImage);
					for(i = 0; i < myPEData.SectionNumber; i++){
						NewSectionSize = (DWORD)GetPE32DataFromMappedFile(FileMapVA, i, UE_SECTIONVIRTUALSIZE);
						if(NewSectionSize % myPEData.SectionAligment != NULL){
							NewSectionSize = (((NewSectionSize / myPEData.SectionAligment) + 1) * myPEData.SectionAligment);
						}
						SetPE32DataForMappedFile(FileMapVA, i, UE_SECTIONRAWOFFSET, (ULONG_PTR)GetPE32DataFromMappedFile(FileMapVA, i, UE_SECTIONVIRTUALOFFSET));
						SetPE32DataForMappedFile(FileMapVA, i, UE_SECTIONRAWSIZE, NewSectionSize);
					}
					AddLogMessage("[x] Image header has been corrected");
					__try{
						AlexProtectorDelta = (DWORD)GetPE32DataFromMappedFile(FileMapVA, NULL, UE_OEP) - 0x1000;
						myAlexSection = (PAlexProt_SectionData)ConvertVAtoFileOffset(FileMapVA, AlexProtectorDelta + 0x4023C1, true);
						while(myAlexSection->SectionVirtualOffset != NULL){
							if(myAlexSection->SectionVirtualOffset < myPEData.NtSizeOfImage && myAlexSection->SectionVirtualSize < myPEData.NtSizeOfImage){
								AlexProtectorSectionData = VirtualAlloc(NULL, myAlexSection->SectionVirtualSize, MEM_COMMIT, PAGE_READWRITE);
								AlexProtectorCompressedData = (ULONG_PTR)GetPE32DataFromMappedFile(FileMapVA, GetPE32SectionNumberFromVA(FileMapVA, (ULONG_PTR)(myAlexSection->SectionVirtualOffset + myPEData.ImageBase)), UE_SECTIONRAWOFFSET) + FileMapVA;
								if(!StaticMemoryDecompress((void*)AlexProtectorCompressedData, myAlexSection->SectionVirtualSize, AlexProtectorSectionData, myAlexSection->SectionVirtualSize, UE_STATIC_APLIB)){
									StaticFileUnload(szUnpackedFileName, false, FileHandle, FileSize, FileMap, FileMapVA);
									AddLogMessage("[!] Error while decompression!");
									AddLogMessage("-> Unpack ended...");
									DeleteFileA(szUnpackedFileName);
									return;
								}
								RtlMoveMemory((void*)AlexProtectorCompressedData, AlexProtectorSectionData, myAlexSection->SectionVirtualSize);
								VirtualFree(AlexProtectorSectionData, NULL, MEM_RELEASE);
								myAlexSection = (PAlexProt_SectionData)((ULONG_PTR)myAlexSection + sizeof AlexProt_SectionData);
							}else{
								StaticFileUnload(szUnpackedFileName, false, FileHandle, FileSize, FileMap, FileMapVA);
								AddLogMessage("[!] Error while decompression!");
								AddLogMessage("-> Unpack ended...");
								DeleteFileA(szUnpackedFileName);
								return;
							}
						}
					}__except(1){
						StaticFileUnload(szUnpackedFileName, false, FileHandle, FileSize, FileMap, FileMapVA);
						AddLogMessage("[!] Error while decompression!");
						AddLogMessage("-> Unpack ended...");
						DeleteFileA(szUnpackedFileName);
						return;
					}
					AddLogMessage("[x] Decrypting import data");
					myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)ConvertVAtoFileOffset(FileMapVA, AlexProtectorDelta + 0x4023B9, true);
					AlexProtectorDecryptSize = myMemoryDecryptor->Array.dwArrayEntry[0];
					myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)ConvertVAtoFileOffset(FileMapVA, AlexProtectorDelta + 0x4023B5, true);
					myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)(ConvertVAtoFileOffset(FileMapVA, AlexProtectorDelta + 0x402531, true)) + myMemoryDecryptor->Array.dwArrayEntry[0]);
					AlexProtectorCompressedData = (ULONG_PTR)myMemoryDecryptor;
					i = AlexProtectorDecryptSize;
					while(i > NULL){
						myMemoryDecryptor->Array.bArrayEntry[0] = myMemoryDecryptor->Array.bArrayEntry[0] ^ 0x7D;
						myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)myMemoryDecryptor + 1);
						i--;
					}
					AddLogMessage("[x] Decompressing import data");
					AlexProtectorSectionData = VirtualAlloc(NULL, AlexProtectorDecryptSize * 10, MEM_COMMIT, PAGE_READWRITE);
					if(StaticMemoryDecompress((void*)AlexProtectorCompressedData, AlexProtectorDecryptSize, AlexProtectorSectionData, AlexProtectorDecryptSize * 10, UE_STATIC_APLIB)){
						myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)AlexProtectorSectionData;
						AddLogMessage("[x] Processing imports...");
						ImporterInit(50 * 1024, myPEData.ImageBase);
						while(myMemoryDecryptor->Array.bArrayEntry[0] != 0x00){
							if(myMemoryDecryptor->Array.bArrayEntry[0] == 0xC3){
								myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)myMemoryDecryptor + 1);
								ItemStringSize = myMemoryDecryptor->Array.bArrayEntry[0];
								myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)myMemoryDecryptor + 1);
								ImporterAddNewDll((char*)myMemoryDecryptor, NULL);
								wsprintfA(LoggingBuffer, "[+] Library: %s", (char*)myMemoryDecryptor);
								AddLogMessage(LoggingBuffer);
								myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)myMemoryDecryptor + ItemStringSize + 1);
								AlexProtectorVirtualIAT = AlexProtectorVirtualIAT + 4;
							}else if(myMemoryDecryptor->Array.bArrayEntry[0] == 0xC4){
								myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)myMemoryDecryptor + 1);
								ImporterAddNewOrdinalAPI(myMemoryDecryptor->Array.dwArrayEntry[0], AlexProtectorVirtualIAT);
								myAPIEntry.szAPIName = (char*)myMemoryDecryptor->Array.dwArrayEntry[0];
								wsprintfA(LoggingBuffer, "  Function: %08X", myAPIEntry.szAPIName);
								AddLogMessage(LoggingBuffer);
								myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)myMemoryDecryptor + 4);
								ItemTableSize = myMemoryDecryptor->Array.bArrayEntry[0];
								myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)myMemoryDecryptor + 1);
								while((int)ItemTableSize > NULL){
									myAPIEntry.EliminatedAddress = myMemoryDecryptor->Array.dwArrayEntry[0];
									myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)myMemoryDecryptor + 4);
									apiEntry.push_back(myAPIEntry);
									ItemTableSize--;
								}
								AlexProtectorVirtualIAT = AlexProtectorVirtualIAT + 4;
							}else{
								ItemStringSize = myMemoryDecryptor->Array.bArrayEntry[0];
								myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)myMemoryDecryptor + 1);
								myAPIEntry.szAPIName = (char*)myMemoryDecryptor;
								wsprintfA(LoggingBuffer, "  Function: %s", myAPIEntry.szAPIName);
								AddLogMessage(LoggingBuffer);
								ImporterAddNewAPI((char*)myMemoryDecryptor, AlexProtectorVirtualIAT);
								myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)myMemoryDecryptor + ItemStringSize + 1);
								ItemTableSize = myMemoryDecryptor->Array.bArrayEntry[0];
								myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)myMemoryDecryptor + 1);
								while((int)ItemTableSize > NULL){
									myAPIEntry.EliminatedAddress = myMemoryDecryptor->Array.dwArrayEntry[0];
									myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)myMemoryDecryptor + 4);
									apiEntry.push_back(myAPIEntry);
									ItemTableSize--;
								}
								AlexProtectorVirtualIAT = AlexProtectorVirtualIAT + 4;
							}
						}
						ImporterMoveIAT();
						ImporterRelocateWriteLocation(myPEData.NtSizeOfImage + myPEData.ImageBase);
						for(i = 0; i < (int)apiEntry.size(); i++){
							myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)ConvertVAtoFileOffset(FileMapVA, apiEntry[i].EliminatedAddress, true));
							ItemWriteAddress = (ULONG_PTR)ImporterFindAPIWriteLocation(apiEntry[i].szAPIName);
							myMemoryDecryptor->Array.dwArrayEntry[0] = ItemWriteAddress;
						}
						StaticFileUnload(szUnpackedFileName, false, FileHandle, FileSize, FileMap, FileMapVA);
						AddLogMessage("[x] Exporting imports");
						ImporterExportIATEx(szUnpackedFileName, ".alexIAT");
						VirtualFree(AlexProtectorSectionData, NULL, MEM_RELEASE);

						AddLogMessage("[x] Processing entry point data");
						if(StaticFileLoad(szUnpackedFileName, UE_ACCESS_ALL, false, &FileHandle, &FileSize, &FileMap, &FileMapVA)){
							myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)ConvertVAtoFileOffset(FileMapVA, AlexProtectorDelta + 0x402385, true);
							AlexProtectorEPAddress = myMemoryDecryptor->Array.dwArrayEntry[0];
							wsprintfA(LoggingBuffer, "[x] Resumed entry point at: %08X", AlexProtectorEPAddress);
							AddLogMessage(LoggingBuffer);
							myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)ConvertVAtoFileOffset(FileMapVA, AlexProtectorDelta + 0x402395, true);
							AlexProtectorDecryptSize = myMemoryDecryptor->Array.dwArrayEntry[0];
							myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)ConvertVAtoFileOffset(FileMapVA, AlexProtectorDelta + 0x402391, true);
							AlexProtectorDecryptedSize = myMemoryDecryptor->Array.dwArrayEntry[0];
							myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)ConvertVAtoFileOffset(FileMapVA, AlexProtectorDelta + 0x4023B9, true);
							ItemTableSize = myMemoryDecryptor->Array.dwArrayEntry[0];
							myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)ConvertVAtoFileOffset(FileMapVA, AlexProtectorDelta + 0x4023B5, true);
							myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)(ConvertVAtoFileOffset(FileMapVA, AlexProtectorDelta + 0x402531, true)) + myMemoryDecryptor->Array.dwArrayEntry[0] + ItemTableSize);
							AlexProtectorSectionData = VirtualAlloc(NULL, AlexProtectorDecryptedSize, MEM_COMMIT, PAGE_READWRITE);
							AlexProtectorCompressedData = (ULONG_PTR)myMemoryDecryptor;
							if(StaticMemoryDecompress((void*)AlexProtectorCompressedData, AlexProtectorDecryptSize, AlexProtectorSectionData, AlexProtectorDecryptedSize, UE_STATIC_APLIB)){
								AddLogMessage("[x] Writing stolen entry redirection data");
								myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)AlexProtectorSectionData + AlexProtectorDecryptedSize);
								myMemoryDecryptor->Array.bArrayEntry[0] = 0x68;
								myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)myMemoryDecryptor + 1);
								myMemoryDecryptor->Array.dwArrayEntry[0] = AlexProtectorEPAddress;
								myMemoryDecryptor = (PMEMORY_COMPARE_HANDLER)((ULONG_PTR)myMemoryDecryptor + 4);
								myMemoryDecryptor->Array.bArrayEntry[0] = 0xC3;
								StaticFileUnload(szUnpackedFileName, false, FileHandle, FileSize, FileMap, FileMapVA);
								SetPE32Data(szUnpackedFileName, NULL, UE_OEP, (ULONG_PTR)AddNewSectionEx(szUnpackedFileName, ".alexEP", AlexProtectorDecryptSize, NULL, AlexProtectorSectionData, AlexProtectorDecryptedSize + 6));
								if(StaticFileLoad(szUnpackedFileName, UE_ACCESS_ALL, false, &FileHandle, &FileSize, &FileMap, &FileMapVA)){
									if(IsDlgButtonChecked(WindowHandle, IDC_REALING)){
										FileSize = RealignPE(FileMapVA, FileSize, 2);
										AddLogMessage("[x] Realigning file!");
									}
									StaticFileUnload(szUnpackedFileName, false, FileHandle, FileSize, FileMap, FileMapVA);
								}
								AddLogMessage("[x] File has been unpacked");
							}else{
								StaticFileUnload(szUnpackedFileName, false, FileHandle, FileSize, FileMap, FileMapVA);
								AddLogMessage("[!] Error while decompression!");
								AddLogMessage("-> Unpack ended...");
								DeleteFileA(szUnpackedFileName);
								return;
							}
							VirtualFree(AlexProtectorSectionData, NULL, MEM_RELEASE);
						}
					}else{
						StaticFileUnload(szUnpackedFileName, false, FileHandle, FileSize, FileMap, FileMapVA);
						AddLogMessage("[!] Error while decompression!");
						AddLogMessage("-> Unpack ended...");
						DeleteFileA(szUnpackedFileName);
						return;
					}
					AddLogMessage("-> Unpack ended...");
				}
			}
		}else{
			AddLogMessage("[Error] Selected file is not a valid PE32 file!");
			AddLogMessage("-> Unpack ended...");
		}
	}
}
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow){

	DialogBoxParamA(hInstance, MAKEINTRESOURCEA(IDD_MAINWINDOW), NULL, (DLGPROC)WndProc, NULL);
	ExitProcess(NULL);
}