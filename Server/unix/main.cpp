#include "server.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
	if(argc == 2)
	{
		TServer* Srv = new TServer();
		Srv->Init(argv[1]);
		Srv->Run();
	

	}else{
		printf("Invalid arguments\n");
	}

	return 0;
}