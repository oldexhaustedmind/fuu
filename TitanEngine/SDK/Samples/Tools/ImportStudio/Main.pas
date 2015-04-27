unit Main;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ComCtrls, ImgList, ToolWin, XPMan, StdCtrls, Buttons, Plugin, SDK, psapi,
  jpeg, ExtCtrls, Menus, ShellApi, StrUtils;

type
  TfrmMain = class(TForm)
    XPManifest1: TXPManifest;
    ImageList1: TImageList;
    ImageList2: TImageList;
    SaveDialog1: TSaveDialog;
    StatusBar1: TStatusBar;
    SaveDialog2: TSaveDialog;
    SaveDialog3: TSaveDialog;
    PageControl2: TPageControl;
    TabSheet3: TTabSheet;
    ToolBar1: TToolBar;
    ToolButton24: TToolButton;
    ToolButton23: TToolButton;
    ToolButton1: TToolButton;
    GrabData: TToolButton;
    ToolButton2: TToolButton;
    ToolButton8: TToolButton;
    ToolButton4: TToolButton;
    ToolButton9: TToolButton;
    ToolButton11: TToolButton;
    ToolButton10: TToolButton;
    ToolButton12: TToolButton;
    ToolButton13: TToolButton;
    ImportData: TListView;
    DataGrabber: TGroupBox;
    GroupBox2: TGroupBox;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    szSearchStart: TEdit;
    szSearchSize: TEdit;
    BitBtn1: TBitBtn;
    GroupBox3: TGroupBox;
    rbOne: TRadioButton;
    rbTwo: TRadioButton;
    rbThree: TRadioButton;
    rbFour: TCheckBox;
    rbCustomStep: TEdit;
    GroupBox4: TGroupBox;
    chkSkipUnknown: TCheckBox;
    chkSkipLastInvalid: TCheckBox;
    chkFixForwarderCalls: TCheckBox;
    chkAutoTrace: TCheckBox;
    chkSkipInvalid: TCheckBox;
    BitBtn2: TBitBtn;
    BitBtn3: TBitBtn;
    BitBtn4: TBitBtn;
    ImportEditor: TGroupBox;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    Label7: TLabel;
    BitBtn6: TBitBtn;
    BitBtn7: TBitBtn;
    BitBtn8: TBitBtn;
    cmbSelectedDLL: TComboBox;
    cmbImportName: TComboBox;
    szIATAddress: TEdit;
    chkStolenImport: TCheckBox;
    Highliter: TGroupBox;
    Label8: TLabel;
    Label9: TLabel;
    Label10: TLabel;
    Label11: TLabel;
    Label12: TLabel;
    BitBtn10: TBitBtn;
    hltLibrary: TComboBox;
    hltImportName: TComboBox;
    szHighliteMin: TEdit;
    szHighliteMax: TEdit;
    BitBtn5: TBitBtn;
    BitBtn9: TBitBtn;
    BitBtn11: TBitBtn;
    BitBtn12: TBitBtn;
    BitBtn13: TBitBtn;
    TraceData: TGroupBox;
    GroupBox6: TGroupBox;
    Label13: TLabel;
    Label14: TLabel;
    Label15: TLabel;
    szSearchFrom: TEdit;
    szMaxInstructions: TEdit;
    BitBtn14: TBitBtn;
    GroupBox8: TGroupBox;
    chkTraceLevel1: TCheckBox;
    chkHashTracerLevel1: TCheckBox;
    chkTracerSkipInvalid: TCheckBox;
    chkDetectRedirections: TCheckBox;
    BitBtn15: TBitBtn;
    BitBtn17: TBitBtn;
    TabSheet4: TTabSheet;
    GroupBox1: TGroupBox;
    ActionLog: TListBox;
    ToolBar2: TToolBar;
    ToolButton3: TToolButton;
    ToolButton5: TToolButton;
    ToolButton7: TToolButton;
    ToolButton6: TToolButton;
    Image1: TImage;
    ToolButton14: TToolButton;
    ToolButton15: TToolButton;
    DumpProc: TGroupBox;
    GroupBox5: TGroupBox;
    Label16: TLabel;
    szEntryAddress: TEdit;
    BitBtn16: TBitBtn;
    BitBtn20: TBitBtn;
    SaveDialog4: TSaveDialog;
    ToolButton16: TToolButton;
    ToolButton17: TToolButton;
    BitBtn19: TBitBtn;
    PopupMenu1: TPopupMenu;
    Imports1: TMenuItem;
    DeleteSelected1: TMenuItem;
    AddEditimport1: TMenuItem;
    ResolvedAPIs1: TMenuItem;
    Savelist1: TMenuItem;
    Loadlist1: TMenuItem;
    Deletetrunks1: TMenuItem;
    N1: TMenuItem;
    race1: TMenuItem;
    raceLevel11: TMenuItem;
    HashTraceLevel11: TMenuItem;
    N3: TMenuItem;
    Clearlist1: TMenuItem;
    Goto1: TMenuItem;
    Selectedaddress1: TMenuItem;
    Selectedaddresspointer1: TMenuItem;
    chkPastePEHeader: TCheckBox;
    TabSheet1: TTabSheet;
    GroupBox7: TGroupBox;
    Image2: TImage;
    Label17: TLabel;
    BitBtn23: TBitBtn;
    AutoFix: TGroupBox;
    Label18: TLabel;
    szSetEP: TEdit;
    GroupBox10: TGroupBox;
    chkPastePEHeaderFirst: TCheckBox;
    chkDumpProcessFirst: TCheckBox;
    chkAutoFixElimination: TCheckBox;
    chkAutoFixRedirection: TCheckBox;
    BitBtn18: TBitBtn;
    BitBtn21: TBitBtn;
    ImpRec1: TMenuItem;
    Browse1: TMenuItem;
    OpenDialog1: TOpenDialog;
    Selectinvalid1: TMenuItem;
    Selecthighlited1: TMenuItem;
    OpenDialog2: TOpenDialog;
    SaveDialog5: TSaveDialog;
    chkAutoFindIATRange: TCheckBox;
    ToolButton18: TToolButton;
    ToolButton19: TToolButton;
    OpenDialog3: TOpenDialog;
    ToolButton20: TToolButton;
    procedure BitBtn4Click(Sender: TObject);
    procedure GrabDataClick(Sender: TObject);
    procedure BitBtn2Click(Sender: TObject);
    procedure BitBtn1Click(Sender: TObject);
    procedure BitBtn3Click(Sender: TObject);
    procedure ToolButton7Click(Sender: TObject);
    procedure ToolButton5Click(Sender: TObject);
    procedure BitBtn8Click(Sender: TObject);
    procedure ToolButton2Click(Sender: TObject);
    procedure cmbSelectedDLLChange(Sender: TObject);
    procedure BitBtn6Click(Sender: TObject);
    procedure BitBtn7Click(Sender: TObject);
    procedure ToolButton8Click(Sender: TObject);
    procedure ToolButton9Click(Sender: TObject);
    procedure BitBtn10Click(Sender: TObject);
    procedure BitBtn13Click(Sender: TObject);
    procedure ImportDataCustomDrawItem(Sender: TCustomListView;
      Item: TListItem; State: TCustomDrawState; var DefaultDraw: Boolean);
    procedure BitBtn12Click(Sender: TObject);
    procedure BitBtn11Click(Sender: TObject);
    procedure BitBtn5Click(Sender: TObject);
    procedure BitBtn9Click(Sender: TObject);
    procedure ToolButton11Click(Sender: TObject);
    procedure Label11Click(Sender: TObject);
    procedure chkStolenImportClick(Sender: TObject);
    procedure ToolButton12Click(Sender: TObject);
    procedure BitBtn17Click(Sender: TObject);
    procedure BitBtn15Click(Sender: TObject);
    procedure BitBtn14Click(Sender: TObject);
    procedure ToolButton23Click(Sender: TObject);
    procedure ToolButton15Click(Sender: TObject);
    procedure BitBtn19Click(Sender: TObject);
    procedure BitBtn16Click(Sender: TObject);
    procedure ToolButton14Click(Sender: TObject);
    procedure ToolButton16Click(Sender: TObject);
    procedure BitBtn20Click(Sender: TObject);
    procedure Clearlist1Click(Sender: TObject);
    procedure Deletetrunks1Click(Sender: TObject);
    procedure Selectedaddress1Click(Sender: TObject);
    procedure Selectedaddresspointer1Click(Sender: TObject);
    procedure BitBtn21Click(Sender: TObject);
    procedure BitBtn23Click(Sender: TObject);
    procedure BitBtn18Click(Sender: TObject);
    procedure Browse1Click(Sender: TObject);
    procedure Selectinvalid1Click(Sender: TObject);
    procedure Image2Click(Sender: TObject);
    procedure hltLibraryChange(Sender: TObject);
    procedure Selecthighlited1Click(Sender: TObject);
    procedure raceLevel11Click(Sender: TObject);
    procedure Savelist1Click(Sender: TObject);
    procedure Loadlist1Click(Sender: TObject);
    procedure ToolButton19Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  frmMain: TfrmMain;
  optionClear:Boolean;
  hProcess,ImageBase,SizeOfImage,SectionAligment,OriginalEntryPoint:LongInt;
  OriginalIATAddress:string;

  function GetProcessImageFileName(hProcess:LongInt;pPIDBases:Pointer;nSize:LongInt): PChar; stdcall;  external 'psapi.dll' name 'GetProcessImageFileNameA';
  function WriteProcessMemoryFixed(hProcess: Integer; lpBaseAddress: Integer; lpBuffer: Pointer; nSize: DWORD;lpNumberOfBytesRead: Pointer): BOOL; stdcall;  external 'kernel32.dll' name 'WriteProcessMemory';
  function ReadProcessMemoryFixed(hProcess: Integer; lpBaseAddress: Integer; lpBuffer: Pointer; nSize: DWORD;lpNumberOfBytesRead: Pointer): BOOL; stdcall;  external 'kernel32.dll' name 'ReadProcessMemory';
  function cRtlMoveMemory(Destination:Pointer; Source:LongInt; Size:LongInt): LongInt; stdcall;  external 'kernel32.dll' name 'RtlMoveMemory';

implementation

{$R *.dfm}

function ConvertIntegerToPointer(InputInteger:LongInt):Pointer;
 var
 Return:Pointer;
begin
  asm
    PUSHAD
    MOV EAX,InputInteger
    MOV Return,EAX
    POPAD
  end;
  ConvertIntegerToPointer := Return;
end;

procedure AddToLog(LogString:string);
begin
 frmMain.ActionLog.Items.Add(LogString);
end;

function CreateVirtual(size:integer):integer;
 var
  retn:integer;
begin
 retn := 0;
 asm
  PUSHAD
  PUSH PAGE_READWRITE
  PUSH MEM_COMMIT
  PUSH size
  PUSH 0
  CALL VirtualAlloc
  MOV retn,EAX
  POPAD
 end;
  CreateVirtual := retn;
end;

function FreeVirtual(address:pointer;size:integer):boolean;
begin
 FreeVirtual := VirtualFree(address,size,MEM_DECOMMIT);
end;

function ReadBuffer(buffaddress:pointer;address,size:integer):integer;
begin
 ReadBuffer := Readmemory(buffaddress,address,size,MM_SILENT);
end;

procedure GetAllLoadedModules;
 var
 pid,cbNeeded:Cardinal;
 va,vf:Pointer;
 aVA:LongInt;
 hVA:PLongInt;
 hDll:PLongInt;
 hwDll:LongInt;
 j,AlreadyAdded:LongInt;
 fName:array[0..MAX_PATH] of char;
begin
 frmMain.cmbSelectedDLL.Clear;
 pid := Plugingetvalue(VAL_HPROCESS);
 va := VirtualAlloc(nil,$2000,MEM_COMMIT,PAGE_READWRITE);
 vf := va;
 EnumProcessModules(pid,va,$800,cbNeeded);
 hDll := PLongInt(va);
 hwDll := hDll^;
 hVA := PLongInt(va);
 while hwDll > 0 do begin
    hVA := PLongInt(va);
    aVA := hVA^;
    hDll := PLongInt(va);
    hwDll := hDll^;
    asm
     PUSHAD
     MOV EAX,va
     ADD EAX,4
     MOV va,EAX
     POPAD
    end;
    if hwDll > 0 then begin
     GetModuleFileNameEx(pid,hwDll,@fName,MAX_PATH);
     if (ExtractFileExt(fName) <> '.exe') and (ExtractFileExt(fName) <> '.EXE') and (ExtractFileExt(fName) <> '.eXe') and (ExtractFileExt(fName) <> '.ExE') then begin
        AlreadyAdded := 0;
        for j := 0 to frmMain.cmbSelectedDLL.Items.Count - 1 do begin
          if LowerCase(frmMain.cmbSelectedDLL.Items[j]) = LowerCase(fName) then AlreadyAdded := 1;
        end;
        if AlreadyAdded = 0 then frmMain.cmbSelectedDLL.Items.Add(LowerCase(ExtractFileName(fName)));
     end;
    end;
 end;
 VirtualFree(vf,0,MEM_RELEASE);
end;

procedure GetAllExports;
var
 szAPIn:string;
 tEAX,Num,i:LongInt;
 tbx:pointer;
 hModul : Cardinal;
 OLDProtect:LongInt;
begin
if frmMain.cmbSelectedDLL.Text <> '' then begin
  OLDProtect := $02040001;
  szAPIn := 'This is one very very very long string!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!';
  tbx := @szAPIn;
 asm
  PUSHAD
  MOV EAX,tbx
  MOV EAX,DWORD PTR[EAX]
  MOV tbx,EAX
  POPAD
 end;
  VirtualProtect(tbx,Length(szAPIn),$40,@OLDProtect);
  hModul := GetModuleHandleA(PChar(frmMain.cmbSelectedDLL.Text));
 if hModul > 0 then begin
  frmMain.cmbImportName.Clear;
 asm
  PUSHAD
  JMP @over
@L071:
  PUSHAD
  MOV EBX,DWORD PTR SS:[ESP+$24]
  MOV ECX,EBX
  ADD EBX,DWORD PTR DS:[EBX+$3C]
  MOV EBX,DWORD PTR DS:[EBX+$78]
  ADD EBX,ECX
  MOV EDI,DWORD PTR DS:[EBX+$20]
  ADD EDI,ECX
  MOV ESI,DWORD PTR DS:[EBX+18h]
  MOV DWORD PTR SS:[ESP+$1C],ESI
  POPAD
  RET 8
@over:
  PUSH 0
  PUSH DWORD PTR[hModul]
  CALL @L071
  MOV tEAX,EAX
  POPAD
 end;

  Num := tEax;

for i := 0 to Num -1 do begin
 asm
  PUSHAD
  JMP @over
@L071:
  PUSHAD
  MOV EBX,DWORD PTR SS:[ESP+$24]
  MOV ECX,EBX
  ADD EBX,DWORD PTR DS:[EBX+$3C]
  MOV EBX,DWORD PTR DS:[EBX+$78]
  ADD EBX,ECX
  MOV EDI,DWORD PTR DS:[EBX+$20]
  ADD EDI,ECX
  MOV ESI,DWORD PTR SS:[ESP+$28]
  CMP ESI,DWORD PTR DS:[EBX+18h]
  JE @exit
@L080:
  LEA EDX,DWORD PTR DS:[EDI+ESI*4]
  MOV EDX,DWORD PTR DS:[EDX]
  ADD EDX,ECX

@L094:
  MOV DWORD PTR SS:[ESP+$1C],EDX
  POPAD
  RET 8
@exit:
  MOV DWORD PTR SS:[ESP+$1C],0
  POPAD
  RET 8
@over:
  PUSH DWORD PTR[i]
  PUSH DWORD PTR[hModul]
  CALL @L071
  CMP EAX,0
  JNE @found
  MOV tEAX,0
  JMP @done
@found:
  MOV tEAX,EAX
@done:
  POPAD
 end;
  tbx := @szAPIn;
 asm
  PUSHAD
  MOV EAX,DWORD PTR[tbx]
  MOV EAX,DWORD PTR[eax]
  MOV ECX,254
  @loop:
  MOV BYTE PTR[EAX],0
  INC EAX
  DEC ECX
  JNE @loop
  MOV EAX,DWORD PTR[tEAX]
  MOV EBX,DWORD PTR[tbx]
  MOV EBX,DWORD PTR[ebx]
@copyname:
  MOV CL,BYTE PTR[EAX]
  MOV BYTE PTR[EBX],CL
  INC EAX
  INC EBX
  CMP CL,0
  JNE @copyname
  POPAD
 end;
 frmMain.cmbImportName.Items.Add(szAPIn);
end;
 if frmMain.cmbImportName.Items.Count > 1 then frmMain.cmbImportName.Text := frmMain.cmbImportName.Items[0];
 end else begin
   if frmMain.ImportEditor.Visible = true then messagedlg('This action can be performed only on currently loaded modules, to procede please load selected module!',mtWarning,[mbOk],0);
 end;
end;
end;

procedure GetAllExportsHighLight;
var
 szAPIn:string;
 tEAX,Num,i:LongInt;
 tbx:pointer;
 hModul : Cardinal;
 OLDProtect:LongInt;
begin
if frmMain.hltLibrary.Text <> '' then begin
  OLDProtect := $02040001;
  szAPIn := 'This is one very very very long string!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!';
  tbx := @szAPIn;
 asm
  PUSHAD
  MOV EAX,tbx
  MOV EAX,DWORD PTR[EAX]
  MOV tbx,EAX
  POPAD
 end;
  VirtualProtect(tbx,Length(szAPIn),$40,@OLDProtect);
  hModul := GetModuleHandleA(PChar(frmMain.hltLibrary.Text));
 if hModul > 0 then begin
  frmMain.hltImportName.Clear;
 asm
  PUSHAD
  JMP @over
@L071:
  PUSHAD
  MOV EBX,DWORD PTR SS:[ESP+$24]
  MOV ECX,EBX
  ADD EBX,DWORD PTR DS:[EBX+$3C]
  MOV EBX,DWORD PTR DS:[EBX+$78]
  ADD EBX,ECX
  MOV EDI,DWORD PTR DS:[EBX+$20]
  ADD EDI,ECX
  MOV ESI,DWORD PTR DS:[EBX+18h]
  MOV DWORD PTR SS:[ESP+$1C],ESI
  POPAD
  RET 8
@over:
  PUSH 0
  PUSH DWORD PTR[hModul]
  CALL @L071
  MOV tEAX,EAX
  POPAD
 end;

  Num := tEax;

for i := 0 to Num -1 do begin
 asm
  PUSHAD
  JMP @over
@L071:
  PUSHAD
  MOV EBX,DWORD PTR SS:[ESP+$24]
  MOV ECX,EBX
  ADD EBX,DWORD PTR DS:[EBX+$3C]
  MOV EBX,DWORD PTR DS:[EBX+$78]
  ADD EBX,ECX
  MOV EDI,DWORD PTR DS:[EBX+$20]
  ADD EDI,ECX
  MOV ESI,DWORD PTR SS:[ESP+$28]
  CMP ESI,DWORD PTR DS:[EBX+18h]
  JE @exit
@L080:
  LEA EDX,DWORD PTR DS:[EDI+ESI*4]
  MOV EDX,DWORD PTR DS:[EDX]
  ADD EDX,ECX

@L094:
  MOV DWORD PTR SS:[ESP+$1C],EDX
  POPAD
  RET 8
@exit:
  MOV DWORD PTR SS:[ESP+$1C],0
  POPAD
  RET 8
@over:
  PUSH DWORD PTR[i]
  PUSH DWORD PTR[hModul]
  CALL @L071
  CMP EAX,0
  JNE @found
  MOV tEAX,0
  JMP @done
@found:
  MOV tEAX,EAX
@done:
  POPAD
 end;
  tbx := @szAPIn;
 asm
  PUSHAD
  MOV EAX,DWORD PTR[tbx]
  MOV EAX,DWORD PTR[eax]
  MOV ECX,254
  @loop:
  MOV BYTE PTR[EAX],0
  INC EAX
  DEC ECX
  JNE @loop
  MOV EAX,DWORD PTR[tEAX]
  MOV EBX,DWORD PTR[tbx]
  MOV EBX,DWORD PTR[ebx]
@copyname:
  MOV CL,BYTE PTR[EAX]
  MOV BYTE PTR[EBX],CL
  INC EAX
  INC EBX
  CMP CL,0
  JNE @copyname
  POPAD
 end;
 frmMain.hltImportName.Items.Add(szAPIn);
end;
 if frmMain.hltImportName.Items.Count > 1 then frmMain.hltImportName.Text := frmMain.hltImportName.Items[0];
 end else begin
   if frmMain.Highliter.Visible = true then messagedlg('This action can be performed only on currently loaded modules, to procede please load selected module!',mtWarning,[mbOk],0);
 end;
end;
end;

procedure GetImportData(oClearList:boolean);
var
 ImportStart,ImportReadPointer,ImportSize,ImportSizeMax,ImportStep,dwPossibleAPI,TestPointer,CheckAddress:LongInt;
 pAPIName,pDLLName:PChar;
 szAPIName,szDLLName,szLastDLLName:string;
 newitem:TListItem;
 i:integer;
 pDLLBases:Pointer;
 cbNeeded:Cardinal;
 NumberOfValidInstuctions,RedirectionType:LongInt;
 NumberOfBytesRead:Cardinal;
begin

 hProcess := Plugingetvalue(VAL_HPROCESS);
 if hProcess = 0 then Abort;
 pDLLBases := VirtualAlloc(nil,$2000,MEM_COMMIT,PAGE_READWRITE);
 EnumProcessModules(hProcess,pDLLBases,$800,cbNeeded);
 if oClearList = True then frmMain.ImportData.Clear;
 ImportStart := StrToInt('$' + frmMain.szSearchStart.Text);
 ImportReadPointer := ImportStart;
 ImportSize := StrToInt('$' + frmMain.szSearchSize.Text);
 ImportSizeMax := ImportSize;
 ImportStep := 1;
 if frmMain.rbFour.Checked = True then begin
   ImportStep := StrToInt('$' + frmMain.rbCustomStep.Text);
 end else begin
   if frmMain.rbOne.Checked = True then ImportStep := 1;
   if frmMain.rbTwo.Checked = True then ImportStep := 4;
   if frmMain.rbThree.Checked = True then ImportStep := 5;
 end;

 while ImportSize > 0 do begin
    szLastDLLName := LowerCase(szDllName);
    frmMain.StatusBar1.SimpleText := ' Progress: ' + IntToStr(Round(((ImportSizeMax - ImportSize)/ImportSizeMax)*100)) + '%';
    dwPossibleAPI := 0;
    if ReadProcessMemoryFixed(hProcess,ImportReadPointer,@dwPossibleAPI,4,@NumberOfBytesRead) = true then begin
      pAPIName := ImporterGetAPINameFromDebugee(hProcess,dwPossibleAPI);
      szAPIName := pAPIName;
      if pAPIName = nil then begin
        szAPIName := IntToHex(ImporterGetAPIOrdinalNumberFromDebugee(hProcess,dwPossibleAPI),8);
        if szAPIName <> 'FFFFFFFF' then pAPIName := PChar(szAPIName);
      end;
    end else begin
      pAPIName := nil;
    end;
    if pAPIName <> nil then begin
      pDLLName := ImporterGetDLLNameFromDebugee(hProcess,dwPossibleAPI);
      szDLLName := pDLLName;
      if pAPIName <> nil then begin
        if (frmMain.chkSkipLastInvalid.Checked = True) then begin
          if (szLastDLLName <> LowerCase(szDllName)) and (frmMain.ImportData.Items.Count > 1) then begin
            newitem := frmMain.ImportData.Items.Add;
            newitem.Caption := IntToHex(ImportReadPointer,8);
            newitem.SubItems.Add(LowerCase(szDllName));
            newitem.SubItems.Add(szAPIName);
            newitem.SubItems.Add('No');
            newitem.ImageIndex := 0;
            newitem.Checked := False;
            AddToLog('Found API: ' + szAPIName + ' from library: ' + LowerCase(szDllName) + ' at address: ' + IntToHex(ImportReadPointer,8));
            ImportReadPointer := ImportReadPointer + 4 - ImportStep;
            ImportSize := ImportSize - 4 + ImportStep;
            CheckAddress := StrToInt('$' + frmMain.ImportData.Items[frmMain.ImportData.Items.Count-1].Caption);
            for i := frmMain.ImportData.Items.Count - 1 downto 0 do begin
              if (StrToInt('$' + frmMain.ImportData.Items[i].Caption) >= CheckAddress - 4) and (frmMain.ImportData.Items[i].ImageIndex = 1) then begin
               AddToLog('Removed invalid API at address: ' + frmMain.ImportData.Items[i].Caption);
               frmMain.ImportData.Items[i].Delete;
              end;
            end;
          end else begin
            newitem := frmMain.ImportData.Items.Add;
            newitem.Caption := IntToHex(ImportReadPointer,8);
            newitem.SubItems.Add(LowerCase(szDllName));
            newitem.SubItems.Add(szAPIName);
            newitem.SubItems.Add('No');
            newitem.ImageIndex := 0;
            newitem.Checked := False;
            AddToLog('Found API: ' + szAPIName + ' from library: ' + LowerCase(szDllName) + ' at address: ' + IntToHex(ImportReadPointer,8));
            ImportReadPointer := ImportReadPointer + 4 - ImportStep;
            ImportSize := ImportSize - 4 + ImportStep;
          end;
        end else begin
          newitem := frmMain.ImportData.Items.Add;
          newitem.Caption := IntToHex(ImportReadPointer,8);
          newitem.SubItems.Add(LowerCase(szDllName));
          newitem.SubItems.Add(szAPIName);
          newitem.SubItems.Add('No');
          newitem.ImageIndex := 0;
          newitem.Checked := False;
          AddToLog('Found API: ' + szAPIName + ' from library: ' + LowerCase(szDllName) + ' at address: ' + IntToHex(ImportReadPointer,8));
          ImportReadPointer := ImportReadPointer + 4 - ImportStep;
          ImportSize := ImportSize - 4 + ImportStep;
        end;
      end else begin
        TestPointer := dwPossibleAPI;
        dwPossibleAPI := Readmemory(@dwPossibleAPI,TestPointer,4,MM_RESILENT);
        if (frmMain.chkSkipUnknown.Checked = False) and (dwPossibleAPI <> 0) then begin
          newitem := frmMain.ImportData.Items.Add;
          newitem.Caption := IntToHex(ImportReadPointer,8);
          newitem.SubItems.Add('?');
          newitem.SubItems.Add('?');
          newitem.SubItems.Add('No');
          newitem.ImageIndex := 1;
          newitem.Checked := False;
          AddToLog('(7) Possible API at address: ' + IntToHex(ImportReadPointer,8));
        end else begin
          if frmMain.chkSkipInvalid.Checked = False then begin
            newitem := frmMain.ImportData.Items.Add;
            newitem.Caption := IntToHex(ImportReadPointer,8);
            newitem.SubItems.Add('?');
            newitem.SubItems.Add('?');
            newitem.SubItems.Add('No');
            newitem.ImageIndex := 1;
            newitem.Checked := False;
            AddToLog('(6) Possible API at address: ' + IntToHex(ImportReadPointer,8));
          end;
        end;
     end;
    end else begin
      NumberOfValidInstuctions := 0;
      if frmMain.chkAutoTrace.Checked = True then RedirectionType := TracerDetectRedirection(hProcess,dwPossibleAPI) else RedirectionType := 0;
      if RedirectionType = 0 then begin
        if frmMain.chkAutoTrace.Checked = True then NumberOfValidInstuctions := TracerLevel1(hProcess,dwPossibleAPI);
        if (NumberOfValidInstuctions > 0) and (NumberOfValidInstuctions < $1000) then begin
          if frmMain.chkAutoTrace.Checked = True then NumberOfValidInstuctions := HashTracerLevel1(hProcess,dwPossibleAPI,NumberOfValidInstuctions);
          if (NumberOfValidInstuctions < $1000) then begin
            TestPointer := dwPossibleAPI;
            dwPossibleAPI := Readmemory(@dwPossibleAPI,TestPointer,4,MM_RESILENT);
            if (frmMain.chkSkipUnknown.Checked = False) and (dwPossibleAPI <> 0) then begin
              newitem := frmMain.ImportData.Items.Add;
              newitem.Caption := IntToHex(ImportReadPointer,8);
              newitem.SubItems.Add('?');
              newitem.SubItems.Add('?');
              newitem.SubItems.Add('No');
              newitem.ImageIndex := 1;
              newitem.Checked := False;
              AddToLog('(5) Possible API at address: ' + IntToHex(ImportReadPointer,8));
            end else begin
              newitem := frmMain.ImportData.Items.Add;
              newitem.Caption := IntToHex(ImportReadPointer,8);
              newitem.SubItems.Add('?');
              newitem.SubItems.Add('?');
              newitem.SubItems.Add('No');
              newitem.ImageIndex := 1;
              newitem.Checked := False;
              AddToLog('(4) Possible API at address: ' + IntToHex(ImportReadPointer,8));
            end;
          end else begin
            pAPIName := ImporterGetAPINameFromDebugee(hProcess,NumberOfValidInstuctions);
            szAPIName := pAPIName;
            if pAPIName = nil then begin
              szAPIName := IntToHex(ImporterGetAPIOrdinalNumberFromDebugee(hProcess,NumberOfValidInstuctions),8);
              if szAPIName <> 'FFFFFFFF' then pAPIName := PChar(szAPIName);
            end;
            pDLLName := ImporterGetDLLNameFromDebugee(hProcess,NumberOfValidInstuctions);
            szDLLName := pDLLName;
            if (pAPIName <> nil) and (pDLLName <> nil) then begin
              newitem := frmMain.ImportData.Items.Add;
              newitem.Caption := IntToHex(ImportReadPointer,8);
              newitem.SubItems.Add(LowerCase(szDllName));
              newitem.SubItems.Add(szAPIName);
              newitem.SubItems.Add('No');
              newitem.ImageIndex := 0;
              newitem.Checked := False;
              AddToLog('Fixed API at address: ' + IntToHex(ImportReadPointer,8) + ' to library: ' + szDLLName + ' import: ' + szAPIName);
            end else begin
              newitem := frmMain.ImportData.Items.Add;
              newitem.Caption := IntToHex(ImportReadPointer,8);
              newitem.SubItems.Add('?');
              newitem.SubItems.Add('?');
              newitem.SubItems.Add('No');
              newitem.ImageIndex := 1;
              newitem.Checked := False;
              AddToLog('(3) Possible API at address: ' + IntToHex(ImportReadPointer,8));
            end;
          end;
        end else begin
          pAPIName := ImporterGetAPINameFromDebugee(hProcess,NumberOfValidInstuctions);
          szAPIName := pAPIName;
          if pAPIName = nil then begin
            szAPIName := IntToHex(ImporterGetAPIOrdinalNumberFromDebugee(hProcess,NumberOfValidInstuctions),8);
            if szAPIName <> 'FFFFFFFF' then pAPIName := PChar(szAPIName);
          end;
          pDLLName := ImporterGetDLLNameFromDebugee(hProcess,NumberOfValidInstuctions);
          szDLLName := pDLLName;
          if (pAPIName <> nil) and (pDLLName <> nil) then begin
            newitem := frmMain.ImportData.Items.Add;
            newitem.Caption := IntToHex(ImportReadPointer,8);
            newitem.SubItems.Add(LowerCase(szDllName));
            newitem.SubItems.Add(szAPIName);
            newitem.SubItems.Add('No');
            newitem.ImageIndex := 0;
            newitem.Checked := False;
            AddToLog('Traced API hash at address: ' + IntToHex(ImportReadPointer,8) + ' to library: ' + szDLLName + ' import: ' + szAPIName);
          end else begin
            Readmemory(@TestPointer,ImportReadPointer,4,MM_RESILENT);
            if (Readmemory(@dwPossibleAPI,TestPointer,4,MM_RESILENT) <> 0) or (frmMain.chkSkipInvalid.Checked = False) then begin
              newitem := frmMain.ImportData.Items.Add;
              newitem.Caption := IntToHex(ImportReadPointer,8);
              newitem.SubItems.Add('?');
              newitem.SubItems.Add('?');
              newitem.SubItems.Add('No');
              newitem.ImageIndex := 1;
              newitem.Checked := False;
              AddToLog('(2) Possible API at address: ' + IntToHex(ImportReadPointer,8));
            end;
          end;
        end;
      end else begin
        AddToLog('Detected API redirection: ' + IntToStr(RedirectionType) + ' at address: ' + IntToHex(ImportReadPointer,8));
        NumberOfValidInstuctions := TracerFixKnownRedirection(hProcess,dwPossibleAPI,RedirectionType);
        pAPIName := ImporterGetAPINameFromDebugee(hProcess,NumberOfValidInstuctions);
        szAPIName := pAPIName;
        if pAPIName = nil then begin
          szAPIName := IntToHex(ImporterGetAPIOrdinalNumberFromDebugee(hProcess,NumberOfValidInstuctions),8);
          if szAPIName <> 'FFFFFFFF' then pAPIName := PChar(szAPIName);
        end;
        pDLLName := ImporterGetDLLNameFromDebugee(hProcess,NumberOfValidInstuctions);
        szDLLName := pDLLName;
        if (pAPIName <> nil) and (pDLLName <> nil) then begin
          newitem := frmMain.ImportData.Items.Add;
          newitem.Caption := IntToHex(ImportReadPointer,8);
          newitem.SubItems.Add(LowerCase(szDllName));
          newitem.SubItems.Add(szAPIName);
          newitem.SubItems.Add('No');
          newitem.ImageIndex := 0;
          newitem.Checked := False;
          AddToLog('Traced API at address: ' + IntToHex(ImportReadPointer,8) + ' to library: ' + szDLLName + ' import: ' + szAPIName);
        end else begin
          if frmMain.chkAutoTrace.Checked = True then NumberOfValidInstuctions := TracerLevel1(hProcess,dwPossibleAPI);
          if (NumberOfValidInstuctions > 0) and (NumberOfValidInstuctions < $1000) then begin
            if frmMain.chkAutoTrace.Checked = True then NumberOfValidInstuctions := HashTracerLevel1(hProcess,dwPossibleAPI,NumberOfValidInstuctions);
            if (NumberOfValidInstuctions < $1000) then begin
              newitem := frmMain.ImportData.Items.Add;
              newitem.Caption := IntToHex(ImportReadPointer,8);
              newitem.SubItems.Add('?');
              newitem.SubItems.Add('?');
              newitem.SubItems.Add('No');
              newitem.ImageIndex := 1;
              newitem.Checked := False;
              AddToLog('(1) Possible API at address: ' + IntToHex(ImportReadPointer,8));
            end else begin
              pAPIName := ImporterGetAPINameFromDebugee(hProcess,NumberOfValidInstuctions);
              szAPIName := pAPIName;
              if pAPIName = nil then begin
                szAPIName := IntToHex(ImporterGetAPIOrdinalNumberFromDebugee(hProcess,NumberOfValidInstuctions),8);
                if szAPIName <> 'FFFFFFFF' then pAPIName := PChar(szAPIName);
              end;
              pDLLName := ImporterGetDLLNameFromDebugee(hProcess,NumberOfValidInstuctions);
              szDLLName := pDLLName;
              if (pAPIName <> nil) and (pDLLName <> nil) then begin
                newitem := frmMain.ImportData.Items.Add;
                newitem.Caption := IntToHex(ImportReadPointer,8);
                newitem.SubItems.Add(LowerCase(szDllName));
                newitem.SubItems.Add(szAPIName);
                newitem.SubItems.Add('No');
                newitem.ImageIndex := 0;
                newitem.Checked := False;
                AddToLog('Traced API at address: ' + IntToHex(ImportReadPointer,8) + ' to library: ' + szDLLName + ' import: ' + szAPIName);
              end else begin
                newitem := frmMain.ImportData.Items.Add;
                newitem.Caption := IntToHex(ImportReadPointer,8);
                newitem.SubItems.Add('?');
                newitem.SubItems.Add('?');
                newitem.SubItems.Add('No');
                newitem.ImageIndex := 1;
                newitem.Checked := False;
                AddToLog('(1) Possible API at address: ' + IntToHex(ImportReadPointer,8));
              end;
            end;
          end else begin
            pAPIName := ImporterGetAPINameFromDebugee(hProcess,NumberOfValidInstuctions);
            szAPIName := pAPIName;
            if pAPIName = nil then begin
              szAPIName := IntToHex(ImporterGetAPIOrdinalNumberFromDebugee(hProcess,NumberOfValidInstuctions),8);
              if szAPIName <> 'FFFFFFFF' then pAPIName := PChar(szAPIName);
            end;
            pDLLName := ImporterGetDLLNameFromDebugee(hProcess,NumberOfValidInstuctions);
            szDLLName := pDLLName;
            if (pAPIName <> nil) and (pDLLName <> nil) then begin
              newitem := frmMain.ImportData.Items.Add;
              newitem.Caption := IntToHex(ImportReadPointer,8);
              newitem.SubItems.Add(LowerCase(szDllName));
              newitem.SubItems.Add(szAPIName);
              newitem.SubItems.Add('No');
              newitem.ImageIndex := 0;
              newitem.Checked := False;
              AddToLog('Traced API at address: ' + IntToHex(ImportReadPointer,8) + ' to library: ' + szDLLName + ' import: ' + szAPIName);
            end else begin
              newitem := frmMain.ImportData.Items.Add;
              newitem.Caption := IntToHex(ImportReadPointer,8);
              newitem.SubItems.Add('?');
              newitem.SubItems.Add('?');
              newitem.SubItems.Add('No');
              newitem.ImageIndex := 1;
              newitem.Checked := False;
              AddToLog('(1) Possible API at address: ' + IntToHex(ImportReadPointer,8));
            end;
          end;
        end;
      end;
    end;
    ImportReadPointer := ImportReadPointer + ImportStep;
    ImportSize := ImportSize - ImportStep;
 end;

 VirtualFree(pDLLBases,0,MEM_RELEASE);
 frmMain.DataGrabber.Visible := False;
 frmMain.StatusBar1.SimpleText := ' Ready...'
end;

procedure PatchAntiAttach(Ppid:Cardinal);
 var
 tLen,pid,hPid,cbNeeded,hNTDLL,hwDll,dPatch,wPatch,ePatch,bThere,wThere,s:Cardinal;
 va,vf,va2,vf2:Pointer;
 aVA,BytesRW:LongInt;
 hVA:PLongInt;
 hDll:PLongInt;
 j,AlreadyAdded:LongInt;
 fName:array[0..MAX_PATH] of char;
 hPatch:Pointer;
begin
 pid := OpenProcess(PROCESS_ALL_ACCESS,False,Ppid);
 va := VirtualAlloc(nil,$2000,MEM_COMMIT,PAGE_READWRITE);
 vf := va;
 va2 := VirtualAlloc(nil,$2000,MEM_COMMIT,PAGE_READWRITE);
 vf2 := va2;
 EnumProcessModules(pid,va,$800,cbNeeded);
 hDll := PLongInt(va);
 hwDll := hDll^;
 hVA := PLongInt(va);
 while hwDll > 0 do begin
    hVA := PLongInt(va);
    aVA := hVA^;
    hDll := PLongInt(va);
    hwDll := hDll^;
    asm
     PUSHAD
     MOV EAX,va
     ADD EAX,4
     MOV va,EAX
     POPAD
    end;
    if hwDll > 0 then begin
     GetModuleFileNameEx(pid,hwDll,@fName,MAX_PATH);
     if LowerCase(ExtractFileName(fName)) = 'ntdll.dll' then begin
      hNTDLL := GetModuleHandle(PChar('ntdll.dll'));
      if hNTDLL > 0 then begin
       hPatch := GetProcAddress(hNTDLL,PChar('DbgUiRemoteBreakin'));
       asm
        PUSHAD
        MOV EAX,hPatch
        MOV dPatch,EAX
        POPAD
       end;
       if dPatch > 0 then begin
        wPatch := dPatch;
        dPatch := dPatch - hNTDLL + hwDll;
        ReadProcessMemoryFixed(-1,dPatch,va2,500,@BytesRW);
        if pid > 0 then begin
         tLen := 0;
         j := 15;
         asm
          PUSHAD
          MOV EAX,va2
          MOV ePatch,EAX
          POPAD
         end;
         while j > 0 do begin
           asm
             PUSHAD
             MOV EAX,ePatch
             MOVZX EBX,BYTE PTR[EAX]
             MOV bThere,EBX
             MOVZX EBX,WORD PTR[EAX]
             MOV wThere,EBX
             POPAD
           end;
           s := StaticLengthDisassemble(ConvertIntegerToPointer(ePatch));
           if (s = 7) and (wThere = $05F6) then begin
             asm
               PUSHAD
               MOV EAX,ePatch
               MOV EAX,DWORD PTR[EAX+2]
               MOV bThere,EAX
               POPAD
             end;
             if bThere >= hwDll then begin
               bThere := bThere - hNTDLL + hwDll;
               asm
                 PUSHAD
                 MOV EAX,ePatch
                 MOV EBX,bThere
                 MOV DWORD PTR[EAX+2],EBX
                 POPAD
               end;
             end;
           end;
           if (s = 5) and (bThere = $68) then begin
             asm
               PUSHAD
               MOV EAX,ePatch
               MOV EAX,DWORD PTR[EAX+1]
               MOV bThere,EAX
               POPAD
             end;
             if bThere >= hwDll then begin
               bThere := bThere - hNTDLL + hwDll;
               asm
                 PUSHAD
                 MOV EAX,ePatch
                 MOV EBX,bThere
                 MOV DWORD PTR[EAX+1],EBX
                 POPAD
               end;
             end;
           end;
           if (s = 5) and ((bThere = $E8) or (bThere = $E9))then begin
             asm
               PUSHAD
               MOV EAX,ePatch
               MOV EAX,DWORD PTR[EAX+1]
               MOV bThere,EAX
               MOV EAX,ePatch
               ADD bThere,EAX
               ADD bThere,5
               POPAD
             end;
             if bThere >= $70000000 then begin
               bThere := bThere - hNTDLL + hwDll;
               asm
                 PUSHAD
                 MOV EAX,ePatch
                 MOV EBX,bThere
                 SUB EBX,EAX
                 SUB EBX,5
                 MOV DWORD PTR[EAX+1],EBX
                 POPAD
               end;
             end;
           end;
           tLen := tLen + s;
           ePatch := ePatch + s;
           j := j - 1;
         end;
         WriteProcessMemoryFixed(pid,dPatch,va2,tLen,@BytesRW);
        end;
       end;
      end;
     end;
    end;
 end;
 CloseHandle(Ppid);
 VirtualFree(vf,0,MEM_RELEASE);
 VirtualFree(vf2,0,MEM_RELEASE);
end;

procedure GetTracerData(TraceType:integer);
var
 i,j,x,f,dwPossibleAPI,ImportReadPointer:LongInt;
 pAPIName,pDLLName:PChar;
 szAPIName,szDLLName:string;
 pDLLBases:Pointer;
 cbNeeded:Cardinal;
 NumberOfValidInstuctions,RedirectionType:LongInt;
 TraceAddress:array[1..1000] of LongInt;
begin

 hProcess := Plugingetvalue(VAL_HPROCESS);
 if hProcess = 0 then Abort;
 pDLLBases := VirtualAlloc(nil,$2000,MEM_COMMIT,PAGE_READWRITE);
 EnumProcessModules(hProcess,pDLLBases,$800,cbNeeded);

 x := 1;
 if TraceType = 0 then begin
   for i := 0 to frmMain.ImportData.Items.Count - 1 do begin
    if frmMain.ImportData.Items[i].Selected = True then begin
      TraceAddress[x] := StrToInt('$' + frmMain.ImportData.Items[i].Caption);
      x := x + 1;
    end;
   end;
 end else begin
   if frmMain.ImportData.ItemIndex > -1 then begin
     TraceAddress[x] := StrToInt('$' + frmMain.ImportData.Items[frmMain.ImportData.ItemIndex].Caption);
     x := 2
   end;
 end;

 f := 0;
 for i := 1 to x do begin
  if TraceType = 0 then begin
   ImportReadPointer := TraceAddress[i];
   Readmemory(@dwPossibleAPI,ImportReadPointer,4,MM_RESILENT);
  end else begin
   ImportReadPointer := StrToInt('$' + frmMain.szSearchFrom.Text);
   dwPossibleAPI := ImportReadPointer;
  end;
  NumberOfValidInstuctions := 0;
  if frmMain.chkDetectRedirections.Checked = True then RedirectionType := TracerDetectRedirection(hProcess,dwPossibleAPI) else RedirectionType := 0;
  if RedirectionType = 0 then begin
    if frmMain.chkTraceLevel1.Checked = True then NumberOfValidInstuctions := TracerLevel1(hProcess,dwPossibleAPI);
    if (NumberOfValidInstuctions > 0) and (NumberOfValidInstuctions < $1000) then begin
      if NumberOfValidInstuctions > StrToInt(frmMain.szMaxInstructions.Text) then NumberOfValidInstuctions := StrToInt(frmMain.szMaxInstructions.Text);
      if frmMain.chkHashTracerLevel1.Checked = True then NumberOfValidInstuctions := HashTracerLevel1(hProcess,dwPossibleAPI,NumberOfValidInstuctions);
      if (NumberOfValidInstuctions < $1000) then begin
        AddToLog('Trace fail for API at address: ' + IntToHex(ImportReadPointer,8));
      end else begin
        if NumberOfValidInstuctions = 0 then begin
          NumberOfValidInstuctions := dwPossibleAPI;
        end;
        pAPIName := ImporterGetAPINameFromDebugee(hProcess,NumberOfValidInstuctions);
        szAPIName := pAPIName;
        if pAPIName = nil then begin
          szAPIName := IntToHex(ImporterGetAPIOrdinalNumberFromDebugee(hProcess,NumberOfValidInstuctions),8);
          if szAPIName <> 'FFFFFFFF' then pAPIName := PChar(szAPIName);
        end;
        pDLLName := ImporterGetDLLNameFromDebugee(hProcess,NumberOfValidInstuctions);
        szDLLName := pDLLName;
        if (pAPIName <> nil) and (pDLLName <> nil) then begin
          for j := 0 to frmMain.ImportData.Items.Count - 1 do begin
            if StrToInt('$' + frmMain.ImportData.Items[j].Caption) = TraceAddress[i] then begin
              f := j;
            end;
          end;
          frmMain.ImportData.Items[f].SubItems[0] := LowerCase(szDllName);
          frmMain.ImportData.Items[f].SubItems[1] := szAPIName;
          frmMain.ImportData.Items[f].ImageIndex := 0;
          frmMain.ImportData.Items[f].Checked := False;
          AddToLog('Fixed API at address: ' + IntToHex(ImportReadPointer,8) + ' to library: ' + szDLLName + ' import: ' + szAPIName);
        end else begin
          AddToLog('Trace fail for API at address: ' + IntToHex(ImportReadPointer,8));
        end;
      end;
    end else begin
      if NumberOfValidInstuctions = 0 then begin
        NumberOfValidInstuctions := dwPossibleAPI;
      end;
      pAPIName := ImporterGetAPINameFromDebugee(hProcess,NumberOfValidInstuctions);
      szAPIName := pAPIName;
      if pAPIName = nil then begin
        szAPIName := IntToHex(ImporterGetAPIOrdinalNumberFromDebugee(hProcess,NumberOfValidInstuctions),8);
        if szAPIName <> 'FFFFFFFF' then pAPIName := PChar(szAPIName);
      end;
      pDLLName := ImporterGetDLLNameFromDebugee(hProcess,NumberOfValidInstuctions);
      szDLLName := pDLLName;
      if (pAPIName <> nil) and (pDLLName <> nil) then begin
        for j := 0 to frmMain.ImportData.Items.Count - 1 do begin
          if StrToInt('$' + frmMain.ImportData.Items[j].Caption) = TraceAddress[i] then begin
            f := j;
          end;
        end;
        frmMain.ImportData.Items[f].SubItems[0] := LowerCase(szDllName);
        frmMain.ImportData.Items[f].SubItems[1] := szAPIName;
        frmMain.ImportData.Items[f].ImageIndex := 0;
        frmMain.ImportData.Items[f].Checked := False;
        AddToLog('Traced API hash at address: ' + IntToHex(ImportReadPointer,8) + ' to library: ' + szDLLName + ' import: ' + szAPIName);
      end else begin
        if Readmemory(@dwPossibleAPI,ImportReadPointer,4,MM_RESILENT) = 0 then begin
         AddToLog('Trace fail for API at address: ' + IntToHex(ImportReadPointer,8));
        end;
      end;
    end;
  end else begin
   AddToLog('Detected API redirection: ' + IntToStr(RedirectionType) + ' at address: ' + IntToHex(ImportReadPointer,8));
   NumberOfValidInstuctions := TracerFixKnownRedirection(hProcess,dwPossibleAPI,RedirectionType);
   pAPIName := ImporterGetAPINameFromDebugee(hProcess,NumberOfValidInstuctions);
   szAPIName := pAPIName;
   if pAPIName = nil then begin
     szAPIName := IntToHex(ImporterGetAPIOrdinalNumberFromDebugee(hProcess,NumberOfValidInstuctions),8);
     if szAPIName <> 'FFFFFFFF' then pAPIName := PChar(szAPIName);
   end;
   pDLLName := ImporterGetDLLNameFromDebugee(hProcess,NumberOfValidInstuctions);
   szDLLName := pDLLName;
   if (pAPIName <> nil) and (pDLLName <> nil) then begin
      for j := 0 to frmMain.ImportData.Items.Count - 1 do begin
        if StrToInt('$' + frmMain.ImportData.Items[j].Caption) = TraceAddress[i] then begin
           f := j;
        end;
      end;
      frmMain.ImportData.Items[f].SubItems[0] := LowerCase(szDllName);
      frmMain.ImportData.Items[f].SubItems[1] := szAPIName;
      frmMain.ImportData.Items[f].ImageIndex := 0;
      frmMain.ImportData.Items[f].Checked := False;
      AddToLog('Traced API at address: ' + IntToHex(ImportReadPointer,8) + ' to library: ' + szDLLName + ' import: ' + szAPIName);
    end else begin
     if frmMain.chkAutoTrace.Checked = True then NumberOfValidInstuctions := TracerLevel1(hProcess,dwPossibleAPI);
     if (NumberOfValidInstuctions > 0) and (NumberOfValidInstuctions < $1000) then begin
      if frmMain.chkAutoTrace.Checked = True then NumberOfValidInstuctions := HashTracerLevel1(hProcess,dwPossibleAPI,NumberOfValidInstuctions);
      if (NumberOfValidInstuctions < $1000) then begin
        for j := 0 to frmMain.ImportData.Items.Count - 1 do begin
          if StrToInt('$' + frmMain.ImportData.Items[j].Caption) = TraceAddress[i] then begin
             f := j;
          end;
        end;
        frmMain.ImportData.Items[f].SubItems[0] := LowerCase(szDllName);
        frmMain.ImportData.Items[f].SubItems[1] := szAPIName;
        frmMain.ImportData.Items[f].ImageIndex := 0;
        frmMain.ImportData.Items[f].Checked := False;
        AddToLog('Traced API at address: ' + IntToHex(ImportReadPointer,8) + ' to library: ' + szDLLName + ' import: ' + szAPIName);
     end else begin
       pAPIName := ImporterGetAPINameFromDebugee(hProcess,NumberOfValidInstuctions);
       szAPIName := pAPIName;
       if pAPIName = nil then begin
         szAPIName := IntToHex(ImporterGetAPIOrdinalNumberFromDebugee(hProcess,NumberOfValidInstuctions),8);
         if szAPIName <> 'FFFFFFFF' then pAPIName := PChar(szAPIName);
       end;
       pDLLName := ImporterGetDLLNameFromDebugee(hProcess,NumberOfValidInstuctions);
       szDLLName := pDLLName;
       if (pAPIName <> nil) and (pDLLName <> nil) then begin
          for j := 0 to frmMain.ImportData.Items.Count - 1 do begin
            if StrToInt('$' + frmMain.ImportData.Items[j].Caption) = TraceAddress[i] then begin
              f := j;
            end;
          end;
          frmMain.ImportData.Items[f].SubItems[0] := LowerCase(szDllName);
          frmMain.ImportData.Items[f].SubItems[1] := szAPIName;
          frmMain.ImportData.Items[f].ImageIndex := 0;
          frmMain.ImportData.Items[f].Checked := False;
          AddToLog('Traced API at address: ' + IntToHex(ImportReadPointer,8) + ' to library: ' + szDLLName + ' import: ' + szAPIName);
        end else begin
          AddToLog('Trace fail for API at address: ' + IntToHex(ImportReadPointer,8));
        end;
      end;
     end;
    end;
  end;
 end;
 VirtualFree(pDLLBases,0,MEM_RELEASE);
 frmMain.TraceData.Visible := False;
 frmMain.StatusBar1.SimpleText := ' Ready...'
end;

procedure TfrmMain.BitBtn4Click(Sender: TObject);
begin
 DataGrabber.Visible := False;
end;

procedure TfrmMain.GrabDataClick(Sender: TObject);
begin
 if DataGrabber.Visible = False then begin
   if ImportEditor.Visible = True then ImportEditor.Visible := False;
   if Highliter.Visible = True then Highliter.Visible := False;
   if TraceData.Visible = True then TraceData.Visible := False;
   if DumpProc.Visible = True then DumpProc.Visible := False;
   if AutoFix.Visible = True then AutoFix.Visible := False;
   DataGrabber.Left := Round(ImportData.Width / 2) - Round(DataGrabber.Width / 2);
   DataGrabber.Top := ImportData.Top + 40;
   DataGrabber.Visible := True;
 end else begin
   DataGrabber.Visible := False;
 end;
end;

procedure TfrmMain.BitBtn2Click(Sender: TObject);
begin
 GetImportData(True);
end;

procedure TfrmMain.BitBtn3Click(Sender: TObject);
begin
 GetImportData(False);
end;

procedure TfrmMain.BitBtn1Click(Sender: TObject);
 var
 pFileName:LongInt;
 pChrFileName:PChar;
 pIATStart,pIATSize:LongInt;
begin
 hProcess := Plugingetvalue(VAL_HPROCESS);
 pFileName := Plugingetvalue(VAL_EXEFILENAME);
 asm
    PUSHAD
    MOV EAX,pFileName
    MOV pChrFileName,EAX
    POPAD
 end;
 ImageBase := GetPE32Data(pChrFileName,0,1);
 SizeOfImage := GetPE32Data(pChrFileName,0,3);
 SectionAligment := GetPE32Data(pChrFileName,0,6);
 pIATStart := 0;
 pIATSize := 0;
 ImporterAutoSearchIATEx(hProcess,ImageBase,ImageBase+SectionAligment,SizeOfImage-SectionAligment,@pIATStart,@pIATSize);
 if (pIATStart <> 0) and (pIATSize <> 0) then begin
   frmMain.szSearchStart.Text := IntToHex(pIATStart,8);
   frmMain.szSearchSize.Text := IntToHex(pIATSize,8);
 end else begin
  if messagedlg('Could not find IAT, PE32 header could be invalid. Do you want to paste PE header from disk?',mtWarning,[mbYes,mbNo],0) = 6 then begin
    PastePEHeader(hProcess,ImageBase,pChrFileName);
  end;
 end;
end;

procedure TfrmMain.ToolButton7Click(Sender: TObject);
begin
 if messagedlg('Do you want to clear action log?',mtWarning,[mbYes,mbNo],0) = 6 then frmMain.ActionLog.Clear;
end;

procedure TfrmMain.ToolButton5Click(Sender: TObject);
begin
 if frmMain.SaveDialog1.Execute then frmMain.ActionLog.Items.SaveToFile(frmMain.SaveDialog1.FileName);
end;

procedure TfrmMain.BitBtn8Click(Sender: TObject);
begin
 ImportEditor.Visible := False;
end;

procedure TfrmMain.ToolButton2Click(Sender: TObject);
begin
 if ImportEditor.Visible = False then begin
   if DataGrabber.Visible = True then DataGrabber.Visible := False;
   if Highliter.Visible = True then Highliter.Visible := False;
   if TraceData.Visible = True then TraceData.Visible := False;
   if DumpProc.Visible = True then DumpProc.Visible := False;   
   ImportEditor.Left := Round(ImportData.Width / 2) - Round(ImportEditor.Width / 2);
   ImportEditor.Top := ImportData.Top + 40;
   if ImportData.ItemIndex > -1 then begin
     szIATAddress.Text := ImportData.Items[ImportData.ItemIndex].Caption;
     OriginalIATAddress := ImportData.Items[ImportData.ItemIndex].Caption;
     GetAllLoadedModules;
     cmbSelectedDLL.Text := ImportData.Items[ImportData.ItemIndex].SubItems[0];
     GetAllExports;
     cmbImportName.Text := ImportData.Items[ImportData.ItemIndex].SubItems[1];
     if ImportData.Items[ImportData.ItemIndex].SubItems[2] = 'No' then frmMain.chkStolenImport.Checked := False else frmMain.chkStolenImport.Checked := True;
   end else begin
     GetAllLoadedModules;
     cmbSelectedDLL.Text := cmbSelectedDLL.Items[0];
     GetAllExports;
     OriginalIATAddress := '';
     frmMain.chkStolenImport.Checked := False;
   end;
   ImportEditor.Visible := True;
  end else begin
   ImportEditor.Visible := False;
  end;
end;

procedure TfrmMain.cmbSelectedDLLChange(Sender: TObject);
begin
 GetAllExports;
end;

procedure TfrmMain.BitBtn6Click(Sender: TObject);
 var
 i,AddedData:integer;
 newitem:TListItem;
begin
 AddedData := 0;
 for i := 0 to frmMain.ImportData.Items.Count - 1 do begin
   if frmMain.ImportData.Items[i].Caption = frmMain.szIATAddress.Text then begin
     if messagedlg('Address: ' + frmMain.szIATAddress.Text + ' already added!' + #13 + #10 + 'Do you want to change it to new data?',mtWarning,[mbYes,mbNo],0) = 6 then begin
       frmMain.ImportData.Items[i].Caption := frmMain.szIATAddress.Text;
       frmMain.ImportData.Items[i].SubItems[0] := LowerCase(frmMain.cmbSelectedDLL.Text);
       frmMain.ImportData.Items[i].SubItems[1] := frmMain.cmbImportName.Text;
       frmMain.ImportData.Items[i].ImageIndex := 0;
       if frmMain.chkStolenImport.Checked = False then frmMain.ImportData.Items[i].SubItems[2] := 'No' else frmMain.ImportData.Items[i].SubItems[2] := 'Yes';
       AddedData := 1;
     end else begin
       AddedData := -1;
     end;
   end;
 end;
 if AddedData = 0 then begin
   newitem := frmMain.ImportData.Items.Add;
   newitem.Caption := frmMain.szIATAddress.Text;
   newitem.SubItems.Add(LowerCase(frmMain.cmbSelectedDLL.Text));
   newitem.SubItems.Add(frmMain.cmbImportName.Text);
   newitem.ImageIndex := 0;
   if frmMain.chkStolenImport.Checked = False then newitem.SubItems.Add('No') else newitem.SubItems.Add('Yes');
   newitem.Checked := False;
   AddedData := 1;
 end;
 if AddedData = 1 then frmMain.ImportEditor.Visible := False;
end;

procedure TfrmMain.BitBtn7Click(Sender: TObject);
 var
 i,AddedData:integer;
 newitem:TListItem;
begin
 AddedData := 0;
 for i := 0 to frmMain.ImportData.Items.Count - 1 do begin
   if frmMain.ImportData.Items[i].Caption = OriginalIATAddress then begin
     frmMain.ImportData.Items[i].Caption := frmMain.szIATAddress.Text;
     frmMain.ImportData.Items[i].SubItems[0] := LowerCase(frmMain.cmbSelectedDLL.Text);
     frmMain.ImportData.Items[i].SubItems[1] := frmMain.cmbImportName.Text;
     frmMain.ImportData.Items[i].ImageIndex := 0;
     if frmMain.chkStolenImport.Checked = False then frmMain.ImportData.Items[i].SubItems[2] := 'No' else frmMain.ImportData.Items[i].SubItems[2] := 'Yes';
     AddedData := 1;
   end;
 end;
 if AddedData = 0 then begin
   if messagedlg('Address: ' + OriginalIATAddress + ' not found!' + #13 + #10 + 'Do you want to add it to import data?',mtWarning,[mbYes,mbNo],0) = 6 then begin
     newitem := frmMain.ImportData.Items.Add;
     if frmMain.chkStolenImport.Checked = False then newitem.Caption := frmMain.szIATAddress.Text else newitem.Caption := '?';
     newitem.SubItems.Add(LowerCase(frmMain.cmbSelectedDLL.Text));
     newitem.SubItems.Add(frmMain.cmbImportName.Text);
     if frmMain.chkStolenImport.Checked = False then newitem.SubItems.Add('No') else newitem.SubItems.Add('Yes');
     newitem.Checked := False;
     newitem.ImageIndex := 0;
     AddedData := 1;
   end;
 end;
 if AddedData = 1 then frmMain.ImportEditor.Visible := False;
end;

procedure TfrmMain.ToolButton8Click(Sender: TObject);
begin
 if messagedlg('Do you want to remove selected imports?',mtWarning,[mbYes,mbNo],0) = 6 then frmMain.ImportData.DeleteSelected;
end;

procedure TfrmMain.ToolButton9Click(Sender: TObject);
 var
 i:integer;
begin
 if HighLiter.Visible = False then begin
   if DataGrabber.Visible = True then DataGrabber.Visible := False;
   if ImportEditor.Visible = True then ImportEditor.Visible := False;
   if TraceData.Visible = True then TraceData.Visible := False;
   if DumpProc.Visible = True then DumpProc.Visible := False;
   if AutoFix.Visible = True then AutoFix.Visible := False;
   HighLiter.Left := Round(ImportData.Width / 2) - Round(HighLiter.Width / 2);
   HighLiter.Top := ImportData.Top + 40;
   GetAllLoadedModules;
   cmbSelectedDLL.Text := cmbSelectedDLL.Items[0];
   GetAllExports;
   for i := 0 to cmbSelectedDLL.Items.Count - 1 do begin
     hltLibrary.Items.Add(cmbSelectedDLL.Items[i]);
   end;
   for i := 0 to cmbImportName.Items.Count - 1 do begin
     hltImportName.Items.Add(cmbImportName.Items[i]);
   end;
   hltLibrary.Text := hltLibrary.Items[0];
   hltImportName.Text := hltImportName.Items[0];
   HighLiter.Visible := True;
 end else begin
   HighLiter.Visible := False;
 end;
end;

procedure TfrmMain.BitBtn10Click(Sender: TObject);
begin
 HighLiter.Visible := False;
end;

procedure TfrmMain.BitBtn13Click(Sender: TObject);
 var
 i:integer;
begin
 for i := 0 to frmMain.ImportData.Items.Count - 1 do begin
   if frmMain.ImportData.Items[i].Checked = True then begin
     frmMain.ImportData.Items[i].Checked := False;
     frmMain.ImportData.Items[i].ImageIndex := 0;
   end;
 end;
 HighLiter.Visible := False;
end;

procedure TfrmMain.ImportDataCustomDrawItem(Sender: TCustomListView;
  Item: TListItem; State: TCustomDrawState; var DefaultDraw: Boolean);
begin
 with ImportData.Canvas.Brush do
  begin
    if (Item.Checked = True) then begin
     Color := clInfoBk;
     ImportData.Canvas.Font.Style := [fsUnderLine];
    end;
  end;
end;

procedure TfrmMain.BitBtn12Click(Sender: TObject);
 var
 i:integer;
begin
 for i := 0 to frmMain.ImportData.Items.Count - 1 do begin
   if frmMain.ImportData.Items[i].Checked = True then frmMain.ImportData.Items[i].Selected := True;
 end;
 frmMain.ImportData.Repaint;
 frmMain.ImportData.SetFocus;
 HighLiter.Visible := False;
end;

procedure TfrmMain.BitBtn11Click(Sender: TObject);
 var
 i:integer;
 HltMin,HltMax:LongInt;
begin
 HltMin := StrToInt('$' + szHighliteMin.Text);
 HltMax := StrToInt('$' + szHighliteMax.Text);
 for i := 0 to frmMain.ImportData.Items.Count - 1 do begin
   if (StrToInt('$' + frmMain.ImportData.Items[i].Caption) >= HltMin) and (StrToInt('$' + frmMain.ImportData.Items[i].Caption) <= HltMax) then begin
     frmMain.ImportData.Items[i].Checked := True;
     frmMain.ImportData.Items[i].ImageIndex := 2;
   end;
 end;
 HighLiter.Visible := False;
end;

procedure TfrmMain.BitBtn5Click(Sender: TObject);
 var
 i:integer;
begin
 for i := 0 to frmMain.ImportData.Items.Count - 1 do begin
   if LowerCase(frmMain.ImportData.Items[i].SubItems[0]) = LowerCase(hltLibrary.Text) then begin
     frmMain.ImportData.Items[i].Checked := True;
     frmMain.ImportData.Items[i].ImageIndex := 2;
   end;
 end;
 HighLiter.Visible := False;
end;

procedure TfrmMain.BitBtn9Click(Sender: TObject);
 var
 i:integer;
begin
 for i := 0 to frmMain.ImportData.Items.Count - 1 do begin
   if LowerCase(frmMain.ImportData.Items[i].SubItems[1]) = LowerCase(hltImportName.Text) then begin
     frmMain.ImportData.Items[i].Checked := True;
     frmMain.ImportData.Items[i].ImageIndex := 2;
   end;
 end;
 HighLiter.Visible := False;
end;

procedure TfrmMain.ToolButton11Click(Sender: TObject);
begin
 frmMain.ImportData.AlphaSort;
end;

procedure TfrmMain.Label11Click(Sender: TObject);
begin
 szHighliteMax.Text := szHighliteMin.Text;
end;


procedure TfrmMain.chkStolenImportClick(Sender: TObject);
begin
 if frmMain.chkStolenImport.Checked = True then frmMain.szIATAddress.Text := IntToHex(0,8) else begin
   if OriginalIATAddress <> '' then frmMain.szIATAddress.Text := OriginalIATAddress else frmMain.szIATAddress.Text := frmMain.szSearchStart.Text;
 end;
end;

procedure TfrmMain.ToolButton12Click(Sender: TObject);
 var
 dwPossibleAPI:LongInt;
begin
 if TraceData.Visible = False then begin
   if DataGrabber.Visible = True then DataGrabber.Visible := False;
   if ImportEditor.Visible = True then ImportEditor.Visible := False;
   if Highliter.Visible = True then Highliter.Visible := False;
   if DumpProc.Visible = True then DumpProc.Visible := False;
   if AutoFix.Visible = True then AutoFix.Visible := False;
   TraceData.Left := Round(ImportData.Width / 2) - Round(TraceData.Width / 2);
   TraceData.Top := ImportData.Top + 40;
   if frmMain.ImportData.ItemIndex > -1 then begin
    if Readmemory(@dwPossibleAPI,StrToInt('$' + frmMain.ImportData.Items[frmMain.ImportData.ItemIndex].Caption),4,MM_RESILENT) = 4 then begin
      frmMain.szSearchFrom.Text := IntToHex(dwPossibleAPI,8);
    end else begin
      frmMain.szSearchFrom.Text := frmMain.ImportData.Items[frmMain.ImportData.ItemIndex].Caption;
    end;
   end else begin
    frmMain.szSearchFrom.Text := '00401000';
   end;
   frmMain.chkHashTracerLevel1.Checked := true;
   TraceData.Visible := True;
 end else begin
   TraceData.Visible := False;
 end;
end;

procedure TfrmMain.BitBtn17Click(Sender: TObject);
begin
 TraceData.Visible := False;
end;

procedure TfrmMain.BitBtn15Click(Sender: TObject);
begin
 GetTracerData(0);
end;

procedure TfrmMain.BitBtn14Click(Sender: TObject);
begin
 GetTracerData(1);
end;

procedure TfrmMain.ToolButton23Click(Sender: TObject);
 var
 i,RetnData:integer;
 fName,LastDll:string;
 IDataCheck:LongInt;
begin
 if frmMain.ImportData.Items.Count > 0 then begin
  RetnData := 0;
  for i := 0 to frmMain.ImportData.Items.Count - 1 do begin
   if frmMain.ImportData.Items[i].ImageIndex = 1 then RetnData := 1;
  end;
  if RetnData = 0 then begin
    if frmMain.SaveDialog3.Execute then begin
      fName := frmMain.SaveDialog3.FileName;
      IDataCheck := GetPE32Data(PAnsiChar(fName), 0, UE_CHECKSUM);
      if IDataCheck = $49444154 then DeleteLastSection(PAnsiChar(fName)) else SetPE32Data(PAnsiChar(fName), 0, UE_CHECKSUM, $49444154);
      ImageBase := GetPE32Data(PChar(fName),0,1);
      ImporterInit(30*1024,ImageBase);
      LastDll := 'unknown';
      for i := 0 to frmMain.ImportData.Items.Count - 1 do begin
        if LowerCase(frmMain.ImportData.Items[i].SubItems[0]) <> LowerCase(LastDll) then begin
          LastDll := LowerCase(frmMain.ImportData.Items[i].SubItems[0]);
          ImporterAddNewDll(PChar(LastDll),StrToInt('$' + frmMain.ImportData.Items[i].Caption));
          if MidStr(frmMain.ImportData.Items[i].SubItems[1], 0, 1) <> '0' then begin
            ImporterAddNewAPI(PChar(frmMain.ImportData.Items[i].SubItems[1]),StrToInt('$' + frmMain.ImportData.Items[i].Caption));
          end else begin
            ImporterAddNewOrdinalAPI(StrToInt('$' + frmMain.ImportData.Items[i].SubItems[1]),StrToInt('$' + frmMain.ImportData.Items[i].Caption));
          end;
        end else begin
          if MidStr(frmMain.ImportData.Items[i].SubItems[1], 0, 1) <> '0' then begin
            ImporterAddNewAPI(PChar(frmMain.ImportData.Items[i].SubItems[1]),StrToInt('$' + frmMain.ImportData.Items[i].Caption));
          end else begin
            ImporterAddNewOrdinalAPI(StrToInt('$' + frmMain.ImportData.Items[i].SubItems[1]),StrToInt('$' + frmMain.ImportData.Items[i].Caption));
          end;
        end;
      end;
      ImporterExportIATEx(PChar(fName), PChar('.RevLabs'));
      RealignPEEx(PChar(fName),0,0);
      MakeAllSectionsRWE(PChar(fName));
      messagedlg('IAT has been fixed!',mtInformation,[mbOk],0);
    end;
  end else begin
    messagedlg('Can not fix imports if there are unresolved imports!',mtError,[mbOk],0);
  end;
 end else begin
  messagedlg('Can not fix imports if IAT is not read!',mtError,[mbOk],0);
 end;
end;

procedure TfrmMain.ToolButton15Click(Sender: TObject);
 var
 wFile:TextFile;
 fline:string;
 i,j,z,z1,apicnt,ftnk,last:integer;
 newitem:TListItem;
 pFileName,hProcess:LongInt;
 pChrFileName:PChar;
begin
 if ImportData.Items.Count = 0 then begin
  messagedlg('Add some imports first!',mtError,[mbOk],0);
  Abort;
 end;
 if SaveDialog4.Execute then begin
   hProcess := Plugingetvalue(VAL_HPROCESS);
   pFileName := Plugingetvalue(VAL_EXEFILENAME);
   asm
      PUSHAD
      MOV EAX,pFileName
      MOV pChrFileName,EAX
      POPAD
   end;
   if (pFileName > 0) and (hProcess > 0) then begin
     ImageBase := GetPE32Data(pChrFileName,0,1);
   end;
   newitem := ImportData.Items.Add;
   newitem.Caption := 'end';
   newitem.SubItems.Add('?');
   newitem.SubItems.Add('?');
  AssignFile(wFile,SaveDialog4.FileName);
   Rewrite(wFile);
     writeln(wFile,'Target: ');
     writeln(wFile,TrimRight('OEP: ' + IntToHex(OriginalEntryPoint,8) + '	IATRVA: ' + IntToHex(StrToInt('$' + ImportData.Items[0].Caption) - ImageBase,8) + '	IATSize: '  + IntToHex(StrToInt('$' + ImportData.Items[ImportData.Items.Count - 2].Caption) - StrToInt('$' + ImportData.Items[0].Caption),8)));
     writeln(wFile,'');
     fline := ImportData.Items[0].SubItems[0];
     j := 0;
    while (j <= ImportData.Items.Count - 1) and (fline <> '?') do begin
      apicnt := 0;
      ftnk := 0;
      z := j;
      last := 0;
     while (LowerCase(ImportData.Items[j].SubItems[0]) = LowerCase(fline)) and (j <= ImportData.Items.Count - 1) do begin
      if j > 0 then last := StrToInt('$' + ImportData.Items[j - 1].Caption) else last := StrToInt('$' + ImportData.Items[j].Caption);
      if ftnk = 0 then ftnk := StrToInt('$' + ImportData.Items[j].Caption) - ImageBase;
      apicnt := apicnt + 1;
      j := j + 1;
 if j = 1 then begin
  if StrToInt('$' + ImportData.Items[1].Caption) - 4 <> StrToInt('$' + ImportData.Items[0].Caption) then break;
 end;
 if j = 2 then begin
  if StrToInt('$' + ImportData.Items[2].Caption) - 4 <> StrToInt('$' + ImportData.Items[1].Caption) then break;
 end;
 if (j > 2) and (j < ImportData.Items.Count - 1) then begin
  if StrToInt('$' + ImportData.Items[j].Caption) - 4 <> StrToInt('$' + ImportData.Items[j - 1].Caption) then break;
 end;
 if j = ImportData.Items.Count - 1 then begin
  if StrToInt('$' + ImportData.Items[j - 1].Caption) - 4 <> StrToInt('$' + ImportData.Items[j - 2].Caption) then break;
 end;
     end;
      j := j - 1;
   if z <= j then begin
        writeln(wFile,'FThunk: ' + IntToHex(ftnk,8) + '	NbFunc: ' + IntToHex(apicnt,8));
     for z1 := z to j do begin
        if z1 <= ImportData.Items.Count - 1 then writeln(wFile,TrimRight('1	' + IntToHex(StrToInt('$' + ImportData.Items[z1].Caption) - ImageBase,8) + '	' + LowerCase(fline) + '	0000	' + ImportData.Items[z1].SubItems[1]));
     end;
        writeln(wFile,'');
   end;
        j := j + 1;
        if j <= ImportData.Items.Count - 1 then fline := ImportData.Items[j].SubItems[0];
    end;
   CloseFile(wFile);
   ImportData.Items[ImportData.Items.Count - 1].Delete;
   messagedlg('ImpRec tree file has been exported!',mtInformation,[mbOk],0);
 end;
end;

procedure TfrmMain.BitBtn19Click(Sender: TObject);
 var
 pFileName, hProcess,fImageBase,fOEP:LongInt;
 fName,pChrFileName:PChar;
begin
 hProcess := Plugingetvalue(VAL_HPROCESS);
 pFileName := Plugingetvalue(VAL_EXEFILENAME);
 asm
    PUSHAD
    MOV EAX,pFileName
    MOV pChrFileName,EAX
    POPAD
 end;
 if hProcess > 0 then begin
   if frmMain.SaveDialog2.Execute then begin
     fImageBase := Plugingetvalue(VAL_MAINBASE);
     fOEP := StrToInt('$' + frmMain.szEntryAddress.Text);
     fName := PChar(frmMain.SaveDialog2.FileName);
     if frmMain.chkPastePEHeader.Checked then begin
       PastePEHeader(hProcess,fImageBase,pChrFileName);
       if DumpProcess(hProcess,fImageBase,fName,fOEP) = false then begin
         messagedlg('Process can not be dumped!',mtError,[mbOk],0);
       end else begin
         messagedlg('Process has been dumped to disk!',mtInformation,[mbOk],0);
       end;
     end else begin
       if DumpProcess(hProcess,fImageBase,fName,fOEP) = false then begin
          PastePEHeader(hProcess,fImageBase,pChrFileName);
          if DumpProcess(hProcess,fImageBase,fName,fOEP) = false then begin
            messagedlg('Process can not be dumped!',mtError,[mbOk],0);
          end else begin
            messagedlg('Process has been dumped to disk!',mtInformation,[mbOk],0);
          end;
       end;
     end;
     DumpProc.Visible := False;
   end;
 end else begin
  messagedlg('Can not dump process if no file is being debuged!',mtError,[mbOk],0);
 end;
end;

procedure TfrmMain.BitBtn16Click(Sender: TObject);
 var
 hThreadId:Cardinal;
 pt:p_thread;
begin
  hThreadId := Plugingetvalue(VAL_MAINTHREADID);
  if hThreadId <> 0 then begin
    pt := Findthread(hThreadId);
    frmMain.szEntryAddress.Text := IntToHex(pt.context.Eip,8);
  end;
end;

procedure TfrmMain.ToolButton14Click(Sender: TObject);
begin
 if DumpProc.Visible = False then begin
   if DataGrabber.Visible = True then DataGrabber.Visible := False;
   if ImportEditor.Visible = True then ImportEditor.Visible := False;
   if Highliter.Visible = True then Highliter.Visible := False;
   if TraceData.Visible = True then TraceData.Visible := False;
   if AutoFix.Visible = True then AutoFix.Visible := False;   
   DumpProc.Left := Round(ImportData.Width / 2) - Round(DumpProc.Width / 2);
   DumpProc.Top := ImportData.Top + 40;
   DumpProc.Visible := True;
 end else begin
   DumpProc.Visible := False;
 end;
end;

procedure TfrmMain.ToolButton16Click(Sender: TObject);
 var
 rVal:LongInt;
 szProcessId:string;
begin
 if messagedlg('Do you want to attach to running process?',mtWarning,[mbYes,mbNo],0) = 6 then begin
  szProcessId := InputBox('Enter data:', 'ProcessId (hex):', IntToHex(Plugingetvalue(VAL_PROCESSID),2));
  if szProcessId <> IntToHex(Plugingetvalue(VAL_PROCESSID),2) then begin
   PatchAntiAttach(StrToInt('$' + szProcessId));
   rVal := Attachtoactiveprocess(StrToInt('$' + szProcessId));
   if rVal > 0 then begin
      messagedlg('Could not attach to running process!',mtError,[mbOk],0);
   end;
  end else begin
   messagedlg('Please enter valid process id!',mtError,[mbOk],0);
  end;
 end;
end;

procedure TfrmMain.BitBtn20Click(Sender: TObject);
begin
 DumpProc.Visible := False;
end;

procedure TfrmMain.Clearlist1Click(Sender: TObject);
begin
 if messagedlg('Do you want to clear import edit list?', mtConfirmation, [mbYes,mbNo], 0) = 6 then frmMain.ImportData.Clear;
end;

procedure TfrmMain.Deletetrunks1Click(Sender: TObject);
 var
 i:integer;
begin
 if messagedlg('Do you want to delete invalid trunks?', mtConfirmation, [mbYes,mbNo], 0) = 6 then begin
   for i := frmMain.ImportData.Items.Count - 1 downto 0 do begin
     if frmMain.ImportData.Items[i].SubItems[0] = '?' then frmMain.ImportData.Items[i].Delete;
   end;
 end;
end;

procedure TfrmMain.Selectedaddress1Click(Sender: TObject);
 var
 threadid,addr : LongInt;
begin
 if frmMain.ImportData.ItemIndex > -1 then begin
  addr := StrToInt('$' + frmMain.ImportData.Items[frmMain.ImportData.ItemIndex].Caption);
  if addr > 0 then begin
   threadid := Getcputhreadid;
   Setcpu(threadid,addr,0,0,CPU_ASMFOCUS);
   Setcpu(threadid,addr,0,0,CPU_REDRAW);
   StatusBar1.SimpleText := ' Navigated to address: ' + frmMain.ImportData.Items[frmMain.ImportData.ItemIndex].Caption;
  end;
 end
  else messagedlg('Please enter some address first!',mtError,[mbOk],0);
end;

procedure TfrmMain.Selectedaddresspointer1Click(Sender: TObject);
 var
 threadid,addr,i : LongInt;
 pAllocatedBuffer:pointer;
 AllocatedBuffer,ResolveAddress:LongInt;
begin
 if frmMain.ImportData.ItemIndex > -1 then begin
  addr := StrToInt('$' + frmMain.ImportData.Items[frmMain.ImportData.ItemIndex].Caption);
  if addr > 0 then begin

  AllocatedBuffer := CreateVirtual(4);
  asm
   PUSHAD
   MOV EAX,AllocatedBuffer
   MOV pAllocatedBuffer,EAX
   POPAD
  end;

  i := ReadBuffer(pAllocatedBuffer,addr,4);
  if i = 0 then begin
   FreeVirtual(pAllocatedBuffer,4);
   messagedlg('OllyDBG could not read memory at address: ' + IntToHex(addr,8),mtError,[mbOk],0);
   Abort;
  end;

  asm
   PUSHAD
   MOV EAX,DWORD PTR[pAllocatedBuffer]
   MOV EAX,DWORD PTR[EAX]
   MOV ResolveAddress,EAX
   POPAD
  end;
  FreeVirtual(pAllocatedBuffer,4);

   threadid := Getcputhreadid;
   Setcpu(threadid,ResolveAddress,0,0,CPU_ASMFOCUS);
   Setcpu(threadid,ResolveAddress,0,0,CPU_REDRAW);
   StatusBar1.SimpleText := ' Navigated to address: ' + IntToHex(ResolveAddress,8);
  end;
 end
  else messagedlg('Please enter some address first!',mtError,[mbOk],0);
end;

procedure TfrmMain.BitBtn21Click(Sender: TObject);
 var
 hProcess,fImageBase:LongInt;
 fName:PChar;
 pFileName:LongInt;
 pChrFileName:PChar;
 pIATStart,pIATSize:LongInt;
begin
 hProcess := Plugingetvalue(VAL_HPROCESS);
 if hProcess = 0 then begin
   messagedlg('Please load some PE32 file and reach its OEP first!',mtError,[mbOk],0);
   Abort;
 end;
 if frmMain.SaveDialog2.Execute then begin
    AutoFix.Visible := False; 
    pFileName := Plugingetvalue(VAL_EXEFILENAME);
 asm
    PUSHAD
    MOV EAX,pFileName
    MOV pChrFileName,EAX
    POPAD
 end;
    fName := PChar(frmMain.SaveDialog2.FileName);
    fImageBase := Plugingetvalue(VAL_MAINBASE);
    ImageBase := GetPE32Data(pChrFileName,0,1);
    SizeOfImage := GetPE32Data(pChrFileName,0,3);
    SectionAligment := GetPE32Data(pChrFileName,0,6);
    pIATStart := 0;
    pIATSize := 0;
    if frmMain.chkPastePEHeaderFirst.Checked then begin
     PastePEHeader(hProcess,fImageBase,pChrFileName);
    end;
     if frmMain.chkAutoFindIATRange.Checked = true then begin
       ImporterAutoSearchIATEx(hProcess,ImageBase,ImageBase+SectionAligment,SizeOfImage-SectionAligment,@pIATStart,@pIATSize);
     end else begin
       pIATStart := StrToInt('$' + frmMain.szSearchStart.Text);
       pIATSize := StrToInt('$' + frmMain.szSearchSize.Text);
     end;
     if (frmMain.chkAutoFixElimination.Checked = true) or ((pIATStart <> 0) and (pIATSize <> 0)) or (frmMain.chkAutoFindIATRange.Checked = false) then begin
       StatusBar1.SimpleText := ' Working please wait... ';
       if ImporterAutoFixIATEx(hProcess, fName, PChar('.RevLabs'), frmMain.chkDumpProcessFirst.Checked, true, StrToInt('$' + frmMain.szSetEP.Text), fImageBase, pIATStart, pIATSize, 1, frmMain.chkAutoFixRedirection.Checked, frmMain.chkAutoFixElimination.Checked, 0) = $400 then begin
         messagedlg('File has been automatically fixed!',mtInformation,[mbOk],0);
       end else begin
         messagedlg('There was a problem with fixing the file automatically!',mtError,[mbOk],0);
       end;
     end;
  end;
  StatusBar1.SimpleText := ' Ready... ';  
  AutoFix.Visible := False;
end;

procedure TfrmMain.BitBtn23Click(Sender: TObject);
 var
 hThreadId:Cardinal;
 pt:p_thread;
begin
 if AutoFix.Visible = False then begin
   if ImportEditor.Visible = True then ImportEditor.Visible := False;
   if Highliter.Visible = True then Highliter.Visible := False;
   if TraceData.Visible = True then TraceData.Visible := False;
   if DumpProc.Visible = True then DumpProc.Visible := False;
   if DataGrabber.Visible = True then DataGrabber.Visible := False;
   AutoFix.Left := Round(ImportData.Width / 2) - Round(AutoFix.Width / 2);
   AutoFix.Top := ImportData.Top + 40;
   AutoFix.Visible := True;
   hThreadId := Plugingetvalue(VAL_MAINTHREADID);
   if hThreadId <> 0 then begin
     pt := Findthread(hThreadId);
     frmMain.szSetEP.Text := IntToHex(pt.context.Eip,8);
   end;
 end else begin
   AutoFix.Visible := False;
 end;
end;

procedure TfrmMain.BitBtn18Click(Sender: TObject);
begin
 AutoFix.Visible := False;
end;

procedure TfrmMain.Browse1Click(Sender: TObject);
var
 i,j,f,x,FoundApi,TraceType:LongInt;
 pAPIName,pDLLName:PChar;
 szAPIName,szDLLName:string;
 TraceAddress:array[1..1000] of LongInt;
begin

 f := 0;
 TraceType := 0;
 hProcess := Plugingetvalue(VAL_HPROCESS);
 if hProcess = 0 then Abort;
 OpenDialog1.InitialDir := ExtractFilePath(Application.ExeName) + 'imports\ImpRec\';
 if OpenDialog1.Execute then begin
   x := 1;
   if TraceType = 0 then begin
     for i := 0 to frmMain.ImportData.Items.Count - 1 do begin
      if frmMain.ImportData.Items[i].Selected = True then begin
        TraceAddress[x] := StrToInt('$' + frmMain.ImportData.Items[i].Caption);
        x := x + 1;
      end;
     end;
   end else begin
     if frmMain.ImportData.ItemIndex > -1 then begin
       TraceAddress[x] := StrToInt('$' + frmMain.ImportData.Items[frmMain.ImportData.ItemIndex].Caption);
       x := 2
     end;
   end;

   for i := 1 to x do begin
      FoundApi := TracerFixRedirectionViaImpRecPlugin(hProcess, PChar(ExtractFileName(OpenDialog1.FileName)), TraceAddress[i]);
      if FoundApi <> 0 then begin
        pAPIName := ImporterGetAPINameFromDebugee(hProcess,FoundApi);
        szAPIName := pAPIName;
        if pAPIName = nil then begin
          szAPIName := IntToHex(ImporterGetAPIOrdinalNumberFromDebugee(hProcess,FoundApi),8);
          if szAPIName <> 'FFFFFFFF' then pAPIName := PChar(szAPIName);
        end;
        pDLLName := ImporterGetDLLNameFromDebugee(hProcess,FoundApi);
        szDLLName := pDLLName;
        if (pAPIName <> nil) and (pDLLName <> nil) then begin
          for j := 0 to frmMain.ImportData.Items.Count - 1 do begin
            if StrToInt('$' + frmMain.ImportData.Items[j].Caption) = TraceAddress[i] then begin
              f := j;
            end;
          end;
          frmMain.ImportData.Items[f].SubItems[0] := LowerCase(szDllName);
          frmMain.ImportData.Items[f].SubItems[1] := szAPIName;
          frmMain.ImportData.Items[f].ImageIndex := 0;
          frmMain.ImportData.Items[f].Checked := False;
          AddToLog('Fixed API at address: ' + frmMain.ImportData.Items[f].Caption + ' to library: ' + szDLLName + ' import: ' + szAPIName);
        end else begin
          AddToLog('Trace fail for API at address: ' + frmMain.ImportData.Items[f].Caption);
        end;
      end;
   end;
 end;
 frmMain.StatusBar1.SimpleText := ' Ready...'
end;

procedure TfrmMain.Selectinvalid1Click(Sender: TObject);
 var
 i:integer;
begin
 for i := frmMain.ImportData.Items.Count - 1 downto 0 do begin
   if frmMain.ImportData.Items[i].SubItems[0] = '?' then frmMain.ImportData.Items[i].Selected := True else frmMain.ImportData.Items[i].Selected := False;
 end;
end;

procedure TfrmMain.Image2Click(Sender: TObject);
begin
 ShellExecuteA(0,PChar('open'),PChar('http://www.reversinglabs.com'),nil,nil,1);
end;

procedure TfrmMain.hltLibraryChange(Sender: TObject);
begin
 GetAllExportsHighLight;
end;

procedure TfrmMain.Selecthighlited1Click(Sender: TObject);
 var
 i:integer;
begin
 for i := frmMain.ImportData.Items.Count - 1 downto 0 do begin
   if frmMain.ImportData.Items[i].ImageIndex = 2 then frmMain.ImportData.Items[i].Selected := True else frmMain.ImportData.Items[i].Selected := False;
 end;
end;

procedure TfrmMain.raceLevel11Click(Sender: TObject);
 var
 dwPossibleAPI:LongInt;
begin
 if TraceData.Visible = False then begin
   if DataGrabber.Visible = True then DataGrabber.Visible := False;
   if ImportEditor.Visible = True then ImportEditor.Visible := False;
   if Highliter.Visible = True then Highliter.Visible := False;
   if DumpProc.Visible = True then DumpProc.Visible := False;
   if AutoFix.Visible = True then AutoFix.Visible := False;
   TraceData.Left := Round(ImportData.Width / 2) - Round(TraceData.Width / 2);
   TraceData.Top := ImportData.Top + 40;
   if frmMain.ImportData.ItemIndex > -1 then begin
    if Readmemory(@dwPossibleAPI,StrToInt('$' + frmMain.ImportData.Items[frmMain.ImportData.ItemIndex].Caption),4,MM_RESILENT) = 4 then begin
      frmMain.szSearchFrom.Text := IntToHex(dwPossibleAPI,8);
    end else begin
      frmMain.szSearchFrom.Text := frmMain.ImportData.Items[frmMain.ImportData.ItemIndex].Caption;
    end;
   end else begin
    frmMain.szSearchFrom.Text := '00401000';
   end;
   frmMain.chkHashTracerLevel1.Checked := false;
   TraceData.Visible := True;
 end else begin
   TraceData.Visible := False;
 end;
end;

procedure TfrmMain.Savelist1Click(Sender: TObject);
 var
 F:TextFile;
 i:integer;
begin
 if SaveDialog5.Execute then begin
   AssignFile(F,SaveDialog5.FileName);
   Rewrite(F);
   writeln(F,'#ImportStudio|SaveFile');
   for i := 0 to frmMain.ImportData.Items.Count - 1 do begin
    writeln(F,frmMain.ImportData.Items[i].Caption);
    writeln(F,frmMain.ImportData.Items[i].SubItems[0]);
    writeln(F,frmMain.ImportData.Items[i].SubItems[1]);
    writeln(F,frmMain.ImportData.Items[i].SubItems[2]);
   end;
   writeln(F,'#EOF#ImportStudio|SaveFile');
   CloseFile(F);
   messagedlg('File has been saved!',mtInformation,[mbOk],0);
 end;
end;

procedure TfrmMain.Loadlist1Click(Sender: TObject);
 var
 fline:String;
 F:TextFile;
 newitem:TListItem;
begin
 if OpenDialog2.Execute then begin
  frmMain.ImportData.Items.Clear;
  AssignFile(F,OpenDialog2.FileName);
   Reset(F);
   readln(F,fline);
   while not(eof(F)) do begin
    readln(F,fline);
    if fline <> '#EOF#ImportStudio|SaveFile' then begin
     newitem := frmMain.ImportData.Items.Add;
     newitem.Caption := fline;
     readln(F,fline);
     newitem.SubItems.Add(fline);
     readln(F,fline);
     newitem.SubItems.Add(fline);
     readln(F,fline);
     newitem.SubItems.Add(fline);
     newitem.Checked := false;
     newitem.ImageIndex := 0;
    end;
   end;
   CloseFile(F);
 end;
end;

procedure EnumerateImports(ptrImport:LongInt); stdcall;
var
 newitem:TListItem;
 pImport:IMPORT_ENUM_DATA;
begin
 cRtlMoveMemory(@pImport, ptrImport, SizeOf(IMPORT_ENUM_DATA));
 newitem := frmMain.ImportData.Items.Add;
 newitem.Caption := IntToHex(pImport.ImportThunk,8);
 newitem.SubItems.Add(LowerCase(pImport.DLLName));
 newitem.SubItems.Add(pImport.APIName);
 newitem.SubItems.Add('No');
 newitem.ImageIndex := 0;
 newitem.Checked := False;
end;

procedure TfrmMain.ToolButton19Click(Sender: TObject);
begin
 if OpenDialog3.Execute then begin
   if ImporterLoadImportTable(PChar(OpenDialog3.FileName)) then begin
      ImporterEnumAddedData(@EnumerateImports);
   end;
 end;
end;

end.

