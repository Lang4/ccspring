#include "ccspring.h"
#include "ccorm.h"
#include "ccarchive.h"
#include "cclog.h"
int main()
{
 
	ccs::initFrom("cc-spring.xml");
	 
	/**
	 * ����DB
	 */
	IDB *db = ccs::getBean<IDB>("db");
	if (db)
	{
		IDBConnection *conn = db->getConnection();
		if (conn)
		{
			TableInfo *table = db->getTable("test");
			Record rec;
			conn->exeInsert(table, &rec);
		}
		db->putConnection(conn);
	}

	/**
	 * ��������
	 **/
	IXML *xml = ccs::getBean<IXML>("xml");
	if (xml)
	{
		if (xml->initFrom("cc-spring.xml"))
		{
			INode *node = xml->firstNode("bean");
			if (node)
			{
				printf("First Bean name is %s\n",node->getStr("name").c_str());
			}
		}
	}
	/**
	 * ������־
	 **/
	ILog * logger = ccs::getBean<ILog>("log");

	logger->debug("Hello,%s a=%d", "world",100);

	/**
	 * ����TCP����
	 */
	// github ��

	/**
	 * �������л�
	 */

	// test archive.cpp


	/**
	 * �����ڴ��
	 */

	
	/**
	 * ����MVC
	 */


	return 0;
}
 