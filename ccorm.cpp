#include "ccorm.h"

FieldInfo * TableInfo::getField(unsigned int index)
{
	if (index < _fields.size())
	{
		return &_fields[index];
	}
	return 0;
}

bool TableInfo::addField(int type, const std::string& name)
{
	FieldInfo field;
	field.type = type;
	field.name = name;
	_fields.push_back(field);
	return true;
}
 

void Record::put(const char *fieldName)
{
	_values[fieldName] = 0;
}

void  Record::put(const char* fieldName, const AnyValue& value)
{
	_values[fieldName] = value;
}
void  Record::put(const char* fieldName, const char* value, unsigned int size)
{
	std::string data;
	data.resize(size);
	memcpy(&data[0], value, size);
	_values[fieldName] = data;
}
bool  Record::find(const std::string& fieldName)
{
	auto it = _values.find(fieldName);
	if (it != _values.end())
	{
		return true;
	}
	return false;
}