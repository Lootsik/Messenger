#include "Server.h"
//�������

//����������

int main()
{
	boost::asio::io_service service;

	Server server{ service };
	
	server.StartAccept();
		
	service.run();


}