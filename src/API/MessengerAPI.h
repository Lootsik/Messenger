#pragma once

#include <Protocol\GlobalInfo.h>
#include <Protocol\TransferredData.h>
#include <string>

struct APIData;

namespace boost {
	namespace system {
		class error_code;
	};
};

class MessengerAPI
{
public:
	MessengerAPI();
	~MessengerAPI();

	bool Connect(const std::string& Address, unsigned short port);
	int TryLogin(const std::string& Login, const std::string& Pass);

	bool Ready();
	TransferredData* GetPacket();


	void GetUserLogin(ID_t Id);
	void LastMessageId(ID_t another);
	void LoadMessage(ID_t another, uint32_t index);
	void SendMessageTo(ID_t to, const std::wstring& Content );

	std::string GetCurrentUserLogin() const;
	ID_t GetCurrentUserID() const;

private:
	
	void _NewEvent(const boost::system::error_code& err_code, size_t bytes);
	void _WriteHandler(const boost::system::error_code& err_code, size_t bytes);
	bool _Working = false;
	APIData* _Data;

	std::string _UserLogin;
	ID_t _UserID;
};

