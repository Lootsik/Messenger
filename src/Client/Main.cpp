#include <API\MessengerAPI.h>
#include <Protocol\LoginResponse.h>

void CallBack(uint32_t type, TransferredData* Data)
{
	LoginResponse* response  = static_cast<LoginResponse*>(Data);
	printf("Response: %d:%d", response->GetValue(), response->GetId());
}



int main()
{
	MessengerAPI api;
	if (api.Connect("127.0.0.1", 8021))
	{
		printf("Error");
		return 1;
	}
	api._Callback = CallBack;
	api.TryLogin("w", "w");
	while (!api.TryGetMessage());
	while (1);
}