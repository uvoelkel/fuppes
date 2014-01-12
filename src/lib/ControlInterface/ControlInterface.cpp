/***************************************************************************
 *            ControlInterface.cpp
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2010-2011 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "ControlInterface.h"

#include <iostream>
#include <sstream>
using namespace std;

#include "../Common/XMLParser.h"

#include "../Fuppes/Core.h"

//CContentDatabase* ControlInterface::m_contentDb = NULL;

//std::map<std::string, FUPPES_CONTROL_ACTION>  ControlInterface::m_actionTypeMap;

ControlInterface::ControlInterface(Fuppes::Core* fuppes, Fuppes::FuppesConfig& fuppesConfig, ContentDatabase::ContentDatabase& contentDatabase) :
        m_fuppesConfig(fuppesConfig),
        m_contentDatabase(contentDatabase)
{
    m_fuppes = fuppes;

    // database
    m_actionMap["GetDatabaseStatus"] = new ControlAction("GetDatabaseStatus", &ControlInterface::execGetDatabaseStatus);
    m_actionMap["DatabaseRebuild"] = new ControlAction("DatabaseRebuild", &ControlInterface::execDatabaseRebuild);
    m_actionMap["DatabaseUpdate"] = new ControlAction("DatabaseUpdate", &ControlInterface::execDatabaseUpdate);
    //m_actionMap["VfolderRebuild"] = new ControlAction("VfolderRebuild", &ControlInterface::execVfolderRebuild);

    m_actionMap["GetDir"] = new ControlAction("GetDir", &ControlInterface::execGetDir);

    m_actionMap["GetSharedObjects"] = new ControlAction("GetSharedObjects", &ControlInterface::execGetSharedObjects);
    m_actionMap["GetSharedObjectTypes"] = new ControlAction("GetSharedObjectTypes", &ControlInterface::execGetSharedObjectTypes);
    m_actionMap["AddSharedObject"] = new ControlAction("AddSharedObject", &ControlInterface::execAddSharedObject);
    m_actionMap["DelSharedObject"] = new ControlAction("DelSharedObject", &ControlInterface::execDelSharedObject);
    m_actionMap["ModSharedObject"] = new ControlAction("ModSharedObject", &ControlInterface::execModSharedObject);

    // system status and info
    m_actionMap["GetVersion"] = new ControlAction("GetVersion", &ControlInterface::execGetVersion);
    m_actionMap["GetHostname"] = new ControlAction("GetHostname", &ControlInterface::execGetHostname);
    m_actionMap["GetUptime"] = new ControlAction("GetUptime", &ControlInterface::execGetUptime);

    // configuration
    m_actionMap["GetConfigFileName"] = new ControlAction("GetConfigFileName", &ControlInterface::execGetConfigFileName);

    m_actionMap["GetInterface"] = new ControlAction("GetInterface", &ControlInterface::execGetInterface);
    m_actionMap["SetInterface"] = new ControlAction("SetInterface", &ControlInterface::execSetInterface);
    m_actionMap["GetHttpPort"] = new ControlAction("GetHttpPort", &ControlInterface::execGetHttpPort);
    m_actionMap["SetHttpPort"] = new ControlAction("SetHttpPort", &ControlInterface::execSetHttpPort);

    m_actionMap["GetCharset"] = new ControlAction("GetCharset", &ControlInterface::execGetCharset);
    m_actionMap["SetCharset"] = new ControlAction("SetCharset", &ControlInterface::execSetCharset);
    m_actionMap["GetTempDir"] = new ControlAction("GetTempDir", &ControlInterface::execGetTempDir);
	m_actionMap["SetTempDir"] = new ControlAction("SetTempDir", &ControlInterface::execSetTempDir);

    m_actionMap["GetUuid"] = new ControlAction("GetUuid", &ControlInterface::execGetUuid);
	m_actionMap["SetUuid"] = new ControlAction("SetUuid", &ControlInterface::execSetUuid);
	m_actionMap["GetIsFixedUuid"] = new ControlAction("GetIsFixedUuid", &ControlInterface::execGetIsFixedUuid);
	m_actionMap["SetIsFixedUuid"] = new ControlAction("SetIsFixedUuidd", &ControlInterface::execSetIsFixedUuid);


    // virtual container
    m_actionMap["GetVirtualContainers"] = new ControlAction("GetVirtualContainers", &ControlInterface::execGetVirtualContainers);

    // transcoding
    m_actionMap["GetTranscodingCacheState"] = new ControlAction("GetTranscodingCacheState", &ControlInterface::execGetTranscodingCacheState);
    m_actionMap["EmptyTranscodingCache"] = new ControlAction("EmptyTranscodingCache", &ControlInterface::execEmptyTranscodingCache);



    /*
     if(0 == ControlInterface::m_actionTypeMap.size()) {
     std::map<std::string, ControlAction*>::iterator iter = m_actionMap.begin();
     for(; iter != m_actionMap.end(); iter++) {
     ControlInterface::m_actionTypeMap[iter->first] = iter->second->action;
     }
     }
     */
}

ControlInterface::~ControlInterface()
{
    std::map<std::string, ControlAction*>::iterator iter = m_actionMap.begin();
    for (; iter != m_actionMap.end(); iter++) {
        delete iter->second;
    }
    m_actionMap.clear();
}

/*
 FUPPES_CONTROL_ACTION ControlInterface::getActionFromString(std::string action) // static
 {
 std::map<std::string, FUPPES_CONTROL_ACTION>::iterator iter = m_actionTypeMap.find(action);
 if(iter != m_actionTypeMap.end())
 return iter->second;
 else
 return FUPPES_CTRL_UNKNOWN;
 }
 */

ControlAction* ControlInterface::getAction(std::string action) // static
{
    std::map<std::string, ControlAction*>::iterator iter = m_actionMap.find(action);
    if (iter != m_actionMap.end())
        return iter->second;
    else
        return NULL;
}

// database
ControlInterface::ErrorCode ControlInterface::execGetDatabaseStatus(const ControlActionParams params, ControlActionParam &result)
{
    result.name = "DatabaseStatus";
    switch (m_contentDatabase.getStatus()) {

        case ContentDatabase::ContentDatabase::Idle:
            result.value = "Idle";
            break;
        case ContentDatabase::ContentDatabase::Stopped:
            result.value = "Stopped";
            break;

        case ContentDatabase::ContentDatabase::RemoveMissing:
            result.value = "RemoveMissing";
            break;
        case ContentDatabase::ContentDatabase::ScanDirectories:
            result.value = "ScanDirectories";
            break;
        case ContentDatabase::ContentDatabase::UpdateMetadata:
            result.value = "UpdateMetadata";
            break;
    }

    return ControlInterface::Ok;
}

ControlInterface::ErrorCode ControlInterface::execDatabaseRebuild(const ControlActionParams params, ControlActionParam &result)
{
    if (ContentDatabase::ContentDatabase::Idle == m_contentDatabase.getStatus() || ContentDatabase::ContentDatabase::Stopped == m_contentDatabase.getStatus()) {
        m_contentDatabase.rebuild();
    }
    return execGetDatabaseStatus(params, result);
}

ControlInterface::ErrorCode ControlInterface::execDatabaseUpdate(const ControlActionParams params, ControlActionParam &result)
{
    if (ContentDatabase::ContentDatabase::Idle == m_contentDatabase.getStatus() || ContentDatabase::ContentDatabase::Stopped == m_contentDatabase.getStatus()) {
        m_contentDatabase.update();
    }
    return execGetDatabaseStatus(params, result);
}

/*
 ControlInterface::ErrorCode ControlInterface::execVfolderRebuild(const ControlActionParams params, ControlActionParam &result)
 {
 std::cout << "ControlInterface::execVfolderRebuild" << std::endl;
 return ControlInterface::Ok;
 }
 */

// <dir show-files = [true|false]>[path]</dir>
ControlInterface::ErrorCode ControlInterface::execGetDir(const ControlActionParams params, ControlActionParams &result)
{
    ControlActionParamsIter iter = params.begin();
    if (iter == params.end() || iter->name != "dir")
        return ControlInterface::MissingInputParam;

    string path = iter->value;
    string sFiles;
    if (!iter->attribute("show-files", sFiles))
        return ControlInterface::MissingInputParam;

    bool files = (sFiles == "true");

    if (path.empty())
        path = "/";

#ifdef WIN32
    if(path == "/") {

        std::list<std::string> letters = fuppes::Directory::getDriveLetters();
        std::list<std::string>::iterator iter;

        for(iter = letters.begin(); iter != letters.end(); iter++) {
            ControlActionParam param;
            param.name = "dir";
            param.attributes["name"] = (*iter) + ":";
            param.value = (*iter) + ":";
            result.push_back(param);
        }
        return ControlInterface::Ok;
    }
#endif

    fuppes::Directory dir(path);
    dir.open();
    fuppes::DirEntryList info = dir.dirEntryList();
    fuppes::DirEntryListIterator dirIter;

    if (path != "/") {
        ControlActionParam param;
        param.name = "parent";
        param.value = dir.path() + "..";
        result.push_back(param);
    }

    for (dirIter = info.begin(); dirIter != info.end(); dirIter++) {

        if (!files && dirIter->type() != fuppes::DirEntry::Directory)
            continue;

        ControlActionParam param;
        param.name = "dir";
        param.attributes["name"] = dirIter->name();
        param.value = dirIter->path();
        result.push_back(param);
    }

    dir.close();
    return ControlInterface::Ok;
}

ControlInterface::ErrorCode ControlInterface::execGetSharedObjects(const ControlActionParams params, ControlActionParams &result)
{
    for (size_t i = 0; i < m_fuppesConfig.sharedObjects.count(); i++) {

        Configuration::SharedObject* obj = m_fuppesConfig.sharedObjects.getSharedObject(i);

        string type;
        switch (obj->type) {
            case ContentDatabase::SharedObject::directory:
                type = "directory";
                break;
            case ContentDatabase::SharedObject::playlist:
                type = "playlist";
                break;
            case ContentDatabase::SharedObject::other:
                type = obj->otherType;
                break;
            default:
                continue;
                break;
        }

        ControlActionParam param;
        param.name = "shared-object";
        param.attributes["uuid"] = obj->uuid;
        param.attributes["path"] = obj->path;
        param.attributes["name"] = obj->name;
        param.attributes["item-type"] = type; // we can't call it type because mootools can't read a property named type
        result.push_back(param);
    }

    return ControlInterface::Ok;
}

ControlInterface::ErrorCode ControlInterface::execGetSharedObjectTypes(const ControlActionParams params, ControlActionParams &result)
{
    ControlActionParam param;

    // built in types
    param.name = "object-type";
    param.value = "directory";
    result.push_back(param);

    param.name = "object-type";
    param.value = "playlist";
    result.push_back(param);

    // source plugins
    /*std::list<std::string> sources = CPluginMgr::sourceNames();
     std::list<std::string>::iterator iter = sources.begin();
     for(;iter != sources.end(); iter++) {
     param.name = "object-type";
     param.value = *iter;
     result.push_back(param);
     }*/

    return ControlInterface::Ok;
}

// <object type="directory" path="[path]" />
// <object type="[itunes|playlist|...]" path="[path]" name="[name]" />
ControlInterface::ErrorCode ControlInterface::execAddSharedObject(const ControlActionParams params, ControlActionParam &result)
{
    ControlActionParamsIter iter = params.begin();
    if (iter == params.end() || iter->name != "object") {
        return ControlInterface::MissingInputParam;
    }

    ContentDatabase::SharedObject so;
    iter->attribute("name", so.name);

    string type;
    if (!iter->attribute("type", type) || !iter->attribute("path", so.path)) {
        return ControlInterface::MissingAttribute;
    }

    so.type = ContentDatabase::SharedObject::unknown;

    if (0 == type.compare("directory")) {
        so.type = ContentDatabase::SharedObject::directory;
    }
    else if (type.compare("playlist") == 0) {
        so.type = ContentDatabase::SharedObject::playlist;
    }
    else if (type.length() > 0) {
        so.type = ContentDatabase::SharedObject::other;
    }

    Configuration::SharedObject* obj = m_fuppesConfig.sharedObjects.add(so);
    m_fuppesConfig.save();

    result.name = "shared-object";
    result.attributes["uuid"] = obj->uuid;
    result.attributes["path"] = obj->path;
    result.attributes["name"] = obj->name;
    result.attributes["item-type"] = type; // we can't call it type because mootools can't read a property named type

    return ControlInterface::Ok;
}

// <object uuid="[uuid]" />
ControlInterface::ErrorCode ControlInterface::execDelSharedObject(const ControlActionParams params, ControlActionParam &result)
{
    ControlActionParamsIter iter = params.begin();
    if (iter == params.end() || iter->name != "object")
        return ControlInterface::MissingInputParam;

    string uuid;
    if (!iter->attribute("uuid", uuid))
        return ControlInterface::MissingAttribute;
    /*
     SharedObject* so = CSharedConfig::sharedObjects()->sharedObject(uuid);
     if(NULL == so)
     return ControlInterface::Error;

     m_fuppes->getContentDatabase()->removeSharedObject(so);
     CSharedConfig::sharedObjects()->removeSharedObject(uuid);
     */
    return ControlInterface::Ok;
}

// <object uuid="[uuid]" type="[type]" path="[path]" name="[name]" />
ControlInterface::ErrorCode ControlInterface::execModSharedObject(const ControlActionParams params, ControlActionParam &result)
{
    ControlActionParamsIter iter = params.begin();
    if (iter == params.end() || iter->name != "object")
        return ControlInterface::MissingInputParam;

    return ControlInterface::Ok;
}

ControlInterface::ErrorCode ControlInterface::execGetVersion(const ControlActionParams params, ControlActionParam &result)
{
    result.name = "Version";
    result.value = FUPPES_VERSION; //CSharedConfig::Shared()->GetAppVersion();
    return ControlInterface::Ok;
}

ControlInterface::ErrorCode ControlInterface::execGetHostname(const ControlActionParams params, ControlActionParam &result)
{
    result.name = "Hostname";
    result.value = m_fuppes->getConfig()->networkSettings.hostname; // CSharedConfig::Shared()->networkSettings.GetHostname();
    return ControlInterface::Ok;
}

ControlInterface::ErrorCode ControlInterface::execGetUptime(const ControlActionParams params, ControlActionParam &result)
{
    stringstream uptime;
    fuppes::DateTime now = fuppes::DateTime::now();
    uptime << (now.toInt() - m_fuppes->getConfig()->globalSettings.startTime.toInt()); // CSharedConfig::Shared()->GetFuppesInstance(0)->startTime().toInt());

    result.name = "Uptime";
    result.value = uptime.str();
    return ControlInterface::Ok;
}

// configuration

ControlInterface::ErrorCode ControlInterface::execGetConfigFileName(const ControlActionParams params, ControlActionParam &result)
{
    result.name = "ConfigFileName";
    result.value = m_fuppesConfig.configFilename;
    return ControlInterface::Ok;
}

ControlInterface::ErrorCode ControlInterface::execGetInterface(const ControlActionParams params, ControlActionParam &result)
{
    result.name = "Interface";
    result.value = m_fuppesConfig.networkSettings.networkInterface;
    return ControlInterface::Ok;
}

ControlInterface::ErrorCode ControlInterface::execSetInterface(const ControlActionParams params, ControlActionParam &result)
{
    ControlActionParamsIter iter = params.begin();
    if (params.end() == iter || "Interface" != iter->name) {
        return ControlInterface::MissingInputParam;
    }

    if (0 == m_fuppesConfig.networkSettings.networkInterface.compare(iter->value)) {
    	return ControlInterface::Unchanged;
    }

    m_fuppesConfig.networkSettings.networkInterface = iter->value;
    m_fuppesConfig.save();
    return ControlInterface::RestartRequired;
}

ControlInterface::ErrorCode ControlInterface::execGetHttpPort(const ControlActionParams params, ControlActionParam &result)
{
    stringstream port;
    port << m_fuppesConfig.networkSettings.httpPort;

    result.name = "HttpPort";
    result.value = port.str();

    return ControlInterface::Ok;
}

ControlInterface::ErrorCode ControlInterface::execSetHttpPort(const ControlActionParams params, ControlActionParam &result)
{
    ControlActionParamsIter iter = params.begin();
    if (params.end() == iter || "HttpPort" != iter->name) {
        return ControlInterface::MissingInputParam;
    }

    stringstream port;
    port << m_fuppesConfig.networkSettings.httpPort;

    if (0 == port.str().compare(iter->value)) {
    	return ControlInterface::Unchanged;
    }

    m_fuppesConfig.networkSettings.httpPort = strtol(iter->value.c_str(), NULL, 10);
    m_fuppesConfig.save();
    return ControlInterface::RestartRequired;
}

ControlInterface::ErrorCode ControlInterface::execGetCharset(const ControlActionParams params, ControlActionParam &result)
{
    result.name = "Charset";
    result.value = m_fuppesConfig.globalSettings.localCharset;
    return ControlInterface::Ok;
}

ControlInterface::ErrorCode ControlInterface::execSetCharset(const ControlActionParams params, ControlActionParam &result)
{
    ControlActionParamsIter iter = params.begin();
    if (params.end() == iter || "Charset" != iter->name) {
        return ControlInterface::MissingInputParam;
    }

    if (0 == m_fuppesConfig.globalSettings.localCharset.compare(iter->value)) {
		return ControlInterface::Unchanged;
	}

    m_fuppesConfig.globalSettings.localCharset = iter->value;
    m_fuppesConfig.save();
    return ControlInterface::RestartRequired;
}

ControlInterface::ErrorCode ControlInterface::execGetTempDir(const ControlActionParams params, ControlActionParam &result)
{
    result.name = "TempDir";
    result.value = m_fuppesConfig.transcodingSettings.tempDir;
    return ControlInterface::Ok;
}

ControlInterface::ErrorCode ControlInterface::execSetTempDir(const ControlActionParams params, ControlActionParam &result)
{
	return ControlInterface::Error;
}


ControlInterface::ErrorCode ControlInterface::execGetUuid(const ControlActionParams params, ControlActionParam &result)
{
	result.name = "Uuid";
	result.value = m_fuppesConfig.globalSettings.uuid;
	return ControlInterface::Ok;
}

ControlInterface::ErrorCode ControlInterface::execSetUuid(const ControlActionParams params, ControlActionParam &result)
{
    ControlActionParamsIter iter = params.begin();
    if (params.end() == iter || "Uuid" != iter->name) {
        return ControlInterface::MissingInputParam;
    }

    if (0 == m_fuppesConfig.globalSettings.fixedUUID.compare(iter->value)) {
		return ControlInterface::Unchanged;
	}

    m_fuppesConfig.globalSettings.fixedUUID = iter->value;
    m_fuppesConfig.save();
    return ControlInterface::RestartRequired;
}

ControlInterface::ErrorCode ControlInterface::execGetIsFixedUuid(const ControlActionParams params, ControlActionParam &result)
{
	result.name = "IsFixedUuid";
	result.value = (m_fuppesConfig.globalSettings.useFixedUUID ? "true" : "false");
	return ControlInterface::Ok;
}

ControlInterface::ErrorCode ControlInterface::execSetIsFixedUuid(const ControlActionParams params, ControlActionParam &result)
{
	ControlActionParamsIter iter = params.begin();
	if (params.end() == iter || "IsFixedUuid" != iter->name) {
		return ControlInterface::MissingInputParam;
	}

	bool fixed = (0 == iter->value.compare("true"));

    if (fixed == m_fuppesConfig.globalSettings.useFixedUUID) {
		return ControlInterface::Unchanged;
	}

	m_fuppesConfig.globalSettings.useFixedUUID = fixed;
	m_fuppesConfig.save();
	return ControlInterface::RestartRequired;
}




// virtual container
ControlInterface::ErrorCode ControlInterface::execGetVirtualContainers(const ControlActionParams params, ControlActionParam &result)
{
    result.name = "VirtualContainers";
    result.attributes["enabled"] = m_fuppesConfig.virtualContainerSettings.enabled ? "true" : "false";

    ControlActionParam param;
    VirtualContainer::VirtualContainerIter iter;
    for (iter = m_fuppesConfig.virtualContainerSettings.virtualContainers.begin(); iter != m_fuppesConfig.virtualContainerSettings.virtualContainers.end(); iter++) {

        param.name = "vcontainer";
        param.value = iter->name;
        param.attributes["enabled"] = iter->enabled ? "true" : "false";
        result.children.push_back(param);
    }

    return ControlInterface::Ok;
}

// transcoding
ControlInterface::ErrorCode ControlInterface::execGetTranscodingCacheState(const ControlActionParams params, ControlActionParam &result)
{
	// todo
	return ControlInterface::Error;
}

ControlInterface::ErrorCode ControlInterface::execEmptyTranscodingCache(const ControlActionParams params, ControlActionParam &result)
{
	// todo
	return ControlInterface::Error;
}
