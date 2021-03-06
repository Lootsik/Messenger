
// MessengerClientDlg.h : header file
//

#pragma once

#include <map>

#include "ChatContainer.h"

#include <API\MessengerAPI.h>

// CMessengerClientDlg dialog
class CMessengerClientDlg : public CDialog
{
// Construction
public:
	MessengerAPI& API;

	CMessengerClientDlg(MessengerAPI& api,CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MESSENGERCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void CMessengerClientDlg::OnTimer(UINT_PTR event);

	using Message_t = std::pair < ID_t, std::wstring>;
	
	void CMessengerClientDlg::OnUserClick();


	std::map<ID_t,ChatContainer> Chats;

	std::map<ID_t, std::string> Users;

	void ResetUserList();

	ID_t ChosenID;

	CListBox UsersListBox;
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
	afx_msg void OnBnClickedLogout();
};
