#include <iostream>
#include <afxsock.h>
#include "FileInfo.h"
#pragma once

using namespace std;

const int FILE_BUFFER_SIZE = 4096;
const int STATUS_BUFFER_SIZE = 8;

class ClientSocket
{
public:
	ClientSocket();
	~ClientSocket();

	void setPort(int port);
	void setServerIP(char *ip);
	void send(char *filename);

private:
	void handle();
	bool openFile();
	bool sendFileInfo();
	bool sendFile();
	bool ifSendSuccess();
	void die(char *msg);

private:
	CSocket _client;
	int _port;
	char *_server_ip;
	char *_filename;
	CFile *_inputFile;
	bool _isDone = false;
};

