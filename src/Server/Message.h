#pragma once
#define BUFFSIZE 256
#include <boost\array.hpp>
class Message
{
public:
	Message();
	using StorageType = boost::array<char, BUFFSIZE>;

	StorageType& Accept() {	return _MessageBuffer; }
	StorageType& Get() { return _MessageBuffer; }

	~Message();
private:
	StorageType _MessageBuffer;
	struct _MessageMarkup
	{
		//will be necessery to add some field there later
		char MessageType;
		//little endian for now TODO: make Big endian
		long unsigned int DataSize;
		
		char Data[1];
	};
};

