
// MessengerClientDlg.h : header file
//

#pragma once
#include "ClientNetwork.h"

// CMessengerClientDlg dialog
class CMessengerClientDlg : public CDialog
{
// Construction
public:
	ClientNetwork& client;

	CMessengerClientDlg(ClientNetwork& _client,CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#if AFX_DESIGN_TIME
	enum { IDD = IDD_MESSENGERCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void CMessengerClientDlg::OnTimer(UINT_PTR event);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonsend();
};
