
#include "types.h"
#include "dialog.h"

int __stdcall WinMain(HINSTANCE pInstance, HINSTANCE, pchar, int)
{
	const pchar About =	"RL!deMEW 10 - 11 SE 1.2 unpacker \r\n\r\n" \
						" Visit Reversing Labs at http://www.reversinglabs.com \r\n\r\n" \
						"  Minimum engine version needed:\r\n" \
						"- TitanEngine 2.0.1 by RevLabs\r\n\r\n" \
						"Unpacker coded by Reversing Labs.";

	cDialog Dialog(pInstance, "RL!deMEW from Reversing Labs", About);
	return 0;
}