// TaskList.cpp: ãëàâíûé ôàéë ïğîåêòà.

#include "stdafx.h"
#include "Server.h"


using namespace std;

const int PORT = 9292;


int main(int argc, char* argv[])
{
	Server *server = new Server();
	server->setPort(PORT);
	server->start();
	return 0;
}