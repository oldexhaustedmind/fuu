
#include "static.h"
#include "sdk/sdk.h"

#include <vector>
#include <algorithm>
#include <intrin.h>

char		gPath[MAX_PATH], gOut[MAX_PATH];
ulong		gMap, gSize, gBase, gOep, gOepVa, gDelta, gDecryptionSize, gInner11Counter, gKey;
puchar		gDecryptionLoop = 0;

void Log(const pchar pFormat, ...)
{
	va_list ArgList;
	va_start(ArgList, pFormat);

	char Buffer[iMax];
	StringCbVPrintfA(Buffer, iMax, pFormat, ArgList);

	SendMessageA(gLog, LB_ADDSTRING, 0, LPARAM(Buffer));
	SendMessageA(gLog, LB_SETCURSEL, WPARAM(SendMessageA(gLog, LB_GETCOUNT, 0, 0) - 1), 0);

	va_end(ArgList);
}

ulong MapVA(ulong pVA)
{
	ulong Return = ulong(ConvertVAtoFileOffset(gMap, pVA, true));
	if(!(Return >= gMap && Return <= gMap + gSize)) Return = 0;

	return Return;
}

ulong MapFind(ulong pFrom, const uchar *pSignature, ulong pSignatureLength, puchar pWildCard = &gWildCard)
{
	return ulong(FindEx(GetCurrentProcess(), pvoid(pFrom), gSize - (pFrom - gMap), pvoid(pSignature), pSignatureLength, pWildCard));
}

void Decrypt(pchar pEncryptedString)
{
	ulong i = 0;

	do pEncryptedString[i] = _rotr8(pEncryptedString[i], iImportKey), i++;
	while(pEncryptedString[i - 1]);
}

bool Unpack(pchar pPath)
{
	bool Return = false, ImportInit = false, GotTLS = false;
	gMap = gSize = gBase = gOep = gOepVa = gDelta = gDecryptionSize = gInner11Counter = gKey = 0;

	FILE_STATUS_INFO Status;
	ZeroMemory(&Status, sizeof Status);

	StringCbCopyA(gPath, MAX_PATH, pPath);

	size_t ExtIndex;
	StringCbLengthA(gPath, MAX_PATH, &ExtIndex);

	ExtIndex--;
	while(ExtIndex > 0 && gPath[ExtIndex] != '.') ExtIndex--;

	if(ExtIndex)
	{
		gPath[ExtIndex] = 0;
		ExtIndex++;

		StringCbPrintfA(gOut, MAX_PATH, "%s.unpacked.%s", gPath, &gPath[ExtIndex]);
		gPath[ExtIndex - 1] = '.';

		DeleteFileA(gOut);
		Log("-> Unpack started...");

		ulong OEP = 0;
		if(CopyFileA(gPath, gOut, true) && IsPE32FileValidEx(gPath, UE_DEPTH_DEEP, &Status))
		{
			pvoid Handle, Mapping;

			if(StaticFileLoad(gOut, UE_ACCESS_ALL, false, &Handle, &gSize, &Mapping, &gMap) && gSize)
			{
				gBase			= ulong(GetPE32DataFromMappedFile(gMap, 0, UE_IMAGEBASE));
				gOepVa = gOep	= ulong(GetPE32DataFromMappedFile(gMap, 0, UE_OEP)) + gBase;

				if(gBase && (gOep = ulong(ConvertVAtoFileOffset(gMap, gOep, true))))
				{
					ulong		StubSize, Stub, Init;
					eVersion	Version;

					if(FetchDeltaValue(gDelta) && (Init = FetchStubData(Stub, StubSize, Version)) && Version != iNone)
					{
						Log("[x] Version %2d detected.", Version + iVersionBase);
						Log("[x] Delta value acquired (%08x).", gDelta);
						Log("[x] Stub starts at %08x, %04x bytes.", Stub, StubSize);

						GotTLS = GetPE32DataFromMappedFile(gMap, 0, UE_TLSTABLEADDRESS) > 0;

						ulong LoopBegin = Init + (Version == iyC13 ? lStubBegin13 : lStubBegin12) + (Version == iyC11 ? iOffsetLoop11 : 0), LoopSize;
						if(Version != iyC11)	LoopSize = MapVA(Stub - iOffsetLoopd);
						else					LoopSize = MapFind(Init, &*sStubLoopEnd11, lStubLoopEnd11) + iOffsetLoop11;

						LoopSize -= LoopBegin;
						if(LoopSize < iTresholdStub)
						{
							gDecryptionLoop = new uchar[LoopSize + 1];
							CopyMemory(&*gDecryptionLoop, pvoid(LoopBegin), LoopSize);

							gDecryptionLoop[LoopSize] = iRetn;

							ulong Temp;
							if(VirtualProtect(&*gDecryptionLoop, LoopSize, PAGE_EXECUTE_READ, &Temp))
							{
								ulong KeySize = (Version == iyC11 ? UE_STATIC_KEY_SIZE_4 : UE_STATIC_KEY_SIZE_1);
								StubSize *= KeySize;

								Stub = MapVA(Stub);
								gDecryptionSize = StubSize;

								StaticMemoryDecryptEx(pvoid(Stub), StubSize, KeySize, &DecryptStub);
								VirtualProtect(&*gDecryptionLoop, LoopSize, Temp, &Temp);

								delete[] gDecryptionLoop;
								gDecryptionLoop = 0;

								Log("[x] Stub has been decrypted.");

								ulong Section = MapFind(gMap, puchar(sSectionException), lSectionException);
								if(Section > gMap)
								{
									ulong BlockSize = iExcSizeLong, Offset, SwitchIndicator, SwitchOffset;
									if(Version == iyC13)
									{
										Offset			= iOffsetExc13;
										SwitchIndicator = iExcSwitch13;
										SwitchOffset	= iOffsetExcSw13;
									}
									else
									{
										Offset			= iOffsetExc12;
										SwitchIndicator = iExcSwitch12;
										SwitchOffset	= iOffsetExcSw12;
									}

									std::vector<ulong> Exceptions;
									Section += lSectionException + Offset;

									while(*pushort(Section - iOffsetExcMagic) == iExcBlockMagic)
									{
										Exceptions.push_back(*pulong(Section));

										if(SwitchOffset && *puchar(Section + SwitchOffset) == SwitchIndicator)
										{
											SwitchIndicator = SwitchOffset = 0;
											BlockSize = (Version == iyC13 ? iExcSizeShort13 : iExcSizeShort12);
										}

										Section += BlockSize;
									}

									PIMAGE_DOS_HEADER		Dos		= PIMAGE_DOS_HEADER(gMap);
									PIMAGE_NT_HEADERS		Nt		= PIMAGE_NT_HEADERS(gMap + Dos->e_lfanew);
									PIMAGE_SECTION_HEADER	Section	= PIMAGE_SECTION_HEADER(ulong(&Nt->OptionalHeader) + Nt->FileHeader.SizeOfOptionalHeader);

									ulong	DecryptSection;
									pushort SectionKey = 0;
									switch(Version)
									{
									case iyC11:
									case iyC12:
										DecryptSection = MapFind(gMap, sSectionDecryption12, lSectionDecryption12) + iOffsetDecrypt12;

										if(Version == iyC11)
											SectionKey = pushort(MapVA(gDelta + *pulong(DecryptSection - iOffsetSectionKey11)));
										break;

									case iyC13:
										DecryptSection = MapFind(gMap, sSectionDecryption13, lSectionDecryption13) + iOffsetDecrypt13;
										DecryptSection = GetJumpDestination(GetCurrentProcess(), DecryptSection);
										break;
									}

									gDecryptionLoop = new uchar[iMaxSecDecSize];
									Log("[x] Decrypting sections.");

									if(CopyMemory(gDecryptionLoop, pvoid(DecryptSection), iMaxSecDecSize) &&
										VirtualProtect(&*gDecryptionLoop, iMaxSecDecSize, PAGE_EXECUTE_READ, &Temp))
									{
										for(ulong i = 0; i < Nt->FileHeader.NumberOfSections; i++)
										{
											char Name[IMAGE_SIZEOF_SHORT_NAME + 1];

											ZeroMemory(Name,					IMAGE_SIZEOF_SHORT_NAME + 1);
											CopyMemory(Name, Section[i].Name,	IMAGE_SIZEOF_SHORT_NAME);

											ulong Current = *pulong(Section[i].Name), RawSize, Key;

											std::vector<ulong>::const_iterator Found = std::find(Exceptions.begin(), Exceptions.end(), Current);
											if(Found != Exceptions.end() || !Section[i].SizeOfRawData || !Section[i].PointerToRawData)
											{
												Log("    > %s skipped due to exception list.", Name);
												continue;
											}

											Current = gMap + Section[i].PointerToRawData;
											RawSize	= Section[i].SizeOfRawData;
											Key		= SectionKey ? SectionKey[i] : 0;
											__asm
											{
												pushad;
												mov esi, Current;
												mov ecx, RawSize;
												mov ebx, Key;

												call gDecryptionLoop;
												popad;
											}

											Log("    > %s successfully decrypted.", Name);
										}
									}

									VirtualProtect(&*gDecryptionLoop, iMaxSecDecSize, Temp, &Temp);
									delete[] gDecryptionLoop;

									gDecryptionLoop = 0;
									Log("[x] Sections have been decrypted.");

									ulong Import = MapFind(gMap, Version == iyC13 ? sImportDecryption13 : sImportDecryption12,
																 Version == iyC13 ? lImportDecryption13 : lImportDecryption12);
									if(Import > gMap)
									{
										Import = MapVA(gDelta + *pulong(Import + (Version == iyC13 ? iOffsetImportVA13 : iOffsetImportVA12)));
										
										struct sReducedDescriptor
										{
											ulong Name;
											ulong FirstThunk;
											ulong OriginalFirstThunk;
										};

										ImporterInit(iImportSize, Nt->OptionalHeader.ImageBase);
										ImportInit = true;

										sReducedDescriptor *Desc = (sReducedDescriptor*)Import;
										for(; Desc->Name; Desc++)
										{
											ulong ThunkVA = Nt->OptionalHeader.ImageBase + (Desc->OriginalFirstThunk ? Desc->OriginalFirstThunk : Desc->FirstThunk);
											PIMAGE_THUNK_DATA Thunk = PIMAGE_THUNK_DATA(MapVA(ThunkVA));

											ThunkVA = Nt->OptionalHeader.ImageBase + Desc->FirstThunk;
											pchar ModuleName = pchar(MapVA(Nt->OptionalHeader.ImageBase + Desc->Name));

											if(ModuleName)
											{
												Decrypt(ModuleName);
												Log("[x] Decrypting imports for module '%s'.", ModuleName);

												ImporterAddNewDll(ModuleName, 0);
												while(Thunk->u1.Function)
												{
													if(!(Thunk->u1.Function & IMAGE_ORDINAL_FLAG32))
													{
														PIMAGE_IMPORT_BY_NAME Name = PIMAGE_IMPORT_BY_NAME(MapVA(Nt->OptionalHeader.ImageBase + Thunk->u1.Function));
														Decrypt(pchar(Name->Name));

														ImporterAddNewAPI(pchar(Name->Name), ThunkVA);
														Log("    > %s", Name->Name);
													}
													else
													{
														ImporterAddNewOrdinalAPI(Thunk->u1.Ordinal, ThunkVA);
														Log("    > (%08x)", Thunk->u1.Ordinal);
													}

													Thunk++;
													ThunkVA += sizeof ulong;
												}
											}
										}

										switch(Version)
										{
										case iyC11:
											LoopBegin = MapFind(gMap, sInnerStubDecryption11, lInnerStubDecryption11);
											if(LoopBegin > gMap)
											{
												Stub			= MapVA(gDelta + *pulong(LoopBegin + iOffsetInnerDest11));
												LoopBegin	   += iOffsetInnerLoop11;

												LoopSize		= Stub - LoopBegin - iIgnoreLoopd11;
												gDecryptionSize = *pulong(LoopBegin + iOffsetInnerSize11 - iOffsetInnerLoop11);

												KeySize			= UE_STATIC_KEY_SIZE_1;
												gDecryptionLoop = new uchar[LoopSize + 1];

												CopyMemory(&*gDecryptionLoop, pvoid(LoopBegin), LoopSize);
												gDecryptionLoop[LoopSize] = iRetn;

												if(gDecryptionSize && LoopSize && gDecryptionLoop && VirtualProtect(&*gDecryptionLoop, LoopSize, PAGE_EXECUTE_READ, &Temp))
												{
													gInner11Counter = 0;

													StaticMemoryDecryptEx(pvoid(Stub), gDecryptionSize, KeySize, &DecryptStub);
													VirtualProtect(&*gDecryptionLoop, LoopSize, Temp, &Temp);

													OEP = MapFind(gMap, sOEPReference11, lOEPReference11);
													if(OEP > gMap) OEP = *pulong(MapVA(gDelta + *pulong(OEP + iOffsetOEPRef11)));
												}

												if(gDecryptionLoop)
												{
													delete[] gDecryptionLoop;
													gDecryptionLoop = 0;
												}
											}
											break;

										case iyC12:
											OEP = MapFind(gMap, sOEPReference12, lOEPReference12);
											if(OEP > gMap) OEP = *pulong(MapVA(gDelta + *pulong(OEP + iOffsetOEPRef12)));
											break;

										case iyC13:
											OEP = MapFind(gMap, sOEPReference13, lOEPReference13);
											if(OEP > gMap) OEP = *pulong(MapVA(gDelta + *pulong(OEP + iOffsetOEPRef13)));
											break;
										}

										if(OEP)
										{
											Log("[x] OEP found (%08x).", OEP);
											SetPE32DataForMappedFile(gMap, 0, UE_OEP, OEP);
										}
										else Log("[x] Could not find OEP.");
									}
								}
							}

							if(gDecryptionLoop) delete[] gDecryptionLoop;
						}
					}
				}

				if(!StaticFileUnload(gOut, true, Handle, gSize, Mapping, gMap))
					Log("[Error] Could not save decrypted file!");
				else
				{
					if(OEP && ImportInit)
					{
						if(!GotTLS)
						{
							Log("[x] Deleting packer section.");
							DeleteLastSection(gOut);
						}
						else Log("[x] Keeping packer section due to TLS table.");

						
						if(!ImporterExportIATEx(gOut, ".revlabs"))
							Log("[x] Could not export IT.");
						else
							Log("[x] IT exported.");

						ImporterCleanup();
					}
				}
			}
			else Log("[Error] Could not load selected file!");

			gMap = 0;
		}
		else Log("[Error] Selected file is not a valid PE32 file!");
	}

	Log("-> Unpack ended...");
	return Return;
}

bool __stdcall DecryptStub(pvoid pCurrent, int pKeySize)
{
	if(pKeySize == UE_STATIC_KEY_SIZE_4)
	{
		__asm
		{
			mov esi, pCurrent;
			mov edi, esi;

			mov ebx, gKey;
			call gDecryptionLoop;

			mov gKey, ebx;
		}
	}
	else
	{
		__asm
		{
			mov esi, pCurrent;
			mov edi, esi;

			mov ecx, gDecryptionSize;
			mov ebx, gInner11Counter;

			call gDecryptionLoop;
			inc gInner11Counter;
		}

		gDecryptionSize--;
	}

	return true;
}

bool FetchDeltaValue(ulong &pDelta)
{
	ulong Delta = ulong(FindEx(GetCurrentProcess(), pvoid(gOep), iTresholdDelta, pvoid(sDelta), lDelta, &gWildCard));
	if(Delta)
	{
		pDelta = *pulong(Delta + iOffsetDelta);

		if((Delta = ulong(ConvertFileOffsetToVA(gMap, Delta, true))))
		{
			Delta += iCallSize;
			pDelta = Delta - pDelta;

			return true;
		}
	}

	pDelta = 0;
	return false;
}

ulong FetchStubData(ulong &pStub, ulong &pStubSize, eVersion &pVersion)
{
	pVersion = iNone;
	pStub = pStubSize = 0;

	ulong Begin = MapFind(gOep, &*sStubBegin13, lStubBegin13);
	if(Begin  > gMap)
	{
		pVersion = iyC13;

		pStub		= *pulong(Begin + iOffsetStub13) + gDelta;
		pStubSize	= *pulong(Begin + iOffsetSizeA) - *pulong(Begin + iOffsetSizeB);
	}
	else if((Begin = MapFind(gOep, &*sStubBegin12, lStubBegin12)) > gMap)
	{
		pVersion = iyC11;

		pStub		= *pulong(Begin + iOffsetStub12) + gDelta;
		pStubSize	= *pulong(Begin + iOffsetSizeA);

		puchar CheckLoop = puchar(MapVA(pStub - iOffsetLoopd));
		if(CheckLoop && *CheckLoop == iLoopdShort)
			pVersion = iyC12;
	}
	else Begin = 0;

	return Begin;
}
