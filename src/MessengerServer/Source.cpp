#include <iostream>
#include "Server.h"
#include "Serialization.h"
#include "Deserialization.h"

int main()
{
	char* data = new char[256];
	memset(data, 0, 256);
	std::string Login{ "Devid" };
	std::string Pass{ "228" };
	Serialization::MakePacketLogin(data, Login, Pass);

	std::string OutLogin;
	std::string OutPass;
	Deserialization::OnLogin(data, 256, OutLogin, OutPass);
	if (Login != OutLogin || OutPass != Pass)
		throw;
	
	std::cout << OutLogin << OutPass;

	boost::asio::io_service service;

	Server server{ service };
	
	server.SetPort(8021);
	MessengerEngine* engine = new MessengerEngine{ &server };
	server.SetMessageEngine(engine);
	server.StartAccept();
		
	service.run();
}