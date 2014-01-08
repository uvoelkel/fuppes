/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _CONFIGURATION_CONTENTDATABASE_H
#define _CONFIGURATION_CONTENTDATABASE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BaseSettings.h"

#include "../ContentDatabase/Configuration.h"

#include "DatabaseSettings.h"
#include "GlobalSettings.h"
#include "SharedObjects.h"

namespace Configuration
{
    class ContentDatabase: public BaseSettings, public ::ContentDatabase::Configuration
    {
        public:
            void readConfig(::Configuration::ConfigFile& configFile) throw(::Configuration::ReadException);
            void setup(PathFinder& pathFinder) throw(::Configuration::SetupException);
            void setup(PathFinder& pathFinder, DatabaseSettings& databaseSettings, GlobalSettings& globalSettings, SharedObjects& sharedObjects) throw (::Configuration::SetupException);

            void writeConfig(::Configuration::ConfigFile& configFile) throw (::Configuration::WriteException);

            bool isReadonly();
            const std::string getLocalCharset();
            const std::string getThumbnailDirectory();

            ::ContentDatabase::SharedObjects_t getSharedObjects();

        private:
            PathFinder* m_pathFinder;
            DatabaseSettings* m_databaseSettings;
            GlobalSettings* m_globalSettings;
            SharedObjects* m_sharedObjects;
    };
}

#endif // _CONFIGURATION_CONTENTDATABASE_H
