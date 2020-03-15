// ClientFileTransfer.cpp: определяет точку входа для консольного приложения.
//
#include "stdafx.h"
#include <afxsock.h>
#include <iostream>
#include "ClientSocket.h"

using namespace std;

const int PORT = 9292;

int main(int argc, char * argv[])
{
	if ((argc < 3)) {
		printf("Usage: %s server_ip filename\n", argv[0]);
		exit(1);
	}

	char* servIP = argv[1];
	char* filename = argv[2];

	ClientSocket *client = new ClientSocket();
	client->setPort(PORT);
	client->setServerIP(servIP);
	client->send(filename);

	return 0;
}

