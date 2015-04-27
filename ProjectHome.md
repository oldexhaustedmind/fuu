**FUU (Faster Universal Unpacker)** is a GUI Windows Tool with a set of tools (plugins)
to help you to unpack, decompress and decrypt most of the programs packed, compressed or encrypted with the very well knowns software protection programs like UPX, ASPack, FSG, ACProtect, etc.

The GUI was designed using [RadASM](http://www.oby.ro/rad_asm/) and [MASM](http://www.masm32.com). Every plugin included in the official release was written
in [ASM](http://en.wikipedia.org/wiki/Assembly_language) using MASM.

The core of every plugin use [TitanEngine SDK](http://reversinglabs.com/products/TitanEngine.php) from [ReversingLabs](http://reversinglabs.com/) under the hood, this help to the developer to write plugins very easy and very fast without the need to worry about some repetitive and boring functions like dump, fix the iat, add sections, etc. You can develop a plugin for FUU in a very easy way using TitanEngine.

Also, FUU include some extra tools like:

  * Generic OEP Finder
  * Cryto Signature Detector
  * Generic Unpacker
  * Signatures Detector (by marcianito at gmail dot com)

Generic OEP Finder, Cryto Signature Detector and Generic Unpacker are from [PEiD's team](http://www.peid.info/).

_**IMPORTANT NOTICE: A ripped version of FUU is distributed under the name of MART!K Unpacker. This tool does not add additional features to the current ones in FUU. The author of this fake tool just ripped the original strings of FUU (main executable file and plugins) for his own. A mention to the original project (FUU) was added to the post in his blog but the sources were not released and corresponded credit were not added in the executables, indeed, the original author was removed and in his place, the name of MART!K was added. Please, contact anyone in the FUU's team if you have any further question or if you know about another case like this were FUU is involved.**_

### Latest changes ###

**Added English version of the TitanEngine + FUU tutorial** (14/07/2011)

> Special thanks goes to:
  * Juan Esteban Forgia (@ztoker) for the translation of the first four pages.
  * Apokliptiko for the translation of the second part of the turorial.
  * Nahuel C. Riva (oh!, that's me :) for the translation of the first part of the tutorial.
  * Francisco Falcón (@fdfalcon) for reviewing the grammar.

**Added Unpacker for PFE by El Tio Pastafrola** [bin](http://fuu.googlecode.com/svn/trunk/bin/x86/Plugins/PFE-FUU.dll) [src](http://goo.gl/8j9LX) (18/02/2011)

**Version 0.1.1 Beta**

**Plugins**

  * Added an unpacker for nPack
  * Added an unpacker for Packman
  * Added an unpacker for dePack

**Tools**

  * Added a PE signature detector (by marciano) based on the [PE Tools Signatures Database](http://petools.org.ru/)

See changelog.txt or the changelog wiki page for more information.

**Version 0.1 Beta**

**Plugins**

  * UPX Unpacker for UPX v1.x - 3.x (DLL and EXE - x86)
  * BeRoExEPacker Unpacker (EXE - x86)
  * FSG Unpacker for v1.x - 2.x (EXE - x86)
  * ASPack Unpacker for ASPack 2.x (EXE - x86)

**Tools**

  * Generic OEP Finder (GenOEP.dll)
  * Crytp Signatures Detector (kanal.dll)
  * Generic Unpacker

**Screenshots**

![http://fuuproject.files.wordpress.com/2010/07/fuu-300x254.jpg](http://fuuproject.files.wordpress.com/2010/07/fuu-300x254.jpg)

### Additional resources ###

  * [PEiD's Homepage](http://www.peid.info/)
  * [MASM32 Homepage](http://www.masm32.com/)
  * [RadASM IDE Homepage](http://www.oby.ro/rad_asm/)
  * [TitanEngine SDK](http://reversinglabs.com/products/TitanEngine.php)