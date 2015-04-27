
#include "types.h"
#include "dialog.h"

int __stdcall WinMain(HINSTANCE pInstance, HINSTANCE, pchar, int)
{
	const pchar About =	"RL!deCryptoCrack'sPEProtector 0.9.2 - 0.9.3 static unpacker \r\n\r\n" \
						" Visit Reversing Labs at http://www.reversinglabs.com \r\n\r\n" \
						"  Minimum engine version needed:\r\n" \
						"- TitanEngine 2.0.2 by RevLabs\r\n\r\n" \
						"Unpacker coded by Reversing Labs.";

	cDialog Dialog(pInstance, "RL!deCryptoCrack'sPEProtector from Reversing Labs", About);
	return 0;
}