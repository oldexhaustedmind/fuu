// LameCrypt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include "SDK\SDK.h"

char szDEBHeaderSignature[] = "!<arch>\n";
char szDEBBinaryData[] = "debian-binary";

typedef struct DEB_HEADER{
    char FileName[16];
    char FileTime[12];
    char Reserved0[6];
    char Reserved1[6];
    char Mode[8];
    char ItemSize[10];
    char TerminateQuote;
    char TerminateNewLine;
}DEB_HEADER, *PDEB_HEADER;

long GetDEBFileNameLengthA(char* szFileName){

	int i = NULL;
	DEB_HEADER DEBHeader;

	while(szFileName[i] != ' ' && szFileName[i] != 0x00 && i <= sizeof DEBHeader.FileName){
		i++;
	}
	return(i--);
}

int main(int argc, char* argv[]){

	DWORD FileSize;
	DWORD FileSizeHigh;
	HANDLE FileHandle;
	DWORD currentReadPointer;
	char szDEBHeaderSignatureCheck[sizeof szDEBHeaderSignature - 1] = {};
	char szExtractFilePath[MAX_PATH * 2] = {};
	char szExtractFileName[MAX_PATH] = {};
	DEB_HEADER DEBHeader = {};

	printf("Unpacker for Deb 1.0\r\nfrom ReversingLabs Corporation - www.reversinglabs.com\r\n Usage: Deb inputFileName.deb x:\\OutputFolder\\\r\n\r\n");
	if(StaticFileOpen(argv[1], GENERIC_READ, &FileHandle, &FileSize, &FileSizeHigh)){
		__try{
			if(StaticFileGetContent(FileHandle, NULL, NULL, &szDEBHeaderSignatureCheck, sizeof szDEBHeaderSignature - 1)){
				if(memcmp(&szDEBHeaderSignatureCheck[0], &szDEBHeaderSignature[0], sizeof szDEBHeaderSignature - 1) == NULL){
					currentReadPointer = sizeof szDEBHeaderSignature - 1;
					printf("Extracting archive...\r\n");
					while(StaticFileGetContent(FileHandle, currentReadPointer, NULL, &DEBHeader, sizeof DEB_HEADER)){
						if(DEBHeader.FileName[0] != NULL && DEBHeader.FileName[1] != 0x00){
							if(DEBHeader.FileName[0] == 0x0A){
								if(!StaticFileGetContent(FileHandle, currentReadPointer + 1, NULL, &DEBHeader, sizeof DEB_HEADER)){
									break;
								}else{
									currentReadPointer++;
								}
							}
							if(memcmp(DEBHeader.FileName, &szDEBBinaryData[0], sizeof szDEBBinaryData - 1) != NULL){
								RtlZeroMemory(&szExtractFileName, sizeof szExtractFileName);
								RtlMoveMemory(&szExtractFileName[0], &DEBHeader.FileName[0], GetDEBFileNameLengthA(DEBHeader.FileName));
								lstrcpyA(szExtractFilePath, argv[2]);
								lstrcatA(szExtractFilePath, "\\");
								lstrcatA(szExtractFilePath, szExtractFileName);
								if(StaticRawMemoryCopyEx(FileHandle, currentReadPointer + sizeof DEBHeader, atoi(DEBHeader.ItemSize), szExtractFilePath)){
									printf(" Extracted file: %s\r\n", szExtractFileName);					
								}
							}
							currentReadPointer = currentReadPointer + sizeof DEBHeader + atoi(DEBHeader.ItemSize);
						}
					}
					StaticFileClose(FileHandle);
				}else{
					printf("Selected file isn't a DEB archive!\r\n");
					StaticFileClose(FileHandle);
					return 0;
				}
			}
		}__except(EXCEPTION_EXECUTE_HANDLER){
			printf("Critical error while unpacking!\r\n");
			StaticFileClose(FileHandle);
			return 0;
		}
		printf("Selected file has been unpacked!\r\n");
	}else{
		printf("Could not find selected input file!\r\n");
	}
	return 0;
}

