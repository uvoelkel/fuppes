/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _CONFIGURATION_BASE_SETTINGS
#define _CONFIGURATION_BASE_SETTINGS

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ConfigFile.h"
#include "PathFinder.h"

namespace Configuration
{
    class Exception: public std::exception
    {
        public:
            const char* what() const throw ()
            {
                return m_msg.c_str();
            }
            ~Exception() throw ()
            {
            }

        protected:
            Exception(const std::string msg) :
                    std::exception()
            {
                m_msg = msg;
            }

        private:
            std::string m_msg;
    };

    class ReadException: public Exception
    {
        public:
            ReadException(const std::string msg) :
                    Exception(msg)
            {
            }
    };

    class SetupException: public Exception
    {
        public:
            SetupException(const std::string msg) :
                    Exception(msg)
            {
            }
    };

    class WriteException: public Exception
    {
        public:
            WriteException(const std::string msg) :
                    Exception(msg)
            {
            }
    };

    class BaseSettings
    {
        public:
            //BaseSettings();
            virtual ~BaseSettings()
            {
            }
            virtual void readConfig(ConfigFile& configFile) throw (Configuration::ReadException) = 0;
            virtual void setup(PathFinder& pathFinder) throw (Configuration::SetupException) = 0;
            virtual void writeConfig(ConfigFile& configFile) throw (Configuration::WriteException) = 0;
    };

}

#endif // _CONFIGURATION_BASE_SETTINGS
