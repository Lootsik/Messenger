#include <Server/Server.h>
#include <Database/Database.h>
#include <Logger/Logger.h>

//Do smth with debug and release env directories

//TODO:
//DANGER:
const char* ConfigFilename = "MessengerServer.cfg";
const char* LogFilename = "Server.log";

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
	Server server{ service };

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