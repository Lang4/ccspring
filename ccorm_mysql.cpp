#include "ccorm.h"
#include <list>
#include "mysql.h"
#define _USE_MYSQL

#ifdef _USE_MYSQL
struct MysqlUrlInfo{
	std::string host;
	std::string user;
	std::string passwd;
	unsigned int port;
	std::string dbName;
	bool supportTransactions;
};
class MySqlDBConnection : public IDBConnection{
public:
	MysqlUrlInfo url;
	MYSQL *mysql = 0;
	enum STEATE{
		INVALID = 1,
		VALID = 2,
		USED = 3,
	}state;

	bool setTransactions(bool supportTransactions)
	{
		if (supportTransactions)
			return (0 == execSql("SET AUTOCOMMIT=0", strlen("SET AUTOCOMMIT=0")));
		else
			return (0 == execSql("SET AUTOCOMMIT=1", strlen("SET AUTOCOMMIT=1")));
	}

	int execSql(const char *sql, unsigned int sqllen)
	{
		int ret = mysql_real_query(mysql, sql, sqllen);
		if (ret)
		{

		}
		return ret;
	}
	
	bool initHandle()
	{
		if (_initMysql())
		{
			finalHandle();
			return false;
		}
		return true;
	}

	void finalHandle()
	{
		if (mysql)
		{
			mysql_close(mysql);
			mysql = 0;
		}
		state = INVALID;
	}
	/**
	 * 选择
	 */
	virtual RecordSet * execSelect(TableInfo *table, Record *column, const std::string &condition)
	{
		using namespace std;
		ret_set.clear();
		ostringstream query_string;

		if (table == NULL || mysql == NULL)
		{
			return NULL;
		}

		unsigned int num_field = table->size();

		query_string << "SELECT ";

		bool first = true;

		if (column)
		{
			std::string fieldvalue = NULL;
			if (column->size() > 0)
			{
				for (unsigned int i = 0; i<num_field; i++)
				{

					FieldInfo* field = table->getField(i);

					if (!column->find(field->name))
					{
						continue;
					}

					if (first)
						first = false;
					else
					{
						query_string << " , ";
					}

					if ((fieldvalue = (*column)[field->name].toString()) == "")
					{
						query_string << field->name;
					}
					else
					{
						query_string << fieldvalue;
					}
				}
			}
			else
			{
				query_string << " * ";
			}
		}
		else
		{
			query_string << " * ";
		}

		query_string << "  FROM " << " `" << table->getTableName() << "` ";

		query_string << condition;

		 
		if (0 == execSql(query_string.str().c_str(), query_string.str().size()))
		{
		}
		else
		{ 
			return NULL;
		}

		my_ulonglong retCount = 0;
		MYSQL_RES *result = NULL;
		{
			result = mysql_store_result(mysql);
			if (result == NULL)
			{
				return NULL;
			}
			retCount = mysql_num_rows(result);

			if (retCount == 0)
			{
				mysql_free_result(result);
				return NULL;
			}
		}

		MYSQL_ROW row;
		num_field = mysql_num_fields(result);
		MYSQL_FIELD* mysql_fields = NULL;
		mysql_fields = mysql_fetch_fields(result);

		if (mysql_num_rows(result) > 0)
		{
			while ((row = mysql_fetch_row(result)))
			{
				Record rec;
				unsigned long *lengths = mysql_fetch_lengths(result);

				for (unsigned int i = 0; i < num_field; i++)
				{
					if (row[i] != NULL)
					{
						FieldInfo* field = table->getField(i);
						if (FIELD_TYPE_BLOB == field->type)
							rec.put(mysql_fields[i].name, row[i], lengths[i]);
						else
							rec.put(mysql_fields[i].name, row[i]);
					}
				}
				 
				rec.tableInfo = table;
				ret_set.put(rec);
			}
		}

		mysql_free_result(result);

		return &ret_set;
	}
	bool ping()
	{
		return mysql_ping(mysql) == 0;
	}
	/**
	* 删除
	*/
	virtual unsigned int exeDelete(TableInfo* table, const std::string &condition)
	{
		using namespace std;
		ostringstream query_string;

		if (table == NULL || mysql == NULL)
		{
			return (unsigned int)-1;
		}

		unsigned int num_field = table->size();

		query_string << "DELETE FROM ";
		query_string << " `" << table->getTableName() << "` ";

		query_string << " " << condition;

		if (0 == execSql(query_string.str().c_str(), query_string.str().size()))
		{
			return (unsigned int)mysql_affected_rows(mysql);
		}
		else
		{
			return (unsigned int)-1;
		}
	}

	/**
	* 更新
	*/
	virtual unsigned int exeUpdate(TableInfo* table, Record* data, const std::string &condition){
		using namespace std;


		ostringstream query_string;
		ostringstream where_string;

		if (table == NULL || data == NULL || mysql == NULL)
		{
			return (unsigned int)-1;
		}

		unsigned int num_field = table->size();

		query_string << "UPDATE ";
		query_string << "`" << table->getTableName() << "`";
		query_string << " SET ";

		bool first = true;

		for (unsigned int i = 0; i<num_field; i++)
		{
			std::string fieldvalue;

			FieldInfo* field = table->getField(i);

			if ((fieldvalue = (*data)[field->name].toString()) == "")
			{
				continue;
			}

			if (first)
				first = false;
			else
			{
				query_string << " ,";
			}

			query_string << "`" << field->name << "`" << " = ";

			switch (field->type)
			{
			case FIELD_TYPE_STRING:
			case FIELD_TYPE_VAR_STRING:
			case FIELD_TYPE_DATE:
			case FIELD_TYPE_TIME:
			case FIELD_TYPE_DATETIME:
			case FIELD_TYPE_YEAR:
			{
				std::string strData;
				strData.resize(fieldvalue.size() * 2 + 1);
				mysql_real_escape_string(mysql, &strData[0], &fieldvalue[0],fieldvalue.size());
				query_string << "\'" << strData << "\'";
			}
			break;
			case FIELD_TYPE_BLOB:
			{
				std::string binData = ((*data)[field->name]).toString();
				std::string strData;
				strData.resize(binData.size() * 2 + 1);
				mysql_real_escape_string(mysql, &strData[0], &binData[0], binData.size());
				query_string << "\'" << strData << "\'";
			}
			break;
			default:
				query_string << fieldvalue;
				break;
			}
		}

		query_string << " " << condition;
 

		if (0 == execSql(query_string.str().c_str(), query_string.str().size()))
		{
			return (unsigned int)mysql_affected_rows(mysql);
		}
		else
		{
			return (unsigned int)-1;
		}
	}

	/**
	* 插入
	*/
	virtual unsigned long exeInsert(TableInfo* table, Record* rec)
	{
		using namespace std;

		ostringstream query_string;
		ostringstream value_string;

		if (table == NULL || rec == NULL || mysql == NULL)
		{
			return (unsigned int)-1;
		}

		unsigned int num_field = table->size();
		query_string << "INSERT INTO ";
		query_string << "`" << table->getTableName() << "`";
		query_string << " ( ";

		value_string << " VALUES( ";

		bool first = true;
		for (unsigned int i = 0; i<num_field; i++)
		{
			std::string  fieldvalue ;
			FieldInfo* field = table->getField(i);

			if ((fieldvalue = (*rec)[field->name].toString()) == "")
			{
				continue;
			}
			if (first)
				first = false;
			else
			{
				query_string << " , ";
				value_string << " , ";
			}

			query_string << "`" << field->name << "`";

			switch (field->type)
			{
			case FIELD_TYPE_STRING:
			case FIELD_TYPE_VAR_STRING:
			case FIELD_TYPE_DATE:
			case FIELD_TYPE_TIME:
			case FIELD_TYPE_DATETIME:
			case FIELD_TYPE_YEAR:
			{
				std::string strData;
				strData.resize(fieldvalue.size() * 2 + 1);
				mysql_real_escape_string(mysql, &strData[0], &fieldvalue[0], fieldvalue.size());
				value_string << "\'" << strData << "\'";
			}

			break;
			case FIELD_TYPE_BLOB:
			{
				std::string binData = ((*rec)[field->name]).toString();

				std::string strData;
				strData.resize(binData.size() * 2 + 1);
				mysql_real_escape_string(mysql, &strData[0], &binData[0], binData.size());
				value_string << "\'" << strData << "\'";
			}
			break;
			default:
				value_string << fieldvalue;
				break;
			}
		}

		query_string << ")" << value_string.str() << ")";

		if (0 == execSql(query_string.str().c_str(), query_string.str().size()))
		{
			return (unsigned long)mysql_insert_id(mysql);
		}
		else
		{
			return (unsigned long)-1;
		}
	}


	/**
	* \brief 事务提交
	* \param handleID 操作的链接句柄
	* \return 成功返回true，失败返回false
	*/
	virtual bool commit() { 
		return (0 == execSql("COMMIT", strlen("COMMIT")));
	}


	/**
	* \brief 事务回滚
	* \param handleID 操作的链接句柄
	* \return 成功返回true，失败返回false
	*/
	virtual bool rollback() {
		return (0 == execSql("ROLLBACK", strlen("ROLLBACK")));
	}

	 
	/**
	* \brief 检查此链接是否支持事务
	* \param handleID 操作的链接句柄
	* \return 支持返回true，否则返回false
	*/
	virtual bool supportTransactions() {
		return isSupportTransactions();
	}

	bool isSupportTransactions() const
	{
		return url.supportTransactions;
	}

	RecordSet ret_set;
private:
	bool _initMysql()
	{
		if (mysql)
		{
			mysql_close(mysql);
			mysql = 0;
		}
		mysql = mysql_init(NULL);
		if (mysql = 0)
		{
			return false;
		}

		if (mysql_real_connect(
			mysql,
			url.host.c_str(),
			url.user.c_str(),
			url.passwd.c_str(),
			url.dbName.c_str(),
			url.port,
			0,
			CLIENT_COMPRESS | CLIENT_MULTI_STATEMENTS | CLIENT_INTERACTIVE) == 0)
		{
			return false;
		}

		if (!setTransactions(url.supportTransactions))
		{
			return false;
		}
		if (0 != execSql("set names utf8", strlen("set names utf8")))
		{
			return false;
		}
		state = VALID;
		return true;
	}
};

/**
 * <bean class="MysqlDB" bean="db" creatortype="singleton">
		<connection dbname="" host="" user="" passwd="" port=""/>
 *</bean>
 */
class MysqlDB :public IDB, public Creator<MysqlDB>{
public:
	void configure(INode *node)
	{
		INode *connectionInfo = node->childNode("connection");
		if (connectionInfo)
		{
			url.dbName = connectionInfo->getStr("dbname");
			url.host = connectionInfo->getStr("host");
			url.user = connectionInfo->getStr("user");
			url.passwd = connectionInfo->getStr("passwd");
			url.port = connectionInfo->get<unsigned int>("port");;
		}
		init();

	}
	bool init()
	{
		MYSQL* mysql_conn = NULL;
		MYSQL_RES* table_res = NULL;

		mysql_conn = mysql_init(NULL);

		if (mysql_conn == NULL)
		{
			return false;
		}

		if (mysql_real_connect(mysql_conn, url.host.c_str(), url.user.c_str(), url.passwd.c_str(), url.dbName.c_str(), url.port, NULL, CLIENT_COMPRESS | CLIENT_INTERACTIVE) == NULL)
		{
			return false;
		}
 
		if (mysql_conn)
		{
			if ((table_res = mysql_list_tables(mysql_conn, NULL)) == NULL)
			{
				if (table_res) mysql_free_result(table_res);
				if (mysql_conn) mysql_close(mysql_conn);
				return false;
			}
			MYSQL_ROW row;

			while ((row = mysql_fetch_row(table_res)))
			{
				this->addTable(mysql_conn, row[0]);
			}

			mysql_free_result(table_res);
		}

		if (mysql_conn) mysql_close(mysql_conn);
		return true;
	}
	/**
	* \brief 加入一个新表
	*/
	bool addTable(MYSQL* mysql_conn, const char* tableName)
	{
		MYSQL_RES* field_res = NULL;
		 
		field_res = mysql_list_fields(mysql_conn, tableName, NULL);

		if (field_res)
		{
			unsigned int num_fields = mysql_num_fields(field_res);
			MYSQL_FIELD* mysql_fields = NULL;
			mysql_fields = mysql_fetch_fields(field_res);

			TableInfo tableInfo;

			for (unsigned int i = 0; i<num_fields; i++)
			{
				if (!tableInfo.addField(mysql_fields[i].type, mysql_fields[i].name))
				{
					mysql_free_result(field_res);
					return false;
				}
			}

			_tables[tableName] = tableInfo;
			mysql_free_result(field_res);
		}
		else
		{
			return false;
		}

		return true;
	}

	IDBConnection * getConnection()
	{
		MySqlDBConnection *conn = NULL;

		//free list
		std::list<MySqlDBConnection*> ::iterator iter = _free_conns.begin();
		for (; iter != _free_conns.end();)
		{
			MySqlDBConnection *c = *iter;
			if (!c)
			{
				++iter;
				continue;
			}

			//ping一下
			if (!c->ping())
			{
				c->finalHandle();
				delete c;
				iter = _free_conns.erase(iter);
			}
			else
			{
				conn = *iter;
				iter = _free_conns.erase(iter);
				break;
			}
		}
		if (!conn)
		{
			conn = new MySqlDBConnection();
			conn->url = url;
			if (!conn->initHandle())
			{
				conn->finalHandle();
				delete conn;
				return 0;
			}
		}

		return conn;
	}
	
	void put(MySqlDBConnection *conn)
	{
		_free_conns.push_back(conn);
	}
	void putConnection(IDBConnection *conn)
	{
		put((MySqlDBConnection*)conn);
	}
	TableInfo * getTable(const std::string &name)
	{
		auto it = _tables.find(name);
		if (it != _tables.end())
		{
			return &it->second;
		}
		return 0;
	}
private:
	MysqlUrlInfo url;
	std::list<MySqlDBConnection*> _free_conns;
	std::map<std::string, TableInfo> _tables;
};


CREATOR("mysqldb", MysqlDB);

#endif