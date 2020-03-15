
// ProcessManagerDlg.cpp : файл реализации
//

#include "stdafx.h"
#include "ProcessManager.h"
#include "ProcessManagerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// диалоговое окно CProcessManagerDlg



CProcessManagerDlg::CProcessManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PROCESSMANAGER_DIALOG, pParent)
{
	m_sIPAddress = _T("127.0.0.1");
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Text(pDX, IDC_IP_ADDRESS_EDIT, m_sIPAddress);
	DDX_Text(pDX, IDC_PROCESS_ID_EDIT, m_ProcessID);
}

BEGIN_MESSAGE_MAP(CProcessManagerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT_BTN, &CProcessManagerDlg::OnBnClickedConnectBtn)
	ON_BN_CLICKED(IDC_DISONECT_BTN, &CProcessManagerDlg::OnBnClickedDisonectBtn)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CProcessManagerDlg::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_KILL_PROCESS_BTN, &CProcessManagerDlg::OnBnClickedKillProcessBtn)
	ON_EN_CHANGE(IDC_IP_ADDRESS_EDIT, &CProcessManagerDlg::OnEnChangeIpAddressEdit)
END_MESSAGE_MAP()


// обработчики сообщений CProcessManagerDlg

BOOL CProcessManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	// Ask Mfc to create/insert a column
	     // Width of items under header

	m_List.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 150);
	m_List.InsertColumn(2, _T("ID"), LVCFMT_LEFT, 100);
	m_List.InsertColumn(3, _T("Threads"), LVCFMT_LEFT, 100);
	m_List.InsertColumn(4, _T("Memory, Kb"), LVCFMT_LEFT, 100);
	m_List.InsertColumn(5, _T("Path"), LVCFMT_LEFT, 500);

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CProcessManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CProcessManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CProcessManagerDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: добавьте свой код обработчика уведомлений
	*pResult = 0;
}


void CProcessManagerDlg::OnBnClickedConnectBtn()
{
	if (AfxSocketInit() == FALSE)
	{
		AfxMessageBox(_T("Failed to Initialize Sockets"));
		return;
	}

	if (m_Client.Create() == FALSE)
	{
		AfxMessageBox(_T("Failed to Create Socket"));
		return;
	}

	if (m_Client.Connect(m_sIPAddress, 9292) == FALSE)
	{
		AfxMessageBox(_T("Failed Connect to: " + m_sIPAddress + "[9292]"));
		m_Client.Close();
		return;
	}
	m_Client.pDlg = this;

	GetProcessList();
	GetDlgItem(IDC_DISONECT_BTN)->EnableWindow(true);
	GetDlgItem(IDC_CONNECT_BTN)->EnableWindow(false);
}


void CProcessManagerDlg::OnBnClickedDisonectBtn()
{
	// TODO: добавьте свой код обработчика уведомлений
	m_Client.ShutDown(2);
	m_Client.Close();

	GetDlgItem(IDC_DISONECT_BTN)->EnableWindow(false);
	GetDlgItem(IDC_CONNECT_BTN)->EnableWindow(true);
}


void CProcessManagerDlg::GetProcessList()
{
	Command *command = new Command();
	strcpy_s(command->name, "GET_LIST");

	m_List.DeleteAllItems();
	m_Client.Send((char*)command, sizeof(Command), 0);
}

void CProcessManagerDlg::AddMsg(ProcessInfo *info) {

	int nItem;
	nItem = m_List.InsertItem(0, CString(info->name));
	m_List.SetItemText(nItem, 1, intToCString(info->id));
	m_List.SetItemText(nItem, 2, intToCString(info->threds));
	m_List.SetItemText(nItem, 3, intToCString(info->memory));
	m_List.SetItemText(nItem, 4, CString(info->path));
}

void CProcessManagerDlg::processKilledSuccess() {
	AfxMessageBox(_T("Process Kill Successfully"));
	updateProcessList();
}

void CProcessManagerDlg::updateProcessList() {
	if (m_Client.ShutDown(2)) {
		m_Client.Close();
		m_Client.Create();

		if (m_Client.Connect(m_sIPAddress, 9292)) {
			Command *command = new Command();
			strcpy_s(command->name, "GET_LIST");

			m_List.DeleteAllItems();
			m_Client.Send((char*)command, sizeof(Command), 0);
		}
	}
}

void CProcessManagerDlg::processKilledError() {
	AfxMessageBox(_T("Process Kill Error"));
}

void CProcessManagerDlg::updateProcessId(CString v) {

	int id = _wtoi(v);

	if (id > 0) {
		m_ProcessID = id;
		GetDlgItem(IDC_KILL_PROCESS_BTN)->EnableWindow(true);
		GetDlgItem(IDC_PROCESS_ID_EDIT)->SetWindowTextW(intToCString(id));
	}
}

CString CProcessManagerDlg::intToCString(int v) {
	CString str;
	str.Format(_T("%d"), v);
	return str;
}

void CProcessManagerDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED)) {
		POSITION pos = m_List.GetFirstSelectedItemPosition();
		if (pos != NULL) {
			int index = m_List.GetNextSelectedItem(pos);
			updateProcessId(m_List.GetItemText(index, 1));
		}
	}
	*pResult = 0;
}

void CProcessManagerDlg::OnBnClickedKillProcessBtn()
{
	if (m_Client.ShutDown(2)) {
		m_Client.Close();
		m_Client.Create();

		if (m_Client.Connect(m_sIPAddress, 9292)) {
			Command *command = new Command();

			char param[11];
			sprintf_s(param, "%d", m_ProcessID);

			strcpy_s(command->name, "KILL");
			strcpy_s(command->param, param);
			m_Client.Send((char*)command, sizeof(Command), 0);
		}
	}
}


void CProcessManagerDlg::OnEnChangeIpAddressEdit()
{
	GetDlgItem(IDC_IP_ADDRESS_EDIT)->GetWindowTextW(m_sIPAddress);
}
