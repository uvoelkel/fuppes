/*
 * This file is part of fuppes
 *
 * (c) 2007-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _DEVICEIDENTIFICATION_IDENTIFICATIONMGR_H
#define _DEVICEIDENTIFICATION_IDENTIFICATIONMGR_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "IdentificationMgrConfig.h"
#include "../Configuration/FuppesConfig.h"

#include "../Security/AccessControl.h"

#include "DeviceSettings.h"
#include "../Fuppes/HttpRequest.h"

#include <string>
#include <map>

namespace DeviceIdentification
{

    class IdentificationMgr
    {
        public:
            //static CDeviceIdentificationMgr* Shared();
            IdentificationMgr(Fuppes::FuppesConfig& fuppesConfig, DeviceIdentification::IdentificationMgrConfig& config, Security::AccessControl& accessControl);
            ~IdentificationMgr();

            bool initialize();
            //void setAccessControl();

            //void IdentifyDevice(CHTTPMessage* pDeviceMessage);
            void identifyDevice(Fuppes::HttpRequest* request);

            //void PrintSettings(std::string* p_sOut = NULL);

            /*void setDefaultDeviceName(std::string deviceName);
            std::string getDefaultDeviceName()
            {
                return m_defaultDeviceName;
            }*/
            //CDeviceSettings* getDefaultDevice();
            CDeviceSettings* getBaseDevice()
            {
                return m_baseSettings;
            }

        private:
            //static CDeviceIdentificationMgr* m_pInstance;

            void replaceDescriptionVars(std::string& value);
            CDeviceSettings* getSettingsForInitialization(const std::string deviceName, const std::string filename);

            //void PrintSetting(CDeviceSettings* pSettings, std::string* p_sOut = NULL);

            Fuppes::FuppesConfig& m_fuppesConfig;
            DeviceIdentification::IdentificationMgrConfig& m_identificationMgrConfig;
            Security::AccessControl& m_accessControl;

            /**
             * the defaultDeviceName is the device that should be used for all request if no mapping is defined.
             * the value is usually "default" but can be any device name
             */
            std::string m_defaultDeviceName;

            /**
             * this are the base settings from the "base.cfg" that are inherited by all other devices
             */
            CDeviceSettings* m_baseSettings;

            CDeviceSettings* m_defaultSettings;

            //Device::Mapping m_deviceMapping;

            std::map<std::string, CDeviceSettings*> m_settings;
            std::map<std::string, CDeviceSettings*>::const_iterator m_settingsIt;

            //Device::DeviceConfiguration* m_defaultConfiguration;
            //std::list<Device::DeviceConfiguration*> m_deviceConfigurations;
            //std::list<Device::DeviceConfiguration*>::const_iterator m_SettingsIt;

    };

}

#endif // _DEVICEIDENTIFICATION_IDENTIFICATIONMGR_H
