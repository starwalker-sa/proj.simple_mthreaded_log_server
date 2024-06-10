#include "server.h"

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[])
{
	TServer* Srv = new TServer();
	Srv->Init("3000");
	Srv->Run();
	return 0;
}