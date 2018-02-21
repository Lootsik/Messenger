#pragma once

#pragma warning(push)
#pragma warning( disable : 4251 )
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#pragma warning(pop)

#include <Logger\Logger.h>

#define FallLog() Logger::LogBoth(Logger::Error, "SQLException thrown in. %s : %s. Error # %d: %s", \
			__FILE__, __func__, e.getErrorCode(), e.what());\
			throw