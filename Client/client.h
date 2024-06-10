#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <string>
#include <vector>

using std::stoi;
using std::string;
using std::vector;
using std::to_string;

class TClient
{
public:
	TClient();
	~TClient();

	bool Init(
		const char* ClntName,
		const char* SrvAddr,
		const char* SrvPort,
		const char* TimeOut);

	void Run();
private:

	string ClntName;
	int TimeOut;
	WSADATA WsaData;
	addrinfo AddrInfo;
	addrinfo* AddrInfoResult;
	SOCKET ServerSocket;
};