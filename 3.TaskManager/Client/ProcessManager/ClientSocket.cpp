// ClientSocket.cpp: файл реализации
//

#include "stdafx.h"
#include "ProcessManager.h"
#include "ClientSocket.h"
#include "ProcessInfo.h"
#include "Command.h"
#include "ProcessManagerDlg.h"
#include <iostream>


// ClientSocket

ClientSocket::ClientSocket()
{
}

ClientSocket::~ClientSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientSocket, CSocket)
	//{{AFX_MSG_MAP(CClientSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0
// функции-члены ClientSocket

void ClientSocket::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	char buff[1000];
	int br = Receive(buff, 1000);
	//if (br == strlen(buff));

	if (br == sizeof(Command)) {
		buff[br] = '\0';

		Command *command;
		command = (Command*)buff;

		if (strcmp(command->name, "KILL") == 0) {
			if (strcmp(command->param, "OK") == 0) {
				((CProcessManagerDlg *)pDlg)->processKilledSuccess();
			}
			else if (strcmp(command->param, "ERROR") == 0) {
				((CProcessManagerDlg *)pDlg)->processKilledError();
			}
		}
	} else if (br == sizeof(ProcessInfo)) {
		buff[br] = '\0';

		ProcessInfo *info;
		info = (ProcessInfo*)buff;

		((CProcessManagerDlg *)pDlg)->AddMsg(info);
	}

	CSocket::OnReceive(nErrorCode);
}