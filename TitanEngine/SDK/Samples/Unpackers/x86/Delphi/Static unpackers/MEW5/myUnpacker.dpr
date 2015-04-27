program myUnpacker;

{$APPTYPE CONSOLE}

uses
  SysUtils, Windows, SDK;

  procedure cRtlMoveMemory(OutMemory:Pointer; InMemory,MemorySize:LongInt); stdcall;  external 'kernel32.dll' name 'RtlMoveMemory';

function StaticCallBack(sMemoryStart,sKeySize:LongInt):boolean; stdcall;
begin
  asm
    PUSHAD
    MOV EAX,sMemoryStart
    MOVZX EBX,BYTE PTR[EAX]
    ROL BL,$29
    ADD BL,$0BA
    ROR BL,$50
    MOV BYTE PTR[EAX],BL
    POPAD
  end;
  StaticCallBack := true;
end;

var
  FileHandle,FileMap:THandle;
  FileSize,FileMapVA:LongInt;
  DataPointer,DecryptStart,DecryptSize:LongInt;
  UnpackedOEP:LongInt;
  szOriginalFile:string;
begin
  { TODO -oUser -cConsole Main : Insert code here }
  writeln('Unpacker for MEW5');
  writeln('from ReversingLabs Corporation - www.reversinglabs.com');
  writeln('');
  if FileExists(ParamStr(1)) then begin
    szOriginalFile := ParamStr(1) + '.bak';
    if CopyFileA(PAnsiChar(ParamStr(1)), PAnsiChar(szOriginalFile), false) then begin
      if StaticFileLoad(PAnsiChar(ParamStr(1)), UE_ACCESS_ALL, false, @FileHandle, @FileSize, @FileMap, @FileMapVA) then begin
        // Read data pointer
        cRtlMoveMemory(@DataPointer, ConvertVAtoFileOffset(FileMapVA, GetPE32DataFromMappedFile(FileMapVA, 0, UE_OEP) + GetPE32DataFromMappedFile(FileMapVA, 0, UE_IMAGEBASE) + 1, true), 4);
        DataPointer := ConvertVAtoFileOffset(FileMapVA, DataPointer, true);
        // Read data
        cRtlMoveMemory(@DecryptSize, DataPointer, 4);
        cRtlMoveMemory(@UnpackedOEP, DataPointer + 4, 4);
        cRtlMoveMemory(@DecryptStart, DataPointer + 8, 4);
        // Convert data
        DecryptStart := ConvertVAtoFileOffset(FileMapVA, DecryptStart, true);
        // Unpack file
        StaticMemoryDecryptEx(DecryptStart, DecryptSize, UE_STATIC_KEY_SIZE_1, @StaticCallBack);
        // Finalize unpacking
        UnpackedOEP := UnpackedOEP - GetPE32DataFromMappedFile(FileMapVA, 0, UE_IMAGEBASE);
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
