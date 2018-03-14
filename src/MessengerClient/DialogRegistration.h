#pragma once
#include "stdafx.h"
#include "resource.h"
#include "afxcmn.h"

#include <API\MessengerAPI.h>

class ClientNetwork;

class DialogRegistration : public CDialogEx
{
public:
	DialogRegistration(MessengerAPI& api);
	bool connected = false;
	MessengerAPI& API;
	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDDialogRegistration};
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
