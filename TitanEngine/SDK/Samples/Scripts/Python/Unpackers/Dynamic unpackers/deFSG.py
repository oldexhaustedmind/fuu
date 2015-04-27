
from teSdk import *

class deUpx():

    def __init__(self):
      self.CbOnInitialize     = fInitializeDbg(self.OnInitialize)
      self.CbOnDbgInit        = fBreakPoint(self.OnDbgInit)
      self.CbOnNext           = fBreakPoint(self.OnNext)
      self.CbOnOEP            = fBreakPoint(self.OnOEP)
      self.CbOnLLA            = fBreakPoint(self.OnLLA)
      self.CbOnGPA            = fBreakPoint(self.OnGPA)

      self.wild = c_ubyte(0)
      self.iSignatureOffset = 0x14
      
      self.dllname = create_string_buffer(' ' * MAX_PATH)
      self.apiname = create_string_buffer(' ' * 256)

      TE.EngineCreateUnpackerWindow("[ RL!deFSG ]", "RL!deFSG 2.0", "RL!deFSG 2.0", "ReversingLabs Corporation", self.CbOnInitialize)

    def OnLLA(self):
      eax = TE.GetContextData(UE_EAX)
      dll = TE.GetRemoteString(self.Info.contents.hProcess, eax, cast(self.dllname, POINTER(c_char * MAX_PATH)), MAX_PATH)

      if dll:
        TE.EngineAddUnpackerWindowLogMessage("[x] Loading DLL '{0}'.".format(self.dllname.value))
        TE.ImporterAddNewDll(self.dllname, 0)

    def OnGPA(self):
      eax = TE.GetContextData(UE_EAX)
      edi = TE.GetContextData(UE_EDI)

      api = TE.GetRemoteString(self.Info.contents.hProcess, eax, cast(self.apiname, POINTER(c_char * 256)), 256)

      TE.EngineAddUnpackerWindowLogMessage("[x] Retrieving API '{0}' at thunk {1:08X}.".format(self.apiname.value if api else eax, edi))
      TE.ImporterAddNewAPI(self.apiname.value if api else eax, edi)

    def OnOEP(self):
      oep = TE.GetContextData(UE_EIP)

      TE.EngineAddUnpackerWindowLogMessage("[x] OEP found: 0x{0:08X}.".format(oep))

      TE.PastePEHeader(self.Info.contents.hProcess, self.Base, self.Input)
      if TE.DumpProcess(self.Info.contents.hProcess, self.Base, self.Output, oep):

          TE.EngineAddUnpackerWindowLogMessage("[x] Process dumped.")
          TE.MakeAllSectionsRWE(self.Output)

          if not self.GotTLS:
            TE.DeleteLastSection(self.Output)
            TE.EngineAddUnpackerWindowLogMessage("[x] Packer section removed.")
          else:
            TE.EngineAddUnpackerWindowLogMessage("[x] Keeping packer section due to TLS.")

          if TE.ImporterExportIATEx(self.Output, ".revlabs"):
            TE.EngineAddUnpackerWindowLogMessage("[x] Imports fixed.")
          else:
            TE.EngineAddUnpackerWindowLogMessage("[!] Cannot fix imports.")
            TE.StopDebug()
            return

          if self.Overlay:
              if TE.CopyOverlay(self.Input, self.Output):
                  TE.EngineAddUnpackerWindowLogMessage("[x] Moving overlay to unpacked file.")
              else: TE.EngineAddUnpackerWindowLogMessage("[x] No overlay found.")

          if self.Realign:
              if TE.RealignPEEx(self.Output, 0, 0):
                  TE.EngineAddUnpackerWindowLogMessage("[x] File has been realigned.")
              else: TE.EngineAddUnpackerWindowLogMessage("[!] File realigning failed.")

          TE.EngineAddUnpackerWindowLogMessage("[x] File has been unpacked to {0}.".format(self.Output))

      TE.StopDebug()

    def OnNext(self):
      TE.StepInto(self.CbOnOEP)

    def OnDbgInit(self):
      self.Verify()

      pat = (c_ubyte *  5)(0x75, 0x03, 0xff, 0x63, 0x0c)
      lla = (c_ubyte * 11)(0xeb, 0x00, 0x00, 0xad, 0x00, 0xad, 0x50, 0xff, 0x53, 0x10, 0x95)
      gpa = (c_ubyte *  9)(0x50, 0x55, 0xff, 0x53, 0x14, 0xab, 0xeb, 0x00, 0x33)

      loc = TE.Find(self.Base, 0x1000, byref(pat), len(pat), byref(self.wild)) + 2
      lla = TE.Find(self.Base, 0x1000, byref(lla), len(lla), byref(self.wild)) + 7
      gpa = TE.Find(self.Base, 0x1000, byref(gpa), len(gpa), byref(self.wild)) + 2

      if loc and lla and gpa:
        if TE.SetBPX(loc, UE_SINGLESHOOT, self.CbOnNext) and \
           TE.SetBPX(lla, UE_BREAKPOINT,  self.CbOnLLA)  and \
           TE.SetBPX(gpa, UE_BREAKPOINT,  self.CbOnGPA):
          return
        else:
          TE.EngineAddUnpackerWindowLogMessage("[!] Could not set a breakpoint.")
      else:
        TE.EngineAddUnpackerWindowLogMessage("[!] Could not locate a pattern.")

      TE.StopDebug()

    def Verify(self):
      Sig = self.Base + self.iSignatureOffset

      Magic = c_ulong(0)
      IO = c_ulong(0)

      if windll.kernel32.ReadProcessMemory(self.Info.contents.hProcess, Sig, byref(Magic), sizeof(c_ulong), byref(IO)) and Magic.value == 0x21475346:
          return

      TE.EngineAddUnpackerWindowLogMessage("[!] FSG signature mismatch!")
      TE.StopDebug()

    def OnInitialize(self, pOriginal, pRealign, pCopyOvl):
      if not pOriginal: return

      self.Input  = pOriginal
      self.Output = pOriginal[:len(pOriginal) - 3] + "unpacked" + pOriginal[len(pOriginal) - 4:]

      self.Realign = pRealign
      self.Overlay = pCopyOvl

      windll.kernel32.DeleteFileA(self.Output)

      TE.EngineAddUnpackerWindowLogMessage("Unpacker for FSG 2.0 packed files")
      TE.EngineAddUnpackerWindowLogMessage("ReversingLabs Corporation / www.reversinglabs.com")

      self.Validity = FILE_STATUS_INFO()
      if not TE.IsPE32FileValidEx(pOriginal, UE_DEPTH_DEEP, byref(self.Validity)) \
        or  self.Validity.OveralEvaluation != UE_RESULT_FILE_OK:
          TE.EngineAddUnpackerWindowLogMessage("[!] The file seems to be invalid.")
          return

      if self.Validity.FileIs64Bit or self.Validity.FileIsDLL:
        TE.EngineAddUnpackerWindowLogMessage("[!] x86 .exe files are supported only.")
        return

      self.Info = TE.InitDebugEx(pOriginal, 0, 0, self.CbOnDbgInit)
      if self.Info:
          TE.EngineAddUnpackerWindowLogMessage("[x] Debugger initialized.")
          self.Base = TE.GetDebuggedFileBaseAddress()

          TE.ImporterInit(0x10000, self.Base)
          self.GotTLS     = TE.GetPE32Data(pOriginal, 0, UE_TLSTABLEADDRESS)
          self.SizeOfImg  = TE.GetPE32Data(pOriginal, 0, UE_SIZEOFIMAGE)

          TE.DebugLoop()
          TE.ImporterCleanup()
      else:
          TE.EngineAddUnpackerWindowLogMessage("[!] Could not initialize debugging!")

      TE.EngineAddUnpackerWindowLogMessage("[x] Exit Code: {0:X}.".format(TE.GetExitCode()))

UPXUnpack = deUpx()