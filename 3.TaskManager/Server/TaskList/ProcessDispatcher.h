#include <vector>
#include <afxwin.h>
#include <string>
#include <tlhelp32.h>
#include <psapi.h>
#include "ProcessInfo.h"


#define REUSE_PROCESS_HANDLE 0
#pragma comment(lib, "psapi.lib") //remember added it
#pragma once

using namespace std;

class ProcessDispatcher
{
public:
	ProcessDispatcher();
	vector<ProcessInfo*> *getProcesses();
	bool killProcess(int processId);

private:
	void collectProccess();
	void wcharToChar(char* dst, WCHAR *src);
	string getProcessPath(DWORD processID);
	int getMemoryUsage(DWORD processID);

private:
	vector<ProcessInfo*> * _list;

};

