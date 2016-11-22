#include "ccspring.h"
#include "ccorm.h"
#include "ccarchive.h"
#include "cclog.h"
int main()
{
 
	ccs::initFrom("cc-spring.xml");
	 
	/**
	 * 关于DB
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
	 * 关于配置
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
	 * 关于日志
	 **/
	ILog * logger = ccs::getBean<ILog>("log");

	logger->debug("Hello,%s a=%d", "world",100);

	/**
	 * 关于TCP网络
	 */
	// github 上

	/**
	 * 关于序列化
	 */

	// test archive.cpp


	/**
	 * 关于内存池
	 */

	
	/**
	 * 关于MVC
	 */


	return 0;
}
 