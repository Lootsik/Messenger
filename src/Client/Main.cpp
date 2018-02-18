#include <API\MessengerAPI.h>
#include <Protocol\LoginResponse.h>
#include <Protocol\Types.h>
MessengerAPI api;
void CallBack(uint32_t type, TransferredData* Data)
{
	if (type == Types::UserInfo)
	{
		std::string str = api.GetUserLogin();
		printf("You are #%d:%s\n", api.GetUserID(), str.c_str());
	}
	//LoginResponse* response  = static_cast<LoginResponse*>(Data);
	//printf("Recived: ");
}

int main()
{
	if (api.Connect("127.0.0.1", 8021))
	{
		printf("Error");
		return 1;
	}
	api._Callback = CallBack;
	api.TryLogin("w", "w");
	
	while (1)
	{
		while (!api.TryGetMessage());
	}
}