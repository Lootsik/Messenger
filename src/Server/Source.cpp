//#define _WIN32_WINNT
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
using namespace std;
using namespace boost::asio;
boost::asio::io_service service;
const unsigned short port = 8021;

int main()
{
	ip::udp::socket socket{ service, ip::udp::endpoint{ip::udp::v4(), port} };

	while (true) {
		boost::array<char, 1> buff;
		ip::udp::endpoint client_ep;
		boost::system::error_code err_code;
		
		socket.receive_from(buffer(buff), client_ep, 0, err_code);

		if (err_code && err_code != boost::asio::error::message_size)
			throw boost::system::system_error(err_code);

		socket.send_to(buffer("hello"), client_ep, 0, err_code);
		
		

	}



}