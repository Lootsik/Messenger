#include "stdafx.h"
#include <string>


#include <Protocol\Types\LoginResponse.h>

#include <boost\asio.hpp>
#include "DialogRegistration.h"


DialogRegistration::DialogRegistration(MessengerAPI& api) :
								CDialogEx(IDDialogRegistration),
								API{ api }
{
}

void DialogRegistration::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, EditIP, IPEdit);

	IPEdit.SetAddress(127, 0, 0, 1);
	SetDlgItemInt(EditPort, 8021);
}

BEGIN_MESSAGE_MAP(DialogRegistration, CDialogEx)
	ON_BN_CLICKED(IDOK, &DialogRegistration::OnBnClickedOk)
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

void DialogRegistration::OnBnClickedOk()
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
	
	//TODO: проверить на допустимые знаки и буквы
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
	//TODO: rewrite somehow
	if (!connected ){
		auto IpString = boost::asio::ip::address_v4{ Address }.to_string();
		try {
			API.Connect(IpString, Port);
		}
		catch(const MessengerAPI::ConnectionRefused&){
			MessageBox((L"Connection refused " + std::wstring{ IpString.begin(), IpString.end() } + L':' + std::to_wstring(Port)).c_str(), L"Refused", 0);
			return;
		}
	}

	connected = true;


	try {
		switch (API.TryLogin(AnciLogin, AnciPass) )
		{
		case LoginResponse::Success:
			CDialogEx::OnOK();
			return;
			break;

		default:
			MessageBox(L"Wrong login or password", L"Error");
		}
	}
	catch (MessengerAPI::Disconnect)
	{
		MessageBox(L"Disconnected", L"Error");
	}
}
