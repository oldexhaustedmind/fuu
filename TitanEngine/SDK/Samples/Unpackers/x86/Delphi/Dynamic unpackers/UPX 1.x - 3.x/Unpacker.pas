unit Unpacker;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, XPMan, StdCtrls, Buttons, ExtCtrls, SDK;

type
  TmainForm = class(TForm)
    Image1: TImage;
    Label1: TLabel;
    Bevel1: TBevel;
    Label2: TLabel;
    browsedFile: TEdit;
    BitBtn1: TBitBtn;
    XPManifest1: TXPManifest;
    GroupBox1: TGroupBox;
    LogBox: TListBox;
    chkRealignFile: TCheckBox;
    Image2: TImage;
    BitBtn2: TBitBtn;
    BitBtn3: TBitBtn;
    BitBtn4: TBitBtn;
    OpenDialog1: TOpenDialog;
    procedure FormCreate(Sender: TObject);
    procedure BitBtn4Click(Sender: TObject);
    procedure BitBtn1Click(Sender: TObject);
    procedure BitBtn2Click(Sender: TObject);
    procedure BitBtn3Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  mainForm: TmainForm;
  hInstance : Cardinal;
  GlobalBuffer : string;
  cbInitCallBack : Pointer;
  UnpackFileNameBuffer : string;
  ReadStringData : array [0..256] of char;
  GlobalUnpackerFolderBuffer : array [0..1024] of char;
  fdImageBase,fdLoadedBase,fdSizeOfImage,fdEntryPoint : Cardinal;
  fdFileIsDll : boolean;
  UnpackerRunning : boolean;
  dtSecondSnapShootOnEP : boolean;
  ProcInfo : PProcessInformation;
  dtPatternBPXAddress : array [0..10] of Cardinal;
  SnapShoot1, SnapShoot2 : string;

  SnapshootMemoryStartRVA, SnapshootMemorySize : Cardinal;

implementation

{$R *.dfm}

function MapFileEx(fName:string; dwReadOrWrite: LongInt; dwFileHWND,dwFileSize,dwFileMap,dwFileMapVA:Pointer):boolean;
 var
 hFile : THandle;
 pVal : Pointer;
 cVal : Cardinal;
begin
 hFile := CreateFile(PAnsiChar(fName), GENERIC_READ+GENERIC_WRITE, FILE_SHARE_READ, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
 if hFile <> INVALID_HANDLE_VALUE then begin
   asm
     PUSHAD
     MOV EAX,hFile
     MOV EBX,dwFileHWND
     MOV DWORD PTR[EBX],EAX
     POPAD
   end;
   cVal := GetFileSize(hFile, nil);
   asm
     PUSHAD
     MOV EAX,cVal
     MOV EBX,dwFileSize
     MOV DWORD PTR[EBX],EAX
     POPAD
   end;
   cVal := CreateFileMapping(hFile, nil, 4, 0, GetFileSize(hFile, nil), nil);
   asm
     PUSHAD
     MOV EAX,cVal
     MOV EBX,dwFileMap
     MOV DWORD PTR[EBX],EAX
     POPAD
   end;
   pVal := MapViewOfFile(cVal, 2, 0, 0, 0);
   asm
     PUSHAD
     MOV EAX,pVal
     MOV EBX,dwFileMapVA
     MOV DWORD PTR[EBX],EAX
     POPAD
   end;
   MapFileEx := true;
 end else begin
   MapFileEx := false;
 end;
end;

procedure UnmapFileEx(dwFileHWND,dwFileSize,dwFileMap,dwFileMapVA:Cardinal);
 var
 pVal : Pointer;
begin
 asm
  PUSHAD
  MOV EAX,dwFileMapVA
  MOV pVal,EAX
  POPAD
 end;
 UnmapViewOfFile(pVal);
 CloseHandle(dwFileMap);
 SetFilePointer(dwFileHWND, dwFileSize, nil, 0);
 SetEndOfFile(dwFileHWND);
 CloseHandle(dwFileHWND);
end;

procedure AddToLog(szLogString:string);
begin
  mainForm.LogBox.Items.Add(szLogString);
  mainForm.LogBox.Selected[mainForm.LogBox.Items.Count - 1] := true;
end;

procedure cbCreateProcess(ptrCreateProcessInfo:PCreateProcessDebugInfo); stdcall;
 var
 pLoadedBase : Pointer;
begin
  pLoadedBase := ptrCreateProcessInfo.lpBaseOfImage;
  asm
   PUSHAD
   MOV EAX,pLoadedBase
   MOV fdLoadedBase,EAX
   POPAD
  end;
  SetCustomHandler(UE_CH_CREATEPROCESS, nil);
  SetBPX(fdLoadedBase + fdEntryPoint, UE_BREAKPOINT, cbInitCallBack);
  ImporterInit(50 * 1024,fdLoadedBase);
end;

procedure cbLoadLibrary(); stdcall;
 var
 cSize,NumberOfBytes,cPosition,wPosition : Cardinal;
 pPosition,rPosition : Pointer;
 MemInfo : MEMORY_BASIC_INFORMATION;
begin
 rPosition := nil;
 cPosition := GetContextData(UE_EIP);
 if cPosition = dtPatternBPXAddress[1] then begin
   cPosition := GetContextData(UE_EAX);
 end;
 if cPosition > fdLoadedBase then begin
   asm
    PUSHAD
    MOV EAX,cPosition
    MOV pPosition,EAX
    MOV rPosition,EAX    
    POPAD
   end; 
   VirtualQueryEx(ProcInfo.hProcess, pPosition, MemInfo, sizeof(MEMORY_BASIC_INFORMATION));
   pPosition := MemInfo.BaseAddress;
   cSize := MemInfo.RegionSize;
   asm
     PUSHAD
     MOV EAX,pPosition
     ADD EAX,cSize
     MOV pPosition,EAX
     POPAD
   end;
   VirtualQueryEx(ProcInfo.hProcess, pPosition, MemInfo, sizeof(MEMORY_BASIC_INFORMATION));
   cSize := MemInfo.RegionSize;
   asm
     PUSHAD
     MOV EAX,pPosition
     ADD EAX,cSize
     MOV wPosition,EAX
     POPAD
   end;
   wPosition := wPosition - cPosition;
   if wPosition > 256 then wPosition := 256;
   if ReadProcessMemory(ProcInfo.hProcess, rPosition, @ReadStringData[0], wPosition, NumberOfBytes) then begin
    ImporterAddNewDll(ReadStringData, 0);
    AddToLog('[x] LoadLibrary BPX -> ' + ReadStringData);
   end;
 end;
end;

procedure cbGetProcAddress(); stdcall;
 var
 cThunk : LongInt;
 cSize,NumberOfBytes,cPosition,wPosition : Cardinal;
 pPosition,rPosition : Pointer;
 MemInfo : MEMORY_BASIC_INFORMATION;
begin
 cThunk := 0;
 rPosition := nil;
 cPosition := GetContextData(UE_EIP);
 if cPosition = dtPatternBPXAddress[2] then begin
   cPosition := GetContextData(UE_EAX);
   cThunk := GetContextData(UE_EBX);
 end else if cPosition = dtPatternBPXAddress[3] then begin
   cPosition := GetContextData(UE_EDI);
   cThunk := GetContextData(UE_EBX);
 end else if cPosition = dtPatternBPXAddress[4] then begin
   cPosition := GetContextData(UE_EDI);
   cThunk := GetContextData(UE_EBX);
 end;
 if cPosition > fdLoadedBase then begin
   asm
    PUSHAD
    MOV EAX,cPosition
    MOV pPosition,EAX
    MOV rPosition,EAX
    POPAD
   end;
   VirtualQueryEx(ProcInfo.hProcess, pPosition, MemInfo, sizeof(MEMORY_BASIC_INFORMATION));
   pPosition := MemInfo.BaseAddress;
   cSize := MemInfo.RegionSize;
   asm
     PUSHAD
     MOV EAX,pPosition
     ADD EAX,cSize
     MOV pPosition,EAX
     POPAD
   end;
   VirtualQueryEx(ProcInfo.hProcess, pPosition, MemInfo, sizeof(MEMORY_BASIC_INFORMATION));
   cSize := MemInfo.RegionSize;
   asm
     PUSHAD
     MOV EAX,pPosition
     ADD EAX,cSize
     MOV wPosition,EAX
     POPAD
   end;
   wPosition := wPosition - cPosition;
   if wPosition > 256 then wPosition := 256;
   if ReadProcessMemory(ProcInfo.hProcess, rPosition, @ReadStringData[0], wPosition, NumberOfBytes) then begin
    ImporterAddNewAPI(ReadStringData, cThunk);
    AddToLog('[x] GetProcAddress BPX -> ' + ReadStringData);
   end;
 end else begin
    ImporterAddNewOrdinalAPI(cPosition, cThunk);
    AddToLog('[x] GetProcAddress BPX -> ' + IntToHex(cPosition,8));
 end;
end;

procedure cbMakeSnapShoot1(); stdcall;
begin
  RelocaterMakeSnapshot(ProcInfo.hProcess, PAnsiChar(SnapShoot1), SnapshootMemoryStartRVA + fdLoadedBase, SnapshootMemorySize);
end;

procedure cbEntryPoint(); stdcall;
 var
 UnpackedOEP : LongInt;
 rPosition : Pointer;
 mImportTableOffset,mRelocTableOffset,pOverlayStart,pOverlaySize,NumberOfBytes : Cardinal;
 rsFileHWND,rsFileSize,rsFileMap,rsFileMapVA : Cardinal;
begin
  try
    UnpackedOEP := GetContextData(UE_EIP) + 1;
    asm
     PUSHAD
     MOV EAX,UnpackedOEP
     MOV rPosition,EAX
     POPAD
    end;
    if ReadProcessMemory(ProcInfo.hProcess, rPosition, @UnpackedOEP, 4, NumberOfBytes) then begin
      UnpackedOEP := UnpackedOEP + GetContextData(UE_EIP) + 5;
    end;
    if fdFileIsDll = false then begin
      PastePEHeader(ProcInfo.hProcess, fdImageBase, PAnsiChar(GlobalBuffer));
      AddToLog('[x] Paste PE32 header!');
    end else begin
      if dtSecondSnapShootOnEP then begin
        RelocaterMakeSnapshot(ProcInfo.hProcess, PAnsiChar(SnapShoot2), SnapshootMemoryStartRVA + fdLoadedBase, SnapshootMemorySize);
      end;
      RelocaterCompareTwoSnapshots(ProcInfo.hProcess, fdLoadedBase, fdSizeOfImage, PAnsiChar(SnapShoot1), PAnsiChar(SnapShoot2), SnapshootMemoryStartRVA + fdLoadedBase);
    end;
    AddToLog('[x] Entry Point at: ' + IntToHex(UnpackedOEP,8));
    DumpProcess(ProcInfo.hProcess, fdLoadedBase, PAnsiChar(UnpackFileNameBuffer), UnpackedOEP);
    AddToLog('[x] Process dumped!');
    StopDebug();
    mImportTableOffset := AddNewSection(PAnsiChar(UnpackFileNameBuffer), '.TEv20', ImporterEstimatedSize() + 200) + fdLoadedBase;
    if fdFileIsDll then begin
      mRelocTableOffset := AddNewSection(PAnsiChar(UnpackFileNameBuffer), '.TEv20', RelocaterEstimatedSize() + 200);
    end;
    if MapFileEx(PAnsiChar(UnpackFileNameBuffer), 0, @rsFileHWND, @rsFileSize, @rsFileMap, @rsFileMapVA) then begin
      if rsFileMapVA > 0 then begin
        ImporterExportIAT(ConvertVAtoFileOffset(rsFileMapVA, mImportTableOffset, true), rsFileMapVA);
        AddToLog('[x] IAT has been fixed!');
        if fdFileIsDll then begin
          RelocaterExportRelocation(ConvertVAtoFileOffset(rsFileMapVA, mRelocTableOffset + fdLoadedBase, true), mRelocTableOffset, rsFileMapVA);
          AddToLog('[x] Exporting relocations!');
        end;
        if mainForm.chkRealignFile.Checked then begin
          rsFileSize := RealignPE(rsFileMapVA, rsFileSize, 2);
        end;
        UnmapFileEx(rsFileHWND,rsFileSize,rsFileMap,rsFileMapVA);
        MakeAllSectionsRWE(PAnsiChar(UnpackFileNameBuffer));
        if fdFileIsDll then begin
          RelocaterChangeFileBase(PAnsiChar(UnpackFileNameBuffer), fdImageBase);
          AddToLog('[x] Rebase file image!');
        end;                         
        if FindOverlay(PAnsiChar(UnpackFileNameBuffer), @pOverlayStart, @pOverlaySize) then begin
          AddToLog('[x] Moving overlay to unpacked file!');
          CopyOverlay(PAnsiChar(GlobalBuffer), PAnsiChar(UnpackFileNameBuffer));
        end;
        AddToLog('[x] File has been unpacked to: ' + ExtractFileName(UnpackFileNameBuffer));
        AddToLog('-> Unpack ended...');
      end else begin
        AddToLog('[Fatal Unpacking Error] Please mail file you tried to unpack to Reversing Labs!');
        AddToLog('-> Unpack ended...');
      end;
    end else begin
      AddToLog('[Fatal Unpacking Error] Please mail file you tried to unpack to Reversing Labs!');
      AddToLog('-> Unpack ended...');
    end;
  except
    ForceClose();
    ImporterCleanup();
    if rsFileMapVA > 0 then begin
      UnmapFileEx(rsFileHWND,rsFileSize,rsFileMap,rsFileMapVA);
    end;
    DeleteFile(UnpackFileNameBuffer);
    AddToLog('[Fatal Unpacking Error] Please mail file you tried to unpack to Reversing Labs!');
    AddToLog('-> Unpack ended...');
  end;
end;

procedure cbFindPatterns(); stdcall;
 var
 DontLog : boolean;
 glWildCard : BYTE;
 cSize,cPosition,wPosition : Cardinal;
 pPosition : Pointer;
 MemInfo : MEMORY_BASIC_INFORMATION;
 dtPatternSize : LongInt;
 dtPattern : array[0..64] of BYTE;
begin
 dtSecondSnapShootOnEP := true;
 DontLog := false;
 glWildCard := $00;
 if fdFileIsDll then begin
   fdLoadedBase := GetDebuggedDLLBaseAddress();
   ImporterInit(50 * 1024, fdLoadedBase);
   RelocaterInit(100 * 1024, fdImageBase, fdLoadedBase);
 end; 
 cPosition := fdLoadedBase + fdEntryPoint;
 asm
  PUSHAD
  MOV EAX,cPosition
  MOV pPosition,EAX
  POPAD
 end;
 VirtualQueryEx(ProcInfo.hProcess, pPosition, MemInfo, sizeof(MEMORY_BASIC_INFORMATION));
 pPosition := MemInfo.BaseAddress;
 cSize := MemInfo.RegionSize;
 asm
   PUSHAD
   MOV EAX,pPosition
   ADD EAX,cSize
   MOV pPosition,EAX
   POPAD
 end;
 VirtualQueryEx(ProcInfo.hProcess, pPosition, MemInfo, sizeof(MEMORY_BASIC_INFORMATION));
 cSize := MemInfo.RegionSize;
 asm
   PUSHAD
   MOV EAX,pPosition
   ADD EAX,cSize
   MOV wPosition,EAX
   POPAD
 end;
 cSize := wPosition - cPosition;

 dtPattern[0] := $50;
 dtPattern[1] := $83;
 dtPattern[2] := $C7;
 dtPattern[3] := $08;
 dtPattern[4] := $FF;
 dtPatternSize := 5;
 dtPatternBPXAddress[1] := Find(cPosition, cSize, @dtPattern[0], dtPatternSize, @glWildCard);
 if dtPatternBPXAddress[1] > 0 then begin
   SetBPX(dtPatternBPXAddress[1], UE_BREAKPOINT, @cbLoadLibrary);
 end else begin
   if DontLog = false then begin
     AddToLog('[Error] File is not packed with UPX 1.x - 3.x');
     AddToLog('-> Unpack ended...');
     StopDebug();
     DontLog := true;
   end;
 end;

 dtPattern[0] := $50;
 dtPattern[1] := $47;
 dtPattern[2] := $00;
 dtPattern[3] := $57;
 dtPattern[4] := $48;
 dtPattern[5] := $F2;
 dtPattern[6] := $AE;
 dtPatternSize := 7;
 dtPatternBPXAddress[2] := Find(cPosition, cSize, @dtPattern[0], dtPatternSize, @glWildCard);
 if dtPatternBPXAddress[2] > 0 then begin
   SetBPX(dtPatternBPXAddress[2], UE_BREAKPOINT, @cbGetProcAddress);
 end;

 dtPattern[0] := $57;
 dtPattern[1] := $48;
 dtPattern[2] := $F2;
 dtPattern[3] := $AE;
 dtPattern[4] := $00;
 dtPattern[5] := $FF;
 dtPatternSize := 6;
 dtPatternBPXAddress[3] := Find(cPosition, cSize, @dtPattern[0], dtPatternSize, @glWildCard);
 if dtPatternBPXAddress[3] > 0 then begin
   SetBPX(dtPatternBPXAddress[3], UE_BREAKPOINT, @cbGetProcAddress);
 end else begin
   if DontLog = false then begin
     AddToLog('[Error] File is not packed with UPX 1.x - 3.x');
     AddToLog('-> Unpack ended...');
     StopDebug();
     DontLog := true;
   end;
 end;

 dtPattern[0] := $89;
 dtPattern[1] := $F9;
 dtPattern[2] := $57;
 dtPattern[3] := $48;
 dtPattern[4] := $F2;
 dtPattern[5] := $AE;
 dtPattern[6] := $52;
 dtPattern[7] := $FF;
 dtPatternSize := 8;
 dtPatternBPXAddress[4] := Find(cPosition, cSize, @dtPattern[0], dtPatternSize, @glWildCard);
 if dtPatternBPXAddress[4] > 0 then begin
   dtPatternBPXAddress[4] := dtPatternBPXAddress[4] + 2;
   SetBPX(dtPatternBPXAddress[4], UE_BREAKPOINT, @cbGetProcAddress);
 end;

 dtPattern[0] := $61;
 dtPattern[1] := $E9;
 dtPatternSize := 2;
 dtPatternBPXAddress[5] := Find(cPosition, cSize, @dtPattern[0], dtPatternSize, @glWildCard);
 if dtPatternBPXAddress[5] > 0 then begin
   dtPatternBPXAddress[5] := dtPatternBPXAddress[5] + 1;
   SetBPX(dtPatternBPXAddress[5], UE_BREAKPOINT, @cbEntryPoint);
 end else begin
   dtPattern[0] := $83;
   dtPattern[1] := $EC;
   dtPattern[2] := $00;
   dtPattern[3] := $E9;
   dtPatternSize := 4;
   dtPatternBPXAddress[5] := Find(cPosition, cSize, @dtPattern[0], dtPatternSize, @glWildCard);
   if dtPatternBPXAddress[5] > 0 then begin
     dtPatternBPXAddress[5] := dtPatternBPXAddress[5] + 3;
     SetBPX(dtPatternBPXAddress[5], UE_BREAKPOINT, @cbEntryPoint);
   end else begin
     if DontLog = false then begin
       AddToLog('[Error] File is not packed with UPX 1.x - 3.x');
       AddToLog('-> Unpack ended...');
       StopDebug();
       DontLog := true;
     end;
   end;
 end;

 if fdFileIsDll then begin
   dtPattern[1] := $31;
   dtPattern[2] := $C0;
   dtPattern[3] := $8A;
   dtPattern[4] := $07;
   dtPattern[5] := $47;
   dtPattern[6] := $09;
   dtPattern[7] := $C0;
   dtPattern[8] := $74;
   dtPattern[9] := $22;
   dtPattern[10] := $3C;
   dtPattern[11] := $EF;
   dtPattern[12] := $77;
   dtPattern[13] := $11;
   dtPattern[14] := $01;
   dtPattern[15] := $C3;
   dtPattern[16] := $8B;
   dtPattern[17] := $03;
   dtPattern[18] := $86;
   dtPattern[19] := $C4;
   dtPattern[20] := $C1;
   dtPattern[21] := $C0;
   dtPattern[22] := $10;
   dtPattern[23] := $86;
   dtPattern[24] := $C4;
   dtPattern[25] := $01;
   dtPattern[26] := $F0;
   dtPattern[27] := $89;
   dtPattern[28] := $03;
   dtPattern[29] := $EB;
   dtPattern[30] := $E2;
   dtPattern[31] := $24;
   dtPattern[32] := $0F;
   dtPattern[33] := $C1;
   dtPattern[34] := $E0;
   dtPattern[35] := $10;
   dtPattern[36] := $66;
   dtPattern[37] := $8B;
   dtPattern[38] := $07;
   dtPattern[39] := $83;
   dtPattern[40] := $C7;
   dtPattern[41] := $02;
   dtPattern[42] := $EB;
   dtPattern[43] := $E2;
   dtPattern[44] := $2B;
   dtPatternSize := 43;
   dtPatternBPXAddress[6] := Find(cPosition, cSize, @dtPattern[1], dtPatternSize, @glWildCard);
   if dtPatternBPXAddress[6] > 0 then begin
     dtPatternBPXAddress[6] := dtPatternBPXAddress[6] - 3;
     SetBPX(dtPatternBPXAddress[6], UE_BREAKPOINT, @cbMakeSnapShoot1);
   end else begin
     if DontLog = false then begin
       AddToLog('[Error] File is not packed with UPX 1.x - 3.x');
       AddToLog('-> Unpack ended...');
       StopDebug();
     end;
   end;
 end;

end;

procedure InitializeUnpacker(szFileName:string;CallBack:Pointer);
 var
 fileExten : string;
 szTempFolder:array[0..1024] of char;
begin
 mainForm.LogBox.Clear;
 AddToLog('-> Unpack started...');
 if FileExists(szFileName) then begin
   if IsPE32FileValidEx(PAnsiChar(szFileName), UE_DEPTH_DEEP, nil) then begin
     cbInitCallBack := CallBack;
     fdImageBase := GetPE32Data(PAnsiChar(szFileName), 0, UE_IMAGEBASE);
     fdEntryPoint := GetPE32Data(PAnsiChar(szFileName), 0, UE_OEP);
     fdSizeOfImage := GetPE32Data(PAnsiChar(szFileName), 0, UE_SIZEOFIMAGE);

     SnapshootMemoryStartRVA := GetPE32Data(PAnsiChar(szFileName), 0, UE_SECTIONVIRTUALOFFSET);
     SnapshootMemorySize := fdEntryPoint - SnapshootMemoryStartRVA;

     szFileName := mainForm.browsedFile.Text;
     fileExten := ExtractFileExt(szFileName);
     UnpackFileNameBuffer := ChangeFileExt(szFileName, '.unpacked' + fileExten);
     fdFileIsDll := IsFileDLL(PAnsiChar(szFileName), 0);
     if fdFileIsDll = false then begin
        ProcInfo := InitDebug(PAnsiChar(szFileName),nil,nil);
     end else begin
        GetTempPathA(1024, szTempFolder);
        SnapShoot1 := szTempFolder + 'snapshoot.1';
        SnapShoot2 := szTempFolder + 'snapshoot.2';
        ProcInfo := InitDLLDebug(PAnsiChar(szFileName),true,nil,nil,CallBack);
     end;
     if ProcInfo <> nil then begin
       if fdFileIsDll = false then begin
         SetCustomHandler(UE_CH_CREATEPROCESS, @cbCreateProcess);
       end;
       DebugLoop();
     end else begin
       AddToLog('[Error]');
       AddToLog('-> Unpack ended...');
     end;
   end else begin
     AddToLog('[Error] Selected file is not a valid PE32 file!');
     AddToLog('-> Unpack ended...');
   end;
 end;
end;

procedure TmainForm.FormCreate(Sender: TObject);
var
  j : integer;
begin
  hInstance := GetModuleHandle(nil);
  GetModuleFileName(0, GlobalUnpackerFolderBuffer, 1024);
  j := Length(GlobalUnpackerFolderBuffer);
  while GlobalUnpackerFolderBuffer[j] <> '\' do begin
    j := j - 1;
  end;
  GlobalUnpackerFolderBuffer[j+1] := #00;
end;

procedure TmainForm.BitBtn4Click(Sender: TObject);
begin
 GlobalBuffer := mainForm.browsedFile.Text;
 if UnpackerRunning = false then begin
   UnpackerRunning := true;
   InitializeUnpacker(GlobalBuffer, @cbFindPatterns);
   UnpackerRunning := false;
 end;
end;

procedure TmainForm.BitBtn1Click(Sender: TObject);
begin
  if OpenDialog1.Execute then begin
   mainForm.browsedFile.Text := OpenDialog1.FileName;
  end;
end;

procedure TmainForm.BitBtn2Click(Sender: TObject);
begin
  Application.Terminate;
end;

procedure TmainForm.BitBtn3Click(Sender: TObject);
begin
  messagedlg('RL!deUPX 1.x - 3.x unpacker'+ #13 + #10 + #13 + #10 + 'Visit Reversing Labs at http://www.reversinglabs.com'+ #13 + #10 + #13 + #10 + '  Minimum engine version needed:'+ #13 + #10 + '- TitanEngine 2.0 by RevLabs' + #13 + #10 + #13 + #10 + 'Unpacker coded by Reversing Labs', mtInformation, [mbOk], 0);
end;

end.
