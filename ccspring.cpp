#include "ccspring.h"
#include "stdio.h"
#include "ccxml.h"


namespace ccs{
	/**
	 * ��ȡ��Bean���������Ķ���
	 */
	void *getVoidBean(const char *name)
	{
		return ObjectFactory::instance()->getBean(name);
	}
	void putBean(const char *name, void *bean)
	{
		ObjectFactory::instance()->putBean(name, bean);
	}

	void *getVoidBean(const char *name, void*buffer, size_t size)
	{
		return ObjectFactory::instance()->getBean(name, buffer, size);
	}
	void initFrom(const char *filename)
	{
		ObjectFactory::instance()->init(filename);
	}
}

/**
* �������ļ��м���Bean
*/
void ObjectFactory::init(const char *filename)
{
	_initCreators();
	IXML xml;
	xml.imp(); // ����Ĭ��ʵ��

	if (xml->initFrom(filename))
	{
		INode * node = xml->firstNode("bean");
		while (node)
		{
			BeanInfo info;
			info.name = node->getStr("name");
			info.type = node->get<int>("creatortype");
			info.creator = _imps[node->getStr("class")];
			if (info.creator)
			{
				info.creator->configure(node);
				_beans[info.name] = info;
			}
			node = node->nextNode("bean");
		}
	}
}
/**
* ��ȡBean
*/
void *ObjectFactory::getBean(const char *name)
{
	auto it = _beans.find(name);
	if (it != _beans.end())
	{
		if (it->second.type == BeanInfo::SINGLETON)
		{
			if (it->second.creator)
				return it->second.creator->self();
		}
		else if (it->second.type == BeanInfo::CLONE)
		{
			auto free_bean_it = _free_beans.find(name);
			if ( free_bean_it != _free_beans.end())
			{
				if (free_bean_it->second.size())
				{
					void * p = free_bean_it->second.back();

					free_bean_it->second.pop_back();
					return p;
				}
			}
			if (it->second.creator)
				return it->second.creator->clone();
		}
	}
	return 0;
}
/**
* ��ȡBean ͨ��clone �ķ�ʽ
*/
void *ObjectFactory::getBean(const char *name, void*buffer, size_t size)
{
	auto it = _beans.find(name);
	if (it != _beans.end())
	{
		if (it->second.creator)
			return it->second.creator->clone_buffer(buffer, size);
	}
	return 0;
}
/**
* �ͷ�Bean
*/
void ObjectFactory::putBean(const char *name,void *bean)
{
	auto it = _beans.find(name);
	if (it != _beans.end())
	{
		if (it->second.type == BeanInfo::CLONE)
		{
			_free_beans[name].push_back(bean);
		}
	}
}

ICreator * ObjectFactory::creator = 0;

void ObjectFactory::_initCreators()
{
	while (creator)
	{
		_imps[creator->name] = creator;
		creator = creator->next;
	}
}