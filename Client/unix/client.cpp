#include "client.h"
#include <chrono>
#include <stdio.h>
#include <unistd.h>


using std::chrono::system_clock;
using std::stoi;
using std::to_string;

string GetCurrentTimestamp();

TClient::TClient():
	AddrInfo{},
	AddrInfoResult{},
	ServerSocket(0),
	TimeOut(0)
{

}

TClient::~TClient()
{

}

bool TClient::Init(const char* ClntName, const char* SrvAddr, const char* SrvPort, const char* TimeOut)
{
	int Result;
	this->TimeOut = stoi(TimeOut);
	this->ClntName = ClntName;

	printf("Init client...%i\n", this->TimeOut);

	AddrInfo.ai_family = AF_INET;
	AddrInfo.ai_socktype = SOCK_STREAM;
	AddrInfo.ai_protocol = 0;

	Result = getaddrinfo(SrvAddr, SrvPort, &AddrInfo, &AddrInfoResult);
	if (Result != 0)
	{
		printf("getaddrinfo failed\n");
		return false;
	}


	ServerSocket = socket(AddrInfoResult->ai_family, AddrInfoResult->ai_socktype, AddrInfoResult->ai_protocol);

	Result = connect(ServerSocket, AddrInfoResult->ai_addr, (int)AddrInfoResult->ai_addrlen);
	if (Result == -1)
	{
		printf("connect failed: %d\n", Result);
		return false;
	}

	return true;
}

void TClient::Run()
{
	ssize_t Result;
	string SentData;
	int SentNum = 0;
	int Count = 10;

	printf("Run client...\n");

	do 
	{
		SentData.clear();
		SentData += GetCurrentTimestamp() + "\t";
		SentData += ClntName;
		//SentData += "\tTest";
		//SentData += to_string(SentNum);


		Result = send(ServerSocket, SentData.c_str(), (int)SentData.size(), 0);
		if (Result == -1)
		{	
			printf("Error sending \n");
			break;
		}

		printf("Bytes Sent: %ld\n", Result);

		sleep(TimeOut);
		++SentNum;
		--Count;
	} while (Count);

	printf("Shuting down...\n");

	freeaddrinfo(AddrInfoResult);
	Result = shutdown(ServerSocket, SHUT_WR);

	if (Result == -1) 
	{
		printf("shutdown failed\n");
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

	return string(Buffer);
}