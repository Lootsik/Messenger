
#if !defined(TYPES_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define TYPES_H

namespace Types
{
enum Types{
    NotType = 0,
    PrevOperationDenied,
    UserInfo,
    MessageRequest,
    MessageResponse,
    LoginRequest,
    LoginResponse,
    Logout,
    UncheckedEvent
};  
}

#endif
