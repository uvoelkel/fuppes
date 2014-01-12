/*
 * This file is part of fuppes
 *
 * (c) 2013-2014 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Log.h"

#include "LoggerFile.h"
#include "LoggerStdout.h"
#include "LoggerSyslog.h"

#include "../Thread/Thread.h"

#undef log

Log::Logger::Type Log::Log::m_loggerType = Logger::Stdout;
int Log::Log::m_loggerLevel = Log::Log::normal | Log::Log::warning | Log::Log::error;

void Log::Log::setType(Logger::Type type) // static
{
	Log::Log::m_loggerType = type;
}

void Log::Log::setLevel(const int level) // static
{
	Log::Log::m_loggerLevel = level | Log::Log::warning | Log::Log::error;
}

Log::Log::Log(const std::string sender, const Log::Level level, const std::string file, const int line)
{
	m_currentLevel = level;

	switch (Log::Log::m_loggerType) {
		case Logger::Null:
			m_logger = NULL;
			break;
		case Logger::Stdout:
			m_logger = new LoggerStdout(Threading::Thread::currentThreadId(), sender, file, line);
			break;
		case Logger::Syslog:
			m_logger = new LoggerSyslog(Threading::Thread::currentThreadId(), sender, file, line);
			break;
		case Logger::File:
			m_logger = new LoggerFile(Threading::Thread::currentThreadId(), sender, file, line);
			break;
	}
}

Log::Log::~Log()
{
	if (NULL != m_logger) {
		m_logger->flush();
		delete m_logger;
	}
}

