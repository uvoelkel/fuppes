/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _LOG_LOG_H
#define _LOG_LOG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#undef log

#include <fuppes_types.h>

#include "Logger.h"
#include <string>

#define LOGNULL if (Logger::Null == Log::m_loggerType) return *this;
#define LOGACTIVE if (0 != (Log::m_loggerLevel & m_currentLevel)) return *this;

namespace Log
{
    class Log
    {
        public:
            enum Level
            {
                none = 0,
                normal = 1 << 0,
                extended = 1 << 1,
                debug = 1 << 2,

                warning = 1 << 3,
                error = 1 << 4
            };

            static void setType(Logger::Type type);
            static void setLevel(const int level);

            Log(const std::string sender, const Log::Level level, const std::string file, const int line);
            ~Log();

            inline Log& operator <<(Log::Level l)
            {
                LOGNULL
                m_currentLevel = l;
                return *this;
            }

            // string types
            inline Log& operator <<(const char* c)
            {
                LOGNULL
                LOGACTIVE
                m_logger->log(c);
                return *this;
            }
            inline Log& operator <<(std::string s)
            {
                LOGNULL
                LOGACTIVE
                m_logger->log(s);
                return *this;
            }

            // boolean types
            inline Log& operator <<(bool b)
            {
                LOGNULL
                LOGACTIVE
                m_logger->log(b);
                return *this;
            }

            // number types
            inline Log& operator <<(int i)
            {
                LOGNULL
                LOGACTIVE
                m_logger->log(i);
                return *this;
            }

            inline Log& operator <<(unsigned int i)
            {
                LOGNULL
                LOGACTIVE
                m_logger->log(i);
                return *this;
            }

            inline Log& operator <<(fuppes_off_t o)
            {
                LOGNULL
                LOGACTIVE
                m_logger->log(o);
                return *this;
            }

            inline Log& operator <<(size_t s)
            {
                LOGNULL
                LOGACTIVE
                m_logger->log(s);
                return *this;
            }

        private:
            static Logger::Type m_loggerType;
            static int	m_loggerLevel;

            Logger* m_logger;
            Log::Level m_currentLevel;

    };
}

#define log(sender) Log::Log(sender, Log::Log::normal, __FILE__, __LINE__)
#define logExt(sender) Log::Log(sender, Log::Log::extended, __FILE__, __LINE__)
#define logDbg(sender) Log::Log(sender, Log::Log::debug, __FILE__, __LINE__)

#endif // _LOG_LOG_H
