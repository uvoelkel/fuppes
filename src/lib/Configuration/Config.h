/*
 * This file is part of fuppes
 *
 * (c) 2011 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */


/**
 * THIS FILE IS DEPRECATED
 */


#ifndef _CONFIG_H
#define _CONFIG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <map>
#include <list>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <iostream>

namespace fuppes
{

#define SECTION_SHARED_OBJECTS      "shared_objects"
#define SECTION_NETWORK             "network"
#define SECTION_DATABASE            "database"
#define SECTION_CONTENT_DIRECTORY   "content_directory"
#define SECTION_GLOBAL              "global_settings"
#define SECTION_VFOLDERS            "vfolders"
#define SECTION_DEVICE_MAPPING      "device_mapping"


#define XPATH_NETWORK_INTERFACE     "/fuppes_config/network/interface/text()"

    
class Config;
  
class ConfigEntry
{
  friend class Config;

  public:
    ConfigEntry() {
      m_node = NULL;
    }
    /*
    ConfigEntry& operator=(const ConfigEntry& entry) {

      key         = entry.key;
      value       = entry.value;
      attributes  = entry.attributes;
      m_node      = entry.m_node;

      return *this;
    }
    */

    void clear() {
      key = "";
      value = "";
      attributes.clear();
      m_node = NULL;
    }
    
  public:
    std::string                         key;
    std::string                         value;
    std::map<std::string, std::string>  attributes;

  private:
    xmlNodePtr                          m_node;
};

typedef std::list<ConfigEntry>  EntryList;
typedef std::list<ConfigEntry>::iterator  EntryListIterator;


class Config
{
  public:
    Config();
    ~Config();

    bool load(std::string filename);

    // find a single string value for 'key' in 'section'
    std::string    getValue(const std::string section, const std::string key, const std::string defaultValue = "");
    // set a the 'value' for 'key' in 'section'
    void           setValue(std::string section, std::string key, std::string value);

    // get the 'attribute' value for 'section'
    std::string    getAttribute(std::string section, std::string attribute);

        
    bool   getEntries(std::string section, std::string subsection, EntryList& list);

    void   setEntry(std::string section, std::string subsection, ConfigEntry& entry);

    void   removeEntry(std::string section, std::string subsection, ConfigEntry& entry);


    xmlXPathObjectPtr xpathFind(std::string xpath);
        
  private:

    std::string         m_filename;
    xmlDocPtr           m_doc;
    xmlNodePtr          m_rootNode;
    xmlXPathContextPtr  m_xpathCtx;
    
    xmlNodePtr        findNode(std::string section, std::string key);
    void save();
    
    std::map<std::string, xmlNodePtr>   m_sections;
};

}

#endif // _CONFIG_H
