#include "stdafx.h"
#include "ClientSocket.h"


ClientSocket::ClientSocket()
{
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)) {
		die("Fatal Error: MFC initialization failed");
	}

	AfxSocketInit(NULL);
}


ClientSocket::~ClientSocket()
{
	delete _server_ip;
}

void ClientSocket::setPort(int port) {
	_port = port;
}

void ClientSocket::setServerIP(char *ip) {
	_server_ip = ip;
}

void ClientSocket::send(char *filename) {

	_filename = filename;

	if (!_client.Create()) {
		die("Socket create failed");
	}

	if (!_client.Connect(CString(_server_ip), _port)) {
		die("Connected failed");
	}

	handle();
}

void ClientSocket::handle() {

	do {
		if (!openFile()) {
			die("Can't open file");
		}

		if (!sendFileInfo()) {
			die("Can't send file info");
		}

		if (!sendFile()) {
			die("Can't send file");
		}
		else {
			printf("File send: success!\n");
		}

		_isDone = true;

	} while (!_isDone);
	_client.Close();
}

bool ClientSocket::openFile() {
	_inputFile = new CFile();
	CFileException fileException;

	if (!_inputFile->Open(CString(_filename), CFile::modeRead | CFile::typeBinary, &fileException)) {
		TRACE(_T("Can't open file %s, error = %u\n"),
			_filename, fileException.m_cause);
		return false;
	}
	return true;
}

bool ClientSocket::sendFileInfo() {
	FileInfo *fileInfo = new FileInfo();
	strcpy_s(fileInfo->name, _filename);
	fileInfo->size = _inputFile->GetLength();

	_client.Send((char*)fileInfo, sizeof(FileInfo), 0);

	delete fileInfo;
	return ifSendSuccess();
}

bool ClientSocket::sendFile() {
	BYTE* sendData = NULL;
	sendData = new BYTE[FILE_BUFFER_SIZE];

	unsigned __int64 cbLeftToSend = _inputFile->GetLength();

	do {
		int amountToSend, doneSoFar, buffOffset;

		amountToSend = _inputFile->Read(sendData, FILE_BUFFER_SIZE);
		buffOffset = 0;

		do {
			doneSoFar = _client.Send(sendData + buffOffset, amountToSend);

			if (doneSoFar == SOCKET_ERROR) {
				die("Send file SOCKET_ERROR");
			}

			buffOffset += doneSoFar;
			amountToSend -= doneSoFar;
			cbLeftToSend -= doneSoFar;
		} while (amountToSend > 0);
	} while (cbLeftToSend > 0);

	return ifSendSuccess();
}

bool ClientSocket::ifSendSuccess() {

	char buff[8];
	int rb = _client.Receive(buff, 8);
	buff[rb] = '\0';
	return true;
}

void ClientSocket::die(char *msg) {
	printf("Die: %s\n", msg);
	exit(1);
}
