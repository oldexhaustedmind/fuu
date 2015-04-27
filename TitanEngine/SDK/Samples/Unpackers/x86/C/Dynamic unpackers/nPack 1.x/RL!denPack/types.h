
#ifndef types_h__
#define types_h__

#pragma once
#include <windows.h>

typedef BYTE					uchar;
typedef USHORT					ushort;
typedef ULONG					ulong;
typedef ULONG_PTR				ulongptr;
typedef LONG_PTR				longptr;

typedef BYTE*					puchar;
typedef USHORT*					pushort;
typedef ULONG*					pulong;
typedef ULONG_PTR*				pulongptr;
typedef long*					plong;
typedef LONG_PTR*				plongptr;

typedef unsigned __int32		uint32;
typedef unsigned __int16		uint16;
typedef unsigned __int8			uint8;

typedef	__int32					int32;
typedef __int16					int16;
typedef __int8					int8;

typedef long long				llong;

typedef unsigned int			uint;

typedef void*					pvoid;
typedef char*					pchar;

#define reg						register ulong

#define PointerArray(pX)		((ulongptr*)(pX))
#define PointerAdd(pX, pY)		((ulong)((pX)) + (ulong)((pY)))

#define WordBuffer(pX)			((ushort*)(pX))

#define iLoop(pExpr)			for(register int i = 0; (pExpr); i++)
#define iLoopDown(pExpr)		for(register int i = (pExpr); i >= 0; i--)

#endif // types_h__