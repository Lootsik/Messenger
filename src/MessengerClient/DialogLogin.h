#pragma once
#include "stdafx.h"
#include "resource.h"

class DialogLogin : public CDialogEx
{
public:
	DialogLogin();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = ResDialogLogin };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

														// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};
