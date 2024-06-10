#include "server.h"
#include <stdio.h>

constexpr size_t BufLen = 512;

TConnectedClient::TConnectedClient():
	Server(nullptr),
	ClientSocket(INVALID_SOCKET)
{

}

bool TConnectedClient::Init(SOCKET ClientSocket, TServer* Server)
{
	this->ClientSocket = ClientSocket;
	this->Server = Server;
	Buffer.resize(BufLen);

	Thread = unique_ptr<thread>((new thread(&TConnectedClient::Proc, this)));
		
	if (!Thread)
	{
		printf("Client thread creating error!\n");
		return false;
	}

	return true;
}

void TConnectedClient::Proc()
{
	int Result;
	shared_ptr<string> Msg;
	do
	{
		Result = recv(ClientSocket, Buffer.data(), Buffer.size(), 0);
		if (Result > 0)
		{
			printf("Bytes received: %d\n", Result);
			printf("Received data: %s\n", Buffer.data());
		}
		else if (Result == 0)
		{
			printf("Connection closing...\n");
		}
		else 
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			break;
		}

	} while (Result > 0);
}

TLogger::TLogger():
	LogFile(nullptr)
{

}

bool TLogger::Init(const char* FileName)
{
	//LogFile = fopen("/log.txt", "a+");

	if(!LogFile)
	{
		printf("File operation error!\n");
		return false;
	}

	Thread = unique_ptr<thread>((new thread(&TLogger::Proc, this)));

	if (!Thread)
	{
		printf("Logger thread creating error!\n");
		return false;
	}

	return true;
}

void TLogger::Enqueue(std::shared_ptr<string> Msg)
{
	unique_lock<mutex> Lock(Mutex);
	Queue.push(Msg);
	QueueEvent.notify_one();
}

void TLogger::Proc()
{
	shared_ptr<string> Msg;
	while (true)
	{
		unique_lock<mutex> Lock(Mutex);
		QueueEvent.wait(Lock, [=] {return Queue.empty() == false; });

		Msg = Queue.front();
		Queue.pop();

		if ((*Msg) == "Exit")
		{
			break;
		}

		WriteStr(Msg);
	}
}

void TLogger::WriteStr(shared_ptr<string> Str)
{
	if (LogFile)
	{
		fprintf(LogFile, Str->c_str());
	}
}

TServer::TServer():
	WsaData{},
	AddrInfo{},
	AddrInfoResult(nullptr),
	ListenSocket(INVALID_SOCKET)
{

}

bool TServer::Init(const char* Port)
{
	bool Result = WSAStartup(MAKEWORD(2, 2), &WsaData);
	if (Result != 0)
	{
		printf("WSAStartup failed: %d\n", Result);
		return false;
	}
	
	AddrInfo.ai_family = AF_INET;
	AddrInfo.ai_socktype = SOCK_STREAM;
	AddrInfo.ai_protocol = IPPROTO_TCP;
	AddrInfo.ai_flags = AI_PASSIVE;

	Result = getaddrinfo(NULL, Port, &AddrInfo, &AddrInfoResult);
	if (Result != 0)
	{
		printf("getaddrinfo failed: %d\n", Result);
		WSACleanup();
		return false;
	}

	ListenSocket = socket(AddrInfoResult->ai_family, AddrInfoResult->ai_socktype, AddrInfoResult->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) 
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(AddrInfoResult);
		WSACleanup();
		return false;
	}

	Result = bind(ListenSocket, AddrInfoResult->ai_addr, (int)AddrInfoResult->ai_addrlen);
	if (Result == SOCKET_ERROR) 
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(AddrInfoResult);
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}
}

void TServer::Run()
{
	SOCKET ClientSocket;
	bool Result;

	while (true)
	{
		Result = listen(ListenSocket, SOMAXCONN);
		if (Result == SOCKET_ERROR) 
		{
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return;
		}

		printf("Ready to accept clients...\n");
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) 
		{
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return;
		}

		printf("Accept a connection from a client\n");
		ConnectedClients.push_back(unique_ptr<TConnectedClient>(new TConnectedClient{}));
		auto& Last = ConnectedClients.back();
		bool Result = Last->Init(ClientSocket, this);

		if (!Result)
		{
			printf("Failed to init client object\n");
		}

	}
}

TLogger* TServer::GetLogger()
{
	return Logger.get();
}