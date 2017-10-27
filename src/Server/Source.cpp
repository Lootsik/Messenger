#include "Server.h"
//таймеры

//отредачить

int main()
{
	boost::asio::io_service service;

	Server server{ service };
	
	server.StartAccept();
		
	service.run();


}