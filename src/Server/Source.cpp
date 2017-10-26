//#define _WIN32_WINNT

#define _STATE_MESSAGE_


#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <map>
#include <string>
#include <algorithm>
//заменить на другую структуру данных
#include <vector>

using namespace boost::asio;
using boost::system::error_code;
boost::asio::io_service service;
const unsigned short port = 8021;

//формат сообщения 
//2 символа - метка типа сообщения,  начинаеться с '-'
//данные, которые соответствуют типу сообщения
//нужно изменить формат когда-то

boost::asio::ip::tcp::acceptor acceptor{ service,ip::tcp::endpoint{ ip::tcp::v4(),port } };
const size_t buffsize = 128;

struct Client;

struct Account {
	Account(std::string& login, std::string& password)
		:_Login{ login }, _Password{ password } {}
	std::string _Login;
	std::string _Password;
	bool _Online = false;
	Client* _Client = nullptr;
	//+++++++++++
};
struct Client {
	Client() {}
	Account* _Account = nullptr;
	bool _LoggedIn = false;
	ip::tcp::socket _Socket = ip::tcp::socket{ service };
	boost::array<char, buffsize> _Buff;
	//+++++++++++
	//если добавлять больше 1 потока, тут нужен будет мутекс
};
std::ostream& operator<<(std::ostream& os, const Client& client)
{
	//в зависимости от того, ввошёл ли клиент в аккаунт, выводим либо логин, либо адрес
	if (client._LoggedIn)
		os << client._Account->_Login;
	else
		os << client._Socket.remote_endpoint().address().to_string() << ':'
		<< client._Socket.remote_endpoint().port();

	return os;
}

struct Server {
	class ClientInAcc {};
	class AccountAlreadyOnline{};
	class WrongPassword {};
	class WrongLogin {};
	
	Server() : _Clients{} {
		//позже сделать загрузку с файла, дб

		Accounts["Devid_nv"] = new Account{ std::string{"Devid_nv"} ,std::string{"1234"} };
		Accounts["Sanya228"] = new Account{ std::string{ "Sanya228" } ,std::string{ "3228" } };
	}

	//для клиетов лучше сделать список или что-то другое
	std::vector<Client*> _Clients;
	//в идеале хеш таблица, но потом 
	std::map<std::string, Account*> Accounts;


	void Login(Client* client, const std::string& entered_login, const std::string& entered_password)
	{
		auto res = Accounts.find(entered_login);
		//не нашли
		if (res == Accounts.end()) {
			throw WrongLogin{};
		}
		//аккаунт найден
		auto account = res->second;
		
		//клиент не может быть подключён к 2 учётным записям одновременно
		if (client->_LoggedIn)
			throw ClientInAcc{};

		//пока что к одному аккаунту может быть подключён только 1 клиент
		if (account->_Online) {
			throw AccountAlreadyOnline{};
		}

		//найдено, проверяем пароль
		if (_PasswordCheck(res->second, entered_login, entered_password)) {
			//"входим в аккаунт"
			client->_Account = account;
			account->_Client = client;

			client->_LoggedIn = true;
			account->_Online = true;

		}
		else
			//неправильный пароль
			throw WrongPassword{};
	}

	void Logout(Client* client)
	{
#ifdef _STATE_MESSAGE_
		std::cout << client->_Account->_Login << "- logged out\n";
#endif // _STATE_MESSAGE_
		//возможно, когда-то стоит изменить последовательность
		client->_Account->_Client = nullptr;
		client->_Account->_Online = false;
		client->_Account = nullptr;

		client->_LoggedIn = false;
	}

	//обработка сообщения
	void ProcessMessage(Client* client, size_t size)
	{
#ifdef _STATE_MESSAGE_

		std::cout << *client << " : Message\n[";
		std::cout.write(client->_Buff.data(), size);
		std::cout << "]\n";

#endif // _STATE_MESSAGE_

		auto& message = client->_Buff;
		
		//достаточно маленькое, чтобы считать неправильным
		if (size < 2)
			return;

		//если первый символ не '-' - неверный формат сообщения
		if (message[0] == '-') {
			switch (message[1])
			{
			case 'a':
				//логинимся
				_LoginMessageProcess(client, size);
				break;

			case 'q':
				//выходим с аккаунта
				Logout(client);
				break;

			case 'm':
				//просто сообщение 
				break;
				
			default:
				break;
			}
		}

	}

	void AcceptMessage(Client* client, const boost::system::error_code& err_code,size_t bytes)
	{
		//ошибка, нужно удалить клиента
		if (err_code) {
			_EraseClient(client, 0);
			return;
		}
		
		//обработка сообщения
		ProcessMessage(client, bytes);
		//регистрируем следующее сообщение
		client->_Socket.async_read_some(buffer(client->_Buff), boost::bind(&Server::AcceptMessage, this, client, _1, _2));
	}


	void AcceptClients(Client* client, const boost::system::error_code& err)
	{
		//вывод информации о новом клиенте 
		std::cout << "New client: " << client->_Socket.remote_endpoint().address().to_string() << ':' << client->_Socket.remote_endpoint().port() << '\n';
		//добавление асинхронной операции для принятого клиента(страшно, нужно упростить мб вывести в другую функцию)
		client->_Socket.async_read_some(buffer(client->_Buff), boost::bind(&Server::AcceptMessage, this, client, _1, _2));
		//добавление клиента в список (не  критично, тк вызовы для сокета уже забинджены)

		_Clients.push_back(client);

		//создаём нового клиента
		Client* new_Client = new Client{};
	
		error_code error;
		//регистрируем обработчик нового коннекта
		acceptor.async_accept(new_Client->_Socket, boost::bind(&Server::AcceptClients, this, new_Client, _1), error);
	}

private:
	//способ проверки можно изменить						оставляю для дб
	bool _PasswordCheck(const Account* account, const std::string& entered_login,const std::string& entered_password)
	{
		//сравниваем
		return account->_Password == entered_password;
	}

	//можно добавить причину
	void _EraseClient(Client* client, const int cause)
	{
#ifdef _STATE_MESSAGE_
		std::cout << *client << " - disconnected\n";
#endif // _STATE_MESSAGE


		error_code err;
		//останавливаем все операции чтения-записи для сокета
		client->_Socket.shutdown(ip::tcp::socket::shutdown_both, err);
		//assert нужно заменить
		//пока не знаю как правильно обрабатывать такие ситуации
		assert(!err);

		//выходим с аккаунта
		if(client->_LoggedIn)
			Logout(client);

		//ищем клиента в списке клиетов
		auto res = find(_Clients.begin(), _Clients.end(), client);
		//если этого клиента в списке нет, что-то пошло не так
		assert(res != _Clients.end());
		
		//удаляем клиента со списка
		_Clients.erase(res);

		//закрываем сокет
		client->_Socket.close();
		delete client;
		return;
	}

	void _LoginMessageProcess(Client* client, const size_t size)
	{
		std::string message(client->_Buff.c_array(), client->_Buff.c_array() + size);

		//в начале -a - знак авторизации
		auto find_res = message.find("-a");
		//string::npos - не нашло

		if (find_res == 0) {
			size_t space_after_login = message.find(" ", 4);
			//не нашло
			if (space_after_login == std::string::npos)
				return;
			std::string entered_login{ message.begin() + 3 , message.begin() + space_after_login };

			size_t space_after_pass = message.find(" ", space_after_login + 1);
			//не нашло
			if (space_after_pass == std::string::npos)
				return;
			std::string entered_password{ message.begin() + space_after_login + 1,message.begin() + space_after_pass };

			//логин
			try
			{
				//логинимся
				Login(client, entered_login, entered_password);
				std::cout << client->_Account->_Login << " - online\n";
			}
			catch (...) {
				//... это всё можно обработать и послать, например, клиенту подсказки
#ifdef _STATE_MESSAGE_	
				std::cout << *client << " - Wrong login or password\n";
#endif
			}

		}
	}
};
//таймеры
int main()
{
	Server server;
	
	Client* new_client = new Client{};
	error_code err_code;
	acceptor.async_accept(new_client->_Socket, boost::bind(&Server::AcceptClients, &server, new_client , _1));
		
	service.run();


}