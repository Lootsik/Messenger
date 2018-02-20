#pragma once
#include <string>

namespace pugi {
	class xml_node;
}

class ConfigParser
{
public:
	bool LoadConfig(const char* Filename);
	bool MakeDefaultConfig(const char* Filename);

	unsigned short GetPort() const { return _Port; }
	int GetTimeout() const { return _Timeout; }
	const std::string& GetHost() const { return _Host; }
	const std::string& GetSchema() const { return _Schema; }
	const std::string& GetLogin() const { return _Login; }
	const std::string& GetPassword() const { return _Password; }

private:
	bool _LoadServerCfg(const pugi::xml_node& server);
	bool _LoadDbCfg(const pugi::xml_node& Db);

	unsigned short _Port = 0;
	unsigned int _Timeout = 0;

	std::string _Host;
	std::string _Schema;
	std::string _Login;
	std::string _Password;
};