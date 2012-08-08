/*
 * mysql_db_impl.cpp
 *
 *  Created on: 2011-6-29
 *      Author: qiyingwang
 */

#include "common/config.h"
#if HAVE_MYSQL_LIB == 1
#include "util/string_helper.hpp"
#include "database/mysql_db_impl.hpp"
#include <mysql.h>
#include <stdarg.h>
using namespace arch::database;
using namespace arch::util;

MySQLConnection::MySQLConnection() :
		m_bAlive(false), m_bRemote(false), m_handler(NULL)
{
	m_szSQL[0] = '\0';
	m_szErrMsg[0] = '\0';

}

MySQLConnection::~MySQLConnection()
{
	Close();
}

int MySQLConnection::Init()
{
	if (NULL != m_handler)
	{
		return 0;
	}
	NEW(m_handler, struct st_mysql);
	if (NULL == m_handler)
	{
		return -1;
	}
	mysql_init(m_handler);
	return 0;
}

int MySQLConnection::Connect(const std::string& host, uint16 port,
		const std::string& db, const std::string& user,
		const std::string& passwd)
{
	if (m_bAlive)
		return 0;
	if (Init() < 0)
	{
		return -1;
	}
	my_bool reconnect = 1;
	mysql_options(m_handler, MYSQL_OPT_RECONNECT, &reconnect);

	unsigned int timeout = 2;
	mysql_options(m_handler, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
	mysql_options(m_handler, MYSQL_OPT_READ_TIMEOUT, &timeout);
	mysql_options(m_handler, MYSQL_OPT_WRITE_TIMEOUT, &timeout);

	unsigned int localfile = 1;
	mysql_options(m_handler, MYSQL_OPT_LOCAL_INFILE, &localfile);

	const char* unix_socket = NULL;
	Properties::iterator found = m_properties.find("UnixSocket");
	if (found != m_properties.end())
	{
		const std::string& str = found->second;
		unix_socket = str.c_str();
	}
	if (NULL
			== mysql_real_connect(m_handler, host.c_str(), user.c_str(),
					passwd.c_str(), db.c_str(), port, unix_socket, 0))
	{
		snprintf(m_szErrMsg, sizeof(m_szErrMsg) - 1, "Connenct error | (%u)%s",
				mysql_errno(m_handler), mysql_error(m_handler));
		return -1;
	}

	m_bAlive = true;
	return 0;
}

void MySQLConnection::Close()
{
	if (m_bAlive)
	{
		mysql_close(m_handler);
		DELETE(m_handler);
		m_bAlive = false;
	}
}

int MySQLConnection::ExecuteUpdate(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int ret = VExecuteUpdate(fmt, ap);
	va_end(ap);
	return ret;
}

int MySQLConnection::Ping()
{
	if (0 == mysql_ping(m_handler))
	{
		return 0;
	}
	snprintf(m_szErrMsg, sizeof(m_szErrMsg) - 1, "Ping failed (%u): %s",
			mysql_errno(m_handler), mysql_error(m_handler));
	return -1;
}

Statement* MySQLConnection::PrepareStatement(const char* fmt, ...)
{
	return NULL;
}

int MySQLConnection::VExecuteUpdate(const char* fmt, va_list ap)
{
	vsnprintf(m_szSQL, sizeof(m_szSQL) - 1, fmt, ap);
	if (mysql_real_query(m_handler, m_szSQL, strlen(m_szSQL)))
	{
		snprintf(m_szErrMsg, sizeof(m_szErrMsg) - 1,
				"Update error (%u) | sql: %s | (%s): %s",
				mysql_errno(m_handler), m_szSQL, mysql_sqlstate(m_handler),
				mysql_error(m_handler));
		return -1;
	}

	return (int) mysql_affected_rows(m_handler);
}

int MySQLConnection::ExecuteQuery(SQLResultSet& result, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	MySQLResultSet* mysql_result = NULL;
	NEW(mysql_result, MySQLResultSet);
	if (NULL == mysql_result)
	{
		return -1;
	}
	int ret = VExecuteQuery(mysql_result, fmt, ap);
	va_end(ap);
	InjectRawResultSet(result, mysql_result, StandardDestructor<ResultSet>);
	return ret;
}

int MySQLConnection::VExecuteQuery(MySQLResultSet* result, const char* fmt,
		va_list ap)
{
	vsnprintf(m_szSQL, sizeof(m_szSQL) - 1, fmt, ap);
	if (mysql_real_query(m_handler, m_szSQL, strlen(m_szSQL)))
	{
		snprintf(m_szErrMsg, sizeof(m_szErrMsg) - 1,
				"Query error (%u) | sql: %s | %s", mysql_errno(m_handler),
				m_szSQL, mysql_error(m_handler));
		return -1;
	}

	struct st_mysql_res * results =
			m_bRemote ?
					mysql_use_result(m_handler) : mysql_store_result(m_handler);
	if (results == NULL && mysql_num_fields(results) != 0)
	{
		snprintf(m_szErrMsg, sizeof(m_szErrMsg) - 1,
				"Fetch error (%u) | sql: %s | %s", mysql_errno(m_handler),
				m_szSQL, mysql_error(m_handler));
		return -1;
	}
	result->SetResult(results);
	return 0;
}

int MySQLConnection::BeginTransaction()
{
	return ExecuteUpdate("BEGIN");
}
int MySQLConnection::CommitTransaction()
{
	return ExecuteUpdate("COMMIT");
}
int MySQLConnection::RollBack()
{
	return ExecuteUpdate("ROLLBACK");
}

int MySQLConnection::GetErrCode() const
{
	unsigned int ret = 0;
	if (m_handler)
		ret = mysql_errno(m_handler);

	return (int) ret;
}

const char* MySQLConnection::GetSQL() const
{
	return m_szSQL;
}

const char* MySQLConnection::GetError() const
{
	return m_szErrMsg;
}

int MySQLConnection::SetProperties(const Properties& props)
{
	m_properties = props;

	return 0;
}

struct st_mysql* MySQLConnection::GetMySQLHandler()
{
	return m_handler;
}

void MySQLConnection::InstallLoadDataHandler(
		int (*local_infile_init)(void **, const char *, void *),
		int (*local_infile_read)(void *, char *, unsigned int),
		void (*local_infile_end)(void *),
		int (*local_infile_error)(void *, char*, unsigned int), void *userdata)
{
	mysql_set_local_infile_handler(m_handler, local_infile_init,
			local_infile_read, local_infile_end, local_infile_error, userdata);
}

int MySQLConnection::Connect(const Properties& props)
{
	std::string host;
	Properties::const_iterator found = props.find("Host");
	if (found != props.end())
	{
		host = found->second;
	}
	std::string db;
	found = props.find("DataBase");
	if (found != props.end())
	{
		db = found->second;
	}
	std::string portstr;
	found = props.find("Port");
	if (found != props.end())
	{
		portstr = found->second;
	}
	std::string user;
	found = props.find("User");
	if (found != props.end())
	{
		user = found->second;
	}
	std::string passwd;
	found = props.find("Password");
	if (found != props.end())
	{
		passwd = found->second;
	}
	uint32 port = 3306;
	if (host.empty())
	{
		host = "127.0.0.1";
	}
	if (!portstr.empty())
	{
		if (!string_touint32(portstr, port))
		{
			snprintf(m_szErrMsg, sizeof(m_szErrMsg) - 1,
					"Invalid 'Port' property specified", mysql_errno(m_handler),
					m_szSQL, mysql_error(m_handler));
			return -1;
		}
	}
	SetProperties(props);
	int ret = Connect(host, (uint16) port, db, user, passwd);
	return ret;

}

//////////////////////////////////////////////////////////
//ResultSet definition
//////////////////////////////////////////////////////////

MySQLResultSet::MySQLResultSet() :
		m_res_set(NULL)
{

}
void MySQLResultSet::SetResult(struct st_mysql_res* res)
{
	if (m_res_set != NULL)
	{
		mysql_free_result(m_res_set);
		m_res_set = NULL;
	}
	m_res_set = res;
}

MySQLResultSet::~MySQLResultSet()
{
	if (m_res_set != NULL)
	{
		mysql_free_result(m_res_set);
		m_res_set = NULL;
	}
}

int MySQLResultSet::Next(Row& row)
{
	if (!m_res_set)
		return -1;

	MYSQL_ROW tmp = mysql_fetch_row(m_res_set);
	if (tmp == NULL)
		return -1;

	row.column_values = tmp;
	row.n_columns = ColumnNum();
	return 0;
}

int MySQLResultSet::ColumnNum()
{
	return m_res_set ? mysql_num_fields(m_res_set) : 0;
}

int MySQLResultSet::ColumnNames(ColumnNameArray& names)
{
	if (NULL == m_res_set)
	{
		return -1;
	}
	names.clear();
	unsigned int num_fields;
	unsigned int i;
	MYSQL_FIELD *fields;

	num_fields = ColumnNum();
	fields = mysql_fetch_fields(m_res_set);
	for (i = 0; i < num_fields; i++)
	{
		names.push_back(fields[i].name);
	}
	return 0;
}

int MySQLResultSet::RowNum()
{
	return m_res_set ? mysql_num_rows(m_res_set) : 0;
}

#endif  /* HAVE_MYSQL_LIB */
