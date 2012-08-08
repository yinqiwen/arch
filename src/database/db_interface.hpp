/*
 * db_interface.h
 *
 *  Created on: 2010-12-16
 *      Author: qiyingwang
 */

#ifndef DB_INTERFACE_H_
#define DB_INTERFACE_H_
#include "util/config_helper.hpp"
#include <string.h>
#include <vector>
#include <string>

#define MAX_SQL_STR_LEN 10*1024
#define MAX_ERR_MSG_LEN (2*MAX_SQL_STR_LEN)

using arch::util::Properties;

namespace arch
{
	namespace database
	{

		struct Row
		{
				char** column_values;
				uint32 n_columns;
				Row() :
						column_values(NULL), n_columns(0)
				{
				}
		};

		class CopiedRow
		{
			private:
				Row m_row;
				char** m_ref;
			public:
				int DeepCopy(const Row& other)
				{
					if (NULL != m_ref)
					{
						return -1;
					}
					if (NULL != other.column_values && other.n_columns > 0)
					{
						char** tmp = NULL;
						NEW(tmp, char*[other.n_columns]);
						if (NULL == tmp)
						{
							return -1;
						}
						for (uint32 i = 0; i < other.n_columns; i++)
						{
							tmp[i] = strdup(other.column_values[i]);
						}
						m_row.column_values = tmp;
						m_row.n_columns = other.n_columns;
						m_ref = tmp;
						return 0;
					}
					return -1;
				}
				CopiedRow(const Row& row) :
						m_ref(NULL)
				{
					DeepCopy(row);
				}
				Row& GetRow()
				{
					return m_row;
				}

				~CopiedRow()
				{
					for (uint32 i = 0; i < m_row.n_columns; i++)
					{
						free(m_ref[i]);
					}
					DELETE_A(m_ref);
				}
		};

		/*
		 * just a simple encapsultion of pointer to struct st_mysql_res
		 * note that, Both copy constructor and assignment operator of
		 * ResultSet are shallow copy which just copy the address of a
		 * result set.
		 */
		class ResultSet
		{
			public:
				typedef std::vector<std::string> ColumnNameArray;
				virtual int ColumnNames(ColumnNameArray& names) = 0;
				virtual int Next(Row& row) = 0;
				virtual int RowNum() = 0;
				virtual int ColumnNum() = 0;
				virtual ~ResultSet()
				{
				}
		};

		typedef void ResultSetDestructor(ResultSet* obj);

		class SQLResultSet
		{
			private:
				ResultSet* m_result;
				ResultSetDestructor* m_destructor;
				inline void SetRawResultSet(ResultSet* set,
						ResultSetDestructor* destructor)
				{
					DestroyRawResultSet();
					m_result = set;
					m_destructor = destructor;
				}
				inline void DestroyRawResultSet()
				{
					if (NULL != m_destructor)
					{
						m_destructor(m_result);
					}
					m_result = NULL;
				}
				friend class Connection;
			public:
				inline SQLResultSet() :
						m_result(NULL), m_destructor(NULL)
				{
				}
				inline int Next(Row& row)
				{
					return m_result->Next(row);
				}
				inline int RowNum()
				{
					return m_result->RowNum();
				}
				inline int ColumnNum()
				{
					return m_result->ColumnNum();
				}
				int ColumnNames(ResultSet::ColumnNameArray& names)
				{
					return m_result->ColumnNames(names);
				}
				inline ~SQLResultSet()
				{
					DestroyRawResultSet();
				}
		};

		struct Statement
		{
			public:
				virtual int BindText(int index, const char* value) = 0;
				virtual int BindInt64(int index, int64 value) = 0;
				virtual int Execute() = 0;
				virtual int Close() = 0;
				virtual ~Statement()
				{
				}
		};

		/*
		 * represent a connection to database.
		 */
		class Connection
		{
			protected:
				void InjectRawResultSet(SQLResultSet& result, ResultSet* raw,
						ResultSetDestructor* destructor)
				{
					result.SetRawResultSet(raw, destructor);
				}
			public:
				virtual Statement* PrepareStatement(const char* fmt, ...) = 0;
				/*
				 * execute update statements, such as insert, update, replace etc.
				 */
				virtual int ExecuteUpdate(const char* fmt, ...) = 0;
				/*
				 * executes query statements,which is select clause.
				 * return a ResultSet object containning query results
				 */
				virtual int ExecuteQuery(SQLResultSet& result, const char* fmt,
						...) = 0;

				virtual int BeginTransaction()
				{
					return -1;
				}
				virtual int CommitTransaction()
				{
					return -1;
				}

				virtual int RollBack()
				{
					return -1;
				}

				/**
				 * return last sql statement
				 */
				virtual const char* GetSQL() const = 0;
				/**
				 * return a error description
				 */
				virtual const char* GetError() const = 0;

				/**
				 * return a error number
				 */
				virtual int GetErrCode() const = 0;

				/**
				 * return if this connection is alive
				 * 0, alive, -1:hanged
				 */
				virtual int Ping() = 0;

				/**
				 *  connect db
				 */
				virtual int Connect(const Properties& props) = 0;

				virtual int SetProperties(const Properties& props) = 0;
				virtual const Properties& GetProperties() = 0;
				/**
				 * Close this db connection
				 */
				virtual void Close() = 0;

				virtual ~Connection()
				{
				}
		};

		class ConnectionTransactionGuard
		{
			private:
				Connection* m_conn;
				bool m_success;
			public:
				ConnectionTransactionGuard(Connection* conn) :
						m_conn(conn), m_success(true)
				{
					if (NULL != m_conn)
					{
						m_conn->BeginTransaction();
					}
				}
				void MarkFailed()
				{
					m_success = false;
				}
				~ConnectionTransactionGuard()
				{
					if (NULL != m_conn)
					{
						if (m_success)
						{
							m_conn->CommitTransaction();
						}
						else
						{
							m_conn->RollBack();
						}
					}
				}
		};

	}
}

#endif /* DB_INTERFACE_H_ */
