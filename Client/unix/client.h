#pragma once

#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>


using std::string;

class TClient
{
public:
	TClient();
	~TClient();

	bool Init(const char* ClntName, const char* SrvAddr, const char* SrvPort, const char* TimeOut);
	void Run();
private:

	string ClntName;
	int TimeOut;
	addrinfo AddrInfo;
	addrinfo* AddrInfoResult;
	int ServerSocket;

};