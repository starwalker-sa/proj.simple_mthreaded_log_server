#include "server.h"
#include <stdio.h>

constexpr size_t BufLen = 512;

TConnectedClient::TConnectedClient():
	Server(nullptr),
	ClientSocket(0)
{

}

bool TConnectedClient::Init(int ClientSocket, TServer* Server)
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
	TLogger* Logger = Server->GetLogger();

	do
	{
		Result = recv(ClientSocket, Buffer.data(), Buffer.size(), 0);

		if (Result > 0) 
		{
			printf("Bytes received: %d\n", Result);
			printf("Received data: %s\n", Buffer.data());
			Msg = shared_ptr<string>(new string(Buffer.data()));
			Logger->Enqueue(Msg);
		}
		else if (Result == 0)
			printf("Connection closing...\n");
		else 
		{
			printf("recv failed\n");
			break;
		}

	}while (Result > 0);

}


TLogger::TLogger()
{

}

bool TLogger::Init(const char* FileName)
{


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
		printf("Got message from client object\n");

		Msg = Queue.front();
		WriteStr(Msg);

		if ((*Msg) == "Exit")
		{
			break;
		}

		Queue.pop();
	}
}

void TLogger::WriteStr(shared_ptr<string> Str)
{
	FILE* LogFile = fopen("./log.txt", "a+");

	if(!LogFile)
	{
		printf("File operation error!\n");
		return;
	}

	if (LogFile)
	{
		//fprintf(LogFile, Str->c_str());
		printf("Write str to file: %s\n", Str->c_str());
		fprintf(LogFile, "%s\n", Str->c_str());
		fclose(LogFile);
	}
}

TServer::TServer():
	AddrInfo{},
	AddrInfoResult(nullptr),
	ServerSocket(0)
{

}

TServer::~TServer()
{
	shutdown(ServerSocket, SHUT_RD);
}

bool TServer::Init(const char* Port)
{
	Logger = unique_ptr<TLogger>(new TLogger{});
	Logger->Init(nullptr);
 

	int Result;
	AddrInfo.ai_family = AF_INET;
	AddrInfo.ai_socktype = SOCK_STREAM;
	AddrInfo.ai_protocol = 0;
	AddrInfo.ai_flags = AI_PASSIVE;

	Result = getaddrinfo(nullptr, Port, &AddrInfo, &AddrInfoResult);
	if (Result != 0) 
	{
		printf("Getting addrinfo error\n");
		return false;
	}

	addrinfo* m = nullptr;
	for(m = AddrInfoResult; m != nullptr; m = m->ai_next)
	{
		ServerSocket = socket(m->ai_family, m->ai_socktype, m->ai_protocol);
        if (ServerSocket == -1)
              continue;
		
		Result = bind(ServerSocket, m->ai_addr, m->ai_addrlen);
		if(Result == 0)
			break;
	}

	if (m == nullptr) 
	{
		printf("Binding socket error\n");
		return false;
	}

	return true;
}

void TServer::Run()
{
	int ClientSocket;
	int Result;

	while (true)
	{
		Result = listen(ServerSocket, SOMAXCONN);
		if (Result != 0) 
		{
			printf("listen failed\n");
			return;
		}

		printf("Ready to accept clients...\n");
		ClientSocket = accept(ServerSocket, nullptr, nullptr);
		if (ClientSocket == -1) 
		{
			printf("accept failed\n");
			return;
		}

		printf("Accept a connection from a client\n");
		ConnectedClients.push_back(unique_ptr<TConnectedClient>(new TConnectedClient{}));
		auto& Last = ConnectedClients.back();
		Result = Last->Init(ClientSocket, this);

		if (!Result)
			printf("Failed to init client object...\n");

	}
}

TLogger* TServer::GetLogger()
{
	return Logger.get();
}