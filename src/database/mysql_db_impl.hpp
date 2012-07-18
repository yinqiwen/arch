/*
 * mysql_db_impl.hpp
 *
 *  Created on: 2011-6-29
 *      Author: qiyingwang
 */

#ifndef MYSQL_DB_IMPL_HPP_
#define MYSQL_DB_IMPL_HPP_
#include "common/base.hpp"
#if HAVE_MYSQL_LIB == 1
#include "database/db_interface.hpp"
#include <string>
#include <deque>
struct st_mysql_res;
struct st_mysql;
namespace arch
{
    namespace database
    {
        class MySQLResultSet: public ResultSet
        {
            public:
                MySQLResultSet();
                void SetResult(struct st_mysql_res* res);
                ~MySQLResultSet();

            private:
                /************************************************************************
                 * Description: 返回结果集中的下一行
                 ************************************************************************/
                virtual int Next(Row& row);
                /************************************************************************
                 * Description: 结果集中列的个数
                 ************************************************************************/
                virtual int ColumnNum();
                /************************************************************************
                 * Description: 结果集中的行数
                 ************************************************************************/
                virtual int RowNum();

                virtual int ColumnNames(ColumnNameArray& names);

            private:
                struct st_mysql_res* m_res_set;
        };

        class MySQLConnection: public Connection
        {
            private:
                Properties m_properties;
                bool m_bAlive;
                bool m_bRemote;
                struct st_mysql* m_handler;
                char m_szSQL[MAX_SQL_STR_LEN];
                char m_szErrMsg[MAX_ERR_MSG_LEN];

            protected:

                virtual int BeginTransaction();
                virtual int CommitTransaction();
                virtual int RollBack();

                int VExecuteUpdate(const char* fmt, va_list ap);
                int VExecuteQuery(MySQLResultSet* result, const char* fmt,
                        va_list ap);
                /*
                 * execute update statements, such as insert, update, replace etc.
                 */
                virtual int ExecuteUpdate(const char* fmt, ...);
                /*
                 * executes query statements,which is select clause.
                 * return a ResultSet object containning query results
                 */
                virtual int ExecuteQuery(SQLResultSet& result, const char* fmt,
                        ...);

                //virtual void Commit();
                //virtual void RollBack();
                /**
                 * return last sql statement
                 */
                virtual const char* GetSQL() const;
                /**
                 * return a error description
                 */
                virtual const char* GetError() const;

                /**
                 * return a error number
                 */
                virtual int GetErrNum() const;

                /**
                 * return if this connection is alive
                 * 0, alive, -1:hanged
                 */
                virtual int Ping();

                /**
                 *  connect db
                 */
                virtual int Connect(const Properties& props);
                /**
                 * Close this db connection
                 */
                virtual void Close();
                virtual int SetProperties(const Properties& props);
                virtual const Properties& GetProperties()
                {
                    return m_properties;
                }
                int Init();
            public:
                MySQLConnection();
                int Connect(const std::string& host, uint16 port,
                        const std::string& db, const std::string& user,
                        const std::string& passwd);
                ~MySQLConnection();
        };
    }
}
#endif
#endif /* MYSQL_DB_IMPL_HPP_ */
