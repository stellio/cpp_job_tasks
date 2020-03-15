#if !defined(AFX_CLIENTSOCKET_H__7CFF06D0_14C3_427B_92A4_2DD2AF08D14E__INCLUDED_)
#define AFX_CLIENTSOCKET_H__7CFF06D0_14C3_427B_92A4_2DD2AF08D14E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientSocket.h : header file
//

#include "Afxsock.h"

// конечный объект команды ClientSocket

class ClientSocket : public CSocket
{
public:
	ClientSocket();
	virtual ~ClientSocket();

	virtual void OnReceive(int nErrorCode);

	CDialog * pDlg;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSOCKET_H__7CFF06D0_14C3_427B_92A4_2DD2AF08D14E__INCLUDED_)