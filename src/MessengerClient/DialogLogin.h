#pragma once
#include "stdafx.h"
#include "resource.h"
#include "afxcmn.h"
class ClientNetwork;

class DialogLogin : public CDialogEx
{
public:
	DialogLogin(ClientNetwork& _client);
	ClientNetwork& client;
	bool connected = false;
	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDDialogLogin};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

														// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CIPAddressCtrl IPEdit;
};
