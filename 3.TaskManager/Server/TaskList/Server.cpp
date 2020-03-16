#include "stdafx.h"
#include "Server.h"


Server::Server()
{
	// Initialize MFC and print an error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)) {
		die("Fatal Error: MFC initialization failed");
	}
	// Initialize the AfxSocket
	AfxSocketInit(NULL);

	_processDispatcher = new ProcessDispatcher();
}

Server::~Server() {
	_processDispatcher = 0;
	delete _processDispatcher;
}

void Server::start() {
	if (!server.Create(_port)) {
		die("Socket.Create() failed");
	}

	if (!server.Listen(MAX_PENDING)) {
		die("Socket.Listen() failed");
	}
	handle();
}

void Server::setPort(int port) {
	_port = port;
}

void Server::handle() {
	while (true) {

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

void Server::processingClient(CSocket &client) {
	char buff[RECVBUFFSIZE];

	int br = client.Receive(buff, sizeof(Command), 0);
	if (br < 0) {
		die("clntSock.Receive() failed");
	}

	if (br == sizeof(Command)) {
		buff[br] = '\0';

		Command *command;
		command = (Command*)buff;

		if (strcmp(command->name, "GET_LIST") == 0) {
			sendProcessList(client);
		}
		else if (strcmp(command->name, "KILL") == 0) {
			int processid = 0;
			bool result = _processDispatcher->killProcess(atoi(command->param));
			sendKillStatus(client, result);
		}
	}

	client.Close();
}

void Server::sendProcessList(CSocket& client) {

	vector<ProcessInfo*> *list = _processDispatcher->getProcesses();

	for (vector<ProcessInfo*>::iterator info = list->begin();
		info != list->end(); ++info) {

		std::cout << "id: " << (*info)->id << endl;
		std::cout << "thread: " << (*info)->threds << std::endl;
		std::cout << "memory: " << (*info)->memory << std::endl;
		std::cout << "path: " << (*info)->path << std::endl;
		std::cout << "name: " << (*info)->name << std::endl;

		client.Send((char*)(*info), sizeof(ProcessInfo));
	}
}


void Server::sendKillStatus(CSocket& socket, bool result) {

	Command *command = new Command();
	strcpy_s(command->name, "KILL");
	strcpy_s(command->param, (result) ? "OK" : "ERROR");

	socket.Send((char*)command, sizeof(Command), 0);
}

void Server::die(char *msg) {
	printf(msg);
	exit(1);
}