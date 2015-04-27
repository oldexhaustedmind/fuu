
from teSdk import *

class deUpx():

    def __init__(self):
        self.CbOnInitialize     = fInitializeDbg(self.OnInitialize)
        self.CbOnDbgInit        = fBreakPoint(self.OnDbgInit)
        self.CbOnNext           = fBreakPoint(self.OnNext)
        self.CbOnPopad          = fBreakPoint(self.OnPopad)
        self.CbOnReloc          = fBreakPoint(self.OnReloc)
        self.CbOnImp            = fBreakPoint(self.OnImp)

        self.wild = c_ubyte(0xcc)

        self.lo   = 0
        self.hi   = 0

        TE.EngineCreateUnpackerWindow("[ RL!deUPX ]", "RL!deUPX 1.x - 3.x", "RL!deUPX 1.x - 3.x",
                                      "ReversingLabs Corporation", self.CbOnInitialize)

    def OnImp(self):
      dst = TE.GetContextData(UE_EBX)

      TE.EngineAddUnpackerWindowLogMessage("[x] Import at {0:08X}.".format(dst))

      if not self.lo:   self.lo = dst
      if dst > self.hi: self.hi = dst
      if dst < self.lo: self.lo = dst

    def OnPopad(self):
        TE.DeleteHardwareBreakPoint(self.Dr)

        if self.Base != self.ImgBase:
            if TE.RelocaterMakeSnapshot(self.Info.contents.hProcess, "snap01.tmp", self.SnapRange + self.Base, self.SnapSize):
                TE.EngineAddUnpackerWindowLogMessage("[x] Created memory snapshot #2.")
            else:
                TE.EngineAddUnpackerWindowLogMessage("[!] Could not create memory snapshot.")
                TE.StopDebug()
                return

            if TE.RelocaterCompareTwoSnapshots(self.Info.contents.hProcess, self.Base, self.SizeOfImg, "snap00.tmp", "snap01.tmp",
                                               self.SnapRange + self.Base):
                TE.EngineAddUnpackerWindowLogMessage("[x] Snapshots compared.")
            else:
                TE.EngineAddUnpackerWindowLogMessage("[!] Could not compare snapshots.")
                TE.StopDebug()
                return

        pad = (c_ubyte * 2)(0x61, 0xe9)
        loc = TE.Find(TE.GetContextData(UE_EIP) - 1, 0x30, byref(pad), len(pad), byref(self.wild))

        if not loc:
          pad = (c_ubyte * 4)(0x83, 0xec, 0xcc, 0xe9)
          loc = TE.Find(TE.GetContextData(UE_EIP) - 1, 0x30, byref(pad), len(pad), byref(self.wild))

          if not loc:
            TE.EngineAddUnpackerWindowLogMessage("[!] Could not find OEP.")
            TE.StopDebug()
            return

          else: loc = loc + 3
        else: loc = loc + 1

        oep = TE.GetJumpDestination(self.Info.contents.hProcess, loc)

        TE.EngineAddUnpackerWindowLogMessage("[x] OEP found: 0x{0:08X}.".format(oep))

        TE.PastePEHeader(self.Info.contents.hProcess, self.Base, self.Input)
        if TE.DumpProcess(self.Info.contents.hProcess, self.Base, self.Output, oep):

            TE.EngineAddUnpackerWindowLogMessage("[x] Process dumped.")
            TE.MakeAllSectionsRWE(self.Output)

            iat = self.lo
            iatsize = self.hi - self.lo

            TE.EngineAddUnpackerWindowLogMessage("[x] IAT begin at 0x{0:08X}, size {1:08X}.".format(iat, iatsize))

            if iat and iatsize and not TE.ImporterAutoFixIATEx(self.Info.contents.hProcess, self.Output, ".revlabs", False, self.Realign,
                                                              oep, self.Base, iat, iatsize, 4, False, False, 0):
                TE.EngineAddUnpackerWindowLogMessage("[!] Cannot fix imports.")
                TE.StopDebug()
                return

            else: TE.EngineAddUnpackerWindowLogMessage("[X] Imports fixed.")

            if self.Base != self.ImgBase:
                if TE.RelocaterExportRelocationEx(self.Output, ".revlabs"):
                    TE.EngineAddUnpackerWindowLogMessage("[X] Relocations fixed.")
                else:
                    TE.EngineAddUnpackerWindowLogMessage("[!] Cannot fix relocations.")
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

    def OnReloc(self):
        if TE.RelocaterMakeSnapshot(self.Info.contents.hProcess, "snap00.tmp", self.SnapRange + self.Base, self.SnapSize):
            TE.EngineAddUnpackerWindowLogMessage("[x] Created memory snapshot #1.")
        else:
            TE.EngineAddUnpackerWindowLogMessage("[!] Could not create memory snapshot.")
            TE.StopDebug()
        return

    def OnNext(self):
        self.Dr = c_ulong(0)
        TE.GetUnusedHardwareBreakPointRegister(byref(self.Dr))

        if not TE.SetHardwareBreakPoint(TE.GetContextData(UE_ESP), self.Dr, UE_HARDWARE_READWRITE, UE_HARDWARE_SIZE_1, self.CbOnPopad):
            TE.EngineAddUnpackerWindowLogMessage("[x] Could not set the breakpoint.")
            TE.StopDebug()
            return

        TE.EngineAddUnpackerWindowLogMessage("[x] Hardware breakpoint set.")

    def OnDbgInit(self):
        if self.IsDLL:  self.Base = TE.GetDebuggedDLLBaseAddress()
        else:           self.Base = TE.GetDebuggedFileBaseAddress()

        pat = (c_ubyte * 6)(0x57, 0x48, 0xf2, 0xae, 0xcc, 0xff)
        loc = TE.Find(self.Base + self.EP, self.SizeOfImg - self.EP, byref(pat), len(pat), byref(self.wild))

        if not TE.SetBPX(loc, UE_BREAKPOINT, self.CbOnImp):
          TE.EngineAddUnpackerWindowLogMessage("[x] Could not locate import pattern.")
          TE.StopDebug()
          return

        if self.Base != self.ImgBase:
            TE.RelocaterInit(100 * 1024, self.ImgBase, self.Base)
            pat = (c_ubyte * 13)(0xb9, 0xcc, 0xcc, 0xcc, 0xcc, 0x8a, 0xcc, 0xcc, 0x2c, 0xe8, 0x3c, 0x01, 0x77)

            loc = TE.Find(self.Base + self.EP, self.SizeOfImg - self.EP, byref(pat), len(pat), byref(self.wild))
            if loc: TE.SetBPX(loc, UE_SINGLESHOOT, self.CbOnReloc)

        if self.IsDLL:
            pat = (c_ubyte * 8)(0x60, 0xbe, 0xcc, 0xcc, 0xcc, 0xcc, 0x8d, 0xbe)
            loc = TE.Find(TE.GetContextData(UE_EIP), 0x30, byref(pat), len(pat), byref(self.wild))

            if loc and TE.SetBPX(loc + 1, UE_SINGLESHOOT, self.CbOnNext): return

        else:
            TE.StepInto(self.CbOnNext)
            return

        TE.StopDebug()

    def OnInitialize(self, pOriginal, pRealign, pCopyOvl):
        if not pOriginal: return

        self.Input  = pOriginal
        self.Output = pOriginal[:len(pOriginal) - 3] + "unpacked" + pOriginal[len(pOriginal) - 4:]

        self.Realign = pRealign
        self.Overlay = pCopyOvl

        self.lo = 0
        self.hi = 0

        windll.kernel32.DeleteFileA(self.Output)
        windll.kernel32.DeleteFileA("snap00.tmp")
        windll.kernel32.DeleteFileA("snap01.tmp")

        TE.EngineAddUnpackerWindowLogMessage("Unpacker for UPX 1.x - 3.x packed files")
        TE.EngineAddUnpackerWindowLogMessage("ReversingLabs Corporation / www.reversinglabs.com")

        self.Validity = FILE_STATUS_INFO()
        if not TE.IsPE32FileValidEx(pOriginal, UE_DEPTH_DEEP, byref(self.Validity)) \
           or  self.Validity.OveralEvaluation != UE_RESULT_FILE_OK:
            TE.EngineAddUnpackerWindowLogMessage("[!] The file seems to be invalid.")
            return

        self.IsDLL = TE.IsFileDLL(pOriginal, 0)

        if not self.IsDLL:
            self.Info = TE.InitDebugEx(pOriginal, 0, 0, self.CbOnDbgInit)
        else:
            self.Info = TE.InitDLLDebug(pOriginal, True, 0, 0, self.CbOnDbgInit)

        if self.Info:
            TE.EngineAddUnpackerWindowLogMessage("[x] Debugger initialized.")

            self.GotTLS     = TE.GetPE32Data(pOriginal, 0, UE_TLSTABLEADDRESS)

            self.SizeOfImg  = TE.GetPE32Data(pOriginal, 0, UE_SIZEOFIMAGE)
            self.ImgBase    = TE.GetPE32Data(pOriginal, 0, UE_IMAGEBASE)
            self.EP         = TE.GetPE32Data(pOriginal, 0, UE_OEP)

            self.SnapRange  = TE.GetPE32Data(pOriginal, 0, UE_SECTIONVIRTUALOFFSET)
            self.SnapSize   = self.EP - self.SnapRange

            TE.DebugLoop()
        else:
            TE.EngineAddUnpackerWindowLogMessage("[!] Could not initialize debugging!")

        windll.kernel32.DeleteFileA("snap00.tmp")
        windll.kernel32.DeleteFileA("snap01.tmp")

        TE.EngineAddUnpackerWindowLogMessage("[x] Exit Code: {0:X}.".format(TE.GetExitCode()))

UPXUnpack = deUpx()