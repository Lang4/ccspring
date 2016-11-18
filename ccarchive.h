#pragma once
#include <map>
#include <string>
#include <sstream>
#include <typeinfo>

class IStream{
public:
	IStream() 
	{
		
	}
	/**
	* 基本数值的封装
	*/
	class AnyValue {
	public:

		/**
		* 基本的数值操作封装
		* 类型
		* 数值指针
		* 向stream 转化
		*/
		class IBase{
		public:
			virtual const std::type_info & type() = 0;
			virtual void *value() = 0;
			virtual void getstream(IStream &ss) = 0;
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
			void getstream(IStream &ss)
			{
				ss << _value;
			}
		private:
			VALUE _value;
		};

		class BaseImp_IStream : public IBase{
		public:
			BaseImp_IStream(const IStream &_value)   {
				this->_value = _value.print();
			}
			const std::type_info & type()
			{
				return typeid(BaseImp_IStream);
			}
			void *value()
			{
				return &_value[0];
			}
			void getstream(IStream &ss)
			{
				ss.parse(_value);
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
			_valuePoint = 0;
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

		AnyValue(const IStream & value)
		{
			_value.resize(sizeof(BaseImp_IStream) + sizeof(int));
			_valuePoint = new(&_value[0])BaseImp_IStream(value);
		}

		AnyValue&operator=(const IStream & value)
		{
			_value.resize(sizeof(BaseImp_IStream) + sizeof(int));
			_valuePoint = new(&_value[0])BaseImp_IStream(value);
			return *this;
		}

		template<typename T>
		operator T() const
		{
			if (_valuePoint && _valuePoint->type() == typeid(T))
			{
				return *static_cast<T*>(_valuePoint->value());
			}
			else
			{
				T value;
				IStream ss;
				if (_valuePoint)
					_valuePoint->getstream(ss);
				ss >> value;
				return value;
			}
		}
		

		operator std::string  () const {
			return str();
		}
		std::string str() const
		{
			std::string value;
			IStream ss;
			if (_valuePoint)
				_valuePoint->getstream(ss);
			ss >> value;
			return value;
		}
		IStream toStream()
		{
			IStream ss;
			if (_valuePoint)
			{
				_valuePoint->getstream(ss);
			}
			return ss;
		}
	private:
		std::string _value; // 数值所在地
		IBase *_valuePoint; // 数值指针
	};
	void set(AnyValue value,const std::string& tag)
	{
		_stream[tag] = value.toStream();
	}
	AnyValue get(const std::string&  tag) const
	{
		AnyValue value;
		auto it = _stream.find(tag);
		if (it != _stream.end())
		{
			value = it->second;
		}
		return value;
	}
	
	template<typename T>
	T get_base()
	{
		std::stringstream ss;
		ss << _content;
		T  t;
		ss >> t;
		return t;
	}
	template<typename T>
	void set_base(T &t)
	{
		std::stringstream ss;
		ss << t;
		_content = ss.str();
	}
	std::string print() const
	{
		if (_stream.empty())
		{
			return _content;
		}
		std::stringstream ss;
		if (_content != "")
			ss << _content << "{";
		else
			ss << "{";
		for (auto i : _stream)
		{
			ss << "["<<i.first << ":" << i.second.print() << "]";
		}
		ss << "}";
		return ss.str();
	}
	void parse(const std::string &content)
	{
		unsigned int index = 0;
		_content.clear();
		_stream.clear();
		while (index < content.size() && content[index] != '{')
		{
			_content.push_back(content[index]);
			++ index;
		}
 
		if (content[index] == '{')
		{
			++index;

			while (index < content.size() && content[index] != '}')
			{
				++index; // '['
				std::string key;
				std::string value;
				IStream stream;
				while (index < content.size() && content[index] != ':')
				{
					key.push_back(content[index]);
					++index;
				}
	 
				++index; // ':'

				while (index < content.size() && content[index] != ']')
				{
					value.push_back(content[index]);
					++index;
				}
			 
				stream.parse(value);

				_stream[key] = stream;
				
				++index; // ']'
			}

			++index; // '}'
		}

	}
	void set_name(const std::string &name)
	{
		_content = name;
	}
	std::string &name()
	{
		return _content;
	}
private:
	std::string _content;
	std::map<std::string,IStream> _stream;

	template<typename TOBJECT>
	friend IStream& operator<<(IStream& out, TOBJECT& s);
	template<typename TOBJECT>
	friend IStream& operator>>(IStream& in, TOBJECT& s);

	friend IStream& operator<<(IStream& out, std::string& s);
	friend IStream& operator>>(IStream& in, std::string& s);
};

/**
* 基本单位的序列化
*/
template<typename TOBJECT>
IStream& operator<<(IStream& out, TOBJECT& s)
{
	out.set_base(s);
	return out;
}
template<typename TOBJECT>
IStream& operator>>(IStream& in, TOBJECT& s)
{
	s = in.get_base<TOBJECT>();
	return in;
}

/**
 * 使用封装
 */
template<typename OBJECT>
class Archive{
public:
	enum INOUT{
		IN,
		OUT,
	};
protected:
	virtual void __bind__fields(IStream &io,INOUT tag) = 0;

	void __bind(IStream &io, INOUT tag, std::string &str, const char *key)
	{
		if (tag == OUT)
		{
			io.set( str,key);
		}
		else
		{
			IStream::AnyValue a = io.get(key);
			str = (std::string) a;
		}
	}
	template<typename T>
	void __bind(IStream &io, INOUT tag, T &a, const char *key)
	{
		if (tag == OUT)
		{
			io.set( a,key);
		}
		else
		{
			a = io.get(key);
		}
	}
};

#define DEC_ARCHIVE_OBJECT()\
void __bind__fields(IStream &io, INOUT tag);

#define IMP_ARCHIVE_OBJECT(OBJECT)\
IStream& operator<<(IStream& out, OBJECT& s)\
{ \
	out.set_name(#OBJECT);\
	s.__bind__fields(out, OBJECT::OUT);\
	return out;\
} \
IStream& operator>>(IStream& in, OBJECT& s)\
{\
	s.__bind__fields(in, OBJECT::IN);\
	return in;\
}\
void OBJECT::__bind__fields(IStream &io, OBJECT::INOUT tag)


#define BIND_FIELD(FIELD) __bind(io,tag,FIELD,#FIELD)