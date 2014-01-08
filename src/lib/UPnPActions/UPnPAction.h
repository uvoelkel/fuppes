/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            UPnPAction.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2009 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#ifndef _UPNPACTION_H
#define _UPNPACTION_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/xmlwriter.h>

#include "../UPnP/UPnPBase.h"
#include "../DeviceIdentification/DeviceSettings.h"
#include <string>

#include "../Database/SortCriteria.h"

#define UPNP_UNKNOWN 0

class UPnPActionFactory;

/* ContentDirectory v1 actions

 GetSearchCapabilities                  R
 GetSortCapabilities                    R
 GetSystemUpdateID                      R
 Browse                                 R
 Search                                 O
 CreateObject                           O
 DestroyObject                          O
 UpdateObject                           O
 ImportResource                         O
 ExportResource                         O
 StopTransferResource                   O
 GetTransferProgress                    O
 DeleteResource                         O
 CreateReference                        O

 */

/* ContentDirectory v3 actions
 
 GetSearchCapabilities()                                                         R
 GetSortCapabilities()                                                           R
 GetSortExtensionCapabilities()                                                  O
 GetFeatureList()                                                                R
 GetSystemUpdateID()                                                             R
 GetServiceResetToken()                                                          R
 Browse()                                                                        R
 Search()                                                                        O
 CreateObject()                                                                  O
 DestroyObject()                                                                 O
 UpdateObject()                                                                  O
 MoveObject()                                                                    O
 ImportResource()                                                                O
 ExportResource()                                                                O
 DeleteResource()                                                                O
 StopTransferResource()                                                          O
 GetTransferProgress()                                                           O
 CreateReference()                                                               O
 FreeFormQuery()                                                                 O
 GetFreeFormQueryCapabilities()                                                  O
 */

typedef enum UPNP_CONTENT_DIRECTORY_ACTIONS
{

    UPNP_UNKNWON = 0,

    UPNP_GET_SEARCH_CAPABILITIES = 1,
    UPNP_GET_SORT_CAPABILITIES = 2,
    UPNP_GET_SORT_EXTENSION_CAPABILITIES = 3,  // v 3 (optional)
    // GetFeatureList()  v3 required
    UPNP_GET_SYSTEM_UPDATE_ID = 4,
    // GetServiceResetToken() v3 required

    UPNP_BROWSE = 5,
    UPNP_SEARCH = 6,

    UPNP_CREATE_OBJECT = 7,
    UPNP_DESTROY_OBJECT = 8,
    UPNP_UPDATE_OBJECT = 9,
    // MoveObject() v3 optional

    // ImportResource()
    // ExportResource()
    // DeleteResource()

    // StopTransferResource()
    // GetTransferProgress()

    UPNP_CREATE_REFERENCE = 10,

// FreeFormQuery() v3 optional
// GetFreeFormQueryCapabilities() v3 optional

} UPNP_CONTENT_DIRECTORY_ACTIONS;

typedef enum UPNP_AV_TRANSPORT_ACTIONS
{
} UPNP_AV_TRANSPORT_ACTIONS;

typedef enum UPNP_CONNECTION_MANAGER_ACTIONS
{
    CMA_UNKNOWN = 0,
    CMA_GET_PROTOCOL_INFO = 1,
    //CMA_PREPARE_FOR_CONNECTION      = 2,
    //CMA_CONNECTION_COMPLETE         = 3,
    CMA_GET_CURRENT_CONNECTION_IDS = 4,
    CMA_GET_CURRENT_CONNECTION_INFO = 5
} UPNP_CONNECTION_MANAGER_ACTIONS;

typedef enum UPNP_X_MS_MEDIA_RECEIVER_REGISTRAR_ACTIONS
{
    UPNP_IS_AUTHORIZED = 1,
    UPNP_IS_VALIDATED = 2,
    UPNP_REGISTER_DEVICE = 3
} UPNP_X_MS_MEDIA_RECEIVER_REGISTRAR_ACTIONS;

class CUPnPAction
{
        friend class CUPnPActionFactory;

    public:
        static CUPnPAction* create(std::string content, CDeviceSettings* deviceSettings, std::string vfolder, bool vfoldersEnabled, upnp_error_code_t& error);

        virtual ~CUPnPAction();

        int GetActionType()
        {

            if (m_nActionType != 0)
                return m_nActionType;

            std::string name = (char*)m_xmlActionNode->name;

            if (m_nTargetDeviceType == UPNP_SERVICE_CONTENT_DIRECTORY) {

                if (name.compare("GetSearchCapabilities") == 0) {
                    m_nActionType = UPNP_GET_SEARCH_CAPABILITIES;
                }
                else if (name.compare("GetSortCapabilities") == 0) {
                    m_nActionType = UPNP_GET_SORT_CAPABILITIES;
                }
                else if (name.compare("GetSortExtensionCapabilities") == 0) {
                    m_nActionType = UPNP_GET_SORT_EXTENSION_CAPABILITIES;
                }
                else if (name.compare("GetSystemUpdateID") == 0) {
                    m_nActionType = UPNP_GET_SYSTEM_UPDATE_ID;
                }
                else if (name.compare("Browse") == 0) {
                    m_nActionType = UPNP_BROWSE;
                }
                else if (name.compare("Search") == 0) {
                    m_nActionType = UPNP_SEARCH;
                }
                else if (name.compare("CreateObject") == 0) {
                    m_nActionType = UPNP_CREATE_OBJECT;
                }
                else if (name.compare("DestroyObject") == 0) {
                    m_nActionType = UPNP_DESTROY_OBJECT;
                }
                else if (name.compare("UpdateObject") == 0) {
                    m_nActionType = UPNP_UPDATE_OBJECT;
                }
                else if (name.compare("CreateReference") == 0) {
                    m_nActionType = UPNP_CREATE_REFERENCE;
                }

            }

            return m_nActionType;
        }

        std::string getAction()
        {
            return m_action;
        }

        std::string GetContent()
        {
            return m_sContent;
        }
        UPNP_DEVICE_TYPE GetTargetDeviceType()
        {
            return m_nTargetDeviceType;
        }

        CDeviceSettings* DeviceSettings()
        {
            return m_pDeviceSettings;
        }
        /*void DeviceSettings(CDeviceSettings* pSettings)
         {
         m_pDeviceSettings = pSettings;
         }*/

        std::string virtualFolderLayout()
        {
            return m_virtualFolderLayout;
        }

        bool virtualFoldersEnabled()
        {
            return m_virtualFoldersEnabled;
        }

        /*void setVirtualFolderLayout(std::string layout)
         {
         m_virtualFolderLayout = layout;
         }*/

        std::string objectId()
        {
            return m_sObjectId;
        }
        object_id_t getObjectId()
        {
            return HexToObjectId(m_sObjectId);
        }

        std::string value(std::string key);
        bool valueAsUInt(std::string key, unsigned int* value);

    protected:
        CUPnPAction();

        CUPnPAction(UPNP_DEVICE_TYPE p_nTargetDeviceType, std::string action, int p_nActionType, std::string p_sContent)
        {
            m_nActionType = p_nActionType;
            m_sContent = p_sContent;
            m_nTargetDeviceType = p_nTargetDeviceType;
            m_action = action;
            m_xmlDoc = NULL;
            m_xmlActionNode = NULL;
            m_pDeviceSettings = NULL;
            m_virtualFoldersEnabled = false;
        }

        virtual upnp_error_code_t parse()
        {
            return upnp_200_ok;
        }

        xmlDocPtr m_xmlDoc;
        xmlNodePtr m_xmlActionNode;

    private:

        UPNP_DEVICE_TYPE m_nTargetDeviceType;
        std::string m_action;
        int m_nActionType;
        std::string m_sContent;

        CDeviceSettings* m_pDeviceSettings;
        std::string m_virtualFolderLayout;
        bool m_virtualFoldersEnabled;

        std::string m_sObjectId;
        std::string m_containerId;

};

class CUPnPBrowseSearchBase: public CUPnPAction
{
    protected:
        CUPnPBrowseSearchBase(UPNP_DEVICE_TYPE p_nTargetDeviceType, int p_nActionType, std::string p_sContent);

    public:
        virtual std::string getQuery(bool count = false) = 0;

        bool IncludeProperty(std::string p_sProperty);
        std::string getSortOrder();

        std::string m_sFilter;
        unsigned int m_nStartingIndex;
        unsigned int m_nRequestedCount;

        std::string m_sortCriteria;
        std::string m_sortCriteriaSQL;
        bool m_isSupportedSort;

        Database::SortCriteria sortCriteria;

    protected:
        std::string m_query;
        std::string m_queryCount;
};

/*
 class FuppesCtrlAction: public CUPnPAction
 {
 public:
 FuppesCtrlAction(std::string action, std::string content)
 :CUPnPAction(FUPPES_SOAP_CONTROL, action, 0, content) {

 }
 };
 */

#endif // _UPNPACTION_H
/*typedef enum tagUPNP_ACTION_TYPE
 {
 UPNP_ACTION_TYPE_UNKNOWN,

 UPNP_ACTION_TYPE_CONTENT_DIRECTORY_BROWSE,
 */
/*
 POST /UPnPServices/ContentDirectory/control/ HTTP/1.1
 Host: 192.168.0.3:60230
 User-Agent: UPnP/1.0 DLNADOC/1.00
 SOAPACTION: "urn:schemas-upnp-org:service:ContentDirectory:1#Search"
 Content-Type: text/xml; charset="utf-8"
 Content-Length: 517

 <?xml version="1.0" encoding="utf-8"?><s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:Search xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1"><ContainerID>0</ContainerID><SearchCriteria>(upnp:class contains "object.item.imageItem") and (dc:title contains "")</SearchCriteria><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>7</RequestedCount><SortCriteria></SortCriteria></u:Search></s:Body></s:Envelope>
 */

//UPNP_ACTION_TYPE_CONTENT_DIRECTORY_SEARCH,
/* POST /UPnPServices/ContentDirectory/control/ HTTP/1.1
 HOST: 192.168.0.3:1117
 SOAPACTION: "urn:schemas-upnp-org:service:ContentDirectory:1#GetSearchCapabilities"
 CONTENT-TYPE: text/xml ; charset="utf-8"
 Content-Length: 299

 <?xml version="1.0" encoding="utf-8"?>
 <s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s=
 "http://schemas.xmlsoap.org/soap/envelope/">
 <s:Body>
 <u:GetSearchCapabilities xmlns:u="urn:schemas-upnp-org:service:ContentDire
 ctory:1" />
 </s:Body>
 </s:Envelope> */

//UPNP_ACTION_TYPE_CONTENT_DIRECTORY_GET_SEARCH_CAPABILITIES,
/*
 POST /UPnPServices/ContentDirectory/control/ HTTP/1.1
 HOST: 192.168.0.3:1117
 SOAPACTION: "urn:schemas-upnp-org:service:ContentDirectory:1#GetSortCapabilities
 "
 CONTENT-TYPE: text/xml ; charset="utf-8"
 Content-Length: 297

 <?xml version="1.0" encoding="utf-8"?>
 <s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s=
 "http://schemas.xmlsoap.org/soap/envelope/">
 <s:Body>
 <u:GetSortCapabilities xmlns:u="urn:schemas-upnp-org:service:ContentDirect
 ory:1" />
 </s:Body>
 </s:Envelope> */

//UPNP_ACTION_TYPE_CONTENT_DIRECTORY_GET_SORT_CAPABILITIES,
/*
 POST /UPnPServices/ContentDirectory/control/ HTTP/1.1
 HOST: 192.168.0.3:1117
 SOAPACTION: "urn:schemas-upnp-org:service:ContentDirectory:1#GetSystemUpdateID"
 CONTENT-TYPE: text/xml ; charset="utf-8"
 Content-Length: 295

 <?xml version="1.0" encoding="utf-8"?>
 <s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s=
 "http://schemas.xmlsoap.org/soap/envelope/">
 <s:Body>
 <u:GetSystemUpdateID xmlns:u="urn:schemas-upnp-org:service:ContentDirector
 y:1" />
 </s:Body>
 </s:Envelope> */

//UPNP_ACTION_TYPE_CONTENT_DIRECTORY_GET_SYSTEM_UPDATE_ID,
/*
 POST /UPnPServices/ConnectionManager/control/ HTTP/1.1
 HOST: 192.168.0.3:1117
 SOAPACTION: "urn:schemas-upnp-org:service:ConnectionManager:1#GetProtocolInfo"
 CONTENT-TYPE: text/xml ; charset="utf-8"
 Content-Length: 294

 <?xml version="1.0" encoding="utf-8"?>
 <s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
 xmlns:s="http://schemas.xmlsoap.org/soap/envelope/">
 <s:Body>
 <u:GetProtocolInfo xmlns:u="urn:schemas-upnp-org:service:ConnectionManager
 :1" />
 </s:Body>
 </s:Envelope> */
//UPNP_ACTION_TYPE_CONTENT_DIRECTORY_GET_PROTOCOL_INFO,
/*
 POST /web/msr_control HTTP/1.1
 User-Agent: Xbox/2.0.4552.0 UPnP/1.0 Xbox/2.0.4552.0
 SOAPACTION: "urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1#IsAuthorized"
 CONTENT-TYPE: text/xml; charset="utf-8"
 Content-Length: 304

 <s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"
 s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
 <s:Body>
 <u:IsAuthorized xmlns:u="urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1">
 <DeviceID></DeviceID>
 </u:IsAuthorized>
 </s:Body>
 </s:Envelope> */
//UPNP_ACTION_TYPE_X_MS_MEDIA_RECEIVER_REGISTRAR_IS_AUTHORIZED,
/*
 POST /web/msr_control HTTP/1.1
 User-Agent: Xbox/2.0.4552.0 UPnP/1.0 Xbox/2.0.4552.0
 SOAPACTION: "urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1#IsValidated"
 CONTENT-TYPE: text/xml; charset="utf-8"
 Content-Length: 302

 <s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"
 s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
 <s:Body>
 <u:IsValidated xmlns:u="urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1">
 <DeviceID></DeviceID>
 </u:IsValidated>
 </s:Body>
 </s:Envelope>
 */
/* UPNP_ACTION_TYPE_X_MS_MEDIA_RECEIVER_REGISTRAR_IS_VALIDATED


 }UPNP_ACTION_TYPE; */

