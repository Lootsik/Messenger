#include "ConfigParser.h"
#include "pugixml.hpp"
#include <climits>
// true if val is between values left and right
template <typename T>
bool __Between(T val, T left, T right)
{
	return left <= val && val <= right;
}
template <typename T>
bool Between(T val, T left, T right)
{
	if (left > right)
		return __Between(val, right, left);

	return __Between(val, left, right);
}

static const char* Value = "value";
static const int WRONG_VALUE = -1;

//TODO: add more such constants
static const char* PortField = "port";
static const char* TimeoutField = "timeout";


bool ConfigParser::LoadConfig(const char* Filename)
{
	pugi::xml_document doc;

	if (!doc.load_file(Filename))
		return false;

	pugi::xml_node node = doc.child("settings");

	return _LoadServerCfg(node.child("server")) &&
				_LoadDbCfg(node.child("database"));

}



bool ConfigParser::_LoadServerCfg(const pugi::xml_node& server)
{
	pugi::xml_node atr = server.child(PortField);
	int TmpPort = atr.attribute(Value).as_int(WRONG_VALUE);

	atr = atr.next_sibling(TimeoutField);
	int TmpTimeout = atr.attribute(Value).as_int(WRONG_VALUE);

	//check, are values are acceptable
	if (!Between(TmpPort, 0, USHRT_MAX))
		return false;

	if (TmpTimeout < 0)
		return false;

	_Port = TmpPort;
	_Timeout = TmpTimeout;
	
	return true;
}



bool ConfigParser::_LoadDbCfg(const pugi::xml_node& Db)
{
	pugi::xml_node atr = Db.child("hostname");
	_Host = atr.attribute(Value).value();

	atr = atr.next_sibling("schema");
	_Schema = atr.attribute(Value).value();

	atr = atr.next_sibling("login");
	_Login = atr.attribute(Value).value();

	atr = atr.next_sibling("password");
	_Password = atr.attribute(Value).value();

	return !(_Host.empty() || _Schema.empty()
			|| _Login.empty() || _Password.empty());
}

bool ConfigParser::MakeDefaultConfig(const char* Filename)
{
	pugi::xml_document doc;

	//fist node
	pugi::xml_node settings = doc.append_child("settings");
	//subnode
	pugi::xml_node server = settings.append_child("server");

	//append values
	server.append_child(PortField).append_attribute(Value) = 8021;
	server.append_child(TimeoutField).append_attribute(Value) = 50000;

	//subnode to settings
	pugi::xml_node database = settings.append_child("database");

	//append values
	database.append_child("hostname").append_attribute(Value) = "";
	database.append_child("schema").append_attribute(Value) = "";
	database.append_child("login").append_attribute(Value) = "";
	database.append_child("password").append_attribute(Value) = "";
	
	return doc.save_file(Filename);
}