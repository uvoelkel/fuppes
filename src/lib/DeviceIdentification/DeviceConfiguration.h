/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _DEVICE_DEVICE_CONFIGURATION_H
#define _DEVICE_DEVICE_CONFIGURATION_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../UPnP/UPnPObjectTypes.h"

#include <map>

namespace Device
{

    class FileConfiguration
    {
        UPnP::objectType_t  m_objectType;
        std::string         m_extension;
        std::string         m_mimeType;        
    };

    
    class DeviceConfiguration
    {
        public:
            DeviceConfiguration& operator=(const DeviceConfiguration& deviceConfig);
            
            void addFileConfiguration(std::string extension, FileConfiguration* config);

        private:
            std::map<std::string, FileConfiguration*>   m_files;
    };

}

#endif // _DEVICE_DEVICE_CONFIGURATION_H
