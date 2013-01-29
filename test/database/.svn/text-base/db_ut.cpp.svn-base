/*
 * util_ut.cpp
 *
 *  Created on: 2011-3-14
 *      Author: qiyingwang
 */
#include "database/database_connection_manager.hpp"
#include <string>
using namespace arch::database;

int main(int argc, char** argv)
{
    Connection* conn = ConnectionManager::CreateConnection(MYSQL_DATABASE);
    Properties props;
    props["Host"] = "127.0.0.1";
    props["Port"] = "3306";
    props["DataBase"] = "tc_cfg";
    props["User"] = "root";
    props["Password"] = "";
    conn->Connect(props);
    SQLResultSet result;
    conn->ExecuteQuery(result, "select * from t_node_log");
    int column_num = result.ColumnNum();
    int row_num = result.RowNum();
    printf("Result column num:%d, row number:%d\n", column_num, row_num);
    int ret = -1;
    do
    {
        Row row;
        ret = result.Next(row);
        if(ret == 0)
        {
            for(int i = 0; i < row.n_columns; i++)
            {
                printf("column[%d] = %s\n", i, row.column_values[i]);
            }
        }
    }
    while(ret == 0);
    return 0;
}
