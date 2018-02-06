#if !defined(TRANSFERABLETEMPLATE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#define TRANSFERABLETEMPLATE_H
#include <GlobalInfo.h>



namespace SerializableTypes {

enum MessageTypes {
	NotType = 0,
	TextMessage = 3,
	AuthenticationData = 4,
	AuthenticationResult = 5,
	Logout = 6,
	UncheckedEvent
};
}
//in derived classes must be inmplemented
//_DataSize, _FromBuffer, ToBuffer methods

// _FromBuffer in derived need to check, valid his part of packet or not
// buffer passed to _ToBuffer already have needed capacity, dont need checksnames
template<typename T, uint16_t MessageTypeID>
class Serializable {
	//TODO: data below almost uselles, mb i should delete it???

    uint16_t _Type = MessageTypeID;
	
	//we actually need this only when load/store, 
	//when we store, we rewrite this
	//mubtable for now, coz i want ToBuffer const
    //delete or rework later
	mutable uint16_t _AdditionalDataSize;
    
    struct Marking
    {
        uint16_t Type;
        //Only data field size!
        uint16_t DataSize;
        alignas(4) BYTE Data[0];
    };
	static const size_t HeaderSize = sizeof(Marking);
    //size checks
	//recived equal to data and header
    static bool _InputSizeCheck(const uint16_t FullSize, const uint16_t AdditionalDataSize);
    //data fit i buffer
	static bool _OutputSizeCheck(const size_t DataSize);
   
    //header to buffer and back
    BYTE* _GetAdditionalData(const BYTE* HeaderBegin)const{
        return ((Marking*)HeaderBegin)->Data;
    }
    //opearations with header
    void _InputHeader(const Marking* Buffer);
    void _OutputHeader(Marking* Buffer) const;
public:
    //Getters
    uint16_t GetType() const { return _Type; }
    uint16_t AdditionalDataSize() const { return _AdditionalDataSize; }
	//get type of variable in buffer
    static uint16_t GetSerializedType(const BYTE* HeaderBegin){
        return ((Marking*)HeaderBegin)->Type;
    }
	//check, that buffer with this size can fit data
	static bool BasickSizeCheck(size_t size) {
		return size >= HeaderSize;
	}
	
	enum Types {
		NotType = 0,
		Message = 3,
		Login = 4,
		LoginResult = 5,
		Logout = 6,
		UncheckedEvent
	};
	//size, needed to pack to buffer
	bool GetSerializedSize()const {
		return HeaderSize + static_cast<const T*>(this)->_DataSize();
	}
    //Serialization/deserialization   
    bool FromBuffer(const BYTE* Buffer, uint32_t recived);
    bool ToBuffer(BYTE* Buffer)const;    
};


namespace SerializableTypes {
	//just easy to write
	class Types : public Serializable<Types, 0> {
	public:
		static bool InitialSizeCheck(size_t size) {
			return BasickSizeCheck(size);
		}
		static uint16_t SerializedType(const BYTE* Buffer) {
			return GetSerializedType(Buffer);
		}

		size_t _DataSize()const { return 0; }
		bool _FromBuffer(const BYTE* Buffer, uint32_t recived) { return true; }
		bool _ToBuffer(BYTE* Buffer)const { return true; }
	};
}




/*
    Serialization/deserialization   
 */
template<typename T,uint16_t Type>
bool Serializable<T,Type>::FromBuffer(const BYTE* Buffer, uint32_t recived)
{
    const Marking* marking = (Marking*)Buffer;
    //checks 
    if( ! _InputSizeCheck(recived, marking->DataSize))
        return false;

    //input header first
    _InputHeader(marking);
    //then other data
    return static_cast<T*>(this)->_FromBuffer(_GetAdditionalData(Buffer), AdditionalDataSize());
}

template<typename T,uint16_t Type>
bool Serializable<T, Type>::ToBuffer( BYTE* Buffer) const
{
    Marking* marking = (Marking*)Buffer;
    //data size
    size_t DataSize = static_cast<const T*>(this)->_DataSize();
    //check
    if(! _OutputSizeCheck(DataSize))
        return false;

    //set the size
    _AdditionalDataSize = DataSize;
    //output header
    _OutputHeader(marking);

    return static_cast<const T*>(this)->_ToBuffer(_GetAdditionalData(Buffer));
}

/*
            Checks
 */

//simple size checks on input
template<typename T, uint16_t Type>
bool Serializable<T, Type>::_InputSizeCheck(const uint16_t FullSize, const uint16_t DataSize)
{
    return FullSize == DataSize + HeaderSize;
}
//check that header + data can fit packet
template<typename T, uint16_t Type>
bool Serializable<T, Type>::_OutputSizeCheck(const size_t DataSize)
{
    return PacketMaxSize >= DataSize + HeaderSize;
}

/*
    Header operations 
 */
template<typename T, uint16_t Type>
void Serializable<T, Type>::_InputHeader(const Marking* Buffer)
{
    _Type = Buffer->Type;
    _AdditionalDataSize = Buffer->DataSize;
}

template<typename T, uint16_t Type>
void Serializable<T, Type>::_OutputHeader( Marking* Buffer)const
{
    Buffer->Type = _Type;
    Buffer->DataSize = _AdditionalDataSize;
}

#endif
