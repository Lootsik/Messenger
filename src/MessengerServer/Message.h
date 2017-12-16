#pragma once

#include <boost\array.hpp>
class Message
{
public:

	static const int Buffsize = 256;
	Message() {}
	using StorageType = boost::array<char, Buffsize>;

	StorageType& Buffer() {	return _MessageBuffer; }
	
	enum MessageType {
		message_ = 3,
		Login = 4,
		Logout = 5
	}; 
	//TODO: change this
	void SetType(MessageType type) {
		reinterpret_cast< _MessageMarkup*>(_MessageBuffer.c_array())->MessageType = (char)type;
	}
	MessageType Type() const {
		return MessageType(reinterpret_cast<const _MessageMarkup*>(_MessageBuffer.cbegin())->MessageType);
	}
	//its only size of data!!!!
	long unsigned int& DataSize() {
		return reinterpret_cast<_MessageMarkup*>(_MessageBuffer.c_array())->DataSize;
	}
	char* Data() {
		return reinterpret_cast<_MessageMarkup*>(_MessageBuffer.c_array())->Data;
	}
	

	~Message(){}

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

