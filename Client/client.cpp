#include "client.h"
#include <thread>
#include <chrono>

using std::chrono::system_clock;
using std::string;

#pragma warning(disable : 4996)
string GetCurrentTimestamp();

TClient::TClient():
	WsaData{},
	AddrInfo{},
	AddrInfoResult{},
	ServerSocket(INVALID_SOCKET),
	TimeOut(0)
{

}

TClient::~TClient()
{
	freeaddrinfo(AddrInfoResult);
	closesocket(ServerSocket);
	WSACleanup();
}

bool TClient::Init(const char* ClntName, const char* SrvAddr, const char* SrvPort, const char* TimeOut)
{
	int Result;
	this->TimeOut = stoi(TimeOut);
	this->ClntName = ClntName;

	printf("Init client... Timeput %i ms\n", this->TimeOut);

	Result = WSAStartup(MAKEWORD(2, 2), &WsaData);

	if (Result != 0) 
	{
		printf("WSAStartup failed: %d\n", Result);
		return false;
	}

	AddrInfo.ai_family = AF_INET;
	AddrInfo.ai_socktype = SOCK_STREAM;
	AddrInfo.ai_protocol = IPPROTO_TCP;

	Result = getaddrinfo(SrvAddr, SrvPort, &AddrInfo, &AddrInfoResult);

	if (Result != 0)
	{
		printf("getaddrinfo failed: %d\n", Result);
		return false;
	}

	ServerSocket = socket(AddrInfoResult->ai_family, AddrInfoResult->ai_socktype,
		AddrInfoResult->ai_protocol);

	if (ServerSocket == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		return false;
	}

	Result = connect(ServerSocket, AddrInfoResult->ai_addr, (int)AddrInfoResult->ai_addrlen);

	if (Result == SOCKET_ERROR)
	{
		printf("connect failed: %d\n", Result);
		return false;
	}

	return true;
}

void TClient::Run()
{
	int Result;
	string SentData;
	int SentNum = 0;
	int Count = 10;

	printf("Run client...\n");

	do 
	{
		SentData.clear();
		SentData += GetCurrentTimestamp() + "\t";
		SentData += ClntName;
		SentData += "\tTest";
		SentData += to_string(SentNum);

		Result = send(ServerSocket, SentData.c_str(), (int)SentData.size(), 0);
		if (Result == SOCKET_ERROR)
		{
			printf("send failed: %d\n", WSAGetLastError());
			break;
		}

		printf("Bytes Sent: %ld\n", Result);

		Sleep(TimeOut);
		++SentNum;
		--Count;
	} while (Count);

	printf("Shuting down...\n");

	Result = shutdown(ServerSocket, SD_SEND);
	if (Result == SOCKET_ERROR) 
	{
		printf("shutdown failed: %d\n", WSAGetLastError());
	}
}

string GetCurrentTimestamp()
{
	auto CurrentTime = system_clock::now();
	char Buffer[80];
	auto Tr = CurrentTime.time_since_epoch().count() / 1000000;
	auto ToMs = Tr % 1000;
	std::time_t Time = system_clock::to_time_t(CurrentTime);
	auto TimeInfo = localtime(&Time);
	strftime(Buffer, 80, "%F %H:%M:%S", TimeInfo);
	sprintf(Buffer, "%s.%03d", Buffer, (int)ToMs);
	return std::string(Buffer);
}