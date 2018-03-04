#pragma once

#include <Protocol\GlobalInfo.h>
#include <Protocol\TransferredData.h>
#include <Protocol\BaseHeader.h>
#include <string>
#include <atomic>
#include <memory>
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

	bool Connected() const {
		return _Connected;
	}

	bool Connect(const std::string& Address, unsigned short port);
	int TryLogin(const std::string& Login, const std::string& Pass);

	bool Ready();
	BaseHeader* GetPacket();



	void GetUserLogin(ID_t Id);
	void LastMessageId(ID_t another);
	void LoadMessage(ID_t another, uint32_t index);
	void SendMessageTo(ID_t to, const std::wstring& Content);

	bool Process(const TransferredData* data);

	std::string GetCurrentUserLogin() const;
	ID_t GetCurrentUserID() const;

	class Disconnect{};

private:
	void _SetUserId(const ID_t id);
	void _SetUserLogin(const std::string& NewLogin);

	bool _Working = false;
	bool _Connected = false;
	std::atomic<bool> _Kicked = false;
	APIData* _Data;


	std::string _UserLogin;
	ID_t _UserID;
};

