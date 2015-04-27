unit Main;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ComCtrls, ImgList, ToolWin, XPMan, StdCtrls, Buttons, Plugin, SDK, psapi,
  jpeg, ExtCtrls, Menus, ShellApi, StrUtils, ClipBrd;

type
  TfrmMain = class(TForm)
    XPManifest1: TXPManifest;
    StatusBar1: TStatusBar;
    Image1: TImage;
    GroupBox1: TGroupBox;
    ToolBar1: TToolBar;
    HandleView: TListView;
    ToolButton1: TToolButton;
    ToolButton2: TToolButton;
    ToolButton3: TToolButton;
    ImageList1: TImageList;
    ToolButton4: TToolButton;
    PopupMenu1: TPopupMenu;
    Closehandle1: TMenuItem;
    ToolButton5: TToolButton;
    ToolButton6: TToolButton;
    Copyname1: TMenuItem;
    N1: TMenuItem;
    ToolButton7: TToolButton;
    ToolButton8: TToolButton;
    ToolButton9: TToolButton;
    ToolButton10: TToolButton;
    gMutextView: TGroupBox;
    MutexView: TListView;
    BitBtn1: TBitBtn;
    PopupMenu2: TPopupMenu;
    MenuItem1: TMenuItem;
    MenuItem2: TMenuItem;
    MenuItem3: TMenuItem;
    procedure ToolButton2Click(Sender: TObject);
    procedure ToolButton4Click(Sender: TObject);
    procedure Closehandle1Click(Sender: TObject);
    procedure ToolButton5Click(Sender: TObject);
    procedure Copyname1Click(Sender: TObject);
    procedure ToolButton7Click(Sender: TObject);
    procedure ToolButton8Click(Sender: TObject);
    procedure MenuItem1Click(Sender: TObject);
    procedure MenuItem3Click(Sender: TObject);
    procedure BitBtn1Click(Sender: TObject);
    procedure ToolButton10Click(Sender: TObject);
    procedure HandleViewColumnClick(Sender: TObject; Column: TListColumn);
    procedure FormCreate(Sender: TObject);
    procedure MutexViewColumnClick(Sender: TObject; Column: TListColumn);

  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  frmMain: TfrmMain;
  LastSortedColumn: integer;
  Ascending: boolean;

implementation

{$R *.dfm}

function SortByColumn(Item1, Item2: TListItem; Data: integer):integer; stdcall;
begin
 if Data = 0 then
   Result := AnsiCompareText(Item1.Caption, Item2.Caption)
 else Result := AnsiCompareText(Item1.SubItems[Data-1], Item2.SubItems[Data-1]);
 if not Ascending then Result := -Result;
end;

procedure TfrmMain.ToolButton2Click(Sender: TObject);
 var
 i:LongInt;
 newitem:TListItem;
 hProcess,ProcessId,hCount,pHandleType:LongInt;
 hHandle:array[0..1024] of LongInt;
 HandleType, HandlerName:PAnsiChar;
begin
 hProcess := Plugingetvalue(VAL_HPROCESS);
 ProcessId := Plugingetvalue(VAL_PROCESSID);
 if ProcessId <> 0 then begin
   frmMain.HandleView.Items.Clear;
   hCount := HandlerEnumerateOpenHandles(ProcessId, @hHandle[0], 1024);
   if hCount > 0 then begin
     for i := 0 to hCount - 1 do begin
       newitem := frmMain.HandleView.Items.Add;
       newitem.Caption := IntToHex(hHandle[i],8);
       newitem.SubItems.Add(IntToHex(HandlerGetHandleDetails(hProcess, ProcessId, hHandle[i], UE_OPTION_HANDLER_RETURN_ACCESS), 8));
       pHandleType := HandlerGetHandleDetails(hProcess, ProcessId, hHandle[i], UE_OPTION_HANDLER_RETURN_TYPENAME);
       asm
          PUSHAD
          MOV EBX,pHandleType
          LEA EAX,DWORD PTR[HandleType]
          MOV DWORD PTR[EAX],EBX
          POPAD
       end;
       if pHandleType <> 0 then newitem.SubItems.Add(HandleType) else newitem.SubItems.Add('-');
       HandlerName := HandlerGetHandleName(hProcess, ProcessId, hHandle[i], true);
       if HandlerName <> nil then newitem.SubItems.Add(HandlerName) else newitem.SubItems.Add('-');
     end;
   end;
 end;
end;

procedure TfrmMain.ToolButton4Click(Sender: TObject);
 var
 i:LongInt;
begin
 if MessageDlg('Do you want to close all selected handles?', mtWarning, [mbYes,mbNo], 0) = 6 then begin
  for i := 0 to frmMain.HandleView.Items.Count - 1 do begin
   if frmMain.HandleView.Items[i].Checked then begin
    HandlerCloseRemoteHandle(Plugingetvalue(VAL_HPROCESS), StrToInt('$' + frmMain.HandleView.Items[i].Caption));
    frmMain.HandleView.Items[i].Selected := true;
   end else begin
    frmMain.HandleView.Items[i].Selected := false;
   end;
  end;
  frmMain.ToolButton2Click(Sender);
 end;
end;

procedure TfrmMain.Closehandle1Click(Sender: TObject);
 var
 i:LongInt;
begin
  for i := 0 to frmMain.HandleView.Items.Count - 1 do begin
    if frmMain.HandleView.Items[i].Selected then frmMain.HandleView.Items[i].Checked := true;
  end;
  frmMain.ToolButton4Click(Sender);
end;

procedure TfrmMain.ToolButton5Click(Sender: TObject);
begin
 Close;
end;

procedure TfrmMain.Copyname1Click(Sender: TObject);
begin
 if frmMain.HandleView.ItemIndex > -1 then ClipBoard.AsText := frmMain.HandleView.Items[frmMain.HandleView.ItemIndex].SubItems[2];
end;

procedure TfrmMain.ToolButton7Click(Sender: TObject);
 var
 i:LongInt;
begin
  for i := 0 to frmMain.HandleView.Items.Count - 1 do begin
    frmMain.HandleView.Items[i].Checked := true;
  end;
end;

procedure TfrmMain.ToolButton8Click(Sender: TObject);
 var
 i:LongInt;
begin
  for i := 0 to frmMain.HandleView.Items.Count - 1 do begin
    frmMain.HandleView.Items[i].Checked := false;
  end;
end;

procedure TfrmMain.MenuItem1Click(Sender: TObject);
begin
 if frmMain.MutexView.ItemIndex > -1 then ClipBoard.AsText := frmMain.MutexView.Items[frmMain.MutexView.ItemIndex].SubItems[2];
end;

procedure TfrmMain.MenuItem3Click(Sender: TObject);
 var
 i:LongInt;
begin
  for i := 0 to frmMain.MutexView.Items.Count - 1 do begin
    if frmMain.MutexView.Items[i].Selected then frmMain.MutexView.Items[i].Checked := true;
  end;
  frmMain.gMutextView.Visible := false;
  if MessageDlg('Do you want to close all selected handles?', mtWarning, [mbYes,mbNo], 0) = 6 then begin
   for i := 0 to frmMain.MutexView.Items.Count - 1 do begin
    if frmMain.MutexView.Items[i].Checked then begin
     HandlerCloseRemoteHandle(Plugingetvalue(VAL_HPROCESS), StrToInt('$' + frmMain.MutexView.Items[i].Caption));
     frmMain.MutexView.Items[i].Selected := true;
    end else begin
     frmMain.MutexView.Items[i].Selected := false;
    end;
   end;
   frmMain.ToolButton2Click(Sender);
  end;
end;

procedure TfrmMain.BitBtn1Click(Sender: TObject);
begin
  frmMain.gMutextView.Visible := false;
end;

procedure TfrmMain.ToolButton10Click(Sender: TObject);
 var
 i:LongInt;
 newitem:TListItem;
 hProcess,ProcessId,hCount,pHandleType:LongInt;
 hHandle:array[0..1024] of LongInt;
 HandleType, HandlerName:PAnsiChar;
begin
 hProcess := Plugingetvalue(VAL_HPROCESS);
 ProcessId := Plugingetvalue(VAL_PROCESSID);
 if ProcessId <> 0 then begin
   frmMain.MutexView.Items.Clear;
   frmMain.gMutextView.Visible := true;
   hCount := HandlerEnumerateOpenMutexes(hProcess, ProcessId, @hHandle[0], 1024);
   if hCount > 0 then begin
     for i := 0 to hCount - 1 do begin
       newitem := frmMain.MutexView.Items.Add;
       newitem.Caption := IntToHex(hHandle[i],8);
       newitem.SubItems.Add(IntToHex(HandlerGetHandleDetails(hProcess, ProcessId, hHandle[i], UE_OPTION_HANDLER_RETURN_ACCESS), 8));
       pHandleType := HandlerGetHandleDetails(hProcess, ProcessId, hHandle[i], UE_OPTION_HANDLER_RETURN_TYPENAME);
       asm
          PUSHAD
          MOV EBX,pHandleType
          LEA EAX,DWORD PTR[HandleType]
          MOV DWORD PTR[EAX],EBX
          POPAD
       end;
       if pHandleType <> 0 then newitem.SubItems.Add(HandleType) else newitem.SubItems.Add('-');
       HandlerName := HandlerGetHandleName(hProcess, ProcessId, hHandle[i], true);
       if HandlerName <> nil then newitem.SubItems.Add(HandlerName) else newitem.SubItems.Add('-');
     end;
   end;
 end;
end;

procedure TfrmMain.HandleViewColumnClick(Sender: TObject;
  Column: TListColumn);
begin
    if Column.Index = LastSortedColumn then
      Ascending := not Ascending
    else
      LastSortedColumn := Column.Index;
    TListView(Sender).CustomSort(@SortByColumn, Column.Index);
end;

procedure TfrmMain.FormCreate(Sender: TObject);
begin
    LastSortedColumn := -1;
    Ascending := True; 
end;

procedure TfrmMain.MutexViewColumnClick(Sender: TObject;
  Column: TListColumn);
begin
    if Column.Index = LastSortedColumn then
      Ascending := not Ascending
    else
      LastSortedColumn := Column.Index;
    TListView(Sender).CustomSort(@SortByColumn, Column.Index);
end;

end.

