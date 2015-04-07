
#include "types.h"
#include "dialog.h"

int __stdcall WinMain(HINSTANCE pInstance, HINSTANCE, pchar, int)
{
	const pchar About =	"RL!detElock 0.8x - 0.9x unpacker \r\n\r\n" \
						" Visit Reversing Labs at http://www.reversinglabs.com \r\n\r\n" \
						"  Minimum engine version needed:\r\n" \
						"- TitanEngine 2.0.3 by RevLabs\r\n\r\n" \
						"Unpacker coded by Reversing Labs.";

	cDialog Dialog(pInstance, "RL!detElock from Reversing Labs", About);
	return 0;
}