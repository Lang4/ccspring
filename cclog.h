#pragma once
#include "ccspring.h"

/**
 * �������־ģ��
 */
class ILog{
public:
	virtual bool trace(const char * pattern, ...) = 0;
	virtual bool debug(const char * pattern, ...) = 0;
};