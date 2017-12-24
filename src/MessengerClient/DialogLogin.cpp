#include "stdafx.h"
#include "DialogLogin.h"
#include <string>
#include "ClientNetwork.h"

DialogLogin::DialogLogin(ClientNetwork& _client) :
								CDialogEx(IDDialogLogin),
								client{_client}
{
}

void DialogLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, EditIP, IPEdit);

	IPEdit.SetAddress(127, 0, 0, 1);
	SetDlgItemInt(EditPort, 8021);
}

BEGIN_MESSAGE_MAP(DialogLogin, CDialogEx)
	ON_BN_CLICKED(IDOK, &DialogLogin::OnBnClickedOk)
END_MESSAGE_MAP()

bool UnicodeToAnci(const CString& Wstring, std::string& ANCIString)
{
	BOOL Res = false;
	int NeedSpace = WideCharToMultiByte(CP_ACP, 0, Wstring.GetString(), Wstring.GetLength(),
		NULL, 0, NULL, &Res);

	ANCIString.resize(NeedSpace);

	WideCharToMultiByte(CP_ACP, 0, Wstring.GetString(), Wstring.GetLength(),
		(LPSTR)(ANCIString.c_str()), NeedSpace, NULL, &Res);

	//all ok
	return Res == false;
}

void DialogLogin::OnBnClickedOk()
{
	CString Login;
	std::string AnciLogin;
	CString Pass;
	std::string AnciPass;
	
	DWORD  Address, Port;

	GetDlgItemText(EditLogin, Login);
	GetDlgItemText(EditPass, Pass);

	//ignore this for now
	Port = GetDlgItemInt(EditPort, 0, 0);
	IPEdit.GetAddress(Address);
	
	if (!UnicodeToAnci(Login, AnciLogin))
	{
		MessageBox(L"Only english letters", L"Error", 0);
		return;
	}
	if (!UnicodeToAnci(Pass, AnciPass))
	{
		MessageBox(L"Only english letters", L"Error", 0);
		return;
	}

	if (!connected && !client.Connect(ip::address_v4{ Address }, Port))
	{
		MessageBox(L"Cannot connect", L"Error", 0);
		return;
	}

	connected = true;
	
	if (!client.Authentication(AnciLogin, AnciPass))
	{
		MessageBox(L"Cannot Login", L"Error", 0);
		return;
	}
	
	CDialogEx::OnOK();
}
