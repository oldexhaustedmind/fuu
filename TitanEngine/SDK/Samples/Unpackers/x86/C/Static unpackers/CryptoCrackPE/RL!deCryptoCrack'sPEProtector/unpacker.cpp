
#include "unpacker.h"

HWND					cUnpacker::lLog;
char					cUnpacker::lPath[MAX_PATH], cUnpacker::lOut[MAX_PATH];

ulong					cUnpacker::lMap, cUnpacker::lKeyA, cUnpacker::lKeyB, cUnpacker::lFirstByte;

puchar					cUnpacker::lDecryption;

long __stdcall FindLocal(LPVOID MemoryStart, DWORD MemorySize, LPVOID SearchPattern, DWORD PatternSize, LPBYTE WildCard)
{
	long Return = 0;

	for(ulong i = 0; i < MemorySize && !Return; i++)
	{
		ulong j;
		for(j = 0; j < PatternSize; j++)
			if(LPBYTE(SearchPattern)[j] != *WildCard && LPBYTE(MemoryStart)[i + j] != LPBYTE(SearchPattern)[j])
				break;

		i += j;
		if(j == PatternSize)
			Return = long(MemoryStart) + i - PatternSize;
	}

	return Return;
}

void cUnpacker::Log(const pchar pFormat, ...)
{
	va_list ArgList;
	va_start(ArgList, pFormat);

	char Buffer[iMax];
	StringCbVPrintfA(Buffer, iMax, pFormat, ArgList);

	SendMessageA(lLog, LB_ADDSTRING, 0, LPARAM(Buffer));
	SendMessageA(lLog, LB_SETCURSEL, WPARAM(SendMessageA(lLog, LB_GETCOUNT, 0, 0) - 1), 0);

	va_end(ArgList);
}

bool __declspec(naked) cUnpacker::CurrentDecryption(pvoid pStart)
{
	__asm
	{
		xor ecx, ecx

		mov esi, pStart
		mov edi, esi

		mov eax, lKeyA
		mov edx, lKeyB

		call lDecryption

		mov lKeyA, eax
		xor eax, eax

		cmp lKeyB, edx
		setne al

		mov lKeyB, edx
		retn
	}
}

bool __stdcall cUnpacker::CustomDecryption(pvoid pStart, int)
{
	if(ulong(pStart) == lFirstByte) return false;
	return CurrentDecryption(pStart);
}

bool cUnpacker::Unpack(pchar pPath)
{
	bool Return = false;

	FILE_STATUS_INFO Status;
	ZeroMemory(&Status, sizeof Status);

	StringCbCopyA(lPath, MAX_PATH, pPath);

	ulong ExtIndex = lstrlenA(lPath) - 1;
	while(ExtIndex > 0 && lPath[ExtIndex] != '.') ExtIndex--;

	if(ExtIndex)
	{
		lPath[ExtIndex] = 0;
		ExtIndex++;

		StringCbPrintfA(lOut, MAX_PATH, "%s.unpacked.%s", lPath, &lPath[ExtIndex]);
		lPath[ExtIndex - 1] = '.';

		DeleteFileA(lOut);
		Log("-> Unpack started...");

		ulong BytesSaved = 0;
		if(CopyFileA(lPath, lOut, true) && IsPE32FileValidEx(lPath, UE_DEPTH_DEEP, &Status))
		{
			pvoid Handle, Mapping;
			ulong Size;

			if(StaticFileLoad(lOut, UE_ACCESS_ALL, false, &Handle, &Size, &Mapping, &lMap))
			{
				bool Success = false;

				ulong Count = ulong(GetPE32DataFromMappedFile(lMap, 0, UE_SECTIONNUMBER));
				ulong Base	= ulong(GetPE32DataFromMappedFile(lMap, 0, UE_IMAGEBASE));

				const uchar SectionBegin[]	= "\x8b\xf3\x03\x77\x0c\x8b\x4f\x10\xb8\xCC\xCC\xCC\xCC\xba",
							SectionEnd[]	= "\xeb\xCC\xc3\x61\x83\xc7\x28\x49\x75",
							ImportsBegin[]	= "\x8d\x7c\x03\x02\xb8\xCC\xCC\xCC\xCC\xba",
							ImportsEnd[]	= "\xeb\xCC\xc3\x83\xc4\x0c\xe9",
							XorSection[]	= "\x30\x04\x31",
							XorImports[]	= "\x30\x04\x39",
							ImportVA[]		= "\x81\x04\x24\xCC\xCC\xCC\xCC\xbe\xCC\xCC\xCC\xCC\xCC\xCC\xff\xCC\xCC\x01\xCC\xCC\xff";

				uchar		WildCard		= 0xCC;

				enum
				{
					iSizeThreshold		= 0x70,

					iSecKeyAOffset		= 9,
					iSecKeyBOffset		= 14,

					iImpKeyAOffset		= 5,
					iImpKeyBOffset		= 10,

					iRetn				= 0xc3,
					iSectionNameMax		= 8,

					iCondJmpOff			= 8,
					iCondJmpAlign		= 2,

					iImportVAOff		= 8,
					iIsOrdinal			= 0x80000000,

					iMaxAPILength		= 256,

					iTlsDiffIndicator	= 8,
					iTlsDirSize			= 0x18
				};

				const ulong	SectionBeginSize	= sizeof SectionBegin	/ sizeof *SectionBegin	- 1,
							SectionEndSize		= sizeof SectionEnd		/ sizeof *SectionEnd	- 1,
							ImportsBeginSize	= sizeof ImportsBegin	/ sizeof *ImportsBegin	- 1,
							ImportsEndSize		= sizeof ImportsEnd		/ sizeof *ImportsEnd	- 1,
							XorSize				= sizeof XorSection		/ sizeof *XorSection	- 1,
							ImportVASize		= sizeof ImportVA		/ sizeof *ImportVA		- 1;

				ulong BeginS = 0, BeginI = 0, EndS = 0, EndI = 0, IO;

				if((BeginS = FindLocal(pvoid(lMap), Size, pvoid(SectionBegin), SectionBeginSize, &WildCard)))
					if((EndS = FindLocal(pvoid(BeginS), Size - (BeginS - lMap), pvoid(SectionEnd), SectionEndSize, &WildCard)))
						if((BeginI = FindLocal(pvoid(lMap), Size, pvoid(ImportsBegin), ImportsBeginSize, &WildCard)))
							EndI = FindLocal(pvoid(BeginI), Size - (BeginI - lMap), pvoid(ImportsEnd), ImportsEndSize, &WildCard);

				ulong TlsDir = 0, TlsDirSize = iTlsDirSize;
				if(BeginS && EndS && BeginI && EndI && EndS - BeginS < iSizeThreshold && EndI - BeginI < iSizeThreshold)
				{
					ulong KeyA, KeyB;

					KeyA		= *pulong(BeginS + iSecKeyAOffset);
					KeyB		= *pulong(BeginS + iSecKeyBOffset);
					
					Log("[x] Gathered section decryption keys (%08X and %08X).", KeyA, KeyB);

					BeginS		= FindLocal(pvoid(BeginS + SectionBeginSize + sizeof ulong), EndS - BeginS, pvoid(XorSection), XorSize, &WildCard);
					lDecryption = new uchar[EndS - BeginS + 1];

					CopyMemory(pvoid(lDecryption), pvoid(BeginS), EndS - BeginS);

					lDecryption[EndS - BeginS] = iRetn;
					if(VirtualProtect(pvoid(lDecryption), EndS - BeginS + 1, PAGE_EXECUTE_READWRITE, &IO))
					{
						Log("[x] Section decryption loop identified. Decrypting sections...");

						PIMAGE_DOS_HEADER Dos			= PIMAGE_DOS_HEADER(lMap);
						PIMAGE_NT_HEADERS Nt			= PIMAGE_NT_HEADERS(lMap + Dos->e_lfanew);
						PIMAGE_SECTION_HEADER Section	= PIMAGE_SECTION_HEADER(ulong(&Nt->OptionalHeader) + Nt->FileHeader.SizeOfOptionalHeader);

						for(ulong i = 0; i < Count - 1; i++)
						{
							char SectionName[iSectionNameMax + 1];

							ZeroMemory(SectionName, iSectionNameMax + 1);
							CopyMemory(SectionName, Section[i].Name, iSectionNameMax);

							if(*pulong(Section[i].Name) == 'rsr.' || *pulong(Section[i].Name) == 'crsr')
							{
								Log("[x] Resource section %s detected, no decryption needed.", SectionName);
								continue;
							}

							if(Section[i].SizeOfRawData)
							{
								Log("[x] Decrypting section %s, size %04X.", SectionName, Section[i].SizeOfRawData);

								lKeyA = KeyA;
								lKeyB = KeyB;
								lFirstByte = lMap + Section[i].PointerToRawData;

								StaticMemoryDecryptSpecial(pvoid(lFirstByte), Section[i].SizeOfRawData, UE_STATIC_KEY_SIZE_1, UE_STATIC_DECRYPTOR_BACKWARD,
									&cUnpacker::CustomDecryption);
							}

							pulong SectionData = pulong(lMap + Section[i].PointerToRawData);
							if(SectionData[0] == SectionData[1] - iTlsDiffIndicator)
								TlsDir = Section[i].VirtualAddress;
						}

						lFirstByte = 0;
						delete[] lDecryption;

						KeyA		= *pulong(BeginI + iImpKeyAOffset);
						KeyB		= *pulong(BeginI + iImpKeyBOffset);

						Log("[x] Gathered import decryption keys (%08X and %08X).", KeyA, KeyB);
						BeginI		= FindLocal(pvoid(BeginI + ImportsBeginSize + sizeof ulong), EndI - BeginI, pvoid(XorImports), XorSize, &WildCard);

						if(BeginI && EndI - BeginI < iSizeThreshold)
						{
							lDecryption = new uchar[EndI - BeginI + 1];
							CopyMemory(pvoid(lDecryption), pvoid(BeginI), EndI - BeginI);

							lDecryption[iCondJmpOff]	-= iCondJmpAlign;
							lDecryption[EndI - BeginI]	=  iRetn;

							ulong Off	= FindLocal(pvoid(lMap), Size, pvoid(ImportVA), ImportVASize, &WildCard);
							ulong ImpVA	= *pulong(Off + iImportVAOff);
							Off			= ulong(ConvertVAtoFileOffset(lMap, ImpVA, true));

							if(Off && VirtualProtect(pvoid(lDecryption), EndS - BeginS + 1, PAGE_EXECUTE_READWRITE, &IO))
							{
								Log("[x] Import decryption loop identified. Decrypting imports...");

								ulong ImportSize = 0;
								PIMAGE_IMPORT_DESCRIPTOR Desc = PIMAGE_IMPORT_DESCRIPTOR(Off);

								Desc--;
								while((++Desc)->Name)
								{
									PIMAGE_THUNK_DATA Thunk = PIMAGE_THUNK_DATA(ConvertVAtoFileOffset(lMap,
										Base + (Desc->OriginalFirstThunk ? Desc->OriginalFirstThunk : Desc->FirstThunk), true));
									
									while(Thunk->u1.Function)
									{
										if(!(Thunk->u1.Function & iIsOrdinal))
										{
											PIMAGE_IMPORT_BY_NAME Name = PIMAGE_IMPORT_BY_NAME(ConvertVAtoFileOffset(lMap, Base + Thunk->u1.Function, true));

											lKeyA = KeyA;
											lKeyB = KeyB;

											StaticMemoryDecryptEx(pvoid(Name->Name), iMaxAPILength, UE_STATIC_KEY_SIZE_1, &cUnpacker::CustomDecryption);
										}

										Thunk++;
									}

									ImportSize += sizeof IMAGE_IMPORT_DESCRIPTOR;
								}

								ImpVA -= Base;

								ulong OEPJmp	= EndI + ImportsEndSize - 1;
								ulong OEP		= ulong(GetJumpDestination(GetCurrentProcess(), OEPJmp)) - lMap;
								
								if(OEP)
								{
									BytesSaved = Section[Nt->FileHeader.NumberOfSections - 1].PointerToRawData;
									OEP	+= Section[Nt->FileHeader.NumberOfSections - 1].VirtualAddress - Section[Nt->FileHeader.NumberOfSections - 1].PointerToRawData;

									SetPE32DataForMappedFile(lMap, 0, UE_IMPORTTABLEADDRESS,	ImpVA);
									SetPE32DataForMappedFile(lMap, 0, UE_IMPORTTABLESIZE,		ImportSize);
									SetPE32DataForMappedFile(lMap, 0, UE_OEP,					OEP);

									if(TlsDir)
									{
										Log("[x] TLS directory found.");
										SetPE32DataForMappedFile(lMap, 0, UE_TLSTABLEADDRESS,	TlsDir);
										SetPE32DataForMappedFile(lMap, 0, UE_TLSTABLESIZE,		TlsDirSize);
									}

									Success = true;

									Log("[x] Import directory entry aligned (%08X, %04X).", ImpVA, ImportSize);
									Log("[x] OEP found: %08X.", OEP + Base);
								}
								else Log("[Error] Couldn't retrieve OEP.");
							}
							else Log("[Error] Error in import decryption loop.");

							delete[] lDecryption;
						}
						else Log("[Error] Signature mismatch (probably not packed with CryptoCrack's Protector?).");
					}
					else Log("[Error] Error in section decryption loop.");
				}
				else Log("[Error] Signature mismatch (probably not packed with CryptoCrack's Protector?).");

				StaticFileUnload(lOut, true, Handle, Size, Mapping, lMap);

				if(Success)
				{
					Log("[x] Protector section removed (%04X bytes saved).", BytesSaved);
					DeleteLastSection(lOut);
				}
			}
			else Log("[Error] Could not load selected file!");
		}
		else Log("[Error] Selected file is not a valid PE32 file!");
	}

	Log("-> Unpack ended...");
	return Return;
}
