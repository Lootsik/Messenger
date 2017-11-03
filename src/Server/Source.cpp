#include "Server.h"
//таймеры

//отредачить

int main()
{
	boost::asio::io_service service;

	Server server{ service, 8021 };
	
	server.StartAccept();
		
	service.run();


}