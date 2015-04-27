require 'SDK'

-- Variables
    ProcessInfo = PROCESS_INFORMATION:new()
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

-- Code

--
-- Breakpoint on relocations
--
local function _TE_cbMakeSnapShoot1()

	if TE_RelocaterMakeSnapshot(ProcessInfo.hProcess, "snapshot.1", vSnapshootMemoryStartRVA + vFileBase, vSnapshootMemorySize) == TE_TRUE then
		print("[x] Created memory snapshot #1 from ".. string.format("%08X",vSnapshootMemoryStartRVA + vFileBase) .. " size " .. string.format("%08X",vSnapshootMemorySize))
	else
		print("Critical error, aborting...")
		TE_StopDebug()
	end
end
TE_cbMakeSnapShoot1_CB = alien.callback(_TE_cbMakeSnapShoot1, "void")

--
-- Breakpoint on LoadLibrary call
--
local function _TE_cbLoadLibrary()

	local RemoteString = alien.buffer(MAX_PATH)

	local cAddress = TE_GetContextData(UE_EIP)
	if cAddress == fUPXSignature1 then
		if TE_GetRemoteString(ProcessInfo.hProcess, TE_GetContextData(UE_EAX), RemoteString, MAX_PATH) == TE_TRUE then
			szRemoteString = tostring(RemoteString)
			print("[x] LoadLibrary BPX -> " .. szRemoteString)
			TE_ImporterAddNewDll(szRemoteString, nil)
		else
			print("Could not read remote string, aborting...")
			TE_StopDebug()
		end
	else
		print("Critical error, aborting...")
		TE_StopDebug()
	end
end
TE_cbLoadLibrary_CB = alien.callback(_TE_cbLoadLibrary, "void")

--
-- Breakpoint on GetProcAddress call
--
local function _TE_cbGetProcAddress()

	local RemoteString = alien.buffer(MAX_PATH)
	local cTargetAddress = 0
	local cThunk = 0

	local cAddress = TE_GetContextData(UE_EIP)
	if cAddress == fUPXSignature2 then
		cTargetAddress = TE_GetContextData(UE_EAX)
		cThunk = TE_GetContextData(UE_EBX)
	elseif cAddress == fUPXSignature3 then
		cTargetAddress = TE_GetContextData(UE_EDI)
		cThunk = TE_GetContextData(UE_EBX)
	elseif cAddress == fUPXSignature4 then
		cTargetAddress = TE_GetContextData(UE_EDI)
		cThunk = TE_GetContextData(UE_EBX)
	else
		print("Critical error, aborting...")
		TE_StopDebug()
	end
	if not(cTargetAddress == 0) then
		if cTargetAddress > vFileBase then
			if TE_GetRemoteString(ProcessInfo.hProcess, cTargetAddress, RemoteString, MAX_PATH) == TE_TRUE then
				szRemoteString = tostring(RemoteString)
				print("[x] GetProcAddress (0x" .. string.format("%08X",cThunk) .. ") BPX -> " .. szRemoteString)
				TE_ImporterAddNewAPI(szRemoteString, cThunk);
			else
				print("Could not read remote string, aborting...")
				TE_StopDebug()
			end
		else
			print("[x] GetProcAddress (0x" .. string.format("%08X",cThunk) .. ") BPX -> " .. string.format("%08X",cTargetAddress))
			TE_ImporterAddNewOrdinalAPI(cTargetAddress, cThunk);
		end
	end
end
TE_cbGetProcAddress_CB = alien.callback(_TE_cbGetProcAddress, "void")

--
-- Breakpoint on Entry point
--
local function _TE_cbEntryPoint()

	vUnpackedOEP = TE_GetJumpDestinationEx(ProcessInfo.hProcess, TE_GetContextData(UE_EIP), TE_TRUE)
	if vFileIsDLL == TE_FALSE then
		TE_PastePEHeader(ProcessInfo.hProcess, vImageBase, fname)
	else
		if TE_RelocaterMakeSnapshot(ProcessInfo.hProcess, "snapshot.2", vSnapshootMemoryStartRVA + vFileBase, vSnapshootMemorySize) == TE_TRUE then
			print("[x] Created memory snapshot #2 from ".. string.format("%08X",vSnapshootMemoryStartRVA + vFileBase) .. " size " .. string.format("%08X",vSnapshootMemorySize))
			if TE_RelocaterCompareTwoSnapshots(ProcessInfo.hProcess, vFileBase, vSizeOfImage, "snapshot.1", "snapshot.2", vSnapshootMemoryStartRVA + vFileBase) == TE_FALSE then
				print("Critical error, aborting...")
			else
				print("[x] Compared snapshots")
				TE_DeleteFileA("snapshot.2")
				TE_DeleteFileA("snapshot.1")
			end
		else
			print("Critical error, aborting...")
		end
	end
	print("[x] Entry Point at: " .. string.format("%08x",vUnpackedOEP))
	if TE_DumpProcess(ProcessInfo.hProcess, vFileBase, fOutName, vUnpackedOEP) == TE_TRUE then
		print("[x] Process dumped!")
		mImportTableOffset = TE_AddNewSection(fOutName, "TEv20", TE_ImporterEstimatedSize() + 200) + vFileBase
		if vFileIsDLL == TE_TRUE then
			mRelocTableOffset = TE_AddNewSection(fOutName, "TEv20", TE_RelocaterEstimatedSize() + 200)
		end
		if TE_StaticFileLoad(fOutName, UE_ACCESS_ALL, TE_FALSE, pFileHandle, pLoadedSize, pFileMap, pFileMapVA) then
			-- Convert pointers to values
			FileMap = pFileMap:get(1,'long')
			FileHandle = pFileHandle:get(1,'long')
			FileMapVA = pFileMapVA:get(1,'long')
			LoadedSize = pLoadedSize:get(1,'long')
			if TE_ImporterExportIAT(TE_ConvertVAtoFileOffset(FileMapVA, mImportTableOffset, TE_TRUE), FileMapVA) == TE_FALSE then
				print("Critical error, aborting...")
			else
				print("[x] IAT has been fixed!")
			end
			if vFileIsDLL == TE_TRUE then
				if TE_RelocaterExportRelocation(TE_ConvertVAtoFileOffset(FileMapVA, mRelocTableOffset + vFileBase, TE_TRUE), mRelocTableOffset, FileMapVA) == TE_FALSE then
					print("Critical error, aborting...")
				else
					print("[x] Exporting relocations!")
				end
			end
			LoadedSize = TE_RealignPE(FileMapVA, LoadedSize, nil);
			print("[x] Realigning file!");
			TE_StaticFileUnload(fOutName, TE_TRUE, FileHandle, LoadedSize, FileMap, FileMapVA)
			TE_MakeAllSectionsRWE(fOutName)
			print("-> Unpack ended...")
			TE_StopDebug()
		end
	else
		print("Critical error, aborting...")
		TE_StopDebug()
	end
end
TE_cbEntryPoint_CB = alien.callback(_TE_cbEntryPoint, "void")

--
-- Initialize unpacking process
--
local function _TE_InitializeDebug()

	print("Locating patterns...")

	local vResult = TE_TRUE
	if vFileIsDLL == TE_TRUE then
		vFileBase = TE_GetDebuggedDLLBaseAddress()
		TE_ImporterInit(50 * 1024, vFileBase);
		TE_RelocaterInit(100 * 1024, vImageBase, vFileBase);
	else
		vFileBase = TE_GetDebuggedFileBaseAddress()
		TE_ImporterInit(50 * 1024, vFileBase);
	end
	vCalculatedSearchSize = TE_GetPE32Data(fname, nil, UE_SIZEOFIMAGE) - vPackedOEP
	vPackedOEP = vPackedOEP + vFileBase
	local UPXSignature1  = alien.array('byte', { 0x50,0x83,0xC7,0x08,0xFF })
	local UPXSignature2  = alien.array('byte', { 0x50,0x47,0x00,0x57,0x48,0xF2,0xAE })
	local UPXSignature3  = alien.array('byte', { 0x57,0x48,0xF2,0xAE,0x00,0xFF })
	local UPXSignature4  = alien.array('byte', { 0x89,0xF9,0x57,0x48,0xF2,0xAE,0x52,0xFF })
	local UPXSignature5  = alien.array('byte', { 0x61,0xE9 })
	local UPXSignature51 = alien.array('byte', { 0x83,0xEC,0x00,0xE9 })
	local UPXSignature6  = alien.array('byte', { 0x31,0xC0,0x8A,0x07,0x47,0x09,0xC0,0x74,0x22,0x3C,0xEF,0x77,0x11,
												  0x01,0xC3,0x8B,0x03,0x86,0xC4,0xC1,0xC0,0x10,0x86,0xC4,0x01,0xF0,
												  0x89,0x03,0xEB,0xE2,0x24,0x0F,0xC1,0xE0,0x10,0x66,0x8B,0x07,0x83,
												  0xC7,0x02,0xEB,0xE2 })


	fUPXSignature1 = TE_Find(vPackedOEP, vCalculatedSearchSize, UPXSignature1.buffer, UPXSignature1.length, nil)
	if fUPXSignature1 == 0 then
		print("Could not find search pattern #1, aborting...")
		vResult = TE_FALSE
		TE_StopDebug()
	end
	fUPXSignature2 = TE_Find(vPackedOEP, vCalculatedSearchSize, UPXSignature2.buffer, UPXSignature2.length, nil)
	fUPXSignature3 = TE_Find(vPackedOEP, vCalculatedSearchSize, UPXSignature3.buffer, UPXSignature3.length, nil)
	if fUPXSignature1 == 0 then
		print("Could not find search pattern #3, aborting...")
		vResult = TE_FALSE
		TE_StopDebug()
	end
	fUPXSignature4 = TE_Find(vPackedOEP, vCalculatedSearchSize, UPXSignature4.buffer, UPXSignature4.length, nil)
	if not(fUPXSignature4 == 0) then
		fUPXSignature4 = fUPXSignature4 + 2
	end
	fUPXSignature5 = TE_Find(vPackedOEP, vCalculatedSearchSize, UPXSignature5.buffer, UPXSignature5.length, nil)
	if fUPXSignature5 == 0 then
		fUPXSignature5 = TE_Find(vPackedOEP, vCalculatedSearchSize, UPXSignature51.buffer, UPXSignature51.length, nil)
		if fUPXSignature5 == 0 then
			print("Could not find search pattern #5, aborting...")
			vResult = TE_FALSE
			TE_StopDebug()
		else
			fUPXSignature5 = fUPXSignature5 + 3
		end
	else
		fUPXSignature5 = fUPXSignature5 + 1
	end
	if vFileIsDLL == TE_TRUE then
		fUPXSignature6 = TE_Find(vPackedOEP, vCalculatedSearchSize, UPXSignature6.buffer, UPXSignature6.length, nil)
		if fUPXSignature6 == 0 then
			print("Could not find search pattern #6, aborting...")
			vResult = TE_FALSE
			TE_StopDebug()
		else
			fUPXSignature6 = fUPXSignature6 - 3
		end
	end
	if vResult == TE_TRUE then
		print("All patterns found...")
		print("[x] Setting breakpoints...")
		if not(fUPXSignature1 == 0) then
			TE_SetBPX(fUPXSignature1, UE_BREAKPOINT, TE_cbLoadLibrary_CB)
		end
		if not(fUPXSignature2 == 0) then
			TE_SetBPX(fUPXSignature2, UE_BREAKPOINT, TE_cbGetProcAddress_CB)
		end
		if not(fUPXSignature3 == 0) then
			TE_SetBPX(fUPXSignature3, UE_BREAKPOINT, TE_cbGetProcAddress_CB)
		end
		if not(fUPXSignature4 == 0) then
			TE_SetBPX(fUPXSignature4, UE_BREAKPOINT, TE_cbGetProcAddress_CB)
		end
		if not(fUPXSignature5 == 0) then
			TE_SetBPX(fUPXSignature5, UE_SINGLESHOOT, TE_cbEntryPoint_CB)
		end
		if not(fUPXSignature6 == 0) then
			TE_SetBPX(fUPXSignature6, UE_BREAKPOINT, TE_cbMakeSnapShoot1_CB)
		end
	else
		print("[Error] File is not packed with UPX 1.x - 3.x")
	end
end
TE_InitializeDebug_CB = alien.callback(_TE_InitializeDebug, "void")

--
-- UPX 1.x - 3.x unpacker
-- ReversingLabs corporation / www.reversinglabs.com
--
	fname = arg[1]
	fOutName = arg[2]
	print("Unpacker for UPX 1.x - 3.x packed files")
	print("ReversingLabs corporation / www.reversinglabs.com\n")
	if not(fname == nil) and not(arg[2] == nil) then
		print("Unpacking " .. fname .. " to " .. fOutName)
		if TE_IsFileDLL(fname, nil) == TE_FALSE then
			vFileIsDLL = TE_FALSE
			pProcessInfo = TE_InitDebugEx(fname, nil, nil, TE_InitializeDebug_CB)
		else
			vFileIsDLL = TE_TRUE
			pProcessInfo = TE_InitDLLDebug(fname, TE_TRUE, nil, nil, TE_InitializeDebug_CB)
		end
		if not(pProcessInfo == nil) then
			print("Starting debugging...")
			TE_RtlMoveMemory(ProcessInfo(), pProcessInfo, PROCESS_INFORMATION.size)
			vSizeOfImage = TE_GetPE32Data(fname, nil, UE_SIZEOFIMAGE)
			vImageBase = TE_GetPE32Data(fname, nil, UE_IMAGEBASE)
			vPackedOEP = TE_GetPE32Data(fname, nil, UE_OEP)
			vSnapshootMemoryStartRVA = TE_GetPE32Data(fname, NULL, UE_SECTIONVIRTUALOFFSET)
			vSnapshootMemorySize = vPackedOEP - vSnapshootMemoryStartRVA
			TE_DebugLoop()
			print("[x] ExitCode: " .. string.format("%08x",TE_GetExitCode()))
			TE_FreeLibrary(TitanEngine)
		else
			print("Could not initialize debugging!")
		end
	else
		print("Usage: UPX.lua inputFile.ext unpackedFile.ext")
	end
