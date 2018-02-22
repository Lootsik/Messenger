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

	int Connect(const std::string& Address, unsigned short port);
	void TryLogin(const std::string& Login, const std::string& Pass);

	bool TryGetMessage(); 
	//Connect to server
	//Authorization
	//Message
	std::string GetUserLogin() const;
	ID_t GetUserID() const;

	void(*_Callback)(uint32_t, TransferredData*);
private:
	
	void _NewEvent(const boost::system::error_code& err_code, size_t bytes);
	void _WriteHandler(const boost::system::error_code& err_code, size_t bytes);
	bool _Working = false;
	APIData* _Data;

	std::string _UserLogin;
	ID_t _UserID;
};

