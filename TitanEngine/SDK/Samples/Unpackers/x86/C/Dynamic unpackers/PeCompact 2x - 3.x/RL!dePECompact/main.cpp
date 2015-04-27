
#include "types.h"
#include "dialog.h"

int __stdcall WinMain(HINSTANCE pInstance, HINSTANCE, pchar, int)
{
	const pchar About =	"RL!dePECompact 2.x - 3.0 unpacker \r\n\r\n" \
						" Visit Reversing Labs at http://www.reversinglabs.com \r\n\r\n" \
						"  Minimum engine version needed:\r\n" \
						"- TitanEngine 2.0.1 by RevLabs\r\n\r\n" \
						"Unpacker coded by ReversingLabs.";

	cDialog Dialog(pInstance, "RL!dePECompact from ReversingLabs", About);
	return 0;
}