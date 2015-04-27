
#ifndef signatures_h__
#define signatures_h__

#pragma once
#include "types.h"

#define ArraySize(pName) (sizeof((pName)) / sizeof((((pName))[0])) - 1)

static uchar	gWildCard					= 0xcc;

const uchar		sDelta[]					= "\xe8\x00\x00\x00\x00\x5d\x81\xed",
				sStubBegin13[]				= "\xb9\xCC\xCC\xCC\xCC\x81\xe9\xCC\xCC\xCC\xCC\x8b\xd5\x81\xc2\xCC\xCC\xCC\xCC\x8d\x3a\x8b\xf7",
				sStubBegin12[]				= "\xb9\xCC\xCC\xCC\xCC\x8d\xbd\xCC\xCC\xCC\xCC\x8b\xf7",
				sStubLoopEnd11[]			= "\xab\xCC\xe2\xCC\x8d\x85\xCC\xCC\xCC\xCC\xb9",
				sSectionException[]			= "\x8b\xf8\x03\x7f\x3c\x8b\xf7\x81\xc6\xf8\x00\x00\x00\x33\xd2",
				sSectionDecryption12[]		= "\x03\xf0\xe8\x02\x00\x00\x00\xeb\xCC\xCC\xCC\xac",
				sSectionDecryption13[]		= "\xe8\xCC\xCC\xCC\xCC\xeb\xCC\x3e\x8b\x76\x0c\x03\xf0\xe8\xCC\xCC\xCC\xCC\x8b\xd5\x81\xc2",
				sImportDecryption12[]		= "\x3b\x85\xCC\xCC\xCC\xCC\x74\xCC\xe9\xCC\xCC\xCC\xCC\x8d\xb5\xCC\xCC\xCC\xCC\xf7\x85\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x56",
				sImportDecryption13[]		= "\x8b\xCC\x81\xCC\xCC\xCC\xCC\xCC\x8d\xCC\x53\x8b\xCC\x81\xCC\xCC\xCC\xCC\xCC\xf7\xCC\x20\x00\x00\x00\xCC\xCC\x56",
				sOEPReference13[]			= "\x8b\xCC\x81\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xc1\xCC\x07\x89\xCC\x24",
				sOEPReference12[]			= "\x8b\xCC\xCC\xCC\xCC\xCC\x03\xCC\xCC\xCC\xCC\xCC\xc1\xCC\x07\x89\xCC\x24",
				sOEPReference11[]			= "\x8b\x85\xCC\xCC\xCC\xCC\x03\x85\xCC\xCC\xCC\xCC\xbb\xCC\xCC\xCC\xCC\x03\xdd\x43\x89\x03",
				sInnerStubDecryption11[]	= "\x8d\xbd\xCC\xCC\xCC\xCC\x8b\xf7\xb9\xCC\xCC\xCC\xCC\x33\xdb\xac";

const ulong		lDelta						= ArraySize(sDelta),
				lStubBegin13				= ArraySize(sStubBegin13),
				lStubBegin12				= ArraySize(sStubBegin12),
				lStubLoopEnd11				= ArraySize(sStubLoopEnd11),
				lSectionException			= ArraySize(sSectionException),
				lSectionDecryption12		= ArraySize(sSectionDecryption12),
				lSectionDecryption13		= ArraySize(sSectionDecryption13),
				lImportDecryption12			= ArraySize(sImportDecryption12),
				lImportDecryption13			= ArraySize(sImportDecryption13),
				lOEPReference13				= ArraySize(sOEPReference13),
				lOEPReference12				= ArraySize(sOEPReference12),
				lOEPReference11				= ArraySize(sOEPReference11),
				lInnerStubDecryption11		= ArraySize(sInnerStubDecryption11);

enum
{
	iTresholdDelta		= 0x50,
	iTresholdStub		= 0xff,

	iOffsetDelta		= 8,

	iOffsetSizeA		= 1,
	iOffsetSizeB		= 7,

	iOffsetStub12		= 7,
	iOffsetStub13		= 15,

	iOffsetLoopd		= 2,
	iOffsetLoop11		= 2,

	iOffsetExc12		= 2,
	iOffsetExc13		= 3,
	iOffsetExcMagic		= 2,

	iOffsetExcSw12		= 5,
	iOffsetExcSw13		= 4,

	iOffsetDecrypt12	= 9,
	iOffsetDecrypt13	= 13,

	iOffsetSectionKey11 = 17,
	iMaxSecDecSize		= 0x100,

	iOffsetImportVA12	= 15,
	iOffsetImportVA13	= 4,

	iCallSize			= 5,

	iExcSizeLong		= 13,

	iExcSizeShort12		= 10,
	iExcSizeShort13		= 9,

	iExcSwitch12		= 2,
	iExcSwitch13		= 0x74,

	iLoopdShort			= 0xe2,
	iRetn				= 0xc3,
	iExcBlockMagic		= 0x3e81,

	iImportKey			= 4,
	iImportSize			= 0xa000,

	iOffsetOEPRef11		= 2,
	iOffsetOEPRef12		= 8,
	iOffsetOEPRef13		= 4,

	iOffsetInnerDest11	= 2,
	iOffsetInnerSize11	= 9,
	iOffsetInnerLoop11	= 15,
	iIgnoreLoopd11		= 2
};

#endif // signatures_h__