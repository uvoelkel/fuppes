/*
 * This file is part of fuppes
 *
 * (c) 2010-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "SharedObjects.h"

using namespace Configuration;

void SharedObjects::readConfig(ConfigFile& configFile) throw (Configuration::ReadException)
{
    xmlNodePtr node = configFile.getNode("/fuppes_config/shared_objects");
    if (NULL == node) {
        throw Configuration::ReadException("config node not found");
    }

    xmlNodePtr child = xmlFirstElementChild(node);
    while (child) {

    	SharedObject so;

        std::string name = (char*)child->name;
        if (0 == name.compare("dir")) {

            so.type = SharedObject::directory;
            so.path = fuppes::Directory::appendTrailingSlash(configFile.getNodeText(child));

        }
        else if (0 == name.compare("object")) {

        	std::string type = configFile.getNodeAttribute(child, "type");
        	if (0 == type.compare("dir")) {

        		so.type = SharedObject::directory;
				so.path = fuppes::Directory::appendTrailingSlash(configFile.getNodeText(child));
        	}
        }

        if (SharedObject::unknown != so.type) {
        	so.xmlNode = child;
        	m_sharedObjects.push_back(so);
        }

        child = xmlNextElementSibling(child);
    }
}

void SharedObjects::setup(PathFinder& pathFinder) throw (Configuration::SetupException)
{

}

void SharedObjects::writeConfig(ConfigFile& configFile) throw (Configuration::WriteException)
{
    xmlNodePtr node = configFile.getNode("/fuppes_config/shared_objects");

    std::vector<SharedObject>::iterator iter;
    for (iter = m_sharedObjects.begin(); iter != m_sharedObjects.end(); iter++) {

        if (SharedObject::directory != iter->type) {
            continue;
        }

        if (NULL == iter->xmlNode) {
            iter->xmlNode = xmlNewChild(node, NULL, BAD_CAST "dir", NULL);
        }
        configFile.setNodeText(iter->xmlNode, iter->path);
        configFile.setNodeAttribute(iter->xmlNode, "type", "dir");
    }
}

size_t SharedObjects::count()
{
    return m_sharedObjects.size();
}

::Configuration::SharedObject* SharedObjects::getSharedObject(size_t index)
{
    return &m_sharedObjects.at(index);
}

::Configuration::SharedObject* SharedObjects::getSharedObject(std::string uuid)
{
    std::vector<SharedObject>::iterator iter;
    for (iter = m_sharedObjects.begin(); iter != m_sharedObjects.end(); iter++) {
        if (0 == iter->uuid.compare(uuid)) {
            return &(*iter);
        }
    }
    return NULL;
}

::Configuration::SharedObject* SharedObjects::add(::ContentDatabase::SharedObject obj)
{
    SharedObject so;
    so.type = obj.type;
    so.name = obj.name;
    so.path = obj.path;
    m_sharedObjects.push_back(so);
    return &m_sharedObjects.back();
}

/*


 #include "../SharedConfig.h"

 #include "../Common/Common.h"

 using namespace std;

 SharedObjects::~SharedObjects()
 {
 clear();
 }

 // shared dirs
 bool SharedObjects::Read(void)
 {
 clear();

 SharedObject* obj;

 fuppes::EntryList list;
 fuppes::EntryListIterator iter;
 m_config->getEntries(SECTION_SHARED_OBJECTS, "", list);

 for(iter = list.begin(); iter != list.end(); iter++) {

 fuppes::ConfigEntry cfg = *iter;

 if(cfg.key.compare("dir") == 0) {
 obj = new SharedObject();
 obj->m_type = SharedObject::directory;
 obj->m_path = fuppes::Directory::appendTrailingSlash(cfg.value);
 obj->m_configEntry = cfg;
 m_sharedObjects.push_back(obj);
 }
 else if(cfg.key.compare("itunes") == 0) {
 obj = new SharedObject();
 obj->m_type = SharedObject::other;
 obj->m_otherType = "itunes";
 obj->m_path = cfg.value;
 obj->m_configEntry = cfg;
 m_sharedObjects.push_back(obj);
 }
 else if(cfg.key.compare("item") == 0) {
 obj = new SharedObject();
 obj->m_type = SharedObject::other;
 obj->m_otherType = cfg.attributes["type"];
 obj->m_name = cfg.attributes["name"];
 obj->m_path = cfg.value;
 obj->m_configEntry = cfg;
 m_sharedObjects.push_back(obj);
 }

 }

 return true;
 }

 void SharedObjects::clear()
 {
 for(m_sharedObjectsIter = m_sharedObjects.begin();
 m_sharedObjectsIter != m_sharedObjects.end();
 m_sharedObjectsIter++) {
 delete *m_sharedObjectsIter;
 }
 m_sharedObjects.clear();
 }

 SharedObject* SharedObjects::addSharedObject(SharedObject::SharedObjectType type, std::string path, std::string name , std::string otherType )
 {
 string tag;
 switch(type) {
 case SharedObject::directory:
 tag = "dir";
 break;
 case SharedObject::playlist:
 tag = "playlist";
 break;
 case SharedObject::other:
 tag = "item";
 break;
 default:
 return NULL;
 break;
 }

 // add the new object to the list
 SharedObject* result = new SharedObject();
 result->m_type = type;
 result->m_path = path; //ToUTF8(path, CSharedConfig::Shared()->contentDirectory.GetLocalCharset());
 result->m_name = name;
 result->m_otherType = otherType;
 m_sharedObjects.push_back(result);

 // and to the config file
 fuppes::ConfigEntry cfg;
 cfg.key = tag;
 cfg.value = result->path();
 if(type == SharedObject::other) {
 cfg.attributes["name"] = result->name();
 cfg.attributes["type"] = result->otherType();
 }
 m_config->setEntry(SECTION_SHARED_OBJECTS, "", cfg);

 // return the new object
 return result;
 }

 void SharedObjects::removeSharedObject(std::string uuid)
 {
 SharedObject* so = sharedObject(uuid);
 if(NULL == so)
 return;

 // remove from config file
 m_config->removeEntry(SECTION_SHARED_OBJECTS, "", so->m_configEntry);

 // remove from list
 for(m_sharedObjectsIter = m_sharedObjects.begin();
 m_sharedObjectsIter != m_sharedObjects.end();
 m_sharedObjectsIter++) {
 if((*m_sharedObjectsIter)->uuid() == uuid) {
 m_sharedObjects.erase(m_sharedObjectsIter);
 break;
 }
 }

 delete so;
 }

 */
