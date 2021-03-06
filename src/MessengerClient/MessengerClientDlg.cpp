
// MessengerClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MessengerClient.h"
#include "MessengerClientDlg.h"
#include "afxdialogex.h"
#include "DialogLogin.h"

#include <Protocol\Types.h>

#include <vector>

#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using UserInfo_t = std::pair<ID_t, std::string>;


static void *THISP;
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMessengerClientDlg dialog



CMessengerClientDlg::CMessengerClientDlg(MessengerAPI& api,CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MESSENGERCLIENT_DIALOG, pParent),
	 API{ api }
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMessengerClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, USERS_LIST_BOX, UsersListBox);
}

BEGIN_MESSAGE_MAP(CMessengerClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ButtonSend, &CMessengerClientDlg::OnBnClickedButtonsend)
	ON_WM_TIMER()
	// list click
	ON_LBN_SELCHANGE(USERS_LIST_BOX, &CMessengerClientDlg::OnUserClick)
	ON_BN_CLICKED(ID_LOGOUT, &CMessengerClientDlg::OnBnClickedLogout)
END_MESSAGE_MAP()


// CMessengerClientDlg message handlers


void CMessengerClientDlg::OnUserClick()
{
	SetDlgItemText(EDIT_MESSAGES, L"");


	UINT Chosen = UsersListBox.GetCurSel();

	if (Chosen >= Users.size())
		return;

	// check write messages
	auto Iter = Users.begin();
	std::advance(Iter, Chosen);

	auto Found = Chats.find( Iter->first);
	
	// save ID
	ChosenID = Iter->first;
	
	
	if (Found == Chats.end())
		return;


	//TODO: divide this when get chat class



	const auto& ChosenChat = Found->second;
	std::wostringstream stream;
	const wchar_t* end = L"\r\n";


	// Get Current user Info
	std::wstring ThisUserLogin;

	{
		std::string LoginANCI= API.GetCurrentUserLogin();
		ThisUserLogin = { LoginANCI.begin() , LoginANCI.end() };
	}

	//getting another user info
	ID_t AnotherID = Found->first;
	std::wstring AnotherLogin{ Users[AnotherID].begin(),Users[AnotherID ].end()};


	

	ID_t ThisUserID = API.GetCurrentUserID();

	size_t messageCount = ChosenChat.MessCount;
	for (size_t i = 0; i < messageCount; ++i)
	{
		if (ChosenChat.Messages[i].first == INVALID_ID)
		{
			continue;
		}
		if (ChosenChat.Messages[i].first == ThisUserID)
			stream << ThisUserLogin;
		else
			stream << AnotherLogin;
		
		stream << L": " << ChosenChat.Messages[i].second << end;;
	}

	SetDlgItemText(EDIT_MESSAGES, stream.str().c_str());
}




void CMessengerClientDlg::OnTimer(UINT_PTR event)
{
	try {
		while (API.Ready())
		{
			auto packet = API.GetPacket();

			switch (packet->GetType())
			{
			case Types::UserInfo:
			{
				auto UserInf = static_cast<UserInfo*>(packet.get());

				ID_t user_id = UserInf->GetId();
				if (UserInf->GetLogin() != "")
				{
					// save this
					Users[user_id] = UserInf->GetLogin();

					// request 
					API.LastMessageId(user_id);

					ResetUserList();
				}
			}break;

			case Types::LastMessageResponse:
			{
				auto LastMessage = static_cast<LastMessageResponse*>(packet.get());

				ID_t last_message = LastMessage->GetMessageId();
				ID_t userId = LastMessage->GetAnotherId();

				if (last_message != 0)
				{
					Chats[userId].another = userId;
					Chats[userId].MessCount = last_message;

					Chats[userId].Messages.resize(last_message);

					while (last_message != 0)
					{
						API.LoadMessage(userId, last_message);
						last_message--;
					}
				}
			}break;

			case Types::Message:
			{
				auto mess = static_cast<Message*>(packet.get());

				if (mess->Sender() == 0)
					break;

				ID_t another = 0;

				if (mess->Sender() == API.GetCurrentUserID())
					another = mess->Receiver();
				else
					another = mess->Sender();

				if (mess->MessageIndex() != 0)
				{
					if (mess->MessageIndex() == Chats[another].MessCount + 1)
					{
						//new message 
						Chats[another].MessCount++;
						Chats[another].Messages.resize(Chats[another].MessCount);
						Chats[another].AddMessage(mess->MessageIndex(), mess->Sender(), mess->Content());
					}
					else if (mess->MessageIndex() <= Chats[another].MessCount)
						Chats[another].AddMessage(mess->MessageIndex(), mess->Sender(), mess->Content());
				}
				else
					OutputDebugString((std::wstring{ L"Failed message Sender Receiver index " } +std::to_wstring(mess->Sender()) +
						L' ' + std::to_wstring(mess->Receiver()) + L' ' + std::to_wstring(mess->MessageIndex() + L'\n')).c_str());



				// redraw messages
				if (mess->Sender() == ChosenID || mess->Receiver() == ChosenID)
					OnUserClick();

			}break;

			default:

				break;
			}
		}
	}
	catch (const MessengerAPI::Disconnect&)
	{
		EndDialog(IDOK);
		KillTimer(228);
		MessageBox(L"Disconnected", L"Error");
	}
}
void CMessengerClientDlg::ResetUserList()
{
	UsersListBox.ResetContent();
	unsigned i = 0;
	for (auto& x : Users)
	{
		UsersListBox.InsertString(i, std::wstring{ x.second.begin(), x.second.end() }.c_str());
		UsersListBox.SetItemHeight(i, 20);
		++i;
	}
}

BOOL CMessengerClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CString IdCapture;
	IdCapture.Format(L"Your ID: %d : %s", API.GetCurrentUserID(), API.GetCurrentUserLogin().c_str() );
	SetDlgItemText(YourID, IdCapture);


	//std::vector<UserInfo_t> Users;

	for (int i = 0; i < 20; ++i)
	{
		API.GetUserLogin(i);
	}

	UINT_PTR res = SetTimer(228, 300,NULL);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMessengerClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMessengerClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMessengerClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMessengerClientDlg::OnBnClickedButtonsend()
{
	if (ChosenID == INVALID_ID)
		return;

	auto ChatIter = Chats.find(ChosenID);

	if (ChatIter == Chats.end())
	{
		Chats[ChosenID].another = ChosenID;
	}


	CString MessageContent;
	
	GetDlgItemText(NEW_MESSAGE_EDIT, MessageContent);
	if (MessageContent.IsEmpty())
		return;

	API.SendMessageTo(ChosenID, std::wstring{ MessageContent.GetString() });

	SetDlgItemText(NEW_MESSAGE_EDIT, L"");
}


void CMessengerClientDlg::OnBnClickedLogout()
{
	CDialog::OnOK();
}
