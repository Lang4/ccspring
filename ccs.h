#pragma once

/**
 cc-spring 
	1、使用依赖注入的方式 实现的C++的轻量级服务器框架 可以热插播 支持各个基础模块的动态变更，为不同方案的集成提供基础机制，不重复创建轮子，同时提供轻量化的轮子
	2、本框架也使用 cc-mvc 框架来做功能开发
	3、系统的轻量化构建包含 1. 内存管理 2.归档 3.网络模块 4.sqlite3 4.mysql 5.xml 6.protobuf 等
 **/
#include "ccspring.h"
#include "ccmemory.h"
#include "ccorm.h"
#include "ccarchive.h"
#include "ccnetwork.h"
