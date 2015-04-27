require 'SDK'

    local SystemKernel2 = alien.load 'kernel32.dll'

-- Variables
    local pFileMap = alien.buffer(alien.sizeof('pointer'))
	pFileMap:set(1,nil,'pointer')
    local pFileHandle = alien.buffer(alien.sizeof('pointer'))
	pFileHandle:set(1,nil,'pointer')
    local pFileMapVA = alien.buffer(alien.sizeof('pointer'))
	pFileMapVA:set(1,nil,'pointer')
    local pLoadedSize = alien.buffer(alien.sizeof('pointer'))
	pLoadedSize:set(1,nil,'pointer')
    local pUnpackedOEP = alien.buffer(alien.sizeof('pointer'))
	pUnpackedOEP:set(1,nil,'pointer')

    SystemKernel2.RtlMoveMemory:types {"pointer","long","long",abi="stdcall"}
    SK_RtlMoveMemory = SystemKernel2.RtlMoveMemory


-- Code

	inFileName = arg[1]
	BackFileName = inFileName .. '.bak'
	print("Unpacker for LameCrypt 1.0\nfrom ReversingLabs Corporation - www.reversinglabs.com\n\n");
	bReturn = TE_CopyFileA(inFileName, BackFileName, TE_FALSE)
	if not(bReturn == 0) then
		bReturn = TE_StaticFileLoad(inFileName, UE_ACCESS_ALL, TE_FALSE, pFileHandle, pLoadedSize, pFileMap, pFileMapVA)
		if not(bReturn == 0) then
			-- Convert pointers to values
			FileMap = pFileMap:get(1,'long')
			FileHandle = pFileHandle:get(1,'long')
			FileMapVA = pFileMapVA:get(1,'long')
			LoadedSize = pLoadedSize:get(1,'long')
			--
			-- Unpack file
			rUnpackedOEP = TE_ConvertVAtoFileOffset(FileMapVA, TE_GetPE32DataFromMappedFile(FileMapVA, nil, UE_OEP) + TE_GetPE32DataFromMappedFile(FileMapVA, NULL, UE_IMAGEBASE), TE_TRUE) + 0x19
			SK_RtlMoveMemory(pUnpackedOEP, rUnpackedOEP, 4)
			UnpackedOEP = pUnpackedOEP:get(1,'long') - TE_GetPE32DataFromMappedFile(FileMapVA, NULL, UE_IMAGEBASE)
			TE_StaticSectionDecrypt(FileMapVA, nil, TE_FALSE, UE_STATIC_DECRYPTOR_XOR, UE_STATIC_KEY_SIZE_1, 0x90)
			TE_SetPE32DataForMappedFile(FileMapVA, nil, UE_OEP, UnpackedOEP)
			TE_StaticFileUnload(inFileName, TE_TRUE, FileHandle, LoadedSize, FileMap, FileMapVA)
			TE_DeleteLastSection(inFileName)
			print("Selected file has been unpacked!\n");
		else
			print("Could not open selected file!")
		end
	else
		print("Could not backup selected file!")
	end
