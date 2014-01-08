/*
 * This file is part of fuppes
 *
 * (c) 2010-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _CONFIGURATION_DEVICEIDENTIFICATION_H
#define _CONFIGURATION_DEVICEIDENTIFICATION_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BaseSettings.h"
#include "VirtualContainerSettings.h"
#include "../DeviceIdentification/IdentificationMgrConfig.h"

#include <string>
#include <map>

namespace Configuration
{
    struct Mapping: public DeviceIdentification::Mapping
    {
        public:
            Mapping() :
                    DeviceIdentification::Mapping()
            {
                xmlNode = NULL;
            }

            xmlNodePtr xmlNode;
    };

    class DeviceIdentificationSettings: public BaseSettings, public DeviceIdentification::IdentificationMgrConfig
    {
        public:
            void readConfig(ConfigFile& configFile) throw (Configuration::ReadException);
            void setup(PathFinder& pathFinder) throw (Configuration::SetupException);
            void setup(PathFinder& pathFinder, VirtualContainerSettings* vcontainerSettings) throw (Configuration::SetupException);
            void writeConfig(ConfigFile& configFile) throw (Configuration::WriteException);
    };

}

#endif // _CONFIGURATION_DEVICEIDENTIFICATION_H
