#ifdef WIN32
#define _WIN32_WINNT 0x0501
#endif
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
	//ip::udp::resolver resolver{ service };

	//ip::udp::resolver::query query(ip::udp::v4(), "127.0.0.1 ", "127.0.0.1");

	ip::udp::endpoint receiver_endpoint{ ip::address::from_string("127.0.0.1"),port };// *resolver.resolve(query);

	ip::udp::socket socket{ service };
	socket.open(ip::udp::v4());

	boost::array<char, 1> send_buf = { { 0 } };
	socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);
	boost::system::error_code err_code;

	boost::array<char, 128> recv_buf;
	ip::udp::endpoint sender_endpoint;
	size_t len = socket.receive_from(
		boost::asio::buffer(recv_buf), sender_endpoint,1,err_code);

	std::cout.write(recv_buf.data(), len);

}