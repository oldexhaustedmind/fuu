## FUU v0.1.1 Beta ##

  * Minor release

### fixed bugs ###

  * Avoid the resize of the main GUI window
  * Fixed an inconsistency between the about showed in the "About" menu and the one showed in the "About" button
  * Fixed the problem with "Refresh" button
  * The open file dialog now has filter strings
  * Editbox is now editable so, you can type the path of the file

### enhancements ###

  * Updated the TitanEngine library (current version 2.0.3)
  * Added a clickable blogspot link and email address
  * Added a save file dialog in every included plugin
  * Added "Drag&Drop" for .exe and .dll files
  * Added a "Copy Overlay Data" button
  * Added "Auto Scroll" to the browse file edit box

### code refactoring ###

  * Added comments to every function in the main GUI and every plugin (see SVN)
  * Added a "GetControlHandles" function (see SVN)
  * Improved the way we check for Windows Messages

### new features ###

  * Added a simple signatures identifier using PETools signatures database (thanks marciano!)
  * Added three newer plugins for nPack, Packman and dePack


## FUU v0.1 Beta ##

### Plugins ###

  * UPX Unpacker for UPX v1.x - 3.x (DLL and EXE - x86)
  * BeRoExEPacker Unpacker (EXE - x86)
  * FSG Unpacker for v1.x - 2.x (EXE - x86)

### Tools ###

  * Generic OEP Finder (GenOEP.dll)
  * Crytp Signatures Detector (kanal.dll)
  * Generic Unpacker