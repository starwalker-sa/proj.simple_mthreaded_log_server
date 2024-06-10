#include "client.h"
#include <memory>
#include <chrono>

#pragma comment(lib, "Ws2_32.lib")

using std::unique_ptr;

int main(int argc, char* argv[])
{
	unique_ptr<TClient> Client = unique_ptr<TClient>(new TClient{});

	if (argc == 5)
	{
		const char* SrvAddr = argv[1];
		const char* ClntName = argv[2];
		const char* PortNum = argv[3];
		const char* TimeOut = argv[4];

		printf("SrvAddr: %s\n", SrvAddr);
		printf("ClntName: %s\n", ClntName);
		printf("PortNum: %s\n", PortNum);
		printf("TimeOut: %s\n", TimeOut);

		bool Result = Client->Init(ClntName, SrvAddr, PortNum, TimeOut);

		if (!Result)
		{
			printf("Cannot init client\n");
			return 0;
		}

		Client->Run();
	}
	else
	{
		printf("Invalid arguments!\n");
	}

	return 0;
}