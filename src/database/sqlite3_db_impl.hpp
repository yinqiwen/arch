/*
 * sqlite3_db_impl.hpp
 *
 *  Created on: 2011-6-29
 *      Author: qiyingwang
 */

#ifndef SQLITE3_DB_IMPL_HPP_
#define SQLITE3_DB_IMPL_HPP_
#include "common/base.hpp"
#if HAVE_SQLITE_LIB == 1
#include "database/db_interface.hpp"
#include <string>
#include <vector>
struct sqlite3;
struct sqlite3_stmt;
namespace arch
{
	namespace database
	{
		class SQLite3Connection;
		class SQLite3Staement: public Statement
		{
			private:
				sqlite3_stmt* m_stmt;
				int BindText(int index, const char* value);
				int BindInt64(int index, int64 value);
				int Execute();
				int Close();
				SQLite3Staement(sqlite3_stmt* stmt);
				friend class SQLite3Connection;
			public:
				~SQLite3Staement();
		};

		class SQLite3ResultSet: public ResultSet
		{
			public:
				SQLite3ResultSet();
				int AppendRow(int n_columns, char** column_values);
				int SetColumnNames(int n_columns, char** column_names);
				~SQLite3ResultSet();
			public:

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
				typedef std::vector<CopiedRow*> RowVector;
				RowVector m_rows;
				ColumnNameArray m_column_names;
				uint32 m_column_num;
				uint32 m_cursor;

		};

		class SQLite3Connection: public Connection
		{
			private:
				Properties m_properties;
				struct sqlite3* m_handler;
				char m_szSQL[MAX_SQL_STR_LEN];
				char m_szErrMsg[MAX_ERR_MSG_LEN];
				int m_error_code;
				int32 m_busy_timeout;
				static int QueryCallBack(void* data, int n_columns,
						char** column_values, char** column_names);
			protected:
				virtual Statement* PrepareStatement(const char* fmt, ...);

				int VExecuteUpdate(const char* fmt, va_list ap);
				int VExecuteQuery(SQLite3ResultSet* result, const char* fmt,
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

				virtual int BeginTransaction();
				virtual int CommitTransaction();
				virtual int RollBack();

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
				virtual int GetErrCode() const;

				/**
				 * return if this connection is alive
				 * 0, alive, -1:hanged
				 */
				virtual int Ping();

				/**
				 *  connect db
				 */
				virtual int Connect(const Properties& props);

				virtual int SetProperties(const Properties& props);
				virtual const Properties& GetProperties()
				{
					return m_properties;
				}
				/**
				 * Close this db connection
				 */
				virtual void Close();
				void SetBusyTimeout(int32 val);
			public:
				SQLite3Connection();
				int Connect(const std::string& path);
				int32 GetBusyTimeout()
				{
					return m_busy_timeout;
				}
				~SQLite3Connection();
		};
	}
}
#endif
#endif /* SQLITE3_DB_IMPL_HPP_ */
