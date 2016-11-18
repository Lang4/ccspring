#pragma once
#include <vector>
#include <map>
#include <typeinfo>
#include <sstream>
#include "ccspring.h"

class FieldInfo {
public:
	/// �ֶ�����
	int type;

	/// �ֶ�����
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
 * һ����¼
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
 * ��¼����
 */
class RecordSet {
public:
	/**
	* \brief ����operator[]�����
	*
	* ͨ��ָ������������ȡ��Ӧ�ļ�¼
	*
	* \param idx:ָ��������
	*
	* \return ���ָ����������Ч���򷵻���Ӧ�ļ�¼ָ�룬�����Ч���򷵻�NULL
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
	* \brief ��ȡ��¼��
	*
	* \return ���ؼ�¼�������û�м�¼������Ϊ0
	*/
	unsigned int size()
	{
		return _records.size();
	}

	/**
	* \brief ��ȡ��¼��
	*
	* \return ���ؼ�¼�������û�м�¼������Ϊ0
	*/
	bool empty(){ return _records.empty(); }

	/**
	* \brief ��Ӽ�¼
	*
	*/
	void put(const Record& rec)
	{
		_records.push_back(rec);
	}


	/**
	* \brief ������м�¼
	*
	*/
	void clear()
	{
		_records.clear();
	}

	/**
	* \brief ��ȡָ������
	*
	* ���������ص�operator[]�������ͬ��
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
 * ���ݿ�����
 */
class IDBConnection {
public:
	/**
	* ѡ��
	*/
	virtual RecordSet * execSelect(TableInfo *table, Record *column, const std::string &condition){
		return 0;
	}

	/**
	* ɾ��
	*/
	virtual unsigned int exeDelete(TableInfo* table, const std::string &condition){
		return 0;
	}

	/**
	* ����
	*/
	virtual unsigned int exeUpdate(TableInfo* table, Record* data, const std::string &condition){
		return 0;
	}

	/**
	* ����
	*/
	virtual unsigned long exeInsert(TableInfo* table, Record* rec){
		return 0;
	}

	/**
	* \brief �����ύ
	* \param handleID ���������Ӿ��
	* \return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool commit(){
		return false;
	}


	/**
	* \brief ����ع�
	* \param handleID ���������Ӿ��
	* \return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool rollback(){
		return false;
	}

	/**
	* \brief ���ô������Ƿ�֧������
	* \param handleID ���������Ӿ��
	* \param supportTransactions  �Ƿ�֧������
	* \return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool setTransactions(bool supportTransactions){
		return false;
	}

	/**
	* \brief ���������Ƿ�֧������
	* \param handleID ���������Ӿ��
	* \return ֧�ַ���true�����򷵻�false
	*/
	virtual bool supportTransactions() {
		return false;
	}
};
/**
 * ���ݿ����
 */
class IDB :public Interface<IDB>{
public:
	/**
	 * ��ȡ����
	 */
	virtual IDBConnection *getConnection() = 0;
	
	/**
	 * ��������
	 */
	virtual void putConnection(IDBConnection *conn) = 0;
	
	/**
	 * ��ȡ��
	 */
	virtual TableInfo * getTable(const std::string &name) = 0;
};


/**
 * ������װǰ��ʹ��
 * IDB *db = core::getBean("db");
 * TableInfo * t_test= db->getTable("T_TEST");
 * if (t_test)
 * {
 *		IDBConnection *conn = db->getConnection(); // ��ȡ����
 *		Record col;
 *		col.put("ID");
 *		col.put("NAME");
 *		RecordSet * rs = conn->exeSelect(t_test,&col,"where id=1");
 *		// ��ȡ�Ľ����
 * 		for (int i = 0; i < rs.size();++i)
 *      {
 *			Record *rec = rset->get(i);
 *			rec->get("ID"); // ��ȡID
 *			rec->get("NAME"); // ��ȡNAME
 *		}
 *		db->putConnection(conn); // �ͷ�����
 * }
 */

class mytable {
public:
	/**
	 * ���ݿ�
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
		
		// ����Record
		// ����
		
	}
};

/*********************************************************************
 * ʹ�� (ʹ��spring �� ��ص�ʹ�� �ͺܼ��� ������һЩ���� �������Ȳ岥)
 * IDB *db = ccs::getBean("db"); // ������ö��������ж�ȡ
 * if (mytable<INFO>::attach(db,"T_INFO"))
 * {
 *		INFO info;
 *		mytable<INFO>::insert(info);
 * }
 *********************************************************************
 * ��ʼ��
 * ccs::initFrom("cc-spring.xml"); // Ԥ��������
 *********************************************************************
 * ����
 * <bean name="db" class="MysqlDB" > <!--MysqlDB �ɻ��� Sqlite3-->
 *		<connection url="" port="" username="" passwd="" dbname="" />
 *		<connectionpool size="10" />
 * </bean>
 *********************************************************************/