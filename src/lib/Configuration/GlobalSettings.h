/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _CONFIGURATION_GLOBAL_SETTINGS
#define _CONFIGURATION_GLOBAL_SETTINGS

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>

#include "BaseSettings.h"
#include "../Common/DateTime.h"

namespace Configuration
{

    class GlobalSettings: public BaseSettings
    {
        public:
            GlobalSettings();

            void readConfig(ConfigFile& configFile) throw(Configuration::ReadException);
            void setup(PathFinder& pathFinder) throw(Configuration::SetupException);
            void writeConfig(ConfigFile& configFile) throw (Configuration::WriteException);

            std::string         applicationDir;
            std::string         resourcesDir;
            std::string         pluginDir;

            std::string         appName;
            std::string         appFullName;
            std::string         appVersion;

            std::string         osName;
            std::string         osVersion;

            std::string         localCharset;

            std::string         tempDir;
            std::string         trashDir;

            bool                useFixedUUID;
            std::string         fixedUUID;

            std::string         uuid;
            fuppes::DateTime    startTime;
    };

}

#endif // _CONFIGURATION_GLOBAL_SETTINGS
