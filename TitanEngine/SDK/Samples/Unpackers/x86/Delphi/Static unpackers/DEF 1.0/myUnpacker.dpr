program myUnpacker;

{$APPTYPE CONSOLE}

uses
  SysUtils, Windows, SDK;

  procedure cRtlMoveMemory(OutMemory:Pointer; InMemory,MemorySize:LongInt); stdcall;  external 'kernel32.dll' name 'RtlMoveMemory';

var
  CurrentSize:LongInt;

function StaticCallBack(sMemoryStart,sKeySize:LongInt):boolean; stdcall;
begin
  asm
    PUSHAD
    MOV EAX,sMemoryStart
    MOVZX EBX,BYTE PTR[EAX]
    MOV ECX,CurrentSize
    XOR BL,CL
    MOV BYTE PTR[EAX],BL
    POPAD
  end;
  CurrentSize := CurrentSize - sKeySize;
  StaticCallBack := true;
end;

var
  i:LongInt;
  SectionNumber:LongInt;
  FileHandle,FileMap:THandle;
  FileSize,FileMapVA:LongInt;
  UnpackedOEP:LongInt;
  szOriginalFile:string;
  szSectionName:array[1..8] of char;
begin
  { TODO -oUser -cConsole Main : Insert code here }
  writeln('Unpacker for DEF 1.0');
  writeln('from ReversingLabs Corporation - www.reversinglabs.com');
  writeln('');
  if FileExists(ParamStr(1)) then begin
    szOriginalFile := ParamStr(1) + '.bak';
    if CopyFileA(PAnsiChar(ParamStr(1)), PAnsiChar(szOriginalFile), false) then begin
      if StaticFileLoad(PAnsiChar(ParamStr(1)), UE_ACCESS_ALL, false, @FileHandle, @FileSize, @FileMap, @FileMapVA) then begin
        cRtlMoveMemory(@UnpackedOEP, ConvertVAtoFileOffset(FileMapVA, GetPE32DataFromMappedFile(FileMapVA, 0, UE_OEP) + GetPE32DataFromMappedFile(FileMapVA, 0, UE_IMAGEBASE) + $25, true), 4);
        UnpackedOEP := UnpackedOEP - GetPE32DataFromMappedFile(FileMapVA, 0, UE_IMAGEBASE);
        SectionNumber := GetPE32DataFromMappedFile(FileMapVA, 0, UE_SECTIONNUMBER);
        for i := 0 to SectionNumber do begin
          cRtlMoveMemory(@szSectionName[1], GetPE32DataFromMappedFile(FileMapVA, i, UE_SECTIONNAME), 8);
          if Ord(szSectionName[8]) = $01 then begin
            CurrentSize := GetPE32DataFromMappedFile(FileMapVA, i, UE_SECTIONRAWSIZE);
            StaticMemoryDecryptEx(GetPE32DataFromMappedFile(FileMapVA, i, UE_SECTIONRAWOFFSET) + FileMapVA, GetPE32DataFromMappedFile(FileMapVA, i, UE_SECTIONRAWSIZE), UE_STATIC_KEY_SIZE_1, @StaticCallBack);
          end;
        end;
        SetPE32DataForMappedFile(FileMapVA, 0, UE_OEP, UnpackedOEP);
        StaticFileUnload(PAnsiChar(ParamStr(1)), true, FileHandle, FileSize, FileMap, FileMapVA);
        writeln('Selected file has been unpacked!');
      end else begin
        writeln('Could not find selected input file!');
      end;
    end else begin
      writeln('Could not find selected input file!');
    end;
  end else begin
    writeln('Could not backup selected input file!');
  end;
end.
