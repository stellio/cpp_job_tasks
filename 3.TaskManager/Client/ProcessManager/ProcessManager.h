
// ProcessManager.h : ������� ���� ��������� ��� ���������� PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"		// �������� �������


// CProcessManagerApp:
// � ���������� ������� ������ ��. ProcessManager.cpp
//

class CProcessManagerApp : public CWinApp
{
public:
	CProcessManagerApp();

// ���������������
public:
	virtual BOOL InitInstance();

// ����������

	DECLARE_MESSAGE_MAP()
};

extern CProcessManagerApp theApp;