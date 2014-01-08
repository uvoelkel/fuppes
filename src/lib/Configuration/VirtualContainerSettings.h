/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _CONFIGURATION_VIRTUALCONTAINER_SETTINGS
#define _CONFIGURATION_VIRTUALCONTAINER_SETTINGS

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BaseSettings.h"
#include "../VirtualContainer/Configuration.h"

namespace Configuration
{

    class VirtualContainerEntry: public VirtualContainer::ConfigEntry
    {
        public:
        VirtualContainerEntry()
            {
                xmlNode = NULL;
            }

            xmlNodePtr        xmlNode;
    };

    class VirtualContainerSettings: public BaseSettings, public VirtualContainer::Configuration
    {
        public:
            VirtualContainerSettings();

            void readConfig(ConfigFile& configFile) throw (::Configuration::ReadException);
            void setup(PathFinder& pathFinder) throw (::Configuration::SetupException);
            void writeConfig(ConfigFile& configFile) throw (::Configuration::WriteException);

    };

}

#endif // _CONFIGURATION_VIRTUALCONTAINER_SETTINGS
