#include "Server.h"
//�������

//����������

int main()
{
	boost::asio::io_service service;

	Server server{ service };
	
	server.SetPort(8021);
	MessengerEngine* engine = new MessengerEngine{ &server };
	server.SetMessageEngine(engine);
	server.StartAccept();
		
	service.run();
}