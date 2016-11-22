/*************************************************
 * XML�ķ�װ
 * 
 *	IXML *xml = ccs::getBean("corexml");
 *	if (xml->initFrom(""))
 *	{
 *		INode *node = xml->firstNode("");
 *		while (node)
 *		{
 *			float a = node->get("name");
 *
 *			node = xml->nextNode("");
 *		}
 *	}
 *	ccs::putBean(xml);
 ************************************************
 *
 */

#pragma once
#include <string>
#include <sstream>

class INode{
public:
	/**
	 * ƽ�нڵ����һ���ڵ�
	 */
	virtual INode*  nextNode(const char *name){
		return 0;
	}
	/**
	 * ��ȡ�ڵ�����
	 */
	virtual std::string getStr(const char *name) {
		return 0;
	}
	template<typename T>
	T get(const char *name)
	{
		std::string value = getStr(name);
		std::stringstream ss;
		ss << value;
		T t;
		ss >> t;
		return t;
	}
	/**
	 * ��ȡ�ӽڵ�
	 */
	virtual INode* childNode(const char *name) {
		return 0;
	}
};

class IXML{
public:
	virtual bool initFrom(const char *name) = 0;
	virtual INode* firstNode(const char *name) = 0;
};

class XML : public IXML{
public:
	XML * operator->()
	{
		if (_buffer.empty()) return 0;
		return static_cast<XML*>((void*)&_buffer[0]);
	}
	virtual bool initFrom(const char *name) {
		return false;
	}
	/**
	 * ��ȡ�ĵ���һ��ֵ
	 */
	virtual INode* firstNode(const char *name){
		return 0;
	}
	void imp();

	template<typename IMP>
	void setImp()
	{
		_buffer.resize(sizeof(IMP) + sizeof(int));
		IMP * pClass = new(&_buffer[0])IMP;
	}
protected:
	std::string _buffer; // ����ʵ���� ��ʹ�� �Ŀռ�
};
