
// ProcessManager.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CProcessManagerApp:
// О реализации данного класса см. ProcessManager.cpp
//

class CProcessManagerApp : public CWinApp
{
public:
	CProcessManagerApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CProcessManagerApp theApp;