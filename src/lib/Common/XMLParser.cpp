/***************************************************************************
 *            XMLParser.cpp
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
 
#include "XMLParser.h"
#include <iostream>
#include <string.h>
using namespace std;

CXMLNode::CXMLNode(xmlNode* p_NodePtr, int p_nIdx, CXMLNode* pParent)
{
  m_pNode = p_NodePtr;
  m_nIdx  = p_nIdx;
  m_nChildCount = 0;
  m_pParent = pParent;

  switch(p_NodePtr->type) {

    case XML_ELEMENT_NODE:
      m_type = ElementNode;
      break;


    case XML_COMMENT_NODE:
      m_type = CommentNode;
      break;
      
    default:
      m_type = UnknownNode;
      break;
  }
  /*
Enum xmlElementType {
    XML_ELEMENT_NODE = 1
    XML_ATTRIBUTE_NODE = 2
    XML_TEXT_NODE = 3
    XML_CDATA_SECTION_NODE = 4
    XML_ENTITY_REF_NODE = 5
    XML_ENTITY_NODE = 6
    XML_PI_NODE = 7
    XML_COMMENT_NODE = 8
    XML_DOCUMENT_NODE = 9
    XML_DOCUMENT_TYPE_NODE = 10
    XML_DOCUMENT_FRAG_NODE = 11
    XML_NOTATION_NODE = 12
    XML_HTML_DOCUMENT_NODE = 13
    XML_DTD_NODE = 14
    XML_ELEMENT_DECL = 15
    XML_ATTRIBUTE_DECL = 16
    XML_ENTITY_DECL = 17
    XML_NAMESPACE_DECL = 18
    XML_XINCLUDE_START = 19
    XML_XINCLUDE_END = 20
    XML_DOCB_DOCUMENT_NODE = 21
}
   */
}

CXMLNode::~CXMLNode()
{
  ClearChildren();
}

void CXMLNode::ClearChildren()
{
  // keep deleting the first element until the list is empty
  while(!m_NodeList.empty()) {
    delete m_NodeList.begin()->second;
    m_NodeList.erase(m_NodeList.begin());
  }
  
  m_nChildCount = 0;
}


int CXMLNode::ChildCount()
{
  xmlNode* pTmpNode;
  
  if(m_nChildCount > 0) {
    return m_nChildCount;
  }
    
  for(pTmpNode = m_pNode->children; pTmpNode; pTmpNode = pTmpNode->next) {
    m_nChildCount++;
  }

  return m_nChildCount;
}

CXMLNode* CXMLNode::ChildNode(int p_nIdx)
{
  CXMLNode* pResult = NULL;
  int nIdx = 0;
  xmlNode* pTmpNode;
  
  m_NodeListIter = m_NodeList.find(p_nIdx);
  if(m_NodeListIter != m_NodeList.end()) {
    pResult = m_NodeList[p_nIdx];
    return pResult;
  }

  for(pTmpNode = m_pNode->children; pTmpNode; pTmpNode = pTmpNode->next) {
    
    if(nIdx == p_nIdx) {
      pResult = new CXMLNode(pTmpNode, nIdx, this);
      m_NodeList[nIdx] = pResult;
      break;
    }

    nIdx++;
  }
  return pResult;
}

CXMLNode* CXMLNode::FindNodeByName(std::string p_sName, 
                             bool p_bRecursive)
{
  CXMLNode* pResult = NULL;
  int i = 0;
                                 
  for(i = 0; i < ChildCount(); i++) {
    
    if(ChildNode(i)->Name().compare(p_sName) == 0) {
      pResult = ChildNode(i);
      break;
    }
      
    // recursively search children's child nodes
    if(p_bRecursive && (ChildNode(i)->ChildCount() > 0)) {
      pResult = ChildNode(i)->FindNodeByName(p_sName, p_bRecursive);
      if(pResult) {
        break;
      }
    }
      
  }
                                 
  return pResult;
}

CXMLNode* CXMLNode::FindNodeByValue(std::string p_sName, 
                              std::string p_sValue, 
                              bool p_bRecursive)
{
  CXMLNode* pResult = NULL;
  int i = 0;
                                 
  for(i = 0; i < ChildCount(); i++) {
    
    if((ChildNode(i)->Name().compare(p_sName) == 0) &&
       (ChildNode(i)->Value().compare(p_sValue) == 0)) {
      pResult = ChildNode(i);
      break;
    }
      
    // recursively search children's child nodes
    if(p_bRecursive && (ChildNode(i)->ChildCount() > 0)) {
      pResult = ChildNode(i)->FindNodeByValue(p_sName, p_sValue, p_bRecursive);
      if(pResult) {
        break;
      }
    }
      
  }
                                 
  return pResult;
}

std::string CXMLNode::Attribute(std::string p_sName)
{
  xmlAttr* attr = m_pNode->properties;
  while(attr) {
    string sAttr = (char*)attr->name;
    if(sAttr.compare(p_sName) == 0)    
      return string((char*)attr->children->content);
    
    attr = attr->next;
  } 
  return "";
}

unsigned int CXMLNode::AttributeAsUInt(std::string p_sName)
{
  string sAttribute = Attribute(p_sName);
  unsigned int nResult = 0;
  if(sAttribute.length() > 0) {
    nResult = strtoul(sAttribute.c_str(), NULL, 0);
  }
  return nResult;
}

void CXMLNode::attributes(std::map<std::string, std::string> &result)
{
  xmlAttr* attr = m_pNode->properties;
  while(attr) {
    string key = (char*)attr->name;
    string value = (char*)attr->children->content;
    result[key] = value;    
    attr = attr->next;
  }
}

void CXMLNode::setAttribute(std::string key, std::string value)
{
  //xmlAttrPtr attr = 
  xmlSetProp(m_pNode, BAD_CAST key.c_str(), BAD_CAST value.c_str());
}


std::string CXMLNode::Name()
{
  return string((char*)m_pNode->name);
}

std::string CXMLNode::Value()
{
  if(m_pNode->children && 
     m_pNode->children->content && 
     m_pNode->children->type == XML_TEXT_NODE) {
    return ((char*)m_pNode->children->content);
  }
  else {
    return "";
  }
}

int CXMLNode::ValueAsInt()
{
  std::string value = Value();
  int result = -1;
  if(isdigit(value.c_str()[0]) != 0)
    result = strtol(value.c_str(), NULL, 0);
  return result;
}

void CXMLNode::Value(std::string p_sValue)
{
  if(m_pNode->children)
    xmlNodeSetContent(m_pNode->children, BAD_CAST p_sValue.c_str());       
  else
    xmlNodeAddContent(m_pNode, BAD_CAST p_sValue.c_str());
}

void CXMLNode::Value(int p_nValue)
{
  char szValue[10];
  sprintf(szValue, "%d", p_nValue);
  Value(szValue);
}

// todo: Verify that this function works as expected.
/*CXMLNode* CXMLNode::AddChild(std::string p_sName, std::string p_sValue)
{

cout << "add child: " << this->name() << "*" << endl;
  
  xmlNode* newChild = xmlNewTextChild(m_pNode, NULL, BAD_CAST p_sName.c_str(), BAD_CAST p_sValue.c_str());    

  ClearChildren();
  
  // TODO Why do we clear the children here? Aren't we trying to add children?
  int idx = ChildCount();
  m_NodeList[idx] = new CXMLNode(newChild, idx, this);

  return m_NodeList[idx];
}*/

void CXMLNode::RemoveChild(int p_nIdx)
{
  xmlNode* pTmpNode;
  int nIdx = 0;

  for(pTmpNode = m_pNode->children; pTmpNode; pTmpNode = pTmpNode->next) {
    
    if(nIdx == p_nIdx) {      
      xmlUnlinkNode(pTmpNode);
      xmlFreeNode(pTmpNode);      
      ClearChildren();
      break;
    }
    nIdx++;
  }  
}

CXMLDocument::CXMLDocument()
{
  m_pRootNode = NULL;
  m_pDoc = NULL;
}

CXMLDocument::~CXMLDocument()
{              
    if(NULL != m_pRootNode) {
        delete m_pRootNode;
        m_pRootNode = NULL;
    }

    if(m_pDoc != NULL) {
        xmlFreeDoc(m_pDoc);
        m_pDoc = NULL;
    }
}

bool CXMLDocument::LoadFromFile(std::string p_sFileName)
{
    if(NULL == m_pRootNode) {
        delete m_pRootNode;
        m_pRootNode = NULL;
    }

    if(m_pDoc != NULL) {
        xmlFreeDoc(m_pDoc);
        m_pDoc = NULL;
    }

    m_sFileName = p_sFileName;
    m_pDoc = xmlReadFile(m_sFileName.c_str(), "UTF-8", XML_PARSE_NOBLANKS);
    return (m_pDoc != NULL);
}

bool CXMLDocument::LoadFromString(std::string p_sXML)
{
    if(NULL == m_pRootNode) {
        delete m_pRootNode;
        m_pRootNode = NULL;
    }

    if(m_pDoc != NULL) {
        xmlFreeDoc(m_pDoc);
        m_pDoc = NULL;
    }

	m_sFileName = "";
	m_pDoc = xmlReadMemory(p_sXML.c_str(), p_sXML.length(), "", NULL, XML_PARSE_NOBLANKS);
	return (m_pDoc != NULL);
}

bool CXMLDocument::Save()
{
	if(m_sFileName.length() > 0) {
		xmlSaveFormatFileEnc(m_sFileName.c_str(), m_pDoc, "UTF-8", 1);
		return true;
	}
	else {
		return false;
	}
}

CXMLNode* CXMLDocument::RootNode()
{
  if(!m_pRootNode) {
    m_pRootNode = new CXMLNode(xmlDocGetRootElement(m_pDoc), 0, NULL);
  }
  return m_pRootNode;
}
