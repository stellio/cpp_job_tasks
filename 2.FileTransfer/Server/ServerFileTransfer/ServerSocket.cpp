#include "stdafx.h"
#include "ServerSocket.h"


ServerSocket::ServerSocket()
{
	// Initialize MFC and print an error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)) {
		die("Fatal Error: MFC initialization failed");
	}
	// Initialize the AfxSocket
	AfxSocketInit(NULL);
}


ServerSocket::~ServerSocket()
{
	server.Close();
}

void ServerSocket::start() {

	if (!server.Create(_port)) {
		die("Socket.Create() failed");
	}

	if (!server.Listen(MAX_PENDING)) {
		die("Socket.Listen() failed");
	}
	handle();
}

void ServerSocket::setPort(int port) {
	_port = port;
}

void ServerSocket::handle() {
	while(true) {

		SOCKADDR_IN clientAddr;
		int clientAddrLength = sizeof(clientAddr);

		CSocket client;

		if (!server.Accept(client)) {
			die("ServerSocket.Accept() failed");
		}

		if (!client.GetPeerName((SOCKADDR*)&clientAddr, &clientAddrLength)) {
			die("ClientSocket.GetPeerName() failed");
		}

		char ip[1000];
		inet_ntop(AF_INET, &(clientAddr.sin_addr), ip, sizeof(ip));
		printf("Handling client %s\n", ip);

		processingClient(client);
	}
}

void ServerSocket::processingClient(CSocket &client) {

	int rb = 0;
	char buff[1024];
	bool isFileBodyPending = false;

	do {
		if (isFileBodyPending) {
			openFile();
			BYTE *recdData = new byte[FILE_BUFFER_SIZE];
			int cbLeftToReceive = _FileInfo->size;
			do
			{
				int iiGet, iiRecd;

				iiGet = (cbLeftToReceive < FILE_BUFFER_SIZE) ? cbLeftToReceive : FILE_BUFFER_SIZE;
				iiRecd = client.Receive(recdData, iiGet);
				// test for errors and get out if they occurred
				if (iiRecd == SOCKET_ERROR)
				{
					die("File Body Receive: SOCKET_ERROR");
				}
				// good data was retrieved, so accumulate it with already-received data
				_outFile->Write(recdData, iiRecd); // Write it
				cbLeftToReceive -= iiRecd;

			} while (cbLeftToReceive > 0);

			client.Send("OK", 2);
			isFileBodyPending = false;
			_outFile->Close();
			
		}
		else {
			rb = client.Receive(buff, 1024, 0);

			if (rb == sizeof(FileInfo)) {
				buff[rb] = '\0';

				_FileInfo = (FileInfo*)buff;

				printf("File name: %s\n", _FileInfo->name);
				printf("File size: %d, bytes\n", (int)_FileInfo->size);

				client.Send("OK", 2);
				isFileBodyPending = true;
			}
		}

	} while (rb > 0);

	printf("Client close connection\n");
	client.Close();
}

bool ServerSocket::openFile() {
	_outFile = new CFile();
	CFileException fe;

	if (_outFile->Open(CString(_FileInfo->name), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, &fe))
	{
		TRACE(_T("Can't open file for write %s, error = %u\n"),
			_FileInfo->name, fe.m_cause);
		return false;
	}
	return true;
}


void ServerSocket::die(char *msg) {
	printf(msg);
	exit(1);
}
