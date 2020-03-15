// TaskList.cpp: главный файл проекта.

#include "stdafx.h"
#include <afx.h>
#include <afxwin.h>
#include <string>
#include <iostream>
#include <tlhelp32.h>
#include <psapi.h>
#include <conio.h>
#include <vector>
#include <afxsock.h>
#include "Command.h"
#include "ProcessInfo.h"

#define REUSE_PROCESS_HANDLE 0
#pragma comment(lib, "psapi.lib") //remember added it

using namespace std;

const int MAX_PENDING = 5;
const int RECVBUFFSIZE = 1000;
const int RECV_BUFFER_SIZE = 4096;
const int PORT = 9292;

std::vector<ProcessInfo*> *processVector;

void DieWithError(char* message);
void HandleTCPClient(CSocket& clntSock);
void sendProcessList(CSocket& socket);
void sendKillStatus(CSocket& socket, bool result);

int getMemoryUsage(DWORD processID);
std::string getProcessPath(DWORD processID);
void updateProcessList(std::vector<ProcessInfo*> * list);
void wcharToChar(char* dst, WCHAR *src);
bool killProcess(int processId);

int main(int argc, char* argv[])
{
	processVector = new std::vector<ProcessInfo*>;

	// Initialize MFC and print an error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)) {
		DieWithError("Fatal Error: MFC initialization failed");
	}
	// Initialize the AfxSocket
	AfxSocketInit(NULL);

	int echoServPort = PORT;
	CSocket servSock;

	if (!servSock.Create(echoServPort)) {
		DieWithError("servSock.Create() failed");
	}

	if (!servSock.Listen(MAX_PENDING)) {
		DieWithError("servSock.Listen() failed");
	}

	for (;;) {

		SOCKADDR_IN echoCliendAdrr;

		int clientLen = sizeof(echoCliendAdrr);

		CSocket clientSocket;

		if (!servSock.Accept(clientSocket)) {
			DieWithError("servSock.Accept() failed");
		}

		if (!clientSocket.GetPeerName((SOCKADDR*)&echoCliendAdrr, &clientLen)) {
			DieWithError("clntSock.GetPeerName() failed");
		}

		cout << "Handling client " << inet_ntoa(echoCliendAdrr.sin_addr) << endl;

		HandleTCPClient(clientSocket);
	}

	return 0;
}

void HandleTCPClient(CSocket& clntSock) {

	char buff[RECVBUFFSIZE]; 

	int br = clntSock.Receive(buff, sizeof(Command), 0);
	if (br < 0) {
		DieWithError("clntSock.Receive() failed");
	}

	if (br == sizeof(Command)) {
		buff[br] = '\0';

		Command *command;
		command = (Command*)buff;

		if (strcmp(command->name, "GET_LIST") == 0) {
			sendProcessList(clntSock);
		}
		else if (strcmp(command->name, "KILL") == 0) {
			int processid = 0;
			bool result = killProcess(atoi(command->param));
			sendKillStatus(clntSock, result);
		}
	}

	clntSock.Close(); // Close client socket
}

void sendProcessList(CSocket& socket) {

	updateProcessList(processVector);

	ProcessInfo *processInfo;

	for (std::vector<ProcessInfo*>::iterator info = processVector->begin();
		info != processVector->end(); ++info) {
	
		std::cout << "id: " << (*info)->id << endl;
		std::cout << "thread: " << (*info)->threds << std::endl;
		std::cout << "memory: " << (*info)->memory << std::endl;
		std::cout << "path: " << (*info)->path << std::endl;
		std::cout << "name: " << (*info)->name << std::endl;

		socket.Send((char*)(*info), sizeof(ProcessInfo));
	}
}

void sendKillStatus(CSocket& socket, bool result) {

	Command *command = new Command();
	strcpy_s(command->name, "KILL");
	strcpy_s(command->param, (result) ? "OK" : "ERROR");

	socket.Send((char*)command, sizeof(Command), 0);
}

void updateProcessList(std::vector<ProcessInfo*> * list) {

	list->clear();
	HANDLE hSnapProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (hSnapProcess != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 process;
		process.dwSize = sizeof(PROCESSENTRY32);
		Process32First(hSnapProcess, &process);
		do
		{
			if (process.th32ProcessID != 0)
			{
				ProcessInfo *processInfo = new ProcessInfo();
				processInfo->id = (int)process.th32ProcessID;

				wcharToChar(processInfo->name, process.szExeFile);
				processInfo->memory = getMemoryUsage(process.th32ProcessID);
				processInfo->threds = (int)process.cntThreads;
				strcpy(processInfo->path, getProcessPath(process.th32ProcessID).c_str());

				if (strlen(processInfo->path) > 0)
					list->push_back(processInfo);
			}

		} while (Process32Next(hSnapProcess, &process));

		CloseHandle(hSnapProcess);
	}
}

int getMemoryUsage(DWORD processID) {
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;
	int usageMemory = 0;

	// Get information about the memory usage of the process.
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (NULL == hProcess)
		return 0;

	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
		usageMemory = int(pmc.WorkingSetSize / 1024);
	}
	CloseHandle(hProcess);
	return usageMemory;
}

bool killProcess(int processId) {
	HANDLE process;
	process = OpenProcess(PROCESS_TERMINATE, false, processId);

	cout << "KIll process: " << processId << endl;

	if (process != NULL) {
		if (TerminateProcess(process, 1) == TRUE) {
			CloseHandle(process);
			return true;
		}
		else {
			CloseHandle(process);
			return false;
		}
	}
	return false;
}

std::string getProcessPath(DWORD processID) {
	std::string returnValue;
	ProcessInfo *processInfo = new ProcessInfo;

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processID);
	if (hProcess != NULL)
	{
		wchar_t filePath[MAX_PATH];
		if (GetModuleFileNameExW(hProcess, NULL, filePath, MAX_PATH))
		{
			char ch[260];
			char DefChar = ' ';
			WideCharToMultiByte(CP_ACP, 0, filePath, -1, ch, 260, &DefChar, NULL);
			std::string ss(ch);
			returnValue = ss;
		}
		CloseHandle(hProcess);
	}
	return returnValue;
}

void DieWithError(char *message) {
	cerr << strerror(errno) << ": " << message << endl;
}

void wcharToChar(char* dst, WCHAR *src) {
	const wchar_t* str = src;
	char buf[260];
	size_t len = wcstombs(buf, str, wcslen(str));
	if (len > 0u)
		buf[len] = '\0';
	strcpy(dst, buf);
}