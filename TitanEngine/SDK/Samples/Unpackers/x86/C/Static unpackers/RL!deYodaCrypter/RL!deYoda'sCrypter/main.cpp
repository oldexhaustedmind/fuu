
#include "types.h"
#include "dialog.h"
#include "static.h"

int __stdcall WinMain(HINSTANCE pInstance, HINSTANCE, pchar, int)
{
	DialogBoxParamA(pInstance, MAKEINTRESOURCEA(rlDialog), 0, DLGPROC(Handler), 0);
	return 0;
}