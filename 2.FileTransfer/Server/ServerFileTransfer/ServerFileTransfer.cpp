// ServerFileTransfer.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <afxsock.h>
#include <iostream>
#include "ServerSocket.h"

using namespace std;

const int PORT = 9292;

int main()
{
	ServerSocket server;
	server.setPort(PORT);
	server.start();

    return 0;
}

