/*
 * This file is part of fuppes
 *
 * (c) 2007-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "../Common/Socket.h"

#include "IdentificationMgr.h"
#include <iostream>

using namespace DeviceIdentification;

#include "DeviceConfigFile.h"

IdentificationMgr::IdentificationMgr(Fuppes::FuppesConfig& fuppesConfig, DeviceIdentification::IdentificationMgrConfig& config, Security::AccessControl& accessControl) :
        m_fuppesConfig(fuppesConfig),
        m_identificationMgrConfig(config),
        m_accessControl(accessControl)
{
    m_baseSettings = NULL;
    m_defaultDeviceName = "default";
    m_defaultSettings = NULL;
}

IdentificationMgr::~IdentificationMgr()
{

    for(m_settingsIt = m_settings.begin(); m_settingsIt != m_settings.end(); ++m_settingsIt) {
        delete m_settingsIt->second;
    }
    m_settings.clear();

    if(NULL != m_baseSettings) {
        delete m_baseSettings;
    }

}

void IdentificationMgr::replaceDescriptionVars(std::string& value)
{
    std::string::size_type pos;

    // version (%v)
    while ((pos = value.find("%v")) != std::string::npos) {
        value = value.replace(pos, 2, m_fuppesConfig.globalSettings.appVersion);
    }
    // short name (%s)
    while ((pos = value.find("%s")) != std::string::npos) {
        value = value.replace(pos, 2, m_fuppesConfig.globalSettings.appName);
    }
    // hostname (%h)
    while ((pos = value.find("%h")) != std::string::npos) {
        value = value.replace(pos, 2, m_fuppesConfig.networkSettings.hostname);
    }
    // ip address (%i)
    while ((pos = value.find("%i")) != std::string::npos) {
        value = value.replace(pos, 2, m_fuppesConfig.networkSettings.ipAddress);
    }
}

bool IdentificationMgr::initialize()
{

    // setup the base device configuration
    m_baseSettings = this->getSettingsForInitialization("base", m_identificationMgrConfig.baseDeviceFilename);
    if(NULL == m_baseSettings) {
        return false;
    }

    // get the default device name and init the default device 
    if(!m_identificationMgrConfig.defaultDeviceName.empty()) {
        m_defaultDeviceName = m_identificationMgrConfig.defaultDeviceName;
    }
    m_defaultSettings = this->getSettingsForInitialization(m_defaultDeviceName, m_identificationMgrConfig.defaultDeviceFilename);
    if(NULL == m_defaultSettings) {
        return false;
    }

    // assign device settings to the mappings
    // and load the configuration for newly created settings
    std::map<std::string, struct DeviceIdentification::Mapping>::iterator iter;
    for(iter = m_identificationMgrConfig.macAddrs.begin(); iter != m_identificationMgrConfig.macAddrs.end(); iter++) {
        (&iter->second)->deviceSettings = this->getSettingsForInitialization(iter->second.deviceName, iter->second.filename);
    }

    for(iter = m_identificationMgrConfig.ipAddrs.begin(); iter != m_identificationMgrConfig.ipAddrs.end(); iter++) {
        (&iter->second)->deviceSettings = this->getSettingsForInitialization(iter->second.deviceName, iter->second.filename);
    }

    replaceDescriptionVars(m_baseSettings->MediaServerSettings()->FriendlyName);
    replaceDescriptionVars(m_baseSettings->MediaServerSettings()->ModelName);
    replaceDescriptionVars(m_baseSettings->MediaServerSettings()->ModelNumber);
    replaceDescriptionVars(m_baseSettings->MediaServerSettings()->ModelDescription);
    replaceDescriptionVars(m_baseSettings->MediaServerSettings()->SerialNumber);

    CDeviceSettings* pSettings;
    for(m_settingsIt = m_settings.begin(); m_settingsIt != m_settings.end(); ++m_settingsIt) {
        pSettings = m_settingsIt->second;

        replaceDescriptionVars(pSettings->MediaServerSettings()->FriendlyName);
        replaceDescriptionVars(pSettings->MediaServerSettings()->ModelName);
        replaceDescriptionVars(pSettings->MediaServerSettings()->ModelNumber);
        replaceDescriptionVars(pSettings->MediaServerSettings()->ModelDescription);
        replaceDescriptionVars(pSettings->MediaServerSettings()->SerialNumber);

        // create server signature for the device
        std::stringstream signature;
        signature << m_fuppesConfig.globalSettings.osName << "/" << m_fuppesConfig.globalSettings.osVersion << ", " << m_fuppesConfig.globalSettings.appName << "/"
                << m_fuppesConfig.globalSettings.appVersion << ", " << "UPnP/1.0";

        // dlna volume 1, 7.2.32
        if(CMediaServerSettings::dlna_1_0 == pSettings->MediaServerSettings()->DlnaVersion) {
            signature << " DLNADOC/1.00";
        }
        else if(CMediaServerSettings::dlna_1_5 == pSettings->MediaServerSettings()->DlnaVersion) {
            signature << " DLNADOC/1.50";
        }

        pSettings->setHttpServerSignature(signature.str());
    }

    return true;
}

void IdentificationMgr::identifyDevice(Fuppes::HttpRequest* pDeviceMessage)
{
    assert(pDeviceMessage != NULL);
    pDeviceMessage->setDefaultDeviceSettings(m_defaultSettings);

    bool found = false;
    std::map<std::string, struct DeviceIdentification::Mapping>::iterator iter;

    std::string mac;
    std::string ip = pDeviceMessage->getRemoteSocket()->remoteAddress();

    // get the mac address
    if(true == m_accessControl.getMacAddressTable()->mac(ip, mac)) {

        // check if the mac is mapped
        iter = m_identificationMgrConfig.macAddrs.find(mac);
        if(m_identificationMgrConfig.macAddrs.end() != iter) {
            pDeviceMessage->setDeviceSettings((&iter->second)->deviceSettings);
            pDeviceMessage->setVfolderLayout(iter->second.vfolder);
            found = true;
        }
    }

    // check if the ip is mapped
    if(false == found) {
        iter = m_identificationMgrConfig.ipAddrs.find(ip);
        if(m_identificationMgrConfig.ipAddrs.end() != iter) {
            pDeviceMessage->setDeviceSettings((&iter->second)->deviceSettings);
            pDeviceMessage->setVfolderLayout(iter->second.vfolder);
            found = true;
        }
    }

    // still no match. so let's use the default device
    if(false == found) {
        pDeviceMessage->setDeviceSettings(m_defaultSettings);

        // check if the default vfolder layout is enabled
        if(m_fuppesConfig.virtualContainerSettings.enabled) {
            VirtualContainer::VirtualContainerIter iter = m_fuppesConfig.virtualContainerSettings.virtualContainers.begin();
            for(; iter != m_fuppesConfig.virtualContainerSettings.virtualContainers.end(); iter++) {
                if(0 == iter->name.compare("default")) {
                    pDeviceMessage->setVfolderLayout((iter->enabled ? "default" : ""));
                    break;
                }
            }
        }

    }

    // check if the requests http header contains a layout request
    // this is nothing that any real upnp renderer should have but
    // the fuppes webinterface uses it and maybe some future config gui
    // could also use this
    if(pDeviceMessage->getHeader()->keyExists("virtual-layout")) {

        std::string layout = pDeviceMessage->getHeader()->getValue("virtual-layout");
        if(0 == layout.compare("none")) {
            pDeviceMessage->setVfolderLayout("");
        }
        else {
            VirtualContainer::VirtualContainerIter viter = m_fuppesConfig.virtualContainerSettings.virtualContainers.begin();
            for(; viter != m_fuppesConfig.virtualContainerSettings.virtualContainers.end(); viter++) {
                if(0 == viter->name.compare(layout)) {
                    pDeviceMessage->setVfolderLayout((viter->enabled ? layout : ""));
                    break;
                }
            }
        }

    }

    pDeviceMessage->setVfoldersEnabled(m_fuppesConfig.virtualContainerSettings.enabled);
}

CDeviceSettings* IdentificationMgr::getSettingsForInitialization(const std::string deviceName, const std::string filename)
{
    if(deviceName.compare("base") == 0) {

        if(NULL == m_baseSettings) {
            m_baseSettings = new CDeviceSettings("base");

            int error;
            CDeviceConfigFile devConf;
            error = devConf.setupDevice(filename, m_baseSettings, "base");
            if(SETUP_DEVICE_SUCCESS != error) {
                delete m_baseSettings;
                m_baseSettings = NULL;
            }
        }

        return m_baseSettings;
    }

    CDeviceSettings* settings = NULL;

    // check if the settings already exists
    std::map<std::string, CDeviceSettings*>::const_iterator iter;
    iter = m_settings.find(deviceName);
    if(iter != m_settings.end()) {
        settings = iter->second;
    }

    // no it does not. create a copy of the base settings
    if(NULL == settings) {
        settings = new CDeviceSettings(deviceName, m_baseSettings);
        m_settings[deviceName] = settings;

        int error;
        CDeviceConfigFile devConf;
        error = devConf.setupDevice(filename, settings, deviceName);
        if(SETUP_DEVICE_SUCCESS != error) {
            delete settings;
        }
    }

    return settings;
}
