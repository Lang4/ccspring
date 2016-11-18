#include "ccspring.h"
#include "ccorm.h"
#include "ccarchive.h"
 
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

	/**
	 * 关于日志
	 **/

	/**
	 * 关于TCP网络
	 */
	

	/**
	 * 关于序列化
	 */


	/**
	 * 关于内存池
	 */

	
	/**
	 * 关于MVC
	 */


	return 0;
}
 