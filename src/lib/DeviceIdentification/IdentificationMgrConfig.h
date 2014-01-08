/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _IDENTIFICATIONMGR_CONFIG_H
#define _IDENTIFICATIONMGR_CONFIG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <map>

#include "DeviceSettings.h"

namespace DeviceIdentification
{
    struct Mapping
    {
        public:
            Mapping()
            {
                deviceSettings = NULL;
            }

            std::string         value;
            std::string         vfolder;
            std::string         deviceName;
            std::string         filename;
            CDeviceSettings*    deviceSettings;
    };

    class IdentificationMgrConfig
    {
        public:
            // absolute path to the base.cfg
            std::string     baseDeviceFilename;

            // name of the default device
            std::string     defaultDeviceName;
            // absolute path to the default device .cfg file
            std::string     defaultDeviceFilename;


            // mac, mapping
            std::map<std::string, struct Mapping> macAddrs;
            // ip, mapping
            std::map<std::string, struct Mapping> ipAddrs;
    };

}
#endif // _IDENTIFICATIONMGR_CONFIG_H
