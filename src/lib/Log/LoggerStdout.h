/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _LOG_LOGGER_STDOUT_H
#define _LOG_LOGGER_STDOUT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Logger.h"

namespace Log
{

    class LoggerStdout: public Logger
    {
    	public:
    		LoggerStdout(Threading::threadId_t threadId, const std::string sender, const std::string file, const int line);

        protected:
            void normalizedLog(const std::string log);
    };

}

#endif // _LOG_LOGGER_STDOUT_H
