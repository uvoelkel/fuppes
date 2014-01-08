/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _FUPPES_CONFIG
#define _FUPPES_CONFIG

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>

#include "BaseSettings.h"

#include "SharedObjects.h"
#include "NetworkSettings.h"
#include "DatabaseSettings.h"
#include "GlobalSettings.h"
#include "DeviceIdentificationSettings.h"
#include "VirtualContainerSettings.h"
#include "ContentDatabase.h"

#include "PathFinder.h"
#include "ConfigFile.h"

namespace Fuppes
{

    class FuppesConfig
    {
        public:
            FuppesConfig();
            void load(const std::string appDir) throw (Configuration::ReadException);
            void setup() throw (Configuration::SetupException);
            void save() throw (Configuration::WriteException);

            std::string                 configFilename;

            // shared objects
            Configuration::SharedObjects    sharedObjects;

            // network
            Configuration::NetworkSettings   networkSettings;
                // interface
                // http port
                // access control

            // database
            Configuration::DatabaseSettings databaseSettings;

            // global settings
            Configuration::GlobalSettings   globalSettings;

            // virtual containers
            Configuration::VirtualContainerSettings    virtualContainerSettings;

            // device identification
            Configuration::DeviceIdentificationSettings deviceIdentificationSettings;

            // content database
            Configuration::ContentDatabase contentDatabaseSettings;


            PathFinder                  pathFinder;
            Configuration::ConfigFile   configFile;

    };

}

#endif // _FUPPES_CONFIG
