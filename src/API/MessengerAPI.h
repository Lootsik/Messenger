#pragma once

#include <string>
#include <atomic>
#include <memory>

#include <Protocol\GlobalInfo.h>
#include <Protocol\TransferredData.h>
#include <Protocol\BaseHeader.h>

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


	//  throw ConnectionRefused
	bool Connect(const std::string& Address, unsigned short port);

	int TryLogin(const std::string& Login, const std::string& Pass);

	// throw Disconnect
	bool Ready();
	std::shared_ptr<TransferredData>  GetPacket();



	void GetUserLogin(ID_t Id);
	void LastMessageId(ID_t another);
	void LoadMessage(ID_t another, uint32_t index);
	void SendMessageTo(ID_t to, const std::wstring& Content);

	bool Process(const TransferredData* data);

	std::string GetCurrentUserLogin() const;
	ID_t GetCurrentUserID() const;

	void Quit();

	class Disconnect{};
	class ConnectionRefused {};

private:
	void _SetUserId(const ID_t id);
	void _SetUserLogin(const std::string& NewLogin);

	bool _Connected = false;
	bool _ConnectionDropped = false;

	APIData* _Data;

	bool _Authorized = false;
	std::string _UserLogin;
	ID_t _UserID;
};

