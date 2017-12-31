#include "Server.h"
#include "Logger.h"
//Do smth with debug and release env directories

//TODO:
//DANGER:
const char* ConfigFilename = "MessengerServer.cfg";
const char* LogFilename = "Server.log";

int main()
{
#if _STATE_MESSAGE_
	Logger::OpenLogFile(LogFilename);
#endif

	boost::asio::io_service service;

	Server server{ service };

	if (!server.LoadFromConfig(ConfigFilename))
	{
		return 1;
	}

	MessengerEngine engine{ &server };
	if (!engine.LoadFromConfig(""))
		return 1;

	engine.FillAccountInfo();


	server.SetMessageEngine(&engine);
	if (!server.Start())
		return 1;
		
	service.run();

}