#include <Network/Network.h>
#include <Database/Database.h>
#include <Logger/Logger.h>

const char* ConfigFilename = "MessengerServer.cfg";
const char* LogFilename = "Network.log";

int main()
{
#if _LOGGING_
	Logger::OpenLogFile(LogFilename);
#endif

	if (!Database::Connect("tcp://127.0.0.1:3306",  "messeger_server_db", "root", "11JustLikeYou11"))
	{
		//Log
		return 1;
	}
	
	boost::asio::io_service service;
	Network server{ service };

	if (!server.LoadFromConfig(ConfigFilename))
	{
		return 1;
	}

	MessengerEngine engine{ &server };
	if (!engine.LoadFromConfig(""))
		return 1;

	//engine.FillAccountInfo();


	server.SetMessageEngine(&engine);
	if (!server.Start())
		return 1;
		
	service.run();
}