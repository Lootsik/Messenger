#include "Server.h"

//Do smth with debug and release env directories

//TODO:
//DANGER:

int main()
{

	boost::asio::io_service service;

	Server server{ service };
	
	if (!server.LoadFromConfig())
	{
		return 1;
	}

	MessengerEngine* engine = new MessengerEngine{ &server };
	server.SetMessageEngine(engine);
	if (!server.Start())
		return 1;
		
	service.run();

}