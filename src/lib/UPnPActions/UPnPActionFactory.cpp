/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            UPnPActionFactory.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#include "UPnPActionFactory.h"
#include "../Common/Common.h"
#include "../Common/RegEx.h"
#include "../SharedLog.h"
#include "../ControlInterface/ControlInterface.h"


#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>

using namespace std;
using namespace fuppes;

CUPnPAction* CUPnPActionFactory::buildActionFromString(std::string p_sContent, CDeviceSettings* pDeviceSettings, std::string vfolderLayout, bool vfoldersEnabled)
{
    upnp_error_code_t error;
    CUPnPAction* action = CUPnPAction::create(p_sContent, pDeviceSettings, vfolderLayout, vfoldersEnabled, error);

    if(NULL == action) {
        //CSharedLog::Log(L_DBG, __FILE__, __LINE__, "unhandled UPnP Action \"%s\"", sName.c_str());
        return NULL;
    }

    if(UPNP_SERVICE_CONTENT_DIRECTORY == action->GetTargetDeviceType()) {

        if(0 == action->getAction().compare("Browse")) {
            if (!parseBrowseAction((CUPnPBrowse*)action, action->m_xmlActionNode)) {
            	delete action;
            	return NULL;
            }
        }
        else if(0 == action->getAction().compare("Search")) {
            if (!parseSearchAction((CUPnPSearch*)action, action->m_xmlActionNode)) {
            	delete action;
            	return NULL;
            }
        }
    }

    return action;
}


inline bool getValue(xmlNodePtr node, std::string &name, std::string &value)
{
  if(node) {
    name = (char*)node->name;
    if(node->children &&
       node->children->content &&
       node->children->type == XML_TEXT_NODE) {
      value = (char*)node->children->content;
    }
    else {
      value = "";
    }
    return true;
  }
  return false;
}

bool CUPnPActionFactory::parseBrowseAction(CUPnPBrowse* pAction, xmlNode* actionNode)
{

/*<?xml version="1.0" encoding="utf-8"?>
  <s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/">
    <s:Body>
     <u:Browse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1">
      <ObjectID>0</ObjectID>
      <BrowseFlag>BrowseDirectChildren</BrowseFlag>
      <Filter>*</Filter>
      <StartingIndex>0</StartingIndex>
      <RequestedCount>10</RequestedCount>
      <SortCriteria />
     </u:Browse>
    </s:Body>
  </s:Envelope>*/

  bool valid = true;
  string name;
  string value;
  xmlNodePtr node = xmlFirstElementChild(actionNode);

  while(node) {

    if(!getValue(node, name, value))
      return false;

    // xbox used ContainerID don't know if this is still true
    if(name.compare("ObjectID") == 0 || name.compare("ContainerID") == 0) { 
      pAction->m_sObjectId = value;
    }
    else if(name.compare("BrowseFlag") == 0) {
      if(value.compare("BrowseMetadata") == 0)
        pAction->m_nBrowseFlag = UPNP_BROWSE_FLAG_METADATA;
      else if(value.compare("BrowseDirectChildren") == 0)
        pAction->m_nBrowseFlag = UPNP_BROWSE_FLAG_DIRECT_CHILDREN;
    }
    else if(name.compare("Filter") == 0) {
      pAction->m_sFilter = value;
    }
    else if(name.compare("StartingIndex") == 0) {
      pAction->m_nStartingIndex = atoi(value.c_str());
    }
    else if(name.compare("RequestedCount") == 0) {
      pAction->m_nRequestedCount = atoi(value.c_str());
    }
    else if(name.compare("SortCriteria") == 0) {
      valid = parseSortCriteria(value, pAction);
    }
    
    node = xmlNextElementSibling(node);
  }

  return valid;
 
/*
  // Object ID
  string sRxObjId;
  if(!pAction->DeviceSettings()->Xbox360Support()) {
    sRxObjId = "<ObjectID.*>(.+)</ObjectID>";
  }
  else {
    // Xbox 360 does a browse using ContainerID instead of ObjectID for Pictures
    sRxObjId = "<ContainerID.*>(.+)</ContainerID>";
  }    
  RegEx rxObjId(sRxObjId);
  if(rxObjId.Search(pAction->GetContent())) {
		if(rxObjId.Match(1).length() > 10)
			return false;

    pAction->m_sObjectId = rxObjId.Match(1);
  }
		
  // Browse flag
  pAction->m_nBrowseFlag = UPNP_BROWSE_FLAG_UNKNOWN;    
  RegEx rxBrowseFlag("<BrowseFlag.*>(.+)</BrowseFlag>");
  if(rxBrowseFlag.Search(pAction->GetContent()))
  {
    string sMatch = rxBrowseFlag.Match(1);
    if(sMatch.compare("BrowseMetadata") == 0)
      pAction->m_nBrowseFlag = UPNP_BROWSE_FLAG_METADATA;
    else if(sMatch.compare("BrowseDirectChildren") == 0)
      pAction->m_nBrowseFlag = UPNP_BROWSE_FLAG_DIRECT_CHILDREN;
  }  
  
  // Filter 
  RegEx rxFilter("<Filter.*>(.+)</Filter>");
  if(rxFilter.Search(pAction->GetContent()))  
    pAction->m_sFilter = rxFilter.Match(1);  
  else
    pAction->m_sFilter = "";

  // Starting index
  RegEx rxStartIdx("<StartingIndex.*>(.+)</StartingIndex>");
  if(rxStartIdx.Search(pAction->GetContent()))  
    pAction->m_nStartingIndex = atoi(rxStartIdx.Match(1).c_str());

  // Requested count
  RegEx rxReqCnt("<RequestedCount.*>(.+)</RequestedCount>");
  if(rxReqCnt.Search(pAction->GetContent()))  
    pAction->m_nRequestedCount = atoi(rxReqCnt.Match(1).c_str());  
*/
  
	//parseSortCriteria(pAction);
}

bool CUPnPActionFactory::parseSearchAction(CUPnPSearch* action, xmlNode* actionNode)
{
  /*
	<?xml version="1.0" encoding="utf-8"?>
	<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/">
	<s:Body>
	  <u:Search xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1">
		  <ContainerID>0</ContainerID>
			<SearchCriteria>(upnp:class contains "object.item.imageItem") and (dc:title contains "")</SearchCriteria>
			<Filter>*</Filter>
			<StartingIndex>0</StartingIndex>
			<RequestedCount>7</RequestedCount>
			<SortCriteria></SortCriteria>
		</u:Search>
	</s:Body>
	</s:Envelope> */


  bool valid = true;
  string name;
  string value;
  xmlNodePtr node = xmlFirstElementChild(actionNode);

  while(node) {

    if(!getValue(node, name, value))
      return false;

    if(name.compare("ContainerID") == 0) { 
        action->m_sObjectId = value;
    }
    else if(name.compare("SearchCriteria") == 0) {
        action->m_searchCriteria = value;
    }
    else if(name.compare("Filter") == 0) {
        action->m_sFilter = value;
    }
    else if(name.compare("StartingIndex") == 0) {
        action->m_nStartingIndex = atoi(value.c_str());
    }
    else if(name.compare("RequestedCount") == 0) {
        action->m_nRequestedCount = atoi(value.c_str());
    }
    else if(name.compare("SortCriteria") == 0) {
      valid = parseSortCriteria(value, action);
    }
    
    node = xmlNextElementSibling(node);
  }

  if(!valid) {
      return false;
  }

  return action->searchCriteria.parse(action->m_searchCriteria, action->m_searchCriteriaSql);

  
  /*
  // Container ID
  RegEx rxContainerID("<ContainerID.*>(.+)</ContainerID>");
  if (rxContainerID.Search(pAction->GetContent()))
    pAction->m_sObjectId = rxContainerID.Match(1);
	
	// Search Criteria
	RegEx rxSearchCriteria("<SearchCriteria.*>(.+)</SearchCriteria>");
	if(rxSearchCriteria.Search(pAction->GetContent()))
	  pAction->m_sSearchCriteria = rxSearchCriteria.Match(1);
	
  // Filter
  RegEx rxFilter("<Filter.*>(.+)</Filter>");
  if(rxFilter.Search(pAction->GetContent()))  
    pAction->m_sFilter = rxFilter.Match(1);  
  else
    pAction->m_sFilter = "";

  // Starting index
  RegEx rxStartIdx("<StartingIndex.*>(.+)</StartingIndex>");
  if(rxStartIdx.Search(pAction->GetContent()))  
    pAction->m_nStartingIndex = atoi(rxStartIdx.Match(1).c_str());

  // Requested count
  RegEx rxReqCnt("<RequestedCount.*>(.+)</RequestedCount>");
  if(rxReqCnt.Search(pAction->GetContent()))  
    pAction->m_nRequestedCount = atoi(rxReqCnt.Match(1).c_str()); 

  // sort cirteria
	parseSortCriteria(pAction);
	
	return true;
  */
}

bool CUPnPActionFactory::parseSortCriteria(std::string sort, CUPnPBrowseSearchBase* action)
{
  // todo: error handling as defined by the upnp forum
  action->m_isSupportedSort = action->sortCriteria.parse(sort, action->m_sortCriteriaSQL);
  return action->m_isSupportedSort;
}

/*
bool CUPnPActionFactory::parseDestroyObjectAction(CUPnPAction* action)
{
<?xml version="1.0" encoding="utf-8"?>
  <s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/">
  <s:Body>
  <u:DestroyObject xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1">
  <ObjectID>0</ObjectID>
  </u:DestroyObject>
  </s:Body>
  </s:Envelope>
  
  
  // object ID
  RegEx rxObjectID("<ObjectID.*>(.+)</ObjectID>");
  if (rxObjectID.Search(action->GetContent().c_str()))
    action->m_sObjectId = rxObjectID.Match(1);
  else
    return false;
    
    
  return true;  
}


bool CUPnPActionFactory::parseCreateReferenceAction(CUPnPAction* action, xmlNode* actionNode) // static
{
<?xml version="1.0" encoding="utf-8"?>
  <s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/">
  <s:Body>
  <u:CreateReference xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1">
  <ObjectID>0</ObjectID>
  <ObjectID>123</ObjectID>
  </u:CreateReference>
  </s:Body>
  </s:Envelope>
  

  string name;
  string value;
  xmlNodePtr node = xmlFirstElementChild(actionNode);

  while(node) {

    if(!getValue(node, name, value))
      return false;

    if(name.compare("ContainerID") == 0) { 
      action->m_containerId = value;
    }
    else if(name.compare("ObjectID") == 0) { 
      action->m_sObjectId = value;
    }

    node = xmlNextElementSibling(node);
  }    
    
  return true;  
}
*/
