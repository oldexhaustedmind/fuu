#include "dialog.h"

int WINAPI WinMain(HINSTANCE pInstance, HINSTANCE, char*, int)
{
const char Title[] = "[ RL!deExeFog ]";
const char About[] = "RL!deExeFog\r\n"
                     "Unpacker for ExeFog 1.x\r\n\r\n"
					 "Unpacker coded by ReversingLabs.\r\n"
					 "Visit Reversing Labs at http://www.reversinglabs.com\r\n\r\n"
					 "Minimum engine version needed:\r\n"
					 "- TitanEngine 2.0.1 by RevLabs";

	cDialog Dialog(pInstance, Title, About);
	return 0;
}
