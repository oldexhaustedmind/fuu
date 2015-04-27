
from teSdk import *

class deLame():

  def __init__(self):
    self.CbOnInitialize = fInitializeDbg(self.OnInitialize)

    TE.EngineCreateUnpackerWindow("[ RL!deLameCrypter ]", "RL!deLameCrypter 1.0", "RL!deLameCrypter 1.0 (Static)", "ReversingLabs Corporation",
      self.CbOnInitialize)

  def OnInitialize(self, pOrig, pRealign, pOverlay):
    if not pOrig: return

    Output = pOrig[:len(pOrig) - 3] + "unpacked" + pOrig[len(pOrig) - 4:]
    windll.kernel32.DeleteFileA(Output)

    TE.EngineAddUnpackerWindowLogMessage("Unpacker for LameCrypt 1.0 packed files")
    TE.EngineAddUnpackerWindowLogMessage("ReversingLabs Corporation / www.reversinglabs.com")
    TE.EngineAddUnpackerWindowLogMessage("")

    if not windll.kernel32.CopyFileA(pOrig, Output, True):
      TE.EngineAddUnpackerWindowLogMessage("[!] Cannot copy input file.")
      return

    Validity = FILE_STATUS_INFO()
    if not TE.IsPE32FileValidEx(pOrig, UE_DEPTH_DEEP, byref(Validity)) or Validity.OveralEvaluation != UE_RESULT_FILE_OK:
      TE.EngineAddUnpackerWindowLogMessage("[!] The file seems to be invalid.")
    else:
      TE.EngineAddUnpackerWindowLogMessage("[x] The file seems to be valid.")

    Handle, Size, Map, MapVA = c_void_p(0), c_ulong(0), c_void_p(0), c_ulong(0)
    if TE.StaticFileLoad(Output, UE_ACCESS_ALL, False, byref(Handle), byref(Size), byref(Map), byref(MapVA)):

      Base = TE.GetPE32DataFromMappedFile(MapVA, 0, UE_IMAGEBASE)

      Oep = TE.GetPE32DataFromMappedFile(MapVA, 0, UE_OEP) + Base
      Oep = cast(TE.ConvertVAtoFileOffset(MapVA, Oep, True) + 0x19, POINTER(c_ulong))
      Oep = Oep.contents

      Oep = Oep.value - Base
      TE.StaticSectionDecrypt(MapVA, 0, 0, UE_STATIC_DECRYPTOR_XOR, UE_STATIC_KEY_SIZE_1, 0x90)
      TE.SetPE32DataForMappedFile(MapVA, 0, UE_OEP, Oep)

      TE.EngineAddUnpackerWindowLogMessage("[x] OEP found: 0x{0:08X}.".format(Oep))
      TE.StaticFileUnload(Output, True, Handle, Size, Map, MapVA)

      TE.DeleteLastSection(Output)

      if pRealign:
        if TE.RealignPEEx(Output, 0, 0):   TE.EngineAddUnpackerWindowLogMessage("[x] File has been realigned.")
        else:                              TE.EngineAddUnpackerWindowLogMessage("[!] File realigning failed.")

      if pOverlay:
        if TE.CopyOverlay(pOrig, Output):  TE.EngineAddUnpackerWindowLogMessage("[x] Moving overlay to unpacked file.")
        else:                              TE.EngineAddUnpackerWindowLogMessage("[x] No overlay found.")

    else:
      TE.EngineAddUnpackerWindowLogMessage("[!] Error while loading file.")
      windll.kernel32.DeleteFileA(Output)

LameUnpack = deLame()