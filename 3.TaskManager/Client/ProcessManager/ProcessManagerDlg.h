
// ProcessManagerDlg.h : ���� ���������
//

#include "ClientSocket.h"
#include "ProcessInfo.h"
#include "Command.h"
#pragma once
#include "afxcmn.h"


// ���������� ���� CProcessManagerDlg
class CProcessManagerDlg : public CDialogEx
{
// ��������
public:

	ClientSocket m_Client;
	void AddMsg(ProcessInfo *info);
	void processKilledSuccess();
	void processKilledError();
	void updateProcessId(CString v);
	void updateProcessList();
	CString intToCString(int v);
	CProcessManagerDlg(CWnd* pParent = NULL);	// ����������� �����������

// ������ ����������� ����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESSMANAGER_DIALOG };
#endif
	CString	m_sIPAddress;
	int m_ProcessID;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// ��������� DDX/DDV


// ����������
protected:
	HICON m_hIcon;

	// ��������� ������� ����� ���������
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_List;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedConnectBtn();
	afx_msg void OnBnClickedDisonectBtn();
	afx_msg void OnBnClickedButton3();
	afx_msg void GetProcessList();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedKillProcessBtn();
	afx_msg void OnEnChangeIpAddressEdit();
};
