/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _LOG_LOGGER_H
#define _LOG_LOGGER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_types.h>
#include "../Thread/Thread.h"

#include <sstream>
#include <string>

namespace Log
{

    class Logger
    {
        public:

            enum Type
            {
                Null,
                File,
                Stdout,
                Syslog
            };

            Logger(Threading::threadId_t threadId, const std::string sender, const std::string file, const int line);
            virtual ~Logger();

            void flush();

            // string types
            void log(const char* c) { m_stream << c << " "; }
            void log(std::string s) { m_stream << s << " "; }

            // boolean types
            void log(bool b) { m_stream << (b ? "true" : "false") << " "; }

            // number types
            void log(int i) { m_stream << i << " "; }
            void log(fuppes_off_t o) { m_stream << o << " "; }
            void log(size_t s) { m_stream << s << " "; }

        protected:
            virtual void normalizedLog(const std::string log) = 0;

            Threading::threadId_t m_threadId;
            std::string m_sender;
            std::string m_file;
            int m_line;

        private:
            std::stringstream   m_stream;
    };

}

#endif // _LOG_LOGGER_H
