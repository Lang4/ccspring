/*************************************************************
 * class IConnection{
 *		// ��д �� �ر�
 *		// ������
 *};
 * ���������ܽ�
 * class IServer{
 *	// ����
 *	// ��ȡ����
 *	// ������
 * };
 * class IClient{
 *	// ����
 *	// ��ȡ����
 *};
 ************************************************************
 * <bean name="serverconnection" class="CCSConnection" genType="new"/>
 * <bean name="server" class="CCSServer" genType="singleton">
 *		<conn bean="serverconnection"/>
 * </bean>
 * <bean name="clientconnection" class="CCClientConnection"/>
 ************************************************************
 * <bean name="client" class="CCSClient" genType="new">
 *		<conn bean="clientconnection"/>
 * </bean>
 ************************************************************/

