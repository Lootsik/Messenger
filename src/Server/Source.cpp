#include "Server.h"
//�������

//����������

int main()
{
	boost::asio::io_service service;

	Server server{ service, 8021 };
	
	server.StartAccept();
		
	service.run();


}