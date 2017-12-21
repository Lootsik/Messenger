#include <iostream>
#include "Server.h"
#include "..\PacketFormat\Serialization.h"
#include "..\PacketFormat\Deserialization.h"

//Do smth with debug and release env directories

//TODO:
//DANGER:


int main()
{
	boost::asio::io_service service;

	Server server{ service };
	
	server.LoadFromConfig();

	MessengerEngine* engine = new MessengerEngine{ &server };
	server.SetMessageEngine(engine);
	server.Start();
		
	service.run();
}