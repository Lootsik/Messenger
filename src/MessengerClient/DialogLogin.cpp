#include "stdafx.h"
#include "DialogLogin.h"
#include <string>

#include <Protocol\Types.h>
#include <Protocol\LoginResponse.h>
#include <boost\asio.hpp>
DialogLogin::DialogLogin(MessengerAPI& api) :
								CDialogEx(IDDialogLogin),
								API{ api }
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
	if (!connected && !API.Connect(boost::asio::ip::address_v4{ Address }.to_string(), Port))
	{
		MessageBox(L"Cannot connect", L"Error", 0);
		return;
	}

	connected = true;

	API.TryLogin(AnciLogin, AnciPass);

	
	for(int i = 0; i < 15;++i )
	{
		if (API.Ready()) 
		{
			TransferredData* Packet =  API.GetPacket();
			if (Packet->GetType() == Types::LoginResponse)
			{
				LoginResponse* Response = (LoginResponse*)Packet;
				if( Response->GetValue() == LoginResponse::Success)
					MessageBox(L"Succes", L"Result");
				else
					MessageBox(L"Error", L"Result");

			}
			break;
		}
		Sleep(200);
	}

	
	
	CDialogEx::OnOK();
}
