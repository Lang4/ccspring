#pragma once
#include "ccspring.h"

/**
 * 定义的日志模块
 */
class ILog{
public:
	virtual bool trace(const char * pattern, ...) = 0;
	virtual bool debug(const char * pattern, ...) = 0;
};