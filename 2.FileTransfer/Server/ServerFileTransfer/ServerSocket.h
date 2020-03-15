#include <iostream>
#include <afxsock.h>
#include "FileInfo.h"
#pragma once

using namespace std;

const int MAX_PENDING = 5;
const int FILE_NAME_SIZE = 256;
const int FILE_BUFFER_SIZE = 4096;


class ServerSocket
{
public:
	ServerSocket();
	~ServerSocket();

	void start();
	void setPort(int port);

private:
	void handle();
	void processingClient(CSocket &client);
	bool openFile();
	void die(char *msg);

private:
	CSocket server;
	int _port;
	CFile *_outFile;
	FileInfo *_FileInfo;
};

