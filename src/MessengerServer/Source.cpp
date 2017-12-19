#include <iostream>
#include "Server.h"
#include "..\PacketFormat\Serialization.h"
#include "..\PacketFormat\Deserialization.h"


//TODO:
//DANGER:
//READABILITY:

int main()
{
	char* data = new char[MaxPacketSize];
	memset(data, 0, MaxPacketSize);
	std::string Login{ "Devid" };
	std::string Pass{ "228" };
	Serialization::MakePacketLogin(data, Login, Pass);

	std::string OutLogin;
	std::string OutPass;
	Deserialization::OnLogin(data, 256, OutLogin, OutPass);
	if (Login != OutLogin || OutPass != Pass)
		throw;

	memset(data, 0, MaxPacketSize);
	unsigned int from = 15, to = 228;
	Serialization::MakePacketMessage(data, from, to, "Shoot to thrill", 15);
	unsigned int ofrom = 0, oto = 0, mess_size = 0;	
	char* Mess = NULL;
	Deserialization::OnMessage(data, MaxPacketSize, ofrom, to, mess_size, Mess);
	
	std::cout << OutLogin << OutPass;

	boost::asio::io_service service;

	Server server{ service };
	
	server.SetPort(8021);
	MessengerEngine* engine = new MessengerEngine{ &server };
	server.SetMessageEngine(engine);
	server.StartAccept();
		
	service.run();
}