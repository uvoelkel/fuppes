/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "GlobalSettings.h"

#include "../Common/Common.h"
#include "../Common/UUID.h"

#include <sstream>

#ifndef WIN32
#include <sys/utsname.h>
#endif

using namespace Configuration;

GlobalSettings::GlobalSettings()
{
    appName = "fuppes";
    appFullName = "Free UPnP Entertainment Service";
    appVersion = FUPPES_VERSION;

    useFixedUUID = false;
    localCharset = "UTF-8";

#ifdef WIN32
    osName = "Windows";

    OSVERSIONINFO osinfo;
    osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (!GetVersionEx(&osinfo)) {
        osVersion = "?";
    }
    else {
        int nMajor = osinfo.dwMajorVersion;
        int nMinor = osinfo.dwMinorVersion;
        int nBuild = osinfo.dwBuildNumber;

        std::stringstream sVer;
        sVer << nMajor << "." << nMinor << "." << nBuild;
        osVersion = sVer.str();
    }
#else
    struct utsname sUtsName;
    uname(&sUtsName);
    osName = sUtsName.sysname;
    osVersion = sUtsName.release;
#endif
}

void GlobalSettings::readConfig(ConfigFile& configFile) throw (Configuration::ReadException)
{
    if(tempDir.empty()) {
        tempDir = configFile.getStringValue("/fuppes_config/global_settings/temp_dir/text()");
    }

    localCharset = configFile.getStringValue("/fuppes_config/global_settings/local_charset/text()");


    xmlNodePtr node = configFile.getNode("/fuppes_config/global_settings/fixed_uuid");
    useFixedUUID = (0 == configFile.getNodeAttribute(node, "enabled").compare("true"));
    fixedUUID = configFile.getStringValue("/fuppes_config/global_settings/fixed_uuid/text()");

    if(useFixedUUID && fixedUUID.empty()) {
        fixedUUID = GenerateUUID();
        configFile.setNodeText(node, fixedUUID);
        configFile.save();
    }
}

void GlobalSettings::setup(PathFinder& pathFinder) throw (Configuration::SetupException)
{
    // setup resource and plugin dirs
    if(resourcesDir.empty()) {
        #ifdef WIN32
        resourcesDir = applicationDir + "data/";
        #else
        resourcesDir = std::string(FUPPES_DATADIR) + "/";
        #endif
    }

    if(pluginDir.empty()) {
        #ifdef WIN32
        pluginDir = applicationDir;
        #else
        pluginDir = std::string(FUPPES_PLUGINDIR) + "/";
        #endif
    }

    // setup the uuid
    if(useFixedUUID) {
        uuid = fixedUUID;
    }
    else {
        uuid = GenerateUUID();
    }


    // setup temp dir
    if(tempDir.empty()) {
        #ifdef WIN32
        tempDir = getenv("TEMP") + std::string("\\fuppes\\");
        #else
        char* szTmp = getenv("TEMP");
        if(szTmp != NULL) {
            tempDir = std::string(szTmp) + "/fuppes/";
        }
        else {
            tempDir = "/tmp/fuppes/";
        }
        #endif
    }

    tempDir = fuppes::Directory::appendTrailingSlash(tempDir);
    if(!fuppes::Directory::create(tempDir)) {
    }
}

void GlobalSettings::writeConfig(ConfigFile& configFile) throw (Configuration::WriteException)
{
    xmlNodePtr node = configFile.getNode("/fuppes_config/global_settings/local_charset");
    configFile.setNodeText(node, localCharset);

    node = configFile.getNode("/fuppes_config/global_settings/fixed_uuid");
	configFile.setNodeText(node, fixedUUID);
	configFile.setNodeAttribute(node, "enabled", (useFixedUUID ? "true" : "false"));
}
