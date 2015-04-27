// A simple plugin for OllyDbg, written by ReversingLabs Corporation
library ImportStudio;

uses
  Windows,
  SysUtils,
  ShellAPI,
  Plugin,
  Main in 'Main.pas' {frmMain};

resourcestring
  VERSION = '1.0.2';
  AUTHOR  = 'ReversingLabs Corporation';
  PLUGIN_NAME = 'ImportStudio';
  ABOUT   = 'Plugin coded by ReversingLabs Corporation' + #13#10 + 'Version: 1.0.1' + #13#10 + 'Web: http://www.reversinglabs.com';

var
  g_hwndOlly: HWND;
  g_hmodOlly: HMODULE;
  SaveDLLProc: TDLLProc;
  MainFormCreated : integer;

function GetExePath: string;
var
  pProcess: PChar;
  sExeFName: string;
begin
  pProcess := PChar(Plugingetvalue(VAL_PROCESSNAME));
 if (pProcess = nil) or (pProcess[0] = #0) then
    sExeFName := GetModuleName(g_hmodOlly)
  else
    sExeFName := PChar(Plugingetvalue(VAL_SYSTEMDIR));

  Result := ExtractFilePath(sExeFName);
end;

function GetComSpec: string;
var
  sCmd: string;
  pFilePart: PChar;
  szBuf: array[0..MAX_PATH] of Char;
begin
  sCmd := GetEnvironmentVariable('COMSPEC');
  if (sCmd = '') or not FileExists(sCmd) then
  begin
    SetString(sCmd, szBuf, SearchPath(nil, 'cmd.exe', nil, SizeOf(szBuf), szBuf, pFilePart));
    if (sCmd = '') then
      SetString(sCmd, szBuf, SearchPath(nil, 'command.com', nil, SizeOf(szBuf), szBuf, pFilePart));
  end;
  Result := sCmd;
end;

function ODBG_Plugindata(name: PChar): Integer; cdecl;
begin
  StrLCopy(name, PChar(PLUGIN_NAME), 32); // Name of plugin
  Result := PLUGIN_VERSION;
end;

function ODBG_Plugininit(ollydbgversion: Integer; hWndOlly: HWND; features: PULONG): Integer; cdecl;
begin
  if (ollydbgversion < PLUGIN_VERSION) then
  begin
    Result := -1;
    Exit;
  end;

  g_hwndOlly := hWndOlly;
  Addtolist(0, 0, 'ImportStudio - Ver %s by %s', VERSION, AUTHOR);

  Result := 0;
end;

function ODBG_Pluginmenu(origin: Integer; pData: PChar; pItem: Pointer): Integer; cdecl;
begin
  case origin of
    PM_MAIN:
      begin
        // Plugin menu in main window
        StrCopy(pData, '0 &ImportStudio, 1 &About...');
        Result := 1;
      end;
  else
    Result := 0; // Any other window
  end;
end;

procedure ODBG_Pluginaction(origin: Integer; action: Integer; pItem: Pointer); cdecl;
var
  sExePath: string;
  WinDir: PChar;
  hComCtl:Cardinal;
begin
  if (origin = PM_MAIN) then
  begin
    sExePath := GetExePath;
     if action = 0 then begin
      if MainFormCreated <> 1 then frmMain := TfrmMain.Create(frmMain);
      GetMem(WinDir, 256);
      GetWindowsDirectory(WinDir, 256);
      StrCat(WinDir, '\system32\comctl32.dll');
      hComCtl := GetModuleHandle(WinDir);
      if hComCtl = 0 then LoadLibrary(WinDir);
      frmMain.Show;
      MainFormCreated := 1;
     end;
     if action = 1 then begin
      Infoline(PChar(' ImportStudio version: ' + VERSION + ' coded by ' + AUTHOR));
      MessageBox(g_hwndOlly, PChar(ABOUT), PChar(PLUGIN_NAME), $40);
     end;
  end;
end;

procedure DLLExit(dwReason: DWORD);
begin
  if (dwReason = DLL_PROCESS_DETACH) then
  begin
    // Uninitialize code here
    OutputDebugString('Plugin unloaded by DLL_PROCESS_DETACH');
  end;

  // Call saved entry point procedure
  if Assigned(SaveDLLProc) then
    SaveDLLProc(dwReason);
end;

exports
  ODBG_Plugindata   name '_ODBG_Plugindata',
  ODBG_Plugininit   name '_ODBG_Plugininit',
  ODBG_Pluginmenu   name '_ODBG_Pluginmenu',
  ODBG_Pluginaction name '_ODBG_Pluginaction';

begin
  // Initialize code here
  SaveDLLProc := @DLLProc;
  DLLProc := @DLLExit;

  g_hmodOlly := GetModuleHandle(nil);
end.
