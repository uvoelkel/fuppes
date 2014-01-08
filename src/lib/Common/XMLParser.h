/***************************************************************************
 *            XMLParser.h
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2007-2008 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#ifndef _XMLPARSER_H
#define _XMLPARSER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <map>

class CXMLNode
{
  public:
    enum ElementType {
      UnknownNode,
      ElementNode,
      AttributeNode,
      TextNode,
      CommentNode
    };
    
    CXMLNode(xmlNode* p_NodePtr, int p_nIdx, CXMLNode* pParent);
    ~CXMLNode();


    CXMLNode::ElementType type() { return m_type; }
    
    int ChildCount();
    CXMLNode* ChildNode(int p_nIdx);
    CXMLNode* parent() { return m_pParent; }
    CXMLNode* FindNodeByName(std::string p_sName, 
                             bool p_bRecursive = false);
    CXMLNode* FindNodeByValue(std::string p_sName, 
                              std::string p_sValue, 
                              bool p_bRecursive = false);

    std::string Attribute(std::string p_sName);
    std::string attribute(std::string key) { return Attribute(key); }
    unsigned int AttributeAsUInt(std::string p_sName);
    void attributes(std::map<std::string, std::string> &result);
    void setAttribute(std::string key, std::string value);
    
    std::string Name();
    std::string name() { return Name(); }
    
    std::string Value();
    std::string value() { return Value(); }
    int ValueAsInt();
    void Value(std::string p_sValue);
    void Value(int p_nValue);
  
    int Index() { return m_nIdx; }
  
    //CXMLNode* AddChild(std::string p_sName, std::string p_sValue);  
    void RemoveChild(int p_nIdx);
    
  private:
    xmlNode* m_pNode;
    int      m_nIdx;
    int      m_nChildCount;
    ElementType m_type;
    std::map<int, CXMLNode*> m_NodeList;
    std::map<int, CXMLNode*>::iterator m_NodeListIter;
  
    CXMLNode* m_pParent;
  
    void ClearChildren();
};

class CXMLDocument
{
  public:
    CXMLDocument();
    ~CXMLDocument();
    bool LoadFromFile(std::string p_sFileName);
		bool LoadFromString(std::string p_sXML);
    bool Save();
  
    CXMLNode* RootNode();
  
  private:
    xmlDocPtr   m_pDoc;
    std::string m_sFileName;
    CXMLNode*   m_pRootNode;
};

#endif // _XMLPARSER_H
