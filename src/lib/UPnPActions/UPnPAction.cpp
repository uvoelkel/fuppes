/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            UPnPAction.cpp
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

#include "UPnPAction.h"
#include "UPnPBrowse.h"
#include "UPnPSearch.h"


CUPnPAction* CUPnPAction::create(std::string content, CDeviceSettings* deviceSettings, std::string vfolder, bool vfoldersEnabled, upnp_error_code_t& error) // static
{
    error = upnp_401_invalid_action;

    xmlDocPtr doc = xmlReadMemory(content.c_str(), content.length(), "", NULL, 0);
    if(NULL == doc) {
        return NULL;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc); // s:Envelope
    if(NULL == root) {
        xmlFreeDoc(doc);
        return NULL;
    }

    xmlNodePtr body = xmlFirstElementChild(root); // s:Body
    if(NULL == body) {
        xmlFreeDoc(doc);
        return NULL;
    }

    xmlNodePtr actionNode = xmlFirstElementChild(body); // u:*
    if(NULL == actionNode) {
        xmlFreeDoc(doc);
        return NULL;
    }

    // check target
    if(NULL == actionNode->nsDef) {
        xmlFreeDoc(doc);
        return NULL;
    }


    std::string target = (char*)actionNode->nsDef->href;
    std::string action = (char*)actionNode->name;

    CUPnPAction* result = NULL;
    UPNP_DEVICE_TYPE targetDevice = UPNP_DEVICE_UNKNOWN;

    if(target.compare("urn:schemas-upnp-org:service:ContentDirectory:1") == 0) {
        targetDevice = UPNP_SERVICE_CONTENT_DIRECTORY;

        if(0 == action.compare("Browse")) {
            result = new CUPnPBrowse(content);
        }
        else if(0 == action.compare("Search")) {
            result = new CUPnPSearch(content);
        }
    }
    else if(target.compare("urn:schemas-upnp-org:service:ConnectionManager:1") == 0) {
        targetDevice = UPNP_SERVICE_CONNECTION_MANAGER;
    }
    else if(target.compare("urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1") == 0) {
        targetDevice = UPNP_SERVICE_X_MS_MEDIA_RECEIVER_REGISTRAR;
    }
    else if(target.compare("urn:fuppes:service:SoapControl:1") == 0) {
        targetDevice = FUPPES_SOAP_CONTROL;
    }

    if(NULL == result) {
        result = new CUPnPAction();
    }

    result->m_pDeviceSettings = deviceSettings;
    result->m_virtualFolderLayout = vfolder;
    result->m_virtualFoldersEnabled = vfoldersEnabled;
    result->m_nTargetDeviceType = targetDevice;
    result->m_xmlDoc = doc;
    result->m_xmlActionNode = actionNode;
    result->m_sContent = content;
    result->m_action = action;

    if(UPNP_DEVICE_UNKNOWN != targetDevice) {
        error = result->parse();
    }
    return result;
}

CUPnPAction::CUPnPAction()
{
    m_xmlDoc = NULL;
    m_xmlActionNode = NULL;
    m_nTargetDeviceType = UPNP_DEVICE_UNKNOWN;
    m_pDeviceSettings = NULL;
    m_nActionType = 0;
    m_virtualFoldersEnabled = false;
}

CUPnPAction::~CUPnPAction()
{
    if(NULL != m_xmlDoc) {
        xmlFreeDoc(m_xmlDoc);
    }
}

std::string CUPnPAction::value(std::string key)
{
    xmlNodePtr tmp = xmlFirstElementChild(m_xmlActionNode);
    while (NULL != tmp) {

        std::string name = (char*)tmp->name;

        if(name.compare(key) != 0) {
            tmp = xmlNextElementSibling(tmp);
            continue;
        }

        xmlChar* content = xmlNodeGetContent(tmp);
        if(NULL == content)
            return "";

        std::string value = (char*)content;
        xmlFree(content);
        return value;
    }

    return "";
}

bool CUPnPAction::valueAsUInt(std::string key, unsigned int* value)
{
    std::string tmp = this->value(key);
    if(tmp.empty())
        return false;

    *value = HexToInt(tmp);
    return true;
}

CUPnPBrowseSearchBase::CUPnPBrowseSearchBase(UPNP_DEVICE_TYPE p_nTargetDeviceType, int p_nActionType, std::string p_sContent) :
        CUPnPAction(p_nTargetDeviceType, "", p_nActionType, p_sContent)
{
    m_nStartingIndex = 0;
    m_nRequestedCount = 0;
    m_isSupportedSort = false;
}

bool CUPnPBrowseSearchBase::IncludeProperty(std::string p_sProperty)
{
    if(m_sFilter.compare("*") == 0) {
        return true;
    }

    if(m_sFilter.find(p_sProperty) != std::string::npos) {
        return true;
    }

    return false;
}

std::string CUPnPBrowseSearchBase::getSortOrder()
{
    if(m_isSupportedSort) {
        return m_sortCriteriaSQL;
    }
    else {
        return " TITLE asc ";
    }
}
