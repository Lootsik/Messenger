#if !defined(MESSAGESTORAGE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Notice: $
   ======================================================================== */
#define MESSAGESTORAGE_H

#include <GlobalInfo.h>
#include <Database\Database.h>

class MessageStorage{

public:
    MessageStorage(Database& DB);

    void GetMessage();
    void AddMessage();
private:
    Database& _DB;
    sql::PreparedStatement* _Prepared_NewUser;
    sql::PreparedStatement* _Prepared_NewUser;
    sql::PreparedStatement* _Prepared_NewUser;
    sql::PreparedStatement* _Prepared_NewUser;
    sql::PreparedStatement* _Prepared_NewUser;
}


#endif
