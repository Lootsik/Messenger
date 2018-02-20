#include <Network\Network.h>
#include <Database\Database.h>
#include <Logger\Logger.h>
#include <Utility\ConfigParser.h>
#include <Windows.h>

const char* ConfigFilename = "Config.xml";
const char* LogFilename = "Server.log";

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
	HANDLE Mutex = CreateMutex(NULL, FALSE, L"{5f981c84-b348-40ac-8dee-8de13545e68f}");

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return 1;
	}

#if _LOGGING_
	Logger::OpenLogFile(LogFilename);
#endif

	ConfigParser Config;
	if (!Config.LoadConfig(ConfigFilename))
	{
		Logger::LogBoth(Logger::Error, 
				"Log file corrupted or contain wrong info, replaced by default, fill database info");
		
		Config.MakeDefaultConfig(ConfigFilename);
		return 3;
	}

	if (!Database::Connect(Config.GetHost(), Config.GetSchema(),
							Config.GetLogin(), Config.GetPassword()))
	{
		//Log
		return 2;
	}
	
	Network server{ service };
	if (!server.SetPort(Config.GetPort()))
		return 3;

	MessengerEngine engine{ &server };

	server.SetMessageEngine(&engine);
	if (!server.Start())
		return 1;
	
	if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
		Logger::Log(Logger::Error, "Could not set control handler");
		return 2;
	}

	service.run();

	CloseHandle(Mutex);
}