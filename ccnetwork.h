/*************************************************************
 * class IConnection{
 *		// 读写 和 关闭
 *		// 错误反馈
 *};
 * 网络层操作总结
 * class IServer{
 *	// 启动
 *	// 获取连接
 *	// 错误反馈
 * };
 * class IClient{
 *	// 启动
 *	// 获取连接
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

