#pragma once
#include <vector>
#include <map>
#include <typeinfo>
#include <sstream>
#include "ccspring.h"

class FieldInfo {
public:
	/// 字段类型
	int type;

	/// 字段名称
	std::string name;
};

class TableInfo {
public:
	unsigned int size()
	{
		return _fields.size();
	}
	const char* getTableName()
	{
		return _tableName.c_str();
	}
	FieldInfo * getField(unsigned int index);
	
	bool addField(int type, const std::string& name);
private:
	std::vector<FieldInfo> _fields;
	std::string _tableName;
};

/**
 * 一条记录
 */
class Record { 
public:
	Record(){

	}
	virtual ~Record()
	{}

	AnyValue& operator[](const std::string &name)
	{
		return _values[name];
	}
	
	void put(const char *fieldName);

	TableInfo *tableInfo = 0;

	unsigned int size()
	{
		return _values.size();
	}
	void put(const char* fieldName, const AnyValue& value);
	void put(const char* fieldName, const char* value, unsigned int size);
	bool find(const std::string& fieldName);
private:
	std::map<std::string, AnyValue> _values;
};

/**
 * 记录集合
 */
class RecordSet {
public:
	/**
	* \brief 重载operator[]运算符
	*
	* 通过指定的行数，获取相应的记录
	*
	* \param idx:指定的行数
	*
	* \return 如果指定的行数有效，则返回相应的记录指针，如果无效，则返回NULL
	*/
	Record* operator[](unsigned int idx)
	{
		if (idx < _records.size())
		{
			return &_records[idx];
		}
		return 0;
	}

	/**
	* \brief 获取记录数
	*
	* \return 返回记录数，如果没有记录，返回为0
	*/
	unsigned int size()
	{
		return _records.size();
	}

	/**
	* \brief 获取记录数
	*
	* \return 返回记录数，如果没有记录，返回为0
	*/
	bool empty(){ return _records.empty(); }

	/**
	* \brief 添加记录
	*
	*/
	void put(const Record& rec)
	{
		_records.push_back(rec);
	}


	/**
	* \brief 清空所有记录
	*
	*/
	void clear()
	{
		_records.clear();
	}

	/**
	* \brief 获取指定的行
	*
	* 功能与重载的operator[]运算符相同。
	*/
	Record* get(unsigned int idx)
	{
		if (idx < _records.size())
		{
			return &_records[idx];
		}
		return 0;
	}
private:
	std::vector<Record> _records;
};

/**
 * 数据库连接
 */
class IDBConnection {
public:
	/**
	* 选择
	*/
	virtual RecordSet * execSelect(TableInfo *table, Record *column, const std::string &condition){
		return 0;
	}

	/**
	* 删除
	*/
	virtual unsigned int exeDelete(TableInfo* table, const std::string &condition){
		return 0;
	}

	/**
	* 更新
	*/
	virtual unsigned int exeUpdate(TableInfo* table, Record* data, const std::string &condition){
		return 0;
	}

	/**
	* 插入
	*/
	virtual unsigned long exeInsert(TableInfo* table, Record* rec){
		return 0;
	}

	/**
	* \brief 事务提交
	* \param handleID 操作的链接句柄
	* \return 成功返回true，失败返回false
	*/
	virtual bool commit(){
		return false;
	}


	/**
	* \brief 事务回滚
	* \param handleID 操作的链接句柄
	* \return 成功返回true，失败返回false
	*/
	virtual bool rollback(){
		return false;
	}

	/**
	* \brief 设置此链接是否支持事务
	* \param handleID 操作的链接句柄
	* \param supportTransactions  是否支持事务
	* \return 成功返回true，失败返回false
	*/
	virtual bool setTransactions(bool supportTransactions){
		return false;
	}

	/**
	* \brief 检查此链接是否支持事务
	* \param handleID 操作的链接句柄
	* \return 支持返回true，否则返回false
	*/
	virtual bool supportTransactions() {
		return false;
	}
};
/**
 * 数据库操作
 */
class IDB :public Interface<IDB>{
public:
	/**
	 * 获取连接
	 */
	virtual IDBConnection *getConnection() = 0;
	
	/**
	 * 回收连接
	 */
	virtual void putConnection(IDBConnection *conn) = 0;
	
	/**
	 * 获取表
	 */
	virtual TableInfo * getTable(const std::string &name) = 0;
};


/**
 * 基本封装前的使用
 * IDB *db = core::getBean("db");
 * TableInfo * t_test= db->getTable("T_TEST");
 * if (t_test)
 * {
 *		IDBConnection *conn = db->getConnection(); // 获取连接
 *		Record col;
 *		col.put("ID");
 *		col.put("NAME");
 *		RecordSet * rs = conn->exeSelect(t_test,&col,"where id=1");
 *		// 获取的结果集
 * 		for (int i = 0; i < rs.size();++i)
 *      {
 *			Record *rec = rset->get(i);
 *			rec->get("ID"); // 获取ID
 *			rec->get("NAME"); // 获取NAME
 *		}
 *		db->putConnection(conn); // 释放连接
 * }
 */

class mytable {
public:
	/**
	 * 数据库
	 */
	TableInfo *tableInfo;
	/**
	 * 
	 */
	void attach(const std::string &tableName)
	{

	}
	template<typename TOBJECT>
	void insert(TOBJECT &object)
	{
		
		// 构建Record
		// 插入
		
	}
};

/*********************************************************************
 * 使用 (使用spring 后 相关的使用 就很简单了 基础的一些方案 可以做热插播)
 * IDB *db = ccs::getBean("db"); // 相关配置都从配置中读取
 * if (mytable<INFO>::attach(db,"T_INFO"))
 * {
 *		INFO info;
 *		mytable<INFO>::insert(info);
 * }
 *********************************************************************
 * 初始化
 * ccs::initFrom("cc-spring.xml"); // 预构建对象
 *********************************************************************
 * 配置
 * <bean name="db" class="MysqlDB" > <!--MysqlDB 可换成 Sqlite3-->
 *		<connection url="" port="" username="" passwd="" dbname="" />
 *		<connectionpool size="10" />
 * </bean>
 *********************************************************************/