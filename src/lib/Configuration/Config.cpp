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

#include "Config.h"
//#include "../Logging/Log.h"
#include "../Common/File.h"

using namespace fuppes;
//using namespace Logging;

#include <assert.h>
#include <iostream>

Config::Config()
{
    m_doc = NULL;
    m_xpathCtx = NULL;
    m_rootNode = NULL;
}

Config::~Config()
{
    if(NULL != m_doc) {
        xmlFreeDoc(m_doc);
    }

    if(NULL != m_xpathCtx) {
        xmlXPathFreeContext(m_xpathCtx);
    }
}

bool Config::load(std::string filename)
{
    m_doc = xmlReadFile(filename.c_str(), "UTF-8", XML_PARSE_NOBLANKS);
    if(m_doc == NULL) {
        return false;
    }

    m_rootNode = xmlDocGetRootElement(m_doc);
    if(m_rootNode == NULL) {
        xmlFreeDoc(m_doc);
        return false;
    }

    xmlAttrPtr version = xmlHasProp(m_rootNode, BAD_CAST "version");
    if(version == NULL) {
        xmlFreeDoc(m_doc);
        return false;
    }

    m_xpathCtx = xmlXPathNewContext(m_doc);
    if(m_xpathCtx == NULL) {
        xmlFreeDoc(m_doc);
        return false;
    }

    xmlNodePtr section = xmlFirstElementChild(m_rootNode);
    std::string name;
    while (section) {
        name = (char*)section->name;
        m_sections[name] = section;
        section = xmlNextElementSibling(section);
    }

    /*
     std::map<std::string, xmlNodePtr>::iterator iter;
     for(iter = m_sections.begin(); iter != m_sections.end(); iter++) {
     std::cout << iter->first << " = " << (char*)iter->second->name << std::endl;
     }
     */
    m_filename = filename;

    return true;
}

void Config::save()
{
    if(!File::writable(m_filename)) {
        std::cout << "WARNING: config file: " << m_filename << " is not writable" << std::endl;
        return;
    }

    int ret = xmlSaveFormatFileEnc(m_filename.c_str(), m_doc, "UTF-8", 1);
    if(ret == -1)
        std::cout << "WARNING: error writing config file" << std::endl;
}

xmlNodePtr Config::findNode(std::string section, std::string key = "")
{
    // find the section
    std::map<std::string, xmlNodePtr>::iterator iter;
    iter = m_sections.find(section);
    if(iter == m_sections.end())
        return NULL;

    // no key => return the section
    if(key.empty())
        return iter->second;

    // find the key
    xmlNodePtr node = xmlFirstElementChild(iter->second);
    while (node) {
        if(key.compare((char*)node->name) == 0)
            return node;
        node = xmlNextElementSibling(node);
    }

    return NULL;
}

std::string Config::getValue(const std::string section, const std::string key, const std::string defaultValue /*= ""*/)
{
    std::string value = defaultValue;

    xmlNodePtr node = findNode(section, key);
    if(node && node->children && node->children->content && node->children->type == XML_TEXT_NODE) {
        value = (char*)node->children->content;
    }

    return value;
}

void Config::setValue(std::string section, std::string key, std::string value)
{
    xmlNodePtr node = findNode(section, key);
    if(node == NULL)
        return;

    if(node->children)
        xmlNodeSetContent(node->children, BAD_CAST value.c_str());
    else
        xmlNodeAddContent(node, BAD_CAST value.c_str());

    save();
}

std::string Config::getAttribute(std::string section, std::string attribute)
{
    std::string value;

    // get the section node
    xmlNodePtr sect = findNode(section);
    if(sect == NULL)
        return value;

    // find the attribute
    xmlAttrPtr attr = sect->properties;
    while (attr) {

        if(attribute.compare((char*)attr->name) == 0) {
            value = (char*)attr->children->content;
            break;
        }

        attr = attr->next;
    }

    return value;
}

bool Config::getEntries(std::string section, std::string subsection, EntryList& list)
{
    // get the (sub-)section node
    xmlNodePtr sect = findNode(section, subsection);
    if(sect == NULL)
        return false;

    ConfigEntry entry;

    // loop through the section's children
    xmlNodePtr node = xmlFirstElementChild(sect);
    while (node) {

        // name
        entry.key = (char*)node->name;

        // value if available
        if(node->children && node->children->content && node->children->type == XML_TEXT_NODE)
            entry.value = (char*)node->children->content;

        // attributes
        xmlAttrPtr attr = node->properties;
        while (attr) {
            entry.attributes[(char*)attr->name] = (char*)attr->children->content;
            attr = attr->next;
        }

        // xml node
        entry.m_node = node;

        list.push_back(entry);
        entry.clear();

        node = xmlNextElementSibling(node);
    }

    return true;
}

void Config::setEntry(std::string section, std::string subsection, ConfigEntry& entry)
{
    // get the (sub-)section node
    xmlNodePtr sect = findNode(section, subsection);
    if(sect == NULL)
        return;

    xmlNodePtr node = entry.m_node;
    // if it's a new entry (node is NULL) we append it to the (sub-)section
    if(NULL == node) {

        if(entry.value.empty())
            node = xmlNewChild(sect, NULL, BAD_CAST entry.key.c_str(), NULL);
        else
            node = xmlNewTextChild(sect, NULL, BAD_CAST entry.key.c_str(), BAD_CAST entry.value.c_str());

    }
    // else we update the node
    else if(NULL != node) {

        // set the value if available
        if(!entry.value.empty()) {
            if(node->children)
                xmlNodeSetContent(node->children, BAD_CAST entry.value.c_str());
            else
                xmlNodeAddContent(node, BAD_CAST entry.value.c_str());
        }

    }

    // add/update attributes
    xmlAttrPtr attr;
    std::map<std::string, std::string>::iterator iter;
    for(iter = entry.attributes.begin(); iter != entry.attributes.end(); iter++) {

        attr = xmlHasProp(node, BAD_CAST iter->first.c_str());
        if(NULL == attr)
            attr = xmlNewProp(node, BAD_CAST iter->first.c_str(), BAD_CAST iter->second.c_str());
        else
            attr = xmlSetProp(node, BAD_CAST iter->first.c_str(), BAD_CAST iter->second.c_str());

    }

    save();
}

void Config::removeEntry(std::string section, std::string subsection, ConfigEntry& entry)
{
    // get the (sub-)section node
    xmlNodePtr sect = findNode(section, subsection);
    if(sect == NULL)
        return;

    xmlNodePtr node = entry.m_node;
    if(NULL == node)
        return;

    xmlUnlinkNode(node);
    xmlFreeNode(node);
    entry.m_node = NULL;

    save();
}

xmlXPathObjectPtr Config::xpathFind(std::string xpath)
{
    return xmlXPathEvalExpression(BAD_CAST xpath.c_str(), m_xpathCtx);
}

