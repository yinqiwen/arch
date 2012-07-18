/*
 * connection_manager.hpp
 *
 *  Created on: 2011-6-29
 *      Author: qiyingwang
 */

#ifndef DATABASE_CONNECTION_MANAGER_HPP_
#define DATABASE_CONNECTION_MANAGER_HPP_
#include "common/base.hpp"
#include "database/db_interface.hpp"
#include <string>
#include <deque>
namespace arch
{
    namespace database
    {
        enum DataBaseType
        {
            INVALID = 0,
            MYSQL = 1,
            SQLITE3 = 2
        };


        class ConnectionManager
        {
            public:
                static Connection* CreateConnection(DataBaseType type);
                static void DestroyConnection(Connection*& conn);
        };
    }
}

#endif /* CONNECTION_MANAGER_HPP_ */
