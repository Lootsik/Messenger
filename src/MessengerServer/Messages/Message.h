#pragma once
#include <string>
#include <GlobalInfo.h>

class Message
{
public:
	using StorageType = std::wstring;

private:
	ID_t _From;
	ID_t _To;
	ID_t _InChatId;
	//remove this later
	bool _Checked = false;
	StorageType _Message;
public:
	Message(ID_t from, ID_t to,ID_t in_chat_id, const std::wstring& message)
		:_From{ from }, _To{ to }, _InChatId{ in_chat_id },_Message {message} {}

	void SetChecked() { _Checked = true; }
	void SetUnchecked() { _Checked = false; }
	bool isChecked() const { return _Checked; }
	ID_t InChatId() const { return _InChatId; }
	ID_t From()const { return _From; }
	ID_t To()const { return _To; }
	const StorageType& GetBuffer() const { return _Message; }

	bool FromPacket(const BYTE* Buffer, uint32_t recived);
	bool ToPacket(BYTE* Buffer)const;
};

