// LameCrypt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include "SDK\SDK.h"

BYTE CryptStub[] = {0x60,0x66,0x9C,0xBB,0x00,0x00,0x00,0x00,0x80,0xB3,0x00,0x00,0x00,0x00,
					0x90,0x4B,0x83,0xFB,0xFF,0x75,0xF3,0x66,0x9D,0x61,0xB8,0x00,0x00,0x00,0x00,0xFF,0xE0};

bool FillStub(DWORD EntryPoint, DWORD SectionOffset, DWORD SectionSize){

	RtlMoveMemory(&CryptStub[4], &SectionSize, sizeof SectionSize);
	RtlMoveMemory(&CryptStub[10], &SectionOffset, sizeof SectionOffset);
	RtlMoveMemory(&CryptStub[25], &EntryPoint, sizeof EntryPoint);
	return(true);
}

int main(int argc, char* argv[]){

	HANDLE FileHandle;
	DWORD FileSize;
	HANDLE FileMap;
	ULONG_PTR FileMapVA;
	DWORD EntryPoint;
	DWORD ImageBase;
	DWORD SectionOffset;
	DWORD SectionSize;
	DWORD OriginalEntryPoint;
	ULONG_PTR SectionWriteOffset;
	char szOriginalFile[MAX_PATH] = {};

	lstrcpyA(szOriginalFile, argv[1]);
	lstrcatA(szOriginalFile, ".bak");
	printf("LameCrypter 1.0\r\nfrom ReversingLabs Corporation - www.reversinglabs.com\r\n\r\n");
	if(CopyFileA(argv[1], szOriginalFile, false)){
		EntryPoint = AddNewSectionEx(argv[1], ".lame", 0x200, NULL, NULL, NULL);
		if(EntryPoint != NULL){
			if(StaticFileLoad(argv[1], UE_ACCESS_ALL, false, &FileHandle, &FileSize, &FileMap, &FileMapVA)){
				ImageBase = (DWORD)GetPE32DataFromMappedFile(FileMapVA, NULL, UE_IMAGEBASE);
				OriginalEntryPoint = (DWORD)GetPE32DataFromMappedFile(FileMapVA, NULL, UE_OEP);
				SectionSize = (DWORD)GetPE32DataFromMappedFile(FileMapVA, NULL, UE_SECTIONVIRTUALSIZE);
				SectionOffset = (DWORD)GetPE32DataFromMappedFile(FileMapVA, NULL, UE_SECTIONVIRTUALOFFSET);
				EntryPoint = EntryPoint + ImageBase;
				SectionOffset = SectionOffset + ImageBase;
				OriginalEntryPoint = OriginalEntryPoint + ImageBase;
				if(FillStub(OriginalEntryPoint, SectionOffset, SectionSize)){
					SetPE32DataForMappedFile(FileMapVA, NULL, UE_SECTIONFLAGS, 0xE0000020);
					SetPE32DataForMappedFile(FileMapVA, NULL, UE_OEP, (ULONG_PTR)(EntryPoint - ImageBase));
					StaticSectionDecrypt(FileMapVA, NULL, false, UE_STATIC_DECRYPTOR_XOR, UE_STATIC_KEY_SIZE_1, 0x90);
					SectionWriteOffset = (ULONG_PTR)ConvertVAtoFileOffset(FileMapVA, EntryPoint, true);
					RtlMoveMemory((void*)SectionWriteOffset, &CryptStub, sizeof CryptStub);
					StaticFileUnload(argv[1], true, FileHandle, FileSize, FileMap, FileMapVA);
					printf("Selected file has been crypted!\r\n");
				}else{
					StaticFileUnload(argv[1], true, FileHandle, FileSize, FileMap, FileMapVA);
					printf("Selected file could not be crypted!\r\n");
				}
			}else{
				printf("Could not find selected input file!\r\n");
			}
		}else{
			printf("Could not add new section to selected input file!\r\n");
		}
	}else{
		printf("Could not backup selected input file!\r\n");
	}
	return 0;
}

