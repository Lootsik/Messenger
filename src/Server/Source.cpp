//#define _WIN32_WINNT
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <map>
#include <string>
//�������� �� ������ ��������� ������
#include <vector>

using namespace boost::asio;
using boost::system::error_code;
boost::asio::io_service service;
const unsigned short port = 8021;



boost::asio::ip::tcp::acceptor acceptor{ service,ip::tcp::endpoint{ ip::tcp::v4(),port } };
const size_t buffsize = 128;

struct Client;

struct Account {
	Account(std::wstring& login, std::wstring& password)
		:_Login{ login }, _Password{ password } {}
	std::wstring _Login;
	std::wstring _Password;
	bool _Online = false;
	Client* _Client = nullptr;
	//+++++++++++
};
struct Client {
	Client() {}
	Account* _Accout = nullptr;
	bool _LoggedIn = false;
	ip::tcp::socket _Socket = ip::tcp::socket{ service };
	boost::array<char, buffsize> _Buff;
	//+++++++++++
};
struct Server {

	//vector<Accout> Accounts{Account{}}
	Server() : _Clients{} {
		//����� ������� �������� � �����, ��
		
		Accounts[L"Devid_nv"] = new Account{ std::wstring{L"Devid_nv"} ,std::wstring{L"1234"} };
		Accounts[L"Sanya228"] = new Account{ std::wstring{ L"Sanya228" } ,std::wstring{ L"3228" } };
	}

	//��� ������� ����� ������� ������ ��� ���-�� ������
	std::vector<Client*> _Clients;
	//� ������ ��� �������, �� ����� 
	std::map<std::wstring, Account*> Accounts;

	void Accept_message(Client* client, const boost::system::error_code& err_code,size_t bytes)
	{
	
		//������, ����� ������� �������
		if (err_code) {
			std::string err_mess = err_code.message();
			std::cout << err_code.category().name() << ':' << err_code.value() << '\n';

			boost::system::error_code err;
			client->_Socket.shutdown(ip::tcp::socket::shutdown_both,err);
			if (err) {
				std::cout << err.message() << '\n';
				assert(false);
			}
			client->_Socket.close();
			delete client;
			return;
		}
		//�������������� ���� ��� ���������
		std::cout.write(client->_Buff.data(), bytes);
		std::cout << '\n';
		client->_Socket.async_read_some(buffer(client->_Buff), boost::bind(&Server::Accept_message, this, client, _1, _2));
	}


	void Accept_clients(Client* client, const boost::system::error_code& err)
	{
		//����� ���������� � ����� ������� 
		std::cout << "New client: " << client->_Socket.remote_endpoint().address().to_string() << ':' << client->_Socket.remote_endpoint().port() << '\n';
		//���������� ����������� �������� ��� ��������� �������(�������, ����� ��������� �� ������� � ������ �������)
		client->_Socket.async_read_some(buffer(client->_Buff), boost::bind(&Server::Accept_message, this, client, _1, _2));
		//���������� ������� � ������ (��  ��������, �� ������ ��� ������ ��� ����������)
//����������� ����� 
		_Clients.push_back(client);
//����� ����������� �����

		//������ ������ �������
		Client* new_Client = new Client{};
	
		error_code error;
		//������������ ���������� ������ ��������
		acceptor.async_accept(new_Client->_Socket, boost::bind(&Server::Accept_clients, this, new_Client, _1), error);
	}


};
//�������
int main()
{
	Server server;
	
	Client* new_client = new Client{};
	error_code err_code;
	acceptor.async_accept(new_client->_Socket, boost::bind(&Server::Accept_clients, &server, new_client , _1));
		
	service.run();


}