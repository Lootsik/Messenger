#if !defined(SERIALIZATIONERROR_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   ======================================================================== */

#define SERIALIZATIONERROR_H

enum SerializationError
{
    Ok = 0,
    InvalidHeader,
    InvalidData,
    InvalidBufferSize,
	LowFrameSizeForType
};


#endif
