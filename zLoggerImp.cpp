/**
 * \file
 * \version  $Id: zLoggerImp.cpp 13 2013-03-20 02:35:18Z zhongjunwei $
 * \author  尹红春,okyhc@263.sina.com
 * \date 2004年11月18日 16时26分14秒 CST
 * \brief Fir项目日志系统定义文件
 *
 */

#include "zLoggerImp.h"
#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/helpers/dateformat.h>
#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/helpers/system.h>
#include <time.h>


const log4cxx::LevelPtr zLoggerImp::zLevel::LEVELALARM( new log4cxx::Level(ALARM_INT,_T("ALARM"),3));
const log4cxx::LevelPtr zLoggerImp::zLevel::LEVELIFFY(new log4cxx::Level(IFFY_INT, _T("IFFY"), 3));
const log4cxx::LevelPtr zLoggerImp::zLevel::LEVELTRACE(new log4cxx::Level(TRACE_INT, _T("TRACE"), 3));
const log4cxx::LevelPtr zLoggerImp::zLevel::LEVELGBUG(new log4cxx::Level(GBUG_INT, _T("GBUG"), 3));

const zLoggerImp::zLevel *  zLoggerImp::zLevel::OFF = new zLevel(log4cxx::Level::OFF);
const zLoggerImp::zLevel *  zLoggerImp::zLevel::FATAL = new zLevel(log4cxx::Level::FATAL);
const zLoggerImp::zLevel *  zLoggerImp::zLevel::ALARM = new zLevel(LEVELALARM);
const zLoggerImp::zLevel *  zLoggerImp::zLevel::ERROR = new zLevel(log4cxx::Level::ERROR);
const zLoggerImp::zLevel *  zLoggerImp::zLevel::IFFY = new zLevel(LEVELIFFY);
const zLoggerImp::zLevel *  zLoggerImp::zLevel::WARN = new zLevel(log4cxx::Level::WARN);
const zLoggerImp::zLevel *  zLoggerImp::zLevel::TRACE = new zLevel(LEVELTRACE);
const zLoggerImp::zLevel *  zLoggerImp::zLevel::INFO = new zLevel(log4cxx::Level::INFO);
const zLoggerImp::zLevel *  zLoggerImp::zLevel::GBUG = new zLevel(LEVELGBUG);
const zLoggerImp::zLevel *  zLoggerImp::zLevel::DEBUG = new zLevel(log4cxx::Level::DEBUG);
const zLoggerImp::zLevel *  zLoggerImp::zLevel::ALL = new zLevel(log4cxx::Level::ALL);

/**
 * \brief zLevel构造函数
 * \param  level 等级数字，类内部定义
 */
zLoggerImp::zLevel::zLevel(log4cxx::LevelPtr level):zlevel(level)
{
}


log4cxx::helpers::TimeZonePtr zLoggerImp::zLoggerLocalFileAppender::tz(log4cxx::helpers::TimeZone::getDefault());

/**
 * \brief 构造一个本地文件Appender 
 */
zLoggerImp::zLoggerLocalFileAppender::zLoggerLocalFileAppender()
{
}

/**
 * \brief 析构时，回收DateFormat内存
 */
zLoggerImp::zLoggerLocalFileAppender::~zLoggerLocalFileAppender()
{
	delete(df);
}

#define remove_and_link(oldpath, newpath) \
	do { \
		remove(T2A(newpath.c_str())); \
		symlink(T2A(oldpath.c_str()), T2A(newpath.c_str())); \
	} while(false)

void zLoggerImp::zLoggerLocalFileAppender::my_rollOver()
{
	/* Compute filename, but only if datePattern is specified */
	if (datePattern.empty())
	{
		errorHandler->error(_T("Missing DatePattern option in my_rollOver()."));

		return;
	}

	log4cxx::String datedFilename = my_fileName + df->format(now);

	// It is too early to roll over because we are still within the
	// bounds of the current interval. Rollover will occur once the
	// next interval is reached.
	if (scheduledFilename == datedFilename)
	{
		return;
	}

	// close current file, and rename it to datedFilename
	this->closeWriter();

	try
	{
		// This will also close the file. This is OK since multiple
		// close operations are safe.
		// append to the file, not clear the old content
		this->setFile(datedFilename, this->fileAppend, this->bufferedIO, this->bufferSize);

		//remove and link
		//remove_and_link(datedFilename, my_fileName);
	}
	catch (log4cxx::helpers::Exception&)
	{
		errorHandler->error(_T("setFile(") + datedFilename + _T(", false) call failed."));
	}

	scheduledFilename = datedFilename;
}

void zLoggerImp::zLoggerLocalFileAppender::subAppend(const log4cxx::spi::LoggingEventPtr& event)
{
	int64_t n = log4cxx::helpers::System::currentTimeMillis();

	if (n >= nextCheck) 
	{
		now = n;
		nextCheck = rc.getNextCheckMillis(now);

		try 
		{
			my_rollOver();
		} 
		catch (log4cxx::helpers::Exception& e)
		{
			log4cxx::helpers::LogLog::error(_T("my_rollOver() failed."), e);
		}
	}

	FileAppender::subAppend(event);
}

void zLoggerImp::zLoggerLocalFileAppender::setMyFile(const log4cxx::String& file)
{
	// Trim spaces from both ends. The users probably does not want
	// trailing spaces in file names.
	my_fileName = log4cxx::helpers::StringHelper::trim(file);
}

/**
 * \brief 设置时区
 *
 * \param timeZone 时区字符串 
 */
void zLoggerImp::zLoggerLocalFileAppender::setTimeZone(const log4cxx::String &timeZone)
{
	std::string tzstr;
	//save the timezone infomation
	//zRTime::save_timezone(tzstr);
	tz=log4cxx::helpers::TimeZone::getTimeZone(timeZone);
	//restore the timezone infomation
	//zRTime::restore_timezone(tzstr);
}

/**
 * \brief 激活所设置的选项
 */
void zLoggerImp::zLoggerLocalFileAppender::activateOptions()
{
	//设置时区和预定文件
	rc.setTimeZone(tz);
	//DailyRollingFileAppender
	if (!datePattern.empty())
	{
		std::string tzstr;
		//save the timezone infomation
		//zRTime::save_timezone(tzstr);
		rc.setType(rc.computeTriggeringPeriod(datePattern));
		//restore the timezone infomation
		//zRTime::restore_timezone(tzstr);

		rc.printPeriodicity();

		now = log4cxx::helpers::System::currentTimeMillis();
		df = new log4cxx::helpers::DateFormat(datePattern, tz);

		//FileAppender
		if (!my_fileName.empty())
		{
			scheduledFilename = my_fileName + df->format(now);

		
			this->setFile(scheduledFilename, this->fileAppend, this->bufferedIO, this->bufferSize);

		}
		else
		{
			log4cxx::helpers::LogLog::warn(_T("File option not set for appender [")+name+_T("]."));
			log4cxx::helpers::LogLog::warn(_T("Are you using FileAppender instead of ConsoleAppender?"));
		}
	}
	else
	{
		log4cxx::helpers::LogLog::error(
				_T("DatePattern options are not set for appender [")
				+ name + _T("]."));
	}
}

/**
 * \brief 构造一个zLoggerImp 
 *
 * \param  name zLoggerImp的名字，将会出现在输出的日志中的每一行
 */
zLoggerImp::zLoggerImp(const log4cxx::String &name)
{
	///std::cout << __PRETTY_FUNCTION__ << std::endl;
	logger = log4cxx::Logger::getLogger(name);
	logger->setLevel(log4cxx::Level::DEBUG);
	//m_info = NULL;
	addConsoleLog();
}
void zLoggerImp::configure(INode *node)
{
	INode *child = node->childNode("info");
	if (child)
	{
		this->name = child->getStr("name");
	}
	logger = log4cxx::Logger::getLogger(name);
	logger->setLevel(log4cxx::Level::DEBUG);
	//m_info = NULL;
	addConsoleLog();
}
/**
 * \brief 析构函数
 */
zLoggerImp::~zLoggerImp()
{
}

/**
 * \brief 得到Logger的名字，它出现在每条日志信息中
 * \return	Logger名字
 */
std::string zLoggerImp::getName()
{
	return logger->getName();
}

/**
 * \brief 设置Logger的名字，它出现在每条日志信息中
 * \param setName 要被设置的名字
 */
void zLoggerImp::setName(const std::string & setName)
{
	//PowerLogger *log=(PowerLogger *)logger;
	//PowerLogger * pl=logger.p;
	((PowerLogger *)logger.p)->setName(setName);
}

/**
 * \brief 添加控制台输出Log
 * \return	成功返回true，否则返回false 
 */
bool zLoggerImp::addConsoleLog()
{
	log4cxx::PatternLayoutPtr cpl = new log4cxx::PatternLayout("\%d{\%y\%m\%d-\%H:\%M:\%S }%c %5p: %m%n");
	std::string s;
//	zRTime::getLocalTZ(s);
	cpl->setTimeZone(s);
	cpl->activateOptions();

	log4cxx::ConsoleAppenderPtr carp = new log4cxx::ConsoleAppender(cpl);
	carp->setName("console");

	logger->addAppender(carp);
	return true;
}

/**
 * \brief 移除控制台Log输出
 */
void zLoggerImp::removeConsoleLog()
{
	log4cxx::AppenderPtr ap=logger->getAppender("console");
	logger->removeAppender(ap);
	ap->close();
}

/**
 * \brief 加一个本地文件Log输出
 *
 * \param file 要输出的文件名，Logger会自动地添加时间后缀 
 * \return 成功返回true，否则返回false
 */
bool zLoggerImp::addLocalFileLog(const std::string &file)
{
	log4cxx::PatternLayoutPtr fpl = new log4cxx::PatternLayout("\%d{\%y\%m\%d-\%H:\%M:\%S }%c %5p: %m%n");
	std::string s;
//	zRTime::getLocalTZ(s);
	fpl->setTimeZone(s);
	fpl->activateOptions();

	zLoggerLocalFileAppender * farp = new  zLoggerLocalFileAppender();
	farp->setDatePattern(".\%y\%m\%d-\%H");
	farp->setTimeZone(s);
	farp->setMyFile(file);
	farp->setLayout(fpl);
	farp->activateOptions();
	farp->setName("localfile:"+file);

	logger->addAppender(farp);
	return true;
}

bool zLoggerImp::addLocalFileLogNoName(const log4cxx::String &file)
{
	log4cxx::PatternLayoutPtr fpl = new log4cxx::PatternLayout("\%d{\%Y/\%m/\%d \%H:\%M:\%S},%m%n");
	std::string s;
//	zRTime::getLocalTZ(s);
	fpl->setTimeZone(s);
	fpl->activateOptions();

	zLoggerLocalFileAppender * farp = new  zLoggerLocalFileAppender();
	farp->setDatePattern(".\%y\%m\%d-\%H");
	farp->setTimeZone(s);
	farp->setMyFile(file);
	farp->setLayout(fpl);
	farp->activateOptions();
	farp->setName("localfile:"+file);

	logger->addAppender(farp);
	return true;
}


bool zLoggerImp::addNewDailyLocalFileLog(const log4cxx::String &file)
{
	log4cxx::PatternLayoutPtr fpl = new log4cxx::PatternLayout("%m%n");
	std::string s;
//	zRTime::getLocalTZ(s);
    fpl->setTimeZone(s);
    fpl->activateOptions();

    zLoggerLocalFileAppender * farp = new  zLoggerLocalFileAppender();
    farp->setDatePattern(".\%y\%m\%d-%H");
    farp->setTimeZone(s);
    farp->setMyFile(file);
    farp->setLayout(fpl);
    farp->activateOptions();
    farp->setName("localfile:"+file);

    logger->addAppender(farp);
    return true;
}
/**
 * \brief 每天的日志输出
 *
 *
 */
bool zLoggerImp::addDailyLocalFileLog(const log4cxx::String &file)
{
    //log4cxx::PatternLayoutPtr fpl = new log4cxx::PatternLayout("\%d{\%y\%m\%d-\%H:\%M:\%S }%c %5p: %m%n");
    log4cxx::PatternLayoutPtr fpl = new log4cxx::PatternLayout("\%d{\%Y/\%m/\%d \%H:\%M:\%S}%c,%m%n");
	std::string s;
//	zRTime::getLocalTZ(s);
    fpl->setTimeZone(s);
    fpl->activateOptions();

    zLoggerLocalFileAppender * farp = new  zLoggerLocalFileAppender();
    //farp->setDatePattern(".\%y\%m\%d-\%H");
    farp->setDatePattern(".\%y\%m\%d");
    farp->setTimeZone(s);
    farp->setMyFile(file);
    farp->setLayout(fpl);
    farp->activateOptions();
    farp->setName("localfile:"+file);

    logger->addAppender(farp);
    return true;
}

bool zLoggerImp::addDailyLocalFileLogNoTime(const log4cxx::String &file)
{
    //log4cxx::PatternLayoutPtr fpl = new log4cxx::PatternLayout("\%d{\%y\%m\%d-\%H:\%M:\%S }%c %5p: %m%n");
    log4cxx::PatternLayoutPtr fpl = new log4cxx::PatternLayout("%m%n");
	std::string s;
//	zRTime::getLocalTZ(s);
    fpl->setTimeZone(s);
    fpl->activateOptions();

    zLoggerLocalFileAppender * farp = new  zLoggerLocalFileAppender();
    //farp->setDatePattern(".\%y\%m\%d-\%H");
    farp->setDatePattern(".\%y\%m\%d");
    farp->setTimeZone(s);
    farp->setMyFile(file);
    farp->setLayout(fpl);
    farp->activateOptions();
    farp->setName("localfile:"+file);

    logger->addAppender(farp);
    return true;
}

/**
 * \brief 移出一个本地文件Log输出
 * \param file 要移除的Log文件名 
 */
void zLoggerImp::removeLocalFileLog(const log4cxx::String &file)
{
	log4cxx::AppenderPtr ap=logger->getAppender("localfile:"+file);
	logger->removeAppender(ap);
	ap->close();
}

#define getMessage(msg,msglen,pat)	\
char msg[msglen];\
do	\
{  va_list ap;	\
	memset(msg,0, msglen);	\
	va_start(ap, pat);		\
	vsnprintf(msg, msglen - 1, pat, ap);	\
	va_end(ap);	\
}while(false)

/**
 * \brief 设置写日志等级
 * \param  zLevelPtr 日志等级.参见 #zLoggerImp::zLevel
 */
void zLoggerImp::setLevel(const zLevel * zLevelPtr)
{
	logger->setLevel(zLevelPtr->zlevel);
}

 

/**
 * \brief 设置写日志等级
 * \param  level 日志等级
 */
void zLoggerImp::setLevel(const std::string &level)
{
	if ("off" == level)
		setLevel(zLevel::OFF);
	else if ("fatal" == level)
		setLevel(zLevel::FATAL);
	else if ("alarm" == level)
		setLevel(zLevel::ALARM);
	else if ("error" == level)
		setLevel(zLevel::ERROR);
	else if ("iffy" == level)
		setLevel(zLevel::IFFY);
	else if ("warn" == level)
		setLevel(zLevel::WARN);
	else if ("trace" == level)
		setLevel(zLevel::TRACE);
	else if ("info" == level)
		setLevel(zLevel::INFO);
	else if ("gbug" == level)
		setLevel(zLevel::GBUG);
	else if ("debug" == level)
		setLevel(zLevel::DEBUG);
	else if ("all" == level)
		setLevel(zLevel::ALL);
}

/**
 * \brief 写日志
 * \param  zLevelPtr 日志等级参见 #zLoggerImp::zLevel
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLoggerImp::log(const zLevel * zLevelPtr,const char * pattern, ...)
{
 
	getMessage(message,MSGBUF_MAX,pattern);

	logger->log(zLevelPtr->zlevel,message);
 
	return true;
}

/**
 * \brief 强制写日志,不受日志等级限制
 * \param  zLevelPtr 日志等级参见 #zLoggerImp::zLevel
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLoggerImp::forceLog(const zLevel * zLevelPtr,const char * pattern, ...)
{

	getMessage(message,MSGBUF_MAX,pattern);

	logger->forcedLog(zLevelPtr->zlevel,message);
 
	return true;
}

/**
 * \brief 写fatal程序日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLoggerImp::fatal(const char * pattern, ...)
{
 
	getMessage(message,MSGBUF_MAX,pattern);

	logger->fatal(message);
 
	return true;
}

/**
 * \brief 写fatal程序日志,并发送到监控服务器
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLoggerImp::fatalM(const char * pattern, ...)
{
 
	getMessage(message,MSGBUF_MAX,pattern);
	
	logger->fatal(message);
 
	return true;
}
/**
 * \brief 写error程序日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLoggerImp::error(const char * pattern, ...)
{
 
	getMessage(message,MSGBUF_MAX,pattern);

	logger->error(message);
 
	return true;
}

/**
 * \brief 写error程序日志,并发送到监控服务器
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLoggerImp::errorM(const char * pattern, ...)
{
 
	getMessage(message,MSGBUF_MAX,pattern);
 
	
	logger->error(message);
 
	return true;
}

/**
 * \brief 写warn程序日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLoggerImp::warn(const char * pattern, ...)
{
	getMessage(message,MSGBUF_MAX,pattern);

	logger->warn(message);
	return true;
}

/**
 * \brief 写warn程序日志,并发送到监控服务器
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLoggerImp::warnM(const char * pattern, ...)
{
	getMessage(message,MSGBUF_MAX,pattern);
	
	logger->warn(message);
	return true;
}

/**
 * \brief 写info程序日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLoggerImp::info(const char * pattern, ...)
{
	getMessage(message,MSGBUF_MAX,pattern);

	logger->info(message);
	return true;
}

/**
 * \brief 写debug程序日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLoggerImp::debug(const char * pattern, ...)
{
	getMessage(message,MSGBUF_MAX,pattern);

	logger->debug(message);
	return true;
}

/**
 * \brief 写alarm游戏日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLoggerImp::alarm(const char * pattern, ...)
{
	getMessage(message,MSGBUF_MAX,pattern);

	logger->log(zLevel::ALARM->zlevel,message);
	return true;
}

/**
 * \brief 写iffy游戏日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLoggerImp::iffy(const char * pattern, ...)
{
 
	getMessage(message,MSGBUF_MAX,pattern);

	logger->log(zLevel::IFFY->zlevel,message);
 
	return true;
}

/**
 * \brief 写trace游戏日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLoggerImp::trace(const char * pattern, ...)
{
 
	getMessage(message,MSGBUF_MAX,pattern);

	logger->log(zLevel::TRACE->zlevel,message);
 
	return true;
}

/**
 * \brief 写gbug游戏日志
 * \param  pattern 输出格式范例，与printf一样
 * \return 成功返回true，否则返回false
 */
bool zLoggerImp::gbug(const char * pattern, ...)
{
 
	getMessage(message,MSGBUF_MAX,pattern);

	logger->log(zLevel::GBUG->zlevel,message);
 
	return true;
}

 