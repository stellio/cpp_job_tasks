#include "stdafx.h"
#include "ProcessDispatcher.h"


ProcessDispatcher::ProcessDispatcher()
{
	_list = new vector<ProcessInfo*>;
}

vector<ProcessInfo*>* ProcessDispatcher::getProcesses() {

	collectProccess();
	return _list;
}

void ProcessDispatcher::collectProccess() {

	_list->clear();
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
				strcpy_s(processInfo->path, getProcessPath(process.th32ProcessID).c_str());

				if (strlen(processInfo->path) > 0)
					_list->push_back(processInfo);
			}

		} while (Process32Next(hSnapProcess, &process));

		CloseHandle(hSnapProcess);
	}
}

int ProcessDispatcher::getMemoryUsage(DWORD processID) {
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

bool ProcessDispatcher::killProcess(int processId) {
	HANDLE process;
	process = OpenProcess(PROCESS_TERMINATE, false, processId);

	printf("KIll process: %d\n", processId);

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

string ProcessDispatcher::getProcessPath(DWORD processID) {
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


void ProcessDispatcher::wcharToChar(char* dst, WCHAR *src) {
	const wchar_t* str = src;
	char buf[260];
	size_t len = wcstombs(buf, str, wcslen(str));
	if (len > 0u)
		buf[len] = '\0';
	strcpy(dst, buf);
}