#pragma once

#include <thread>
#include <string>
#include <queue>
#include <mutex>
#include <cstdio>
#include <memory>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>

using std::string;
using std::thread;
using std::queue;
using std::mutex;
using std::FILE;
using std::unique_ptr;
using std::shared_ptr;
using std::condition_variable;
using std::unique_lock;
using std::vector;

class TServer;

class TConnectedClient
{
public:
	TConnectedClient();

	bool Init(int ClientSocket, TServer* Server);
private:
	void Proc();

	unique_ptr<thread> Thread;
	vector<char> Buffer;
	int ClientSocket;
	TServer* Server;
};

class TLogger
{
public:
	TLogger();

	bool Init(const char* FileName);
	void Enqueue(std::shared_ptr<string> Msg);

private:

	void Proc();
	void WriteStr(shared_ptr<string> Str);

	//FILE* LogFile;
	unique_ptr<thread> Thread;
	mutex Mutex;
	queue<shared_ptr<string>> Queue;
	condition_variable QueueEvent;
};


class TServer
{
public:
	TServer();
	~TServer();

	bool Init(const char* Port);
	void Run();
	TLogger* GetLogger();

private:
	
	addrinfo AddrInfo;
	addrinfo* AddrInfoResult;
	int ServerSocket;
	
	mutex Mutex;
	unique_ptr<TLogger> Logger;
	vector<unique_ptr<TConnectedClient>> ConnectedClients;
};