/*
 * This file is part of fuppes
 *
 * (c) 2011-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "ConfigFile.h"
#include "../Common/File.h"

using namespace Configuration;

#include <assert.h>
#include <iostream>

ConfigFile::ConfigFile()
{
    m_doc = NULL;
    m_xpathCtx = NULL;
    m_rootNode = NULL;

    m_ctxt = NULL;
}

ConfigFile::~ConfigFile()
{
    if (NULL != m_xpathCtx) {
        xmlXPathFreeContext(m_xpathCtx);
    }

    if (NULL != m_doc) {
        xmlFreeDoc(m_doc);
    }

    if (NULL != m_ctxt) {
        xmlFreeParserCtxt(m_ctxt);
    }
}

bool ConfigFile::load(std::string filename)
{
    m_ctxt = xmlNewParserCtxt();
    if (m_ctxt == NULL) {
        fprintf(stderr, "Failed to allocate parser context\n");
        return false;
    }

    //m_doc = xmlReadFile(filename.c_str(), "UTF-8", XML_PARSE_NOBLANKS);
    m_doc = xmlCtxtReadFile(m_ctxt, filename.c_str(), "UTF-8", XML_PARSE_NOBLANKS);
    if (m_doc == NULL) {
        return false;
    }

    m_rootNode = xmlDocGetRootElement(m_doc);
    if (m_rootNode == NULL) {
        xmlFreeDoc(m_doc);
        m_doc = NULL;
        return false;
    }

    xmlAttrPtr version = xmlHasProp(m_rootNode, BAD_CAST "version");
    if (version == NULL) {
        xmlFreeDoc(m_doc);
        m_doc = NULL;
        return false;
    }

    m_xpathCtx = xmlXPathNewContext(m_doc);
    if (m_xpathCtx == NULL) {
        xmlFreeDoc(m_doc);
        m_doc = NULL;
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

bool ConfigFile::save()
{
    if (!fuppes::File::writable(m_filename)) {
        std::cout << "WARNING: config file: " << m_filename << " is not writable" << std::endl;
        return false;
    }

    int ret = xmlSaveFormatFileEnc(m_filename.c_str(), m_doc, "UTF-8", 1);
    if (ret == -1) {
        std::cout << "WARNING: error writing config file" << std::endl;
        return false;
    }
    return true;
}

xmlNodePtr ConfigFile::findNode(std::string section, std::string key = "")
{
    // find the section
    std::map<std::string, xmlNodePtr>::iterator iter;
    iter = m_sections.find(section);
    if (iter == m_sections.end())
        return NULL;

    // no key => return the section
    if (key.empty())
        return iter->second;

    // find the key
    xmlNodePtr node = xmlFirstElementChild(iter->second);
    while (node) {
        if (key.compare((char*)node->name) == 0)
            return node;
        node = xmlNextElementSibling(node);
    }

    return NULL;
}

std::string ConfigFile::getValue(const std::string section, const std::string key, const std::string defaultValue /*= ""*/)
{
    std::string value = defaultValue;

    xmlNodePtr node = findNode(section, key);
    if (node && node->children && node->children->content && node->children->type == XML_TEXT_NODE) {
        value = (char*)node->children->content;
    }

    return value;
}

void ConfigFile::setValue(std::string section, std::string key, std::string value)
{
    xmlNodePtr node = findNode(section, key);
    if (node == NULL)
        return;

    if (node->children)
        xmlNodeSetContent(node->children, BAD_CAST value.c_str());
    else
        xmlNodeAddContent(node, BAD_CAST value.c_str());

    //save();
}

std::string ConfigFile::getAttribute(std::string section, std::string attribute)
{
    std::string value;

    // get the section node
    xmlNodePtr sect = findNode(section);
    if (sect == NULL)
        return value;

    // find the attribute
    xmlAttrPtr attr = sect->properties;
    while (attr) {

        if (attribute.compare((char*)attr->name) == 0) {
            value = (char*)attr->children->content;
            break;
        }

        attr = attr->next;
    }

    return value;
}

bool ConfigFile::getEntries(std::string section, std::string subsection, EntryList& list)
{
    // get the (sub-)section node
    xmlNodePtr sect = findNode(section, subsection);
    if (sect == NULL)
        return false;

    ConfigEntry entry;

    // loop through the section's children
    xmlNodePtr node = xmlFirstElementChild(sect);
    while (node) {

        // name
        entry.key = (char*)node->name;

        // value if available
        if (node->children && node->children->content && node->children->type == XML_TEXT_NODE)
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

void ConfigFile::setEntry(std::string section, std::string subsection, ConfigEntry& entry)
{
    // get the (sub-)section node
    xmlNodePtr sect = findNode(section, subsection);
    if (sect == NULL)
        return;

    xmlNodePtr node = entry.m_node;
    // if it's a new entry (node is NULL) we append it to the (sub-)section
    if (NULL == node) {

        if (entry.value.empty())
            node = xmlNewChild(sect, NULL, BAD_CAST entry.key.c_str(), NULL);
        else
            node = xmlNewTextChild(sect, NULL, BAD_CAST entry.key.c_str(), BAD_CAST entry.value.c_str());

    }
    // else we update the node
    else if (NULL != node) {

        // set the value if available
        if (!entry.value.empty()) {
            if (node->children)
                xmlNodeSetContent(node->children, BAD_CAST entry.value.c_str());
            else
                xmlNodeAddContent(node, BAD_CAST entry.value.c_str());
        }

    }

    // add/update attributes
    xmlAttrPtr attr;
    std::map<std::string, std::string>::iterator iter;
    for (iter = entry.attributes.begin(); iter != entry.attributes.end(); iter++) {

        attr = xmlHasProp(node, BAD_CAST iter->first.c_str());
        if (NULL == attr)
            attr = xmlNewProp(node, BAD_CAST iter->first.c_str(), BAD_CAST iter->second.c_str());
        else
            attr = xmlSetProp(node, BAD_CAST iter->first.c_str(), BAD_CAST iter->second.c_str());

    }

    //save();
}

void ConfigFile::removeEntry(std::string section, std::string subsection, ConfigEntry& entry)
{
    // get the (sub-)section node
    xmlNodePtr sect = findNode(section, subsection);
    if (sect == NULL)
        return;

    xmlNodePtr node = entry.m_node;
    if (NULL == node)
        return;

    xmlUnlinkNode(node);
    xmlFreeNode(node);
    entry.m_node = NULL;

    //save();
}

/*
 xmlXPathObjectPtr ConfigFile::xpathFind(std::string xpath)
 {
 return xmlXPathEvalExpression(BAD_CAST xpath.c_str(), m_xpathCtx);
 }
 */

xmlNodePtr ConfigFile::getNode(const std::string xpath)
{
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST xpath.c_str(), m_xpathCtx);
    if (NULL == xpathObj) {
        return NULL;
    }

    if (XPATH_NODESET == xpathObj->type && 1 == xpathObj->nodesetval->nodeNr) {
        xmlNodePtr node = xpathObj->nodesetval->nodeTab[0];
        xmlXPathFreeObject(xpathObj);
        return node;
    }

    return NULL;
}

std::string ConfigFile::getNodeText(xmlNodePtr node)
{
    if (node->children && node->children->content && node->children->type == XML_TEXT_NODE) {
        return (char*)node->children->content;
    }

    return "";
}

void ConfigFile::setNodeText(xmlNodePtr node, const std::string value)
{
    if (node->children) {
        xmlNodeSetContent(node->children, BAD_CAST value.c_str());
    }
    else {
        xmlNodeAddContent(node, BAD_CAST value.c_str());
    }
}

std::string ConfigFile::getNodeAttribute(xmlNodePtr node, const std::string key, const std::string defaultValue /* = ""*/)
{
    xmlAttrPtr attr = node->properties;
    while (attr) {
        if (0 == key.compare((char*)attr->name)) {
            std::string value = (char*)attr->children->content;
            return value;
        }
        attr = attr->next;
    }

    return defaultValue;
}

void ConfigFile::setNodeAttribute(xmlNodePtr node, const std::string key, const std::string value)
{
    xmlAttrPtr attr = xmlHasProp(node, BAD_CAST key.c_str());
    if (NULL == attr) {
        attr = xmlNewProp(node, BAD_CAST key.c_str(), BAD_CAST value.c_str());
    }
    else {
        attr = xmlSetProp(node, BAD_CAST key.c_str(), BAD_CAST value.c_str());
    }
}

bool ConfigFile::getBoolValue(const std::string xpath)
{
    return ("true" == this->getStringValue(xpath));
}

std::string ConfigFile::getStringValue(const std::string xpath)
{
    std::string result;

    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST xpath.c_str(), m_xpathCtx);
    if (NULL == xpathObj) {
        return result;
    }

    xpathObj = xmlXPathConvertString(xpathObj);
    result = (char*)xpathObj->stringval;

    xmlXPathFreeObject(xpathObj);
    return result;
}

EntryList ConfigFile::getEntries(const std::string xpath)
{
    EntryList result;

    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST xpath.c_str(), m_xpathCtx);
    if (NULL == xpathObj) {
        std::cout << "getEntries xpath failed" << std::endl;
        return result;
    }

    //std::cout << "getEntries TYPE :" << xpathObj->type << std::endl;

    if (XPATH_NODESET == xpathObj->type) {

        //std::cout << "getEntries COUNT :" << xpathObj->nodesetval->nodeNr << std::endl;

        ConfigEntry entry;

        for (int i = 0; i < xpathObj->nodesetval->nodeNr; i++) {

            xmlNodePtr node = xpathObj->nodesetval->nodeTab[i];

            //std::cout << "NODE: " << node->type << " " << node->name << std::endl;

            if (XML_ELEMENT_NODE == node->type) {

                // name
                entry.key = (char*)node->name;

                // value if available
                if (node->children && node->children->content && node->children->type == XML_TEXT_NODE)
                    entry.value = (char*)node->children->content;

                // attributes
                xmlAttrPtr attr = node->properties;
                while (attr) {
                    entry.attributes[(char*)attr->name] = (char*)attr->children->content;
                    attr = attr->next;
                }

                result.push_back(entry);
                entry.clear();
            }

        }
    }

    xmlXPathFreeObject(xpathObj);
    return result;
}
