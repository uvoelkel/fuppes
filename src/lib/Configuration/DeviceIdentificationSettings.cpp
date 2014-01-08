/*
 * This file is part of fuppes
 *
 * (c) 2010-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "DeviceIdentificationSettings.h"

using namespace Configuration;

void DeviceIdentificationSettings::readConfig(ConfigFile& configFile) throw (Configuration::ReadException)
{
    xmlNodePtr node = configFile.getNode("/fuppes_config/devices");
    if(NULL == node) {
        throw Configuration::ReadException("config node not found");
    }

    // get the default device name
    defaultDeviceName = configFile.getNodeAttribute(node, "default", "default");

    // device mappings
    std::map<std::string, struct Mapping> macAddrs;
    std::map<std::string, struct Mapping> ipAddrs;

    // get the device mappings
    xmlNodePtr child = xmlFirstElementChild(node);
    while (child) {

        std::string name = (char*)child->name;

        if(0 != name.compare("ip") || 0 != name.compare("mac")) {
            child = xmlNextElementSibling(child);
            continue;
        }

        Mapping mapping;
        mapping.value = configFile.getNodeAttribute(child, "value");
        mapping.vfolder = configFile.getNodeAttribute(child, "vfolder");
        if(0 == mapping.vfolder.compare("none")) {
            mapping.vfolder = "";
        }
        mapping.deviceName = configFile.getNodeAttribute(child, "device");
        mapping.xmlNode = child;

        if(0 == name.compare("ip")) {
            ipAddrs[mapping.value] = mapping;
        }
        else if(0 == name.compare("mac")) {
            macAddrs[mapping.value] = mapping;
        }

        child = xmlNextElementSibling(child);
    }

}

void DeviceIdentificationSettings::setup(PathFinder& pathFinder) throw (Configuration::SetupException)
{
    throw SetupException("please use DeviceIdentificationSettings::setup(PathFinder&, VirtualFolderSettings*)");
}

void DeviceIdentificationSettings::setup(PathFinder& pathFinder, VirtualContainerSettings* vcontainerSettings) throw (Configuration::SetupException)
{
    // find the base and default config files
    baseDeviceFilename = pathFinder.findDeviceInPath("base");
    if(baseDeviceFilename.empty()) {
        throw SetupException("base device config file not found");
    }

    defaultDeviceFilename = pathFinder.findDeviceInPath(defaultDeviceName);
    if(defaultDeviceFilename.empty()) {
        throw SetupException("default device config file not found");
    }


    // find the device config files and reset vfolder if vfolders are disabled
    DeviceIdentification::Mapping* mapping;
    std::map<std::string, struct DeviceIdentification::Mapping>::iterator iter;

    for(iter = macAddrs.begin(); iter != macAddrs.end(); iter++) {
        mapping = &iter->second;

        if(mapping->deviceName.empty()) {
            mapping->deviceName = defaultDeviceName;
        }

        mapping->filename = pathFinder.findDeviceInPath(mapping->deviceName);
        if(mapping->filename.empty()) {
            throw SetupException("device config file not found");
        }

        if(!vcontainerSettings->enabled) {
            mapping->vfolder = "";
        }
    }

    for(iter = ipAddrs.begin(); iter != ipAddrs.end(); iter++) {
        mapping = &iter->second;

        if(mapping->deviceName.empty()) {
            mapping->deviceName = defaultDeviceName;
        }

        mapping->filename = pathFinder.findDeviceInPath(mapping->deviceName);
        if(mapping->filename.empty()) {
            throw SetupException("device config file not found");
        }

        if(!vcontainerSettings->enabled) {
            mapping->vfolder = "";
        }
    }

}


void DeviceIdentificationSettings::writeConfig(ConfigFile& configFile) throw (Configuration::WriteException)
{

}
