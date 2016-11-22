﻿/**
 * \file
 * \version  $Id: zLogger.h 13 2013-03-20 02:35:18Z zhongjunwei $
 * \author  尹红春,okyhc@263.sina.com
 * \date 2004年11月18日 15时25分57秒 CST
 * 
 * \brief Fir新的日志系统声明文件
 *
 * 本文件是定义了三个类<code>zLogger::zLevel</code>,<code>zLogger</code>,<code>zLogger::zLoggerLocalFileAppender</code>
 */

#ifndef _ZLOGGER_H_
#define _ZLOGGER_H_

#include "cclog.h"
#include <log4cxx/logger.h>
#include <log4cxx/dailyrollingfileappender.h>
/**
 * \def MSGBUF_MAX
 * \brief Fir日志系统定义的最大日志长度 
 */
#define MSGBUF_MAX 4096

//class zServerInfoManager;

/**
 * \brief Fir项目的日志类，以Log4cxx基础构建的。
 *
 * 目前实现了三种写日志方式，即控制台、本地文件和Syslog系统。Syslog的等级为user。
 *
 * 默认日志级别是#DEBUG
 *
 * 此类为线程安全类。
 */
class zLoggerImp : public ILog, public Creator<zLoggerImp>
{		 
	public:
		 
		/**
		 * \brief zLevel声明了几个日志等级
		 *
		 * 除了用log4cxx提供的标准日志等级作为日志等级外，还自定义了游戏日志等级.
		 *
		 * 程序日志等级关系为 #OFF> #FATAL> #ERROR> #WARN> #INFO> #DEBUG> #ALL
		 *
		 * 游戏日志等级关系为 #ALARM> #IFFY> #TRACE> #GBUG
		 *
		 * 游戏日志等级与程序日志等级关系: #ALARM=#ERROR, #IFFY=#WARN, #TRACE=#INFO, #GBUG=#DEBUG
		 */
		class zLevel
		{
			friend class zLoggerImp;
			private:
			/**
			 * \brief Fir项目所支持日志等级数字定义
			 */
			enum zLevelInt
			{
				ALARM_INT	= log4cxx::Level::ERROR_INT,
				IFFY_INT	= log4cxx::Level::WARN_INT,
				TRACE_INT	= log4cxx::Level::INFO_INT,
				GBUG_INT	= log4cxx::Level::DEBUG_INT
			};

			static const log4cxx::LevelPtr LEVELALARM;
			static const log4cxx::LevelPtr LEVELIFFY;
			static const log4cxx::LevelPtr LEVELTRACE;
			static const log4cxx::LevelPtr LEVELGBUG;

			log4cxx::LevelPtr zlevel;
			zLevel(log4cxx::LevelPtr level);

			public:
			/**
			 * \brief 当zLogger等级设置为OFF，除了用#forceLog函数，否则不会输出任何日志
			 */
			static const zLevel * OFF;
			/**
			 * \brief 当zLogger等级设置为FATAL，只输出FATAL等级的日志
			 *
			 * 程序致命错误，已经无法提供正常的服务功能。
			 */
			static const zLevel * FATAL;
			/**
			 * \brief 当zLogger等级设置为ERROR，只输出大于等于此等级的日志
			 *
			 * 错误，可能不能提供某种服务，但可以保证程序正确运行。
			 */
			static const zLevel * ERROR;
			/**
			 * \brief 当zLogger等级设置为ALARM，与ERROR同一级别
			 *
			 * 报警，游戏数据发生错误，比如检测到有外挂，游戏数据异常等等。与ERROR同一级别。
			 */
			static const zLevel * ALARM;
			/**
			 * \brief 当zLogger等级设置为WARN，只输出大于等于此等级的日志
			 *
			 * 警告，某些地方需要引起注意，比如没有配置文件，但程序用默认选项可以使用。
			 */
			static const zLevel * WARN;
			/**
			 * \brief 当zLogger等级设置为IFFY。
			 *
			 * 可疑的，需要追查的一些游戏数据，比如说一个变动的数据超出某种范围。与WARN同一级别。
			 */
			static const zLevel * IFFY;
			/**
			 * \brief 当zLogger等级设置为INFO，只输出大于等于此等级的日志
			 *
			 * 信息，提供一般信息记录，多用于一些程序状态的记录。
			 */
			static const zLevel * INFO;
			/**
			 * \brief 当zLogger等级设置为TRACE
			 *
			 * 	游戏跟踪，几乎要游戏中所有的关键数据跟踪，便于日后查找问题。
			 *
			 * 	比如角色升级，PK死亡，任务跟踪等等游戏事件。与INFO同一级别。
			 */
			static const zLevel * TRACE;
			/**
			 * \brief 当zLogger等级设置为DEBUG，输出所有等级的日志
			 */
			static const zLevel * DEBUG;
			/**
			 * \brief 当zLogger等级设置为GBUG
			 *
			 * 调试用的游戏数据。与DEBUG同一级别。
			 */
			static const zLevel * GBUG;
			/**
			 * \brief 当zLogger等级设置为ALL，输出所有等级的日志
			 */
			static const zLevel * ALL;
		};
		zLoggerImp(){}
		void configure(INode *node);
		zLoggerImp(const log4cxx::String & name);
		~zLoggerImp();

		std::string  getName();
		void setName(const std::string & setName);
		bool addConsoleLog();
		void removeConsoleLog();
		bool addLocalFileLog(const std::string &file);
		bool addLocalFileLogNoName(const log4cxx::String &file);
		bool addDailyLocalFileLog(const log4cxx::String &file);
		bool addDailyLocalFileLogNoTime(const log4cxx::String &file);
		void removeLocalFileLog(const log4cxx::String &file);
		bool addNewDailyLocalFileLog(const log4cxx::String &file);

		void setLevel(const zLevel * zLevelPtr);
		//void setInfoM(zServerInfoManager * const info);
		void setLevel(const std::string &level);
		bool log(const zLevel * zLevelPtr,const char * pattern, ...) ;
		bool forceLog(const zLevel * zLevelPtr,const char * pattern, ...) ;
		bool debug(const char * pattern, ...) ;
		bool error(const char * pattern, ...) ;
		bool errorM(const char * pattern, ...) ;
		bool info(const char * pattern, ...) ;
		bool fatal(const char * pattern, ...) ;
		bool fatalM(const char * pattern, ...) ;
		bool warn(const char * pattern, ...) ;
		bool warnM(const char * pattern, ...) ;
		bool alarm(const char * pattern, ...) ;
		bool iffy(const char * pattern, ...) ;
		bool trace(const char * pattern, ...) ;
		bool gbug(const char * pattern, ...) ;
		bool isgood(){return logger!=NULL?true:false;};

	private:
		/**
		 * \brief 本类只为了能以正确的时间写文件名,而从log4cxx::DailyRollingFileAppender继承实现的
		 *
		 * 增加了#setTimeZone函数和重构了#activateOptions函数,其他的用法请参见log4cxx手册
		 */
		class zLoggerLocalFileAppender:public log4cxx::DailyRollingFileAppender
		{
			private:
				/**
				 * \brief 写日志文件名所用的时区
				 */
				static log4cxx::helpers::TimeZonePtr tz;
				log4cxx::String my_fileName;
				void my_rollOver();
				void subAppend(const log4cxx::spi::LoggingEventPtr& event);
			public:
				zLoggerLocalFileAppender();
				~zLoggerLocalFileAppender();

				void setMyFile(const log4cxx::String& file);
				void setTimeZone(const log4cxx::String &timeZone);
				void activateOptions();
		};

		class PowerLogger:public log4cxx::Logger
		{
			public:
				void setName(const log4cxx::String  &s)
				{
					name=s;
				}
				
			protected:
				PowerLogger(const log4cxx::String &s):Logger(s) { }

		};

		log4cxx::LoggerPtr logger;
};
 
#endif
