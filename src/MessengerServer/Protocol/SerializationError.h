#if !defined(SERIALIZATIONERROR_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define SERIALIZATIONERROR_H

enum SerializationError
{
    Ok = 0,
    InvalidHeader,
    InvalidData,
    InvalidBufferSize
};


#endif
