#include <iostream>
#include <afxsock.h>
#include "Command.h"
#include "ProcessInfo.h"
#include "ProcessDispatcher.h"

const int MAX_PENDING = 5;
const int RECVBUFFSIZE = 1000;

using namespace std;
#pragma once
class Server
{
public:
	Server();
	~Server();

	void start();
	void setPort(int port);

private:
	void handle();
	void processingClient(CSocket &client);
	void sendKillStatus(CSocket& socket, bool result);
	void sendProcessList(CSocket& client);
	void die(char *msg);

private:
	CSocket server;
	int _port;
	ProcessDispatcher *_processDispatcher;
};

