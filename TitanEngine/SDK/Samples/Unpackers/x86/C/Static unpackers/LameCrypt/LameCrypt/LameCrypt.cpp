// LameCrypt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include "SDK\SDK.h"


int main(int argc, char* argv[]){

	HANDLE FileHandle;
	DWORD FileSize;
	HANDLE FileMap;
	ULONG_PTR FileMapVA;
	DWORD UnpackedOEP;
	char szOriginalFile[MAX_PATH] = {};

	lstrcpyA(szOriginalFile, argv[1]);
	lstrcatA(szOriginalFile, ".bak");
	printf("Unpacker for LameCrypt 1.0\r\nfrom ReversingLabs Corporation - www.reversinglabs.com\r\n\r\n");
	if(CopyFileA(argv[1], szOriginalFile, false)){
		if(StaticFileLoad(argv[1], UE_ACCESS_ALL, false, &FileHandle, &FileSize, &FileMap, &FileMapVA)){
			RtlMoveMemory(&UnpackedOEP, (void*)(ConvertVAtoFileOffset(FileMapVA, (ULONG_PTR)GetPE32DataFromMappedFile(FileMapVA, NULL, UE_OEP) + (ULONG_PTR)GetPE32DataFromMappedFile(FileMapVA, NULL, UE_IMAGEBASE), true) + 0x19), 4);
			UnpackedOEP = UnpackedOEP - (DWORD)GetPE32DataFromMappedFile(FileMapVA, NULL, UE_IMAGEBASE);
			StaticSectionDecrypt(FileMapVA, NULL, false, UE_STATIC_DECRYPTOR_XOR, UE_STATIC_KEY_SIZE_1, 0x90);
			SetPE32DataForMappedFile(FileMapVA, NULL, UE_OEP, (ULONG_PTR)UnpackedOEP);
			StaticFileUnload(argv[1], true, FileHandle, FileSize, FileMap, FileMapVA);
			DeleteLastSection(argv[1]);
			printf("Selected file has been unpacked!\r\n");
		}else{
			printf("Could not find selected input file!\r\n");
		}
	}else{
		printf("Could not backup selected input file!\r\n");
	}
	return 0;
}

