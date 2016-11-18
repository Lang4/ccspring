#include "ccspring.h"
#include "ccorm.h"
#include "ccarchive.h"
 
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

	/**
	 * ������־
	 **/

	/**
	 * ����TCP����
	 */
	

	/**
	 * �������л�
	 */


	/**
	 * �����ڴ��
	 */

	
	/**
	 * ����MVC
	 */


	return 0;
}
 