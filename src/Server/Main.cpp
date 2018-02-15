#include <Network/Network.h>
#include <Database/Database.h>
#include <Logger/Logger.h>
#include <Windows.h>
const char* ConfigFilename = "MessengerServer.cfg";
const char* LogFilename = "Network.log";

boost::asio::io_service service;


BOOL WINAPI consoleHandler(DWORD signal) 
{
	if (signal == CTRL_C_EVENT)
	{
		boost::system::error_code err;
		service.stop();
		Logger::Log(Logger::Success, "Server stopped");
	}
	return TRUE;
}


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
	
	if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
		Logger::Log(Logger::Error, "Could not set control handler");
		return 2;
	}

	service.run();
}