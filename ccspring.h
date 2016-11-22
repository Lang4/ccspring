#pragma once
#include <string>
#include <unordered_map>
#include "ccxml.h"

/**
* ������ֵ�ķ�װ
*/
class AnyValue {
public:
	class IBase{
	public:
		virtual const std::type_info & type() = 0;
		virtual void *value() = 0;
		virtual std::string get_value() = 0;
	};

	AnyValue(const AnyValue &value)
	{
		this->_value = value._value;
		if (value._value.empty())
		{
			this->_valuePoint = 0;
		}
		else
		{
			this->_valuePoint = (IBase*)&this->_value[0];
		}
	}
	AnyValue& operator=(const AnyValue &value)
	{
		this->_value = value._value;
		if (value._value.empty())
		{
			this->_valuePoint = 0;
		}
		else
		{
			this->_valuePoint = (IBase*)&this->_value[0];
		}
		return *this;
	}

	/**
	* ��ֵ ʵ�ֻ����Ĳ���
	*/
	template<typename VALUE>
	class BaseImp : public IBase{
	public:
		BaseImp(const VALUE &_value) :_value(_value) {
		}
		const std::type_info & type()
		{
			return typeid(VALUE);
		}
		void *value()
		{
			return &_value;
		}
		std::string get_value()
		{
			std::stringstream ss;
			ss << _value;
			return ss.str();
		}
	private:
		VALUE _value;
	};

	class StringImp :public IBase{
	public:
		StringImp(const std::string &value) :_value(value){}
		const std::type_info & type()
		{
			return typeid(std::string);
		}
		void *value()
		{
			return &_value[0];
		}
		std::string get_value()
		{
			return _value;
		}
	private:
		std::string _value;
	};

	AnyValue()
	{
		_valuePoint = 0;
	}
	~AnyValue()
	{

	}

	template<typename VALUE>
	AnyValue(const VALUE & value)
	{
		_value.resize(sizeof(BaseImp<VALUE>) + sizeof(int));
		_valuePoint = new(&_value[0])BaseImp<VALUE>(value);
	}

	template<typename VALUE>
	AnyValue&operator=(const VALUE & value)
	{
		_value.resize(sizeof(BaseImp<VALUE>) + sizeof(int));
		_valuePoint = new(&_value[0])BaseImp<VALUE>(value);
		return *this;
	}

	AnyValue(const std::string & value)
	{
		_value.resize(sizeof(StringImp) + sizeof(int));
		_valuePoint = new(&_value[0])StringImp(value);
	}

	AnyValue&operator=(const StringImp & value)
	{
		_value.resize(sizeof(StringImp) + sizeof(int));
		_valuePoint = new(&_value[0])StringImp(value);
		return *this;
	}
	operator unsigned int() const
	{
		std::string value = _valuePoint->get_value();

		return atoi(value.c_str());

	}
	operator double() const
	{
		std::string value = _valuePoint->get_value();

		return atof(value.c_str());

	}

	operator  std::string () const
	{
		return _valuePoint->get_value();
	}
	std::string toString()
	{
		return _valuePoint->get_value();
	}
private:
	std::string _value; // ��ֵ���ڵ�
	IBase *_valuePoint = 0; // ��ֵָ��
};

/**
 * ���������ķ�װ
 */
template<typename T>
class Singleton {
public:
	static T* instance()
	{
		return &o;
	}
private:
	static T o;
};
template<typename T>
T Singleton<T>::o;

/**
 * �����Ĺ����ӿ�
 */
namespace ccs{
	template<typename BEAN>
	BEAN * getBean(const char *name)
	{
		return static_cast<BEAN*>(getVoidBean(name));
	}

	extern void *getVoidBean(const char *name, void*buffer, size_t size);

	extern void *getVoidBean(const char *name);

	extern void putBean(const char *name, void *bean);

	extern void initFrom(const char *filename);
}

 
/**
 * �ӿڵĻ��� �κνӿ���Ҫ�̳иû���
 */
template<typename T>
class Interface {
public:
	T * operator->()
	{
		if (_buffer.empty()) return 0;
		return static_cast<T*>((void*)&_buffer[0]);
	}
	virtual ~Interface()
	{
		_buffer.clear();
	}
	template<typename IMP>
	void setImp()
	{
		_buffer.resize(sizeof(IMP) + sizeof(int));
		IMP * pClass = new(&_buffer[0])IMP;
	}
	void imp(const char *name)
	{
		ccs::getVoidBean(name,&_buffer[0],_buffer.size());
	}
protected:
	std::string _buffer; // ����ʵ���� ��ʹ�� �Ŀռ�
};

/**
 * ʵ�ֵĻ���
 */
class ICreator{
public:
	std::string name;
	virtual void *clone(){ return 0; }
	virtual void *self(){ return this; }
	virtual void* clone_buffer(void *buffer, size_t size){ return 0; }
	/**
	 * ����
	 */
	virtual void configure(INode *node){}

	ICreator *next = 0;
};


template<typename T>
class Creator : public ICreator {
public:
	void*clone()
	{
		return new T();
	}
	virtual void *self(){ return static_cast<T*>(this); }
	virtual void* clone_buffer(void *buffer, size_t size)
	{
		if (size != sizeof(T)) return 0;
		return(void*) new (buffer)T;
	}
};


class ObjectFactory :public Singleton<ObjectFactory> {
public:
	/**
	 * �������ļ��м���Bean
	 */
	void init(const char *filename);
	/**
	 * ��ȡBean
	 */
	void *getBean(const char *name);
	/**
	 * �ͷ�Bean
	 */
	void putBean(const char *name,void *bean);

	/**
	 * ��ȡBean ͨ��clone �ķ�ʽ
	 */
	void *getBean(const char *name, void*buffer, size_t size);

	void setCreator(const char *name, ICreator *creator)
	{
		creator->name = name;
		creator->next = this->creator;
		this->creator = creator;
	}
private:
	struct BeanInfo{
		std::string name; // ����
		ICreator * creator = 0 ; // ������
		enum CREATE_TYPE{
			CREATE_NULL = 0,
			SINGLETON = 1, // �����ķ�ʽ��������
			CLONE = 2, // ��¡�ķ�ʽ�������� 
		};
		int type = SINGLETON;
	};
	std::unordered_map<std::string,std::vector<void *> > _free_beans;
	std::unordered_map<std::string, BeanInfo > _beans;
	std::unordered_map<std::string, ICreator *> _imps;

	/**
	 * ����һ����Bean
	 */
	void *_createBean(const char *name);

	/**
	 * ��ʼ����������
	 */
	void _initCreators();

	static ICreator * creator ;
};

	
template<typename T>
class AutoRegisterCreator{
public:
	AutoRegisterCreator(const char *name)
	{
		ObjectFactory::instance()->setCreator(name, &t);
	}
	T t;
};

#define CREATOR(name,CLASS) AutoRegisterCreator<CLASS> CLASS##T(name) 
