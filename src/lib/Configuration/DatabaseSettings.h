/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _CONFIGURATION_DATABASE_SETTINGS_H
#define _CONFIGURATION_DATABASE_SETTINGS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BaseSettings.h"

#include <fuppes_database_plugin.h>

namespace Configuration
{
    class DatabaseSettings : public BaseSettings, public Database::ConnectionParameters
    {
        public:
            DatabaseSettings();
            void readConfig(ConfigFile& configFile) throw(Configuration::ReadException);
            void setup(PathFinder& pathFinder) throw(Configuration::SetupException);
            void writeConfig(ConfigFile& configFile) throw (Configuration::WriteException);
    };
}

#endif // _CONFIGURATION_DATABASE_SETTINGS_H
