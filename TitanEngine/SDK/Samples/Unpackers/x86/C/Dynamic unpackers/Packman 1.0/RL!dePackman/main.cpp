#include "dialog.h"

int WINAPI WinMain(HINSTANCE pInstance, HINSTANCE, char*, int)
{
const char Title[] = "[ RL!dePackman ]";
const char About[] = "RL!dePackman\r\n"
                     "Unpacker for Packman 1.0\r\n\r\n"
					 "Unpacker coded by ReversingLabs.\r\n"
					 "Visit Reversing Labs at http://www.reversinglabs.com\r\n\r\n"
					 "Minimum engine version needed:\r\n"
					 "- TitanEngine 2.0.1 by RevLabs";

	cDialog Dialog(pInstance, Title, About);
	return 0;
}
