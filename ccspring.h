#pragma once
#include <string>
#include <unordered_map>
#include "ccxml.h"

/**
* 基本数值的封装
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
	* 数值 实现基本的操作
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
	std::string _value; // 数值所在地
	IBase *_valuePoint = 0; // 数值指针
};

/**
 * 基本单例的封装
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
 * 基本的工厂接口
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
 * 接口的基类 任何接口需要继承该基类
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
	std::string _buffer; // 具体实现类 所使用 的空间
};

/**
 * 实现的基类
 */
class ICreator{
public:
	std::string name;
	virtual void *clone(){ return 0; }
	virtual void *self(){ return this; }
	virtual void* clone_buffer(void *buffer, size_t size){ return 0; }
	/**
	 * 配置
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
	 * 从配置文件中加载Bean
	 */
	void init(const char *filename);
	/**
	 * 获取Bean
	 */
	void *getBean(const char *name);
	/**
	 * 释放Bean
	 */
	void putBean(const char *name,void *bean);

	/**
	 * 获取Bean 通过clone 的方式
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
		std::string name; // 名字
		ICreator * creator = 0 ; // 创建者
		enum CREATE_TYPE{
			CREATE_NULL = 0,
			SINGLETON = 1, // 单例的方式产生对象
			CLONE = 2, // 克隆的方式产生对象 
		};
		int type = SINGLETON;
	};
	std::unordered_map<std::string,std::vector<void *> > _free_beans;
	std::unordered_map<std::string, BeanInfo > _beans;
	std::unordered_map<std::string, ICreator *> _imps;

	/**
	 * 创建一个新Bean
	 */
	void *_createBean(const char *name);

	/**
	 * 初始化创建者们
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
