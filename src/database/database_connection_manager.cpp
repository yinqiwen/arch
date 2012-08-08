/*
 * database_connection_manager.cpp
 *
 *  Created on: 2011-6-29
 *      Author: qiyingwang
 */
#include "database/database_connection_manager.hpp"
#include "database/mysql_db_impl.hpp"
#include "database/sqlite3_db_impl.hpp"

using namespace arch::database;

//std::deque<Connection*> ConnectionManager::m_connection_store;

Connection* ConnectionManager::CreateConnection(DataBaseType type)
{
	Connection* conn = NULL;
	switch (type)
	{
		case MYSQL_DATABASE:
		{
#if HAVE_MYSQL_LIB == 1
			NEW(conn, MySQLConnection);
#endif
			break;
		}
		case SQLITE3_DATABASE:
		{
#if HAVE_SQLITE_LIB == 1
			NEW(conn, SQLite3Connection);
#endif
			break;
		}
		default:
		{
			return NULL;
		}
	}
	return conn;
}

void ConnectionManager::DestroyConnection(Connection*& conn)
{
	RETURN_IF_NULL(conn);
	conn->Close();
	DELETE(conn);
}
