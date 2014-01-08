/***************************************************************************
 *            ContentDirectory.cpp
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

#include "ContentDirectory.h" 
#include "ContentDirectoryDescription.cpp"
#include "../UPnPActions/UPnPBrowse.h"
//#include "../SharedConfig.h"
#include "../SharedLog.h"
#include "../Common/Common.h"
#include "../Common/RegEx.h"
#include "../Dlna/DLNA.h"
//#include "VirtualContainerMgr.h"
#include "../ContentDatabase/ContentDatabase.h"

#include "../Database/ObjectManager.h"
#include "../Database/QueryBuilder.h"

//#include <iostream>
#include <sstream>
#include <libxml/xmlwriter.h>
#include <time.h>

using namespace std;
using namespace fuppes;
using namespace Dlna;

#define OBJECT_ID_FORMAT "%"PRObjectId //"%010X"
CContentDirectory::CContentDirectory() :
        UPnPService(UPNP_SERVICE_CONTENT_DIRECTORY, 1, "")
{
    m_hasSubtitles = false;
}

CContentDirectory::~CContentDirectory()
{
}

std::string CContentDirectory::getServiceDescription()
{
    return sContentDirectoryDescription;
}

/* HandleUPnPAction */
/*
 void CContentDirectory::HandleUPnPAction(CUPnPAction* pUPnPAction, CHTTPMessage* pMessageOut)
 {
 string sContent;
 upnp_error_code_t error = upnp_200_ok;

 switch((UPNP_CONTENT_DIRECTORY_ACTIONS)pUPnPAction->GetActionType())
 {
 // Browse
 case UPNP_BROWSE:
 error = HandleUPnPBrowse((CUPnPBrowse*)pUPnPAction, &sContent);
 break;
 // Search
 case UPNP_SEARCH:
 error = HandleUPnPSearch((CUPnPSearch*)pUPnPAction, &sContent);
 break;

 // GetSearchCapabilities
 case UPNP_GET_SEARCH_CAPABILITIES:
 HandleUPnPGetSearchCapabilities(pUPnPAction, &sContent);
 break;
 // GetSortCapabilities
 case UPNP_GET_SORT_CAPABILITIES:
 HandleUPnPGetSortCapabilities(pUPnPAction, &sContent);
 break;
 // GetSortExtensionsCapabilities
 case UPNP_GET_SORT_EXTENSION_CAPABILITIES:
 HandleUPnPGetSortExtensionCapabilities(pUPnPAction, &sContent);
 break;
 // GetSystemUpdateID
 case UPNP_GET_SYSTEM_UPDATE_ID:
 HandleUPnPGetSystemUpdateID(pUPnPAction, &sContent);
 break;

 // CreateObject
 case UPNP_CREATE_OBJECT:
 CreateObject(pUPnPAction, &sContent);
 break;
 // DestroyObject
 case UPNP_DESTROY_OBJECT:
 DestroyObject(pUPnPAction, &sContent);
 break;
 // UpdateObject
 case UPNP_UPDATE_OBJECT:
 UpdateObject(pUPnPAction, &sContent);
 break;

 // CreateReference
 case UPNP_CREATE_REFERENCE:
 CreateReference(pUPnPAction, &sContent);
 break;

 default:
 //ASSERT(true == false);
 error = upnp_401_invalid_action;
 break;
 }

 if(error == upnp_200_ok && !sContent.empty()) {
 pMessageOut->SetMessage(HTTP_MESSAGE_TYPE_200_OK, "text/xml; charset=\"utf-8\"");
 pMessageOut->SetContent(sContent);
 }
 // todo add error messages
 else if(error != upnp_200_ok || sContent.empty()) {
 pMessageOut->SetMessage(HTTP_MESSAGE_TYPE_500_INTERNAL_SERVER_ERROR, "text/xml; charset=\"utf-8\"");

 sContent =
 "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
 "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
 "  <s:Body>"
 "    <s:Fault>"
 "      <faultcode>s:Client</faultcode>"
 "      <faultstring>UPnPError</faultstring>"
 "      <detail>"
 "        <UPnPError xmlns=\"urn:schemas-upnp-org:control-1-0\">"
 "          <errorCode>401</errorCode>"
 "          <errorDescription>Invalid Action</errorDescription>"
 "        </UPnPError>"
 "      </detail>"
 "    </s:Fault>"
 "  </s:Body>"
 "</s:Envelope>";

 pMessageOut->SetContent(sContent);
 }
 }
 */

Fuppes::HttpResponse* CContentDirectory::handleAction(CUPnPAction* upnpAction, std::string action, Fuppes::HttpRequest* request)
{
    string content;
    upnp_error_code_t error = upnp_200_ok;

    switch ((UPNP_CONTENT_DIRECTORY_ACTIONS)upnpAction->GetActionType()) {
        // Browse
        case UPNP_BROWSE:
            error = HandleUPnPBrowse((CUPnPBrowse*)upnpAction, &content);
            break;
            // Search
        case UPNP_SEARCH:
            error = HandleUPnPSearch((CUPnPSearch*)upnpAction, &content);
            break;

            // GetSearchCapabilities
        case UPNP_GET_SEARCH_CAPABILITIES:
            HandleUPnPGetSearchCapabilities(upnpAction, &content);
            break;
            // GetSortCapabilities
        case UPNP_GET_SORT_CAPABILITIES:
            HandleUPnPGetSortCapabilities(upnpAction, &content);
            break;
            // GetSortExtensionsCapabilities
        case UPNP_GET_SORT_EXTENSION_CAPABILITIES:
            HandleUPnPGetSortExtensionCapabilities(upnpAction, &content);
            break;
            // GetSystemUpdateID
        case UPNP_GET_SYSTEM_UPDATE_ID:
            HandleUPnPGetSystemUpdateID(upnpAction, &content);
            break;

            // CreateObject
        case UPNP_CREATE_OBJECT:
            CreateObject(upnpAction, &content);
            break;
            // DestroyObject
        case UPNP_DESTROY_OBJECT:
            DestroyObject(upnpAction, &content);
            break;
            // UpdateObject
        case UPNP_UPDATE_OBJECT:
            UpdateObject(upnpAction, &content);
            break;

            // CreateReference
        case UPNP_CREATE_REFERENCE:
            CreateReference(upnpAction, &content);
            break;

        default:
            //ASSERT(true == false);
            error = upnp_401_invalid_action;
            break;
    }

    Fuppes::HttpResponse* response = NULL;

    if (error == upnp_200_ok && !content.empty()) {

    	//std::cout << "CD: " << content << std::endl;

        response = new Fuppes::HttpResponse(Http::OK, "text/xml; charset=\"utf-8\"", content);
        //response->getHeader()->setValue("Content-Type", "text/xml; charset=\"utf-8\"");

    }
    // todo add error messages
    else if (error != upnp_200_ok || content.empty()) {

        content = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                "  <s:Body>"
                "    <s:Fault>"
                "      <faultcode>s:Client</faultcode>"
                "      <faultstring>UPnPError</faultstring>"
                "      <detail>"
                "        <UPnPError xmlns=\"urn:schemas-upnp-org:control-1-0\">"
                "          <errorCode>401</errorCode>"
                "          <errorDescription>Invalid Action</errorDescription>"
                "        </UPnPError>"
                "      </detail>"
                "    </s:Fault>"
                "  </s:Body>"
                "</s:Envelope>";

        response = new Fuppes::HttpResponse(Http::INTERNAL_SERVER_ERROR, "text/xml; charset=\"utf-8\"", content);
        //response->getHeader()->setValue("Content-Type", "text/xml; charset=\"utf-8\"");
    }

    return response;
}

/* HandleUPnPBrowse */
upnp_error_code_t CContentDirectory::HandleUPnPBrowse(CUPnPBrowse* pUPnPBrowse, std::string* p_psResult)
{
    upnp_error_code_t error = upnp_401_invalid_action;

    xmlTextWriterPtr writer;
    xmlBufferPtr buf;

    buf = xmlBufferCreate();
    writer = xmlNewTextWriterMemory(buf, 0);
    xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);

    // root
    xmlTextWriterStartElementNS(writer, BAD_CAST "s", BAD_CAST "Envelope", NULL);
    xmlTextWriterWriteAttributeNS(writer, BAD_CAST "s", BAD_CAST "encodingStyle", BAD_CAST "http://schemas.xmlsoap.org/soap/envelope/", BAD_CAST "http://schemas.xmlsoap.org/soap/encoding/");

    // body
    xmlTextWriterStartElementNS(writer, BAD_CAST "s", BAD_CAST "Body", NULL);

    // browse response
    xmlTextWriterStartElementNS(writer, BAD_CAST "u", BAD_CAST "BrowseResponse", BAD_CAST "urn:schemas-upnp-org:service:ContentDirectory:1");

    // result
    xmlTextWriterStartElement(writer, BAD_CAST "Result");

    // build result xml
    xmlTextWriterPtr resWriter;
    xmlBufferPtr resBuf;
    resBuf = xmlBufferCreate();
    resWriter = xmlNewTextWriterMemory(resBuf, 0);
    xmlTextWriterStartDocument(resWriter, NULL, "UTF-8", NULL);

    xmlTextWriterStartElementNS(resWriter, NULL, BAD_CAST "DIDL-Lite", BAD_CAST "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");
    xmlTextWriterWriteAttribute(resWriter, BAD_CAST "xmlns:dc", BAD_CAST "http://purl.org/dc/elements/1.1/");
    xmlTextWriterWriteAttribute(resWriter, BAD_CAST "xmlns:upnp", BAD_CAST "urn:schemas-upnp-org:metadata-1-0/upnp/");
    xmlTextWriterWriteAttribute(resWriter, BAD_CAST "xmlns:sec", BAD_CAST "http://www.sec.co.kr/");
    xmlTextWriterWriteAttribute(resWriter, BAD_CAST "xmlns:dlna", BAD_CAST "urn:schemas-dlna-org:metadata-1-0/");

    unsigned int nNumberReturned = 0;
    unsigned int nTotalMatches = 0;

    bool valid = true;
    switch (pUPnPBrowse->browseFlag()) {

        case UPNP_BROWSE_FLAG_METADATA:
            error = BrowseMetadata(resWriter, &nTotalMatches, &nNumberReturned, pUPnPBrowse);
            break;
        case UPNP_BROWSE_FLAG_DIRECT_CHILDREN:
            error = BrowseDirectChildren(resWriter, &nTotalMatches, &nNumberReturned, pUPnPBrowse);
            break;
        default:
            valid = false;
            break;
    }

    if (m_hasSubtitles) {
        //xmlTextWriterWriteAttribute(resWriter, BAD_CAST "xmlns:sec", BAD_CAST "http://www.sec.co.kr/");
    }

    // finalize result xml
    xmlTextWriterEndElement(resWriter);
    xmlTextWriterEndDocument(resWriter);
    xmlFreeTextWriter(resWriter);

    std::string sResOutput;
    sResOutput = (const char*)resBuf->content;

    xmlBufferFree(resBuf);
    sResOutput = sResOutput.substr(strlen("<?xml version=\"1.0\" encoding=\"UTF-8\"?> "));
    xmlTextWriterWriteString(writer, BAD_CAST sResOutput.c_str());

    // end result
    xmlTextWriterEndElement(writer);

    // number returned
    xmlTextWriterStartElement(writer, BAD_CAST "NumberReturned");
    xmlTextWriterWriteFormatString(writer, "%u", nNumberReturned);
    xmlTextWriterEndElement(writer);

    // total matches
    xmlTextWriterStartElement(writer, BAD_CAST "TotalMatches");
    xmlTextWriterWriteFormatString(writer, "%u", nTotalMatches);
    xmlTextWriterEndElement(writer);

    // update id
    xmlTextWriterStartElement(writer, BAD_CAST "UpdateID");
    xmlTextWriterWriteFormatString(writer, "%u", ContentDatabase::ContentDatabase::getSystemUpdateId());
    xmlTextWriterEndElement(writer);

    // end browse response
    xmlTextWriterEndElement(writer);

    // end body
    xmlTextWriterEndElement(writer);

    // end root
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);

    //std::stringstream output;
    //string sResult
    if (valid)
        *p_psResult = (const char*)buf->content;
    //output << (const char*)buf->content;
    xmlBufferFree(buf);

    return error;
//cout << *p_psResult << endl;

    /**p_psResult = sResult;
     CSharedLog::Shared()->Log(L_DEBUG, sResult, __FILE__, __LINE__);*/
}

upnp_error_code_t CContentDirectory::BrowseMetadata(xmlTextWriterPtr pWriter, unsigned int* p_pnTotalMatches, unsigned int* p_pnNumberReturned, CUPnPBrowse* pUPnPBrowse)
{

    //cout << "BrowseMetadata VIRTUAL LAYOUT: " << pUPnPBrowse->virtualFolderLayout() << ":" << endl;

    // total matches and
    // number returned are always 1
    // on metadata browse
    *p_pnTotalMatches = 1;
    *p_pnNumberReturned = 1;

    Database::ObjectManager objMgr;
    Database::Item item;

    // get container type
    OBJECT_TYPE nContainerType = CONTAINER_STORAGEFOLDER;
    if (pUPnPBrowse->getObjectId() > 0) {

        if (false == objMgr.findOne(pUPnPBrowse->getObjectId(), pUPnPBrowse->virtualFolderLayout(), item)) {
            return upnp_701_no_such_object;
        }

        nContainerType = item.type;
    }

    // get child count
    string sChildCount = "0";
    if (nContainerType < CONTAINER_MAX) {

        fuppes_int64_t childCount = objMgr.getChildCount(pUPnPBrowse->getObjectId(), pUPnPBrowse->virtualFolderLayout(), pUPnPBrowse->DeviceSettings()->getSupportedObjectTypes(), pUPnPBrowse->DeviceSettings()->getSupportedFileExtensions());
        if (-1 == childCount) {
            return upnp_720_cannot_process_the_request;
        }
        stringstream tmp;
        tmp << childCount;
        sChildCount = tmp.str();

        /*
         sql = qry.build(SQL_COUNT_CHILD_OBJECTS, pUPnPBrowse->GetObjectIDAsUInt(), pUPnPBrowse->virtualFolderLayout(), pUPnPBrowse->DeviceSettings()->getSupportedObjectTypes());
         qry.select(sql);
         sChildCount = qry.result()->asString("COUNT");
         */
    }

    string sParentId;

    // root folder
    if (pUPnPBrowse->getObjectId() == 0) {
        sParentId = "-1";

        xmlTextWriterStartElement(pWriter, BAD_CAST "container");

        xmlTextWriterWriteAttribute(pWriter, BAD_CAST "id", BAD_CAST pUPnPBrowse->objectId().c_str());
        xmlTextWriterWriteAttribute(pWriter, BAD_CAST "searchable", BAD_CAST "false");
        xmlTextWriterWriteAttribute(pWriter, BAD_CAST "parentID", BAD_CAST sParentId.c_str());
        xmlTextWriterWriteAttribute(pWriter, BAD_CAST "restricted", BAD_CAST "true");
        xmlTextWriterWriteAttribute(pWriter, BAD_CAST "childCount", BAD_CAST sChildCount.c_str());

        xmlTextWriterStartElement(pWriter, BAD_CAST "dc:title");
        xmlTextWriterWriteString(pWriter, BAD_CAST "root");
        xmlTextWriterEndElement(pWriter);

        xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:class");
        xmlTextWriterWriteString(pWriter, BAD_CAST "object.container");
        xmlTextWriterEndElement(pWriter);

        xmlTextWriterEndElement(pWriter);
    }

    // sub folders
    else {
        BuildDescription(pWriter, item, pUPnPBrowse);
    }

    return upnp_200_ok;
}

upnp_error_code_t CContentDirectory::BrowseDirectChildren(xmlTextWriterPtr pWriter, unsigned int* p_pnTotalMatches, unsigned int* p_pnNumberReturned, CUPnPBrowse* pUPnPBrowse)
{

    //cout << "BrowseDirectChildren VIRTUAL LAYOUT: " << pUPnPBrowse->virtualFolderLayout() << ":" << endl;

    // get total matches
    Database::ObjectManager objMgr;
    fuppes_int64_t childCount = objMgr.getChildCount(pUPnPBrowse->getObjectId(), pUPnPBrowse->virtualFolderLayout(), pUPnPBrowse->DeviceSettings()->getSupportedObjectTypes(), pUPnPBrowse->DeviceSettings()->getSupportedFileExtensions());
    if (-1 == childCount) {
        cout << "ERROR COUNT CHILDREN" << endl;
        return upnp_720_cannot_process_the_request;
    }

    //std::cout << "CContentDirectory:: child count: " << childCount << std::endl;

    *p_pnTotalMatches = childCount; //result.getRow(0)->getColumnInt64("COUNT");

    // container is empty
    if (0 == childCount) {
        p_pnNumberReturned = 0;
        return upnp_200_ok;
    }

    // starting index >= count
    if (pUPnPBrowse->m_nStartingIndex >= childCount) {
        p_pnNumberReturned = 0;
        return upnp_200_ok;
    }

    // get the child objects
    unsigned int tmpInt = *p_pnNumberReturned;

    Database::Item child;
    if (0 < *p_pnTotalMatches
            && false
                    == objMgr.findChildren(pUPnPBrowse->getObjectId(), pUPnPBrowse->virtualFolderLayout(), pUPnPBrowse->DeviceSettings()->getSupportedObjectTypes(), pUPnPBrowse->DeviceSettings()->getSupportedFileExtensions(), pUPnPBrowse->m_sortCriteriaSQL,
                            pUPnPBrowse->m_nStartingIndex, pUPnPBrowse->m_nRequestedCount, child)) {
        cout << "ERROR FIND CHILDREN : " << *p_pnTotalMatches << " - " << pUPnPBrowse->m_nStartingIndex << " - " << pUPnPBrowse->m_nRequestedCount << endl;
        return upnp_720_cannot_process_the_request;
    }

    do {
        BuildDescription(pWriter, child, pUPnPBrowse);
        child.clear();
        tmpInt++;
    } while (objMgr.nextChild(child));

    *p_pnNumberReturned = tmpInt;
    return upnp_200_ok;
}

void CContentDirectory::BuildDescription(xmlTextWriterPtr pWriter, const Database::Item& item, CUPnPBrowseSearchBase* pUPnPBrowse)
{
    // container
    if (CONTAINER_MAX > item.type) {

        if ((CONTAINER_PLAYLISTCONTAINER == item.type) && pUPnPBrowse->DeviceSettings()->playlistStyle() != CDeviceSettings::container) {
            BuildItemDescription(pWriter, item, pUPnPBrowse, item.type, item.getParentIdAsHex());
        }
        else {
            BuildContainerDescription(pWriter, item, pUPnPBrowse);
        }

    }
    // item
    else if (ITEM <= item.type) {
        BuildItemDescription(pWriter, item, pUPnPBrowse, item.type, item.getParentIdAsHex());
    }
}

void CContentDirectory::BuildContainerDescription(xmlTextWriterPtr pWriter, const Database::Item& item, CUPnPBrowseSearchBase* pUPnPBrowse)
{

    //CContentDatabase* pDb = new CContentDatabase();
    //stringstream sSql;
    //SQLQuery qry;

    //string sDevice = pUPnPBrowse->virtualFolderLayout();

    //cout << "BuildContainerDescription DEVICE: " << sDevice << "*" << endl;

    /*sSql = string("select count(*) as COUNT from MAP_OBJECTS ") +
     "where PARENT_ID = " + pSQLResult->asString("OBJECT_ID") + " and " + sDevice;*/

    /*sSql <<
     "select count(*) as COUNT " <<
     "from OBJECTS o, MAP_OBJECTS m " <<
     "where " <<
     "m.PARENT_ID = " << pSQLResult->asString("OBJECT_ID") << " and " <<
     "o.OBJECT_ID = m.OBJECT_ID and " <<
     "o.HIDDEN = 0 and " <<
     "m." << sDevice << " and o." << sDevice;*/

    //string sql = qry.build(SQL_COUNT_CHILD_OBJECTS, pSQLResult->asString("OBJECT_ID"), sDevice, objectTypes);
    //qry.select(sql);
    //if(!qry.eof())
    //    sChildCount = qry.result()->asString("COUNT");
    // get child count
    //string objectTypes = pUPnPBrowse->DeviceSettings()->getSupportedObjectTypes();
    Database::ObjectManager objMgr;
    fuppes_int64_t childCount = objMgr.getChildCount(item.objectId, pUPnPBrowse->virtualFolderLayout(), pUPnPBrowse->DeviceSettings()->getSupportedObjectTypes(), pUPnPBrowse->DeviceSettings()->getSupportedFileExtensions());

    stringstream tmp;
    tmp << childCount;
    string sChildCount = tmp.str();

    // container
    xmlTextWriterStartElement(pWriter, BAD_CAST "container");

    xmlTextWriterWriteAttribute(pWriter, BAD_CAST "id", BAD_CAST item.getObjectIdAsHex().c_str());
    xmlTextWriterWriteAttribute(pWriter, BAD_CAST "searchable", BAD_CAST "true");
    xmlTextWriterWriteAttribute(pWriter, BAD_CAST "parentID", BAD_CAST item.getParentIdAsHex().c_str());
    xmlTextWriterWriteAttribute(pWriter, BAD_CAST "restricted", BAD_CAST "true");
    xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST "childCount", "%"PRIi64, childCount);

    // title
    string sTitle = item.title;
    int nLen = pUPnPBrowse->DeviceSettings()->DisplaySettings()->nMaxFileNameLength;
    if (nLen > 0 && pUPnPBrowse->DeviceSettings()->DisplaySettings()->bShowChildCountInTitle) {
        nLen -= (sChildCount.length() + 3); // "_(n)"
    }
    sTitle = TrimFileName(sTitle, nLen);
    if (pUPnPBrowse->DeviceSettings()->DisplaySettings()->bShowChildCountInTitle) {
        sTitle = sTitle + " (" + sChildCount + ")";
    }

    //xmlTextWriterStartElementNS(pWriter, BAD_CAST "dc", BAD_CAST "title", BAD_CAST "http://purl.org/dc/elements/1.1/");     
    xmlTextWriterStartElement(pWriter, BAD_CAST "dc:title");
    xmlTextWriterWriteString(pWriter, BAD_CAST sTitle.c_str());
    xmlTextWriterEndElement(pWriter);

    xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:class");
    xmlTextWriterWriteString(pWriter, BAD_CAST UPnP::typeToString(item.type).c_str());
    xmlTextWriterEndElement(pWriter);

    // writeStatus (optional)
    /*if(pUPnPBrowse->IncludeProperty("upnp:writeStatus")) {    
     xmlTextWriterStartElementNS(pWriter, BAD_CAST "upnp", BAD_CAST "writeStatus", BAD_CAST "urn:schemas-upnp-org:metadata-1-0/upnp/");
     xmlTextWriterWriteString(pWriter, BAD_CAST "UNKNOWN");
     xmlTextWriterEndElement(pWriter);
     }*/

    if (CONTAINER_ALBUM_MUSICALBUM == item.type) {

        if (pUPnPBrowse->IncludeProperty("upnp:artist") && !item.details->av_artist.empty()) {
            xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:artist");
            xmlTextWriterWriteString(pWriter, BAD_CAST item.details->av_artist.c_str());
            xmlTextWriterEndElement(pWriter);
        }

        if (pUPnPBrowse->IncludeProperty("upnp:genre") && !item.details->av_genre.empty()) {
            xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:genre");
            xmlTextWriterWriteString(pWriter, BAD_CAST item.details->av_genre.c_str());
            xmlTextWriterEndElement(pWriter);
        }

    } //  type == CONTAINER_ALBUM_MUSIC_ALBUM

    writeAlbumArtUrl(pWriter, pUPnPBrowse, item);

    /*
     if(p_nContainerType == CONTAINER_PLAYLIST_CONTAINER) {
     // res
     xmlTextWriterStartElement(pWriter, BAD_CAST "res");

     string sTmp;
     string ext = ExtractFileExt(pSQLResult->asString("FILE_NAME"));

     //sTmp = "http-get:*:" + pSQLResult->asString("MIME_TYPE") + ":*";
     #warning todo MIME TYPE
     sTmp = "http-get:*:todo:*";
     xmlTextWriterWriteAttribute(pWriter, BAD_CAST "protocolInfo", BAD_CAST sTmp.c_str());


     sTmp = "http://" + m_sHTTPServerURL + "/MediaServer/Playlists/" + szObjId +
     "." + ext;
     //xmlTextWriterWriteAttribute(pWriter, BAD_CAST "importUri", BAD_CAST sTmp.str().c_str());

     xmlTextWriterWriteString(pWriter, BAD_CAST sTmp.c_str());
     xmlTextWriterEndElement(pWriter);
     }
     */

    // end container
    xmlTextWriterEndElement(pWriter);
}

void CContentDirectory::BuildItemDescription(xmlTextWriterPtr pWriter, const Database::Item& item, CUPnPBrowseSearchBase* pUPnPBrowse, OBJECT_TYPE p_nObjectType, std::string p_sParentId)
{
    // item
    xmlTextWriterStartElement(pWriter, BAD_CAST "item");

    xmlTextWriterWriteAttribute(pWriter, BAD_CAST "id", BAD_CAST item.getObjectIdAsHex().c_str());
    xmlTextWriterWriteAttribute(pWriter, BAD_CAST "parentID", BAD_CAST item.getParentIdAsHex().c_str());
    xmlTextWriterWriteAttribute(pWriter, BAD_CAST "restricted", BAD_CAST "true");
    if (0 != item.refId) {
        xmlTextWriterWriteAttribute(pWriter, BAD_CAST "refID", BAD_CAST item.getRefIdAsHex().c_str());
    }

    // date
    if (pUPnPBrowse->IncludeProperty("dc:date") && !item.details->date.empty()) {
        xmlTextWriterStartElementNS(pWriter, BAD_CAST "dc", BAD_CAST "date", BAD_CAST "http://purl.org/dc/elements/1.1/");
        xmlTextWriterWriteString(pWriter, BAD_CAST item.details->date.c_str());
        xmlTextWriterEndElement(pWriter);
    }

    // writeStatus (optional)
    /*if(pUPnPBrowse->IncludeProperty("upnp:writeStatus")) {
     xmlTextWriterStartElementNS(pWriter, BAD_CAST "upnp", BAD_CAST "writeStatus", BAD_CAST "urn:schemas-upnp-org:metadata-1-0/upnp/");
     xmlTextWriterWriteString(pWriter, BAD_CAST "UNKNOWN");
     xmlTextWriterEndElement(pWriter);
     }*/

    switch (p_nObjectType) {
        case ITEM_AUDIO_ITEM:
            case ITEM_AUDIO_ITEM_MUSIC_TRACK:
            BuildAudioItemDescription(pWriter, item, pUPnPBrowse, item.getObjectIdAsHex());
            break;
        case ITEM_AUDIO_ITEM_AUDIO_BROADCAST:
            BuildAudioBroadcastItemDescription(pWriter, item, pUPnPBrowse, item.getObjectIdAsHex());
            break;
        case ITEM_VIDEO_ITEM_VIDEO_BROADCAST:
            BuildVideoBroadcastItemDescription(pWriter, item, pUPnPBrowse, item.getObjectIdAsHex());
            break;
        case ITEM_IMAGE_ITEM:
            case ITEM_IMAGE_ITEM_PHOTO:
            BuildImageItemDescription(pWriter, item, pUPnPBrowse, item.getObjectIdAsHex());
            break;
        case ITEM_VIDEO_ITEM:
            case ITEM_VIDEO_ITEM_MOVIE:
            BuildVideoItemDescription(pWriter, item, pUPnPBrowse, item.getObjectIdAsHex());
            break;
        case CONTAINER_PLAYLISTCONTAINER:
            BuildPlaylistItemDescription(pWriter, item, pUPnPBrowse, item.getObjectIdAsHex());
            break;

        default:
            break;
    }

    /* end item */
    xmlTextWriterEndElement(pWriter);
}

void CContentDirectory::BuildAudioItemDescription(xmlTextWriterPtr pWriter, const Database::Item& item, CUPnPBrowseSearchBase* pUPnPBrowse, std::string p_sObjectID)
{
    string sExt = item.extension; // ExtractFileExt(pSQLResult->asString("FILE_NAME"));

    // title
    xmlTextWriterStartElement(pWriter, BAD_CAST "dc:title");

    string sFileName;
    if (!item.title.empty()) {
        sFileName = item.title;

        // trim filename
        if (pUPnPBrowse->DeviceSettings()->DisplaySettings()->nMaxFileNameLength > 0) {
            sFileName = TrimFileName(sFileName, pUPnPBrowse->DeviceSettings()->DisplaySettings()->nMaxFileNameLength);
        }
    }

    xmlTextWriterWriteString(pWriter, BAD_CAST sFileName.c_str());
    xmlTextWriterEndElement(pWriter);

    // class
    xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:class");
    xmlTextWriterWriteString(pWriter, BAD_CAST pUPnPBrowse->DeviceSettings()->ObjectTypeAsStr(sExt).c_str());
    xmlTextWriterEndElement(pWriter);

    if (pUPnPBrowse->IncludeProperty("upnp:artist") && !item.details->av_artist.empty()) {
        xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:artist");
        xmlTextWriterWriteString(pWriter, BAD_CAST item.details->av_artist.c_str());
        xmlTextWriterEndElement(pWriter);
    }

    if (pUPnPBrowse->IncludeProperty("upnp:album") && !item.details->av_album.empty()) {
        xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:album");
        xmlTextWriterWriteString(pWriter, BAD_CAST item.details->av_album.c_str());
        xmlTextWriterEndElement(pWriter);
    }

    if (pUPnPBrowse->IncludeProperty("upnp:genre") && !item.details->av_genre.empty()) {
        xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:genre");
        xmlTextWriterWriteString(pWriter, BAD_CAST item.details->av_genre.c_str());
        xmlTextWriterEndElement(pWriter);
    }

    if (pUPnPBrowse->IncludeProperty("upnp:originalTrackNumber") && 0 < item.details->a_trackNumber) {
        xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:originalTrackNumber");
        xmlTextWriterWriteFormatString(pWriter, "%i", item.details->a_trackNumber);
        xmlTextWriterEndElement(pWriter);
    }

    // albumArt
    writeAlbumArtUrl(pWriter, pUPnPBrowse, item);

    // res
    xmlTextWriterStartElement(pWriter, BAD_CAST "res");

    bool bTranscode = pUPnPBrowse->DeviceSettings()->DoTranscode(sExt);
    string sMimeType = pUPnPBrowse->DeviceSettings()->MimeType(sExt);
    string targetExt = pUPnPBrowse->DeviceSettings()->Extension(sExt);

    // res@protocolInfo
    string profile;
    int channels = 0;
    int bitrate = 0;
    if (pUPnPBrowse->DeviceSettings()->dlnaVersion() != CMediaServerSettings::dlna_none) {
        if (!bTranscode) {
            channels = item.details->a_channels; // pSQLResult->asInt("A_CHANNELS");
            bitrate = item.details->a_bitrate; // pSQLResult->asInt("A_BITRATE");
        }

        DLNA::getAudioProfile(targetExt, channels, bitrate, profile, sMimeType);
    }

    string sTmp = BuildProtocolInfo(bTranscode, sMimeType, profile, pUPnPBrowse);
    xmlTextWriterWriteAttribute(pWriter, BAD_CAST "protocolInfo", BAD_CAST sTmp.c_str());

    // res@duration
    if (pUPnPBrowse->IncludeProperty("res@duration") && 0 < item.details->av_duration) {
        string dur = FormatHelper::msToUpnpDuration(item.details->av_duration);
        xmlTextWriterWriteAttribute(pWriter, BAD_CAST "duration", BAD_CAST dur.c_str());
    }

    // res@nrAudioChannels
    if (pUPnPBrowse->IncludeProperty("res@nrAudioChannels") && 0 < channels) {
        xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST "nrAudioChannels", "%i", channels);
    }

    // res@sampleFrequency (Hz)
    if (pUPnPBrowse->IncludeProperty("res@sampleFrequency")) {
        if (!bTranscode && 0 != item.details->a_samplerate) {
            xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST "sampleFrequency", "%i", item.details->a_samplerate);
        }
        else if (bTranscode && pUPnPBrowse->DeviceSettings()->TargetAudioSampleRate(sExt) > 0) {
            xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST "sampleFrequency", "%i", pUPnPBrowse->DeviceSettings()->TargetAudioSampleRate(sExt));
        }
    }

    // res@bitrate (bytes! per second)
    if (pUPnPBrowse->IncludeProperty("res@bitrate")) {
        if (!bTranscode && 0 != item.details->a_bitrate) {
            xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST "bitrate", "%i", (item.details->a_bitrate / 8));
        }
        else if (bTranscode && pUPnPBrowse->DeviceSettings()->TargetAudioBitRate(sExt) > 0) {
            xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST "bitrate", "%i", (pUPnPBrowse->DeviceSettings()->TargetAudioBitRate(sExt) / 8));
        }
    }

    // res@bitsPerSample
    if (!bTranscode && pUPnPBrowse->IncludeProperty("res@bitsPerSample") && 0 != item.details->a_bitsPerSample) {
        xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST "bitsPerSample", "%i", item.details->a_bitsPerSample);
    }

    // res@size
    if (!bTranscode && pUPnPBrowse->IncludeProperty("res@size") && 0 != item.size) {
        xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST "size", "%"PROff_t, item.size);
    }

    sExt = pUPnPBrowse->DeviceSettings()->Extension(sExt);

    sTmp = "http://" + m_sHTTPServerURL + "/audio/" + buildObjectAlias(p_sObjectID, item, pUPnPBrowse->DeviceSettings()) + "." + sExt;
    if (pUPnPBrowse->virtualFoldersEnabled() && pUPnPBrowse->virtualFolderLayout().empty()) {
        sTmp += "?vfolder=none";
    }

    xmlTextWriterWriteString(pWriter, BAD_CAST sTmp.c_str());
    xmlTextWriterEndElement(pWriter);
}

void CContentDirectory::BuildAudioBroadcastItemDescription(xmlTextWriterPtr pWriter, const Database::Item& item, CUPnPBrowseSearchBase* pUPnPBrowse, std::string /*p_sObjectID*/)
{
    // title
    xmlTextWriterStartElement(pWriter, BAD_CAST "dc:title");
    // trim filename
    string title = TrimFileName(item.title, pUPnPBrowse->DeviceSettings()->DisplaySettings()->nMaxFileNameLength);
    //sFileName = TruncateFileExt(title);
    xmlTextWriterWriteString(pWriter, BAD_CAST title.c_str());
    xmlTextWriterEndElement(pWriter);

    // class
    xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:class");
    xmlTextWriterWriteString(pWriter, BAD_CAST "object.item.audioItem.audioBroadcast");
    xmlTextWriterEndElement(pWriter);

    // genre (item.audioItem)
    if (pUPnPBrowse->IncludeProperty("upnp:genre") && !item.details->av_genre.empty()) {
        xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:genre");
        xmlTextWriterWriteString(pWriter, BAD_CAST item.details->av_genre.c_str());
        xmlTextWriterEndElement(pWriter);
    }

    // dc:description
    if (pUPnPBrowse->IncludeProperty("dc:description") && !item.details->description.empty()) {
        xmlTextWriterStartElement(pWriter, BAD_CAST "dc:description");
        xmlTextWriterWriteString(pWriter, BAD_CAST item.details->description.c_str());
        xmlTextWriterEndElement(pWriter);
    }

    // album art
    writeAlbumArtUrl(pWriter, pUPnPBrowse, item);

    // res
    xmlTextWriterStartElement(pWriter, BAD_CAST "res");

    string protocolInfo = "http-get:*:audio/mpeg:*";
    xmlTextWriterWriteAttribute(pWriter, BAD_CAST "protocolInfo", BAD_CAST protocolInfo.c_str());

    xmlTextWriterWriteString(pWriter, BAD_CAST item.path.c_str());
    xmlTextWriterEndElement(pWriter);
}

void CContentDirectory::BuildImageItemDescription(xmlTextWriterPtr pWriter, const Database::Item& item, CUPnPBrowseSearchBase* pUPnPBrowse, std::string p_sObjectID)
{
    string sExt = item.extension; //ExtractFileExt(pSQLResult->asString("FILE_NAME"));
    bool bTranscode = pUPnPBrowse->DeviceSettings()->DoTranscode(sExt);

    // title
    xmlTextWriterStartElement(pWriter, BAD_CAST "dc:title");
    // trim filename
    string title = item.title; //pSQLResult->asString("TITLE");
    if (pUPnPBrowse->DeviceSettings()->DisplaySettings()->nMaxFileNameLength > 0) {
        title = TrimFileName(title, pUPnPBrowse->DeviceSettings()->DisplaySettings()->nMaxFileNameLength);
    }
    //sFileName = TruncateFileExt(sFileName);
    xmlTextWriterWriteString(pWriter, BAD_CAST title.c_str());
    xmlTextWriterEndElement(pWriter);

    // class
    xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:class");
    xmlTextWriterWriteString(pWriter, BAD_CAST pUPnPBrowse->DeviceSettings()->ObjectTypeAsStr(sExt).c_str());
    xmlTextWriterEndElement(pWriter);

    /* storageMedium */
    /*if(pUPnPBrowse->IncludeProperty("upnp:storageMedium")) {
     xmlTextWriterStartElementNS(pWriter, BAD_CAST "upnp", BAD_CAST "storageMedium", BAD_CAST "urn:schemas-upnp-org:metadata-1-0/upnp/");
     xmlTextWriterWriteString(pWriter, BAD_CAST "UNKNOWN");
     xmlTextWriterEndElement(pWriter);
     }*/

    /* longDescription upnp No
     rating upnp No
     description dc No
     publisher dc No
     date dc No
     rights dc No */

    // date
    if (pUPnPBrowse->IncludeProperty("dc:date")) {
        if (!item.details->date.empty()) {
            xmlTextWriterStartElementNS(pWriter, BAD_CAST "dc", BAD_CAST "date", BAD_CAST "http://purl.org/dc/elements/1.1/");
            xmlTextWriterWriteString(pWriter, BAD_CAST item.details->date.c_str());
            xmlTextWriterEndElement(pWriter);
        }
    }

    // res
    xmlTextWriterStartElement(pWriter, BAD_CAST "res");

    // res@protocolInfo
    string profile;
    string sMimeType = pUPnPBrowse->DeviceSettings()->MimeType(sExt);
    string targetExt = pUPnPBrowse->DeviceSettings()->Extension(sExt);

    if (pUPnPBrowse->DeviceSettings()->dlnaVersion() != CMediaServerSettings::dlna_none) {
        DLNA::getImageProfile(targetExt, item.details->iv_width, item.details->iv_height, profile, sMimeType);
    }

    string sTmp = BuildProtocolInfo(bTranscode, sMimeType, profile, pUPnPBrowse);
    xmlTextWriterWriteAttribute(pWriter, BAD_CAST "protocolInfo", BAD_CAST sTmp.c_str());

    // res@resolution
    if (pUPnPBrowse->IncludeProperty("res@resolution")) {
        // todo rescaling

        if (0 != item.details->iv_width && 0 != item.details->iv_height) {
            //sTmp = pSQLResult->asString("IV_WIDTH") + "x" + pSQLResult->asString("IV_HEIGHT");
            xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST "resolution", "%ix%i", item.details->iv_width, item.details->iv_height);
        }
        else if (pUPnPBrowse->DeviceSettings()->ShowEmptyResolution()) {
            xmlTextWriterWriteAttribute(pWriter, BAD_CAST "resolution", BAD_CAST "0x0");
        }
    }

    // res@size
    if (!bTranscode && pUPnPBrowse->IncludeProperty("res@size") && 0 < item.size) {
        xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST "size", "%"PROff_t, item.size);
    }

    sExt = pUPnPBrowse->DeviceSettings()->Extension(sExt);

    sTmp = "http://" + m_sHTTPServerURL + "/image/" + buildObjectAlias(p_sObjectID, item, pUPnPBrowse->DeviceSettings()) + "." + sExt;
    if (pUPnPBrowse->virtualFoldersEnabled() && pUPnPBrowse->virtualFolderLayout().empty()) {
        sTmp += "?vfolder=none";
    }

    xmlTextWriterWriteString(pWriter, BAD_CAST sTmp.c_str());
    xmlTextWriterEndElement(pWriter);

}

void CContentDirectory::BuildVideoItemDescription(xmlTextWriterPtr pWriter, const Database::Item& item, CUPnPBrowseSearchBase* pUPnPBrowse, std::string p_sObjectID)
{
    string sExt = item.extension; //ExtractFileExt(pSQLResult->asString("FILE_NAME"));

    bool bTranscode = pUPnPBrowse->DeviceSettings()->DoTranscode(sExt, item.details->audioCodec, item.details->videoCodec);

    // title
    xmlTextWriterStartElement(pWriter, BAD_CAST "dc:title");
    // trim filename
    string title = item.title;
    if (pUPnPBrowse->DeviceSettings()->DisplaySettings()->nMaxFileNameLength > 0) {
        title = TrimFileName(title, pUPnPBrowse->DeviceSettings()->DisplaySettings()->nMaxFileNameLength);
    }

    xmlTextWriterWriteString(pWriter, BAD_CAST title.c_str());
    xmlTextWriterEndElement(pWriter);

    // class
    xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:class");
    xmlTextWriterWriteString(pWriter, BAD_CAST pUPnPBrowse->DeviceSettings()->ObjectTypeAsStr(sExt).c_str());
    xmlTextWriterEndElement(pWriter);

    // albumArt
    writeAlbumArtUrl(pWriter, pUPnPBrowse, item);

    // subtitle
    if (item.details->v_hasSubtitles) {
        //<sec:CaptionInfoEx sec:type="srt">http://subtile.url.srt</sec:CaptionInfoEx>
        xmlTextWriterStartElement(pWriter, BAD_CAST "sec:CaptionInfoEx");
        xmlTextWriterWriteAttribute(pWriter, BAD_CAST "sec:type", BAD_CAST "srt");
        string sub = "http://" + m_sHTTPServerURL + "/video/" + p_sObjectID + ".srt";
        xmlTextWriterWriteString(pWriter, BAD_CAST sub.c_str());
        xmlTextWriterEndElement(pWriter);
        m_hasSubtitles = true;
    }

    /*
     if(pSQLResult->asUInt("ALBUM_ART_ID") > 0 || CPluginMgr::hasMetadataPlugin("ffmpegthumbnailer")) {
     xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:albumArtURI");
     xmlTextWriterWriteAttribute(pWriter, BAD_CAST "xmlns:dlna", BAD_CAST "urn:schemas-dlna-org:metadata-1-0/");
     xmlTextWriterWriteAttribute(pWriter, BAD_CAST "dlna:profileID", BAD_CAST "JPEG_SM");

     string url = "http://" + m_sHTTPServerURL + "/ImageItems/";
     if(pSQLResult->asUInt("ALBUM_ART_ID") > 0) {
     char szArtId[11];
     sprintf(szArtId, OBJECT_ID_FORMAT, pSQLResult->asUInt("ALBUM_ART_ID"));
     url += szArtId;
     }
     else {
     url += p_sObjectID;
     }
     url += ".jpg?vfolder=none";

     xmlTextWriterWriteString(pWriter, BAD_CAST url.c_str());
     xmlTextWriterEndElement(pWriter);
     }*/

    // res
    xmlTextWriterStartElement(pWriter, BAD_CAST "res");

    string sMimeType = pUPnPBrowse->DeviceSettings()->MimeType(sExt, item.details->audioCodec, item.details->videoCodec);
    string targetExt = pUPnPBrowse->DeviceSettings()->Extension(sExt, item.details->audioCodec, item.details->videoCodec);

    // res@protocolInfo
    string profile;
    if (pUPnPBrowse->DeviceSettings()->dlnaVersion() != CMediaServerSettings::dlna_none) {
        DLNA::getVideoProfile(targetExt, item.details->audioCodec, item.details->videoCodec, profile, sMimeType);
    }

    string sTmp = BuildProtocolInfo(bTranscode, sMimeType, profile, pUPnPBrowse);
    xmlTextWriterWriteAttribute(pWriter, BAD_CAST "protocolInfo", BAD_CAST sTmp.c_str());

    // res@duration
    if (pUPnPBrowse->IncludeProperty("res@duration") && 0 < item.details->av_duration) {
        string dur = FormatHelper::msToUpnpDuration(item.details->av_duration);
        xmlTextWriterWriteAttribute(pWriter, BAD_CAST "duration", BAD_CAST dur.c_str());
    }

    // res@resolution
    if (pUPnPBrowse->IncludeProperty("res@resolution") && 0 != item.details->iv_width && 0 != item.details->iv_height) {
        if (0 != item.details->iv_width && 0 != item.details->iv_height) {
            xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST "resolution", "%ix%i", item.details->iv_width, item.details->iv_height);
        }
        else if (pUPnPBrowse->DeviceSettings()->ShowEmptyResolution()) {
            xmlTextWriterWriteAttribute(pWriter, BAD_CAST "resolution", BAD_CAST "0x0");
        }
    }

    // res@bitrate
    if (pUPnPBrowse->IncludeProperty("res@bitrate")) {

        if (bTranscode) {
            int nBitRate = 0; // pUPnPBrowse->DeviceSettings()->FileSettings(sExt)->pTranscodingSettings->VideoBitRate();
            if (nBitRate > 0) {
                stringstream sBitRate;
                sBitRate << nBitRate;
                xmlTextWriterWriteAttribute(pWriter, BAD_CAST "bitrate", BAD_CAST sBitRate.str().c_str());
            }
        }
        else if (0 < item.details->v_bitrate) {
            xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST "bitrate", "%i", item.details->v_bitrate);
        }
    }

    // res@size
    if (!bTranscode && pUPnPBrowse->IncludeProperty("res@size") && 0 < item.size) {
        xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST "size", "%"PROff_t, item.size);
    }

    sExt = pUPnPBrowse->DeviceSettings()->Extension(sExt, item.details->audioCodec, item.details->videoCodec);

    sTmp = "http://" + m_sHTTPServerURL + "/video/" + buildObjectAlias(p_sObjectID, item, pUPnPBrowse->DeviceSettings()) + "." + sExt;
    if (pUPnPBrowse->virtualFoldersEnabled() && pUPnPBrowse->virtualFolderLayout().empty()) {
        sTmp += "?vfolder=none";
    }

    xmlTextWriterWriteString(pWriter, BAD_CAST sTmp.c_str());
    xmlTextWriterEndElement(pWriter);
}

void CContentDirectory::BuildVideoBroadcastItemDescription(xmlTextWriterPtr pWriter, const Database::Item& item, CUPnPBrowseSearchBase* pUPnPBrowse, std::string /*p_sObjectID*/)
{
    /* // title
     xmlTextWriterStartElement(pWriter, BAD_CAST "dc:title");
     // trim filename
     string sFileName = TrimFileName(pSQLResult->asString("FILE_NAME"), pUPnPBrowse->DeviceSettings()->DisplaySettings()->nMaxFileNameLength, true);
     sFileName = TruncateFileExt(sFileName);
     xmlTextWriterWriteString(pWriter, BAD_CAST sFileName.c_str());
     xmlTextWriterEndElement(pWriter);

     // class
     xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:class");
     xmlTextWriterWriteString(pWriter, BAD_CAST "object.item.audioItem.audioBroadcast");
     xmlTextWriterEndElement(pWriter);

     // res
     xmlTextWriterStartElement(pWriter, BAD_CAST "res");
     xmlTextWriterWriteString(pWriter, BAD_CAST pSQLResult->asString("PATH").c_str());
     xmlTextWriterEndElement(pWriter);   */

    // title
    xmlTextWriterStartElement(pWriter, BAD_CAST "dc:title");
    // trim filename
    string title = TrimFileName(item.title, pUPnPBrowse->DeviceSettings()->DisplaySettings()->nMaxFileNameLength);
    xmlTextWriterWriteString(pWriter, BAD_CAST title.c_str());
    xmlTextWriterEndElement(pWriter);

    // class
    xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:class");
    xmlTextWriterWriteString(pWriter, BAD_CAST "object.item.videoItem.videoBroadcast");
    xmlTextWriterEndElement(pWriter);

    // res
    xmlTextWriterStartElement(pWriter, BAD_CAST "res");

    string sTmp = BuildProtocolInfo(false, item.details->streamMimeType, "", pUPnPBrowse);
    xmlTextWriterWriteAttribute(pWriter, BAD_CAST "protocolInfo", BAD_CAST sTmp.c_str());

    xmlTextWriterWriteString(pWriter, BAD_CAST item.path.c_str());
    xmlTextWriterEndElement(pWriter);
}

void CContentDirectory::BuildPlaylistItemDescription(xmlTextWriterPtr pWriter, const Database::Item& item, CUPnPBrowseSearchBase* pUPnPBrowse, std::string p_sObjectID)
{
    // title
    xmlTextWriterStartElement(pWriter, BAD_CAST "dc:title");
    // trim filename
    string sFileName = TrimFileName(item.title, pUPnPBrowse->DeviceSettings()->DisplaySettings()->nMaxFileNameLength);
    //sFileName = TruncateFileExt(sFileName);
    xmlTextWriterWriteString(pWriter, BAD_CAST sFileName.c_str());
    xmlTextWriterEndElement(pWriter);

    // class
    xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:class");
    xmlTextWriterWriteString(pWriter, BAD_CAST "object.item.playlistItem");
    xmlTextWriterEndElement(pWriter);

    // res
    xmlTextWriterStartElement(pWriter, BAD_CAST "res");

    string ext = item.extension; //ExtractFileExt(pSQLResult->asString("FILE_NAME"));

    switch (pUPnPBrowse->DeviceSettings()->playlistStyle()) {
        case CDeviceSettings::container:
            case CDeviceSettings::file:
            break;
        case CDeviceSettings::pls:
            ext = "pls";
            break;
        case CDeviceSettings::m3u:
            ext = "m3u";
            break;
        case CDeviceSettings::wpl:
            ext = "wpl";
            break;
        case CDeviceSettings::xspf:
            ext = "xspf";
            break;
    }

    string mimeType = pUPnPBrowse->DeviceSettings()->MimeType(ext);

    std::stringstream sTmp;
    sTmp << "http-get:*:" << mimeType << ":*";
    xmlTextWriterWriteAttribute(pWriter, BAD_CAST "protocolInfo", BAD_CAST sTmp.str().c_str());
    sTmp.str("");

    sTmp << "http://" << m_sHTTPServerURL << "/MediaServer/Playlists/" << p_sObjectID << "." << ext;
    xmlTextWriterWriteString(pWriter, BAD_CAST sTmp.str().c_str());
    xmlTextWriterEndElement(pWriter);
}

void CContentDirectory::HandleUPnPGetSearchCapabilities(CUPnPAction* /*pAction*/, std::string* p_psResult)
{
    *p_psResult = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
            "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
            "  <s:Body>"
            "    <u:GetSearchCapabilitiesResponse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\">"
            "      <SearchCaps>" + Database::SearchCriteria::getSearchCapabilities() + "</SearchCaps>"
            "    </u:GetSearchCapabilitiesResponse>"
            "  </s:Body>"
            "</s:Envelope>";

    //<SearchCaps>dc:title,dc:creator,upnp:artist,upnp:genre,upnp:album,dc:date,upnp:originalTrackNumber,upnp:class,@id,@refID,upnp:albumArtURI</SearchCaps>
}

void CContentDirectory::HandleUPnPGetSortCapabilities(CUPnPAction* /*pAction*/, std::string* p_psResult)
{
    *p_psResult = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
            "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
            "  <s:Body>"
            "    <u:GetSortCapabilitiesResponse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\">"
            "      <SortCaps>" + Database::SortCriteria::getSortCapabilities() + "</SortCaps>"
            "    </u:GetSortCapabilitiesResponse>"
            "  </s:Body>"
            "</s:Envelope>";

    // upnp:class,
    // <SortCaps>dc:title,dc:creator,upnp:artist,upnp:genre,upnp:album,dc:date,upnp:originalTrackNumber,Philips:shuffle</SortCaps>
}

void CContentDirectory::HandleUPnPGetSortExtensionCapabilities(CUPnPAction* /*pAction*/, std::string* p_psResult)
{
    *p_psResult = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
            "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
            "  <s:Body>"
            "    <u:GetSortExtensionsCapabilitiesResponse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\">"
            "      <SortExtensionCaps>" + Database::SortCriteria::getSortExtensionCapabilities() + "</SortExtensionCaps>"
            "    </u:GetSortExtensionsCapabilitiesResponse>"
            "  </s:Body>"
            "</s:Envelope>";
}

void CContentDirectory::HandleUPnPGetSystemUpdateID(CUPnPAction* /*pAction*/, std::string* p_psResult)
{
    stringstream result;
    result << "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
            "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
            "  <s:Body>"
            "    <u:GetSystemUpdateIDResponse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\">"
            "      <Id>" << ContentDatabase::ContentDatabase::getSystemUpdateId() << "</Id>"
            "    </u:GetSystemUpdateIDResponse>"
            "  </s:Body>"
            "</s:Envelope>";

    *p_psResult = result.str();
}

upnp_error_code_t CContentDirectory::HandleUPnPSearch(CUPnPSearch* pSearch, std::string* p_psResult)
{

	// todo :: build parent id list if pSearch->getObjectId() > 0

    fuppes_int64_t totalMatches = 0;
    fuppes_int64_t numberReturned = 0;

    Database::QueryBuilder qb;
    std::string device = pSearch->virtualFolderLayout();

    // get the total matches
    qb.select("o", "ID", Database::ObjectDetails, Database::AbstractQueryBuilder::count);
    device.empty() ? qb.where("o", "DEVICE", "is", "NULL") : qb.where("o", "DEVICE", "=", "?");
    qb.rawSql("and " + pSearch->m_searchCriteriaSql)
            .orderBy(pSearch->m_sortCriteriaSQL);

    Database::PreparedStatement getCount;
    if (device.empty()) {
        getCount.bindText(0, device);
    }
    qb.getQuery(getCount);

    Database::QueryResult count;
    if (!getCount.select(count)) {
        return upnp_401_invalid_action;
    }

    totalMatches = count.getRow(0)->getColumnInt64("COUNT");

    // get the items
    qb.select(Database::ObjectDetails);
    device.empty() ? qb.where("o", "DEVICE", "is", "NULL") : qb.where("o", "DEVICE", "=", "?");
    qb.rawSql("and " + pSearch->m_searchCriteriaSql)
            .orderBy(pSearch->m_sortCriteriaSQL);

    if (0 < pSearch->m_nStartingIndex || 0 < pSearch->m_nRequestedCount) {
        qb.limit(pSearch->m_nStartingIndex, pSearch->m_nRequestedCount);
    }

    Database::PreparedStatement getItems;
    if (device.empty()) {
        getItems.bindText(0, device);
    }
    qb.getQuery(getItems);

    // build result
    xmlTextWriterPtr writer;
    xmlBufferPtr buf;

    buf = xmlBufferCreate();
    writer = xmlNewTextWriterMemory(buf, 0);
    xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);

    // root
    xmlTextWriterStartElementNS(writer, BAD_CAST "s", BAD_CAST "Envelope", NULL);
    xmlTextWriterWriteAttributeNS(writer, BAD_CAST "s", BAD_CAST "encodingStyle", BAD_CAST "http://schemas.xmlsoap.org/soap/envelope/", BAD_CAST "http://schemas.xmlsoap.org/soap/encoding/");

    // body
    xmlTextWriterStartElementNS(writer, BAD_CAST "s", BAD_CAST "Body", NULL);

    // search response
    xmlTextWriterStartElementNS(writer, BAD_CAST "u", BAD_CAST "SearchResponse", BAD_CAST "urn:schemas-upnp-org:service:ContentDirectory:1");

    // result
    xmlTextWriterStartElement(writer, BAD_CAST "Result");

    // build result xml
    xmlTextWriterPtr resWriter;
    xmlBufferPtr resBuf;
    resBuf = xmlBufferCreate();
    resWriter = xmlNewTextWriterMemory(resBuf, 0);
    xmlTextWriterStartDocument(resWriter, NULL, "UTF-8", NULL);

    xmlTextWriterStartElementNS(resWriter, NULL, BAD_CAST "DIDL-Lite", BAD_CAST "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");
    xmlTextWriterWriteAttribute(resWriter, BAD_CAST "xmlns:dc", BAD_CAST "http://purl.org/dc/elements/1.1/");
    xmlTextWriterWriteAttribute(resWriter, BAD_CAST "xmlns:upnp", BAD_CAST "urn:schemas-upnp-org:metadata-1-0/upnp/");
    xmlTextWriterWriteAttribute(resWriter, BAD_CAST "xmlns:dlna", BAD_CAST "urn:schemas-dlna-org:metadata-1-0/");

    Database::Item item;

    while (getItems.next(item)) {
        BuildItemDescription(resWriter, item, pSearch, item.type, item.getParentIdAsHex());
        numberReturned++;
    }

    /*while (!qry.eof()) {
     pRow = qry.result();
     #warning TODO
     //BuildDescription(resWriter, pRow, pSearch, "0");
     nNumberReturned++;

     qry.next();
     }*/

    // finalize result xml
    xmlTextWriterEndElement(resWriter);
    xmlTextWriterEndDocument(resWriter);
    xmlFreeTextWriter(resWriter);

    string sResOutput = (const char*)resBuf->content;

    xmlBufferFree(resBuf);
    sResOutput = sResOutput.substr(strlen("<?xml version=\"1.0\" encoding=\"UTF-8\"?> "));
    xmlTextWriterWriteString(writer, BAD_CAST sResOutput.c_str());

    // end result
    xmlTextWriterEndElement(writer);

    // number returned
    xmlTextWriterStartElement(writer, BAD_CAST "NumberReturned");
    xmlTextWriterWriteFormatString(writer, "%"PRIi64, numberReturned);
    xmlTextWriterEndElement(writer);

    // total matches
    xmlTextWriterStartElement(writer, BAD_CAST "TotalMatches");
    xmlTextWriterWriteFormatString(writer, "%"PRIi64, totalMatches);
    xmlTextWriterEndElement(writer);

    // update id
    xmlTextWriterStartElement(writer, BAD_CAST "UpdateID");
    xmlTextWriterWriteFormatString(writer, "%u", ContentDatabase::ContentDatabase::getSystemUpdateId());
    xmlTextWriterEndElement(writer);

    // end search response
    xmlTextWriterEndElement(writer);

    // end body
    xmlTextWriterEndElement(writer);

    // end root
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);

    string output;
    output = (const char*)buf->content;
    CSharedLog::Log(L_DBG, __FILE__, __LINE__, output);

    xmlBufferFree(buf);
    //delete qry;

    *p_psResult = output;

    return upnp_200_ok;

}

void CContentDirectory::HandeUPnPDestroyObject(CUPnPAction* pAction, std::string* p_psResult)
{
    /*
     if(CSharedConfig::Shared()->globalSettings->TrashDir().length() == 0 ||
     CDatabase::connectionParams().readonly == true) {
     return;
     }

     //cout << "DESTROY OBJECT: " << pAction->GetObjectIDAsInt() << endl;

     stringstream sql;
     sql << "select TYPE, PATH, FILE_NAME from OBJECTS where OBJECT_ID = " << pAction->GetObjectIDAsUInt() << " and DEVICE is NULL";

     CSQLQuery* qry = CDatabase::query();
     qry->select(sql.str());
     sql.str("");

     string objects;
     if(qry->eof()) {
     delete qry;
     return;
     }

     CSQLResult* result = qry->result();
     OBJECT_TYPE type = (OBJECT_TYPE)result->asInt("TYPE");


     #ifndef WIN32
     time_t now;
     char nowtime[26];
     time(&now);
     ctime_r(&now, nowtime);
     nowtime[24] = '\0';
     string sNowtime = nowtime;
     #else
     char timeStr[9];
     _strtime(timeStr);
     string sNowtime = timeStr;
     #endif

     // create target dir
     string targetDir = CSharedConfig::Shared()->globalSettings->TrashDir();
     if(type < ITEM) { // container
     targetDir += result->asString("FILE_NAME") + "_" + sNowtime + "/";
     }
     else {
     targetDir += sNowtime + "/";
     }

     if(!Directory::create(targetDir)) {
     cout << "contentdir: error creating trash folder : " << targetDir << endl;
     delete qry;
     return;
     }
     if(type >= ITEM) { // item
     targetDir += result->asString("FILE_NAME");
     }

     // move
     //cout << "mv " << db.GetResult()->asString("PATH") << " " << targetDir << endl;

     int ret = rename(string(result->asString("PATH") + result->asString("FILE_NAME")).c_str(),
     targetDir.c_str());
     if(ret != 0) {
     cout << "contentdir: error moving to trash folder" << endl;
     delete qry;
     return;
     }


     // delete from db
     //CContentDatabase::Shared()->deleteObject(pAction->GetObjectIDAsUInt());
     delete qry;

     #warning todo: error code
     *p_psResult = "";
     */
}

/* stolen from libdlna :: copyright (C) 2007-2008 Benjamin Zores */

/*
 # Play speed
 #    1 normal
 #    0 invalid
 DLNA_ORG_PS = 'DLNA.ORG_PS'
 DLNA_ORG_PS_VAL = '1'
 */

enum dlna_org_playSpeed
{
    ps_invalid = 0,
    ps_normal = 1
};

/*
 # Conversion Indicator
 #    1 transcoded
 #    0 not transcoded
 DLNA_ORG_CI = 'DLNA.ORG_CI'
 DLNA_ORG_CI_VAL = '0'
 */

enum dlna_org_conversionIndicator
{
    ci_none = 0,
    ci_transcoded = 1
};

/*
 # Operations
 #    00 not time seek range, not range
 #    01 range supported
 #    10 time seek range supported
 #    11 both supported
 DLNA_ORG_OP = 'DLNA.ORG_OP'
 DLNA_ORG_OP_VAL = '01'
 */

enum dlna_org_operations
{
    op_none = 0x00,
    op_range = 0x01,
    op_time = 0x10,
    op_both = 0x11
};

/*
 # Flags
 #    senderPaced                      80000000  31
 #    lsopTimeBasedSeekSupported       40000000  30
 #    lsopByteBasedSeekSupported       20000000  29
 #    playcontainerSupported           10000000  28
 #    s0IncreasingSupported            08000000  27
 #    sNIncreasingSupported            04000000  26
 #    rtspPauseSupported               02000000  25
 #    streamingTransferModeSupported   01000000  24
 #    interactiveTransferModeSupported 00800000  23
 #    backgroundTransferModeSupported  00400000  22
 #    connectionStallingSupported      00200000  21
 #    dlnaVersion15Supported           00100000  20
 DLNA_ORG_FLAGS = 'DLNA.ORG_FLAGS'
 DLNA_ORG_FLAGS_VAL = '01500000000000000000000000000000'
 */

enum dlna_org_flags
{
    flag_senderPaced = (1 << 31),
    flag_lsopTimeBasedSeekSupported = (1 << 30),
    flag_lsopByteBasedSeekSupported = (1 << 29),
    flag_playcontainerSupported = (1 << 28),
    flag_s0IncreasingSupported = (1 << 27),
    flag_sNIncreasingSupported = (1 << 26),
    flag_rtspPauseSupported = (1 << 25),
    flag_streamingTransferModeSupported = (1 << 24),
    flag_interactiveTransferModeSupported = (1 << 23),
    flag_backgroundTransferModeSupported = (1 << 22),
    flag_connectionStallingSupported = (1 << 21),
    flag_dlnaVersion15Supported = (1 << 20)
};

std::string CContentDirectory::buildDlnaInfo(bool transcode, std::string dlnaProfile) // static
{
    string result = "";

    // play speed
    dlna_org_playSpeed ps = ps_normal;

    // conversion indicator
    dlna_org_conversionIndicator ci = ci_none;
    if (transcode)
        ci = ci_transcoded;

    // operations
    dlna_org_operations op = op_range;
    if (transcode)
        op = op_none;

    // flags
    int flags = 0;
    flags = flag_streamingTransferModeSupported | flag_backgroundTransferModeSupported | flag_connectionStallingSupported | flag_dlnaVersion15Supported;
    if (!transcode)
        flags |= flag_lsopByteBasedSeekSupported;

    char dlna_info[448];
    if (!dlnaProfile.empty()) {
        sprintf(dlna_info, "%s=%s;%s=%.2x;%s=%d;%s=%d;%s=%.8x%.24x", "DLNA.ORG_PN", dlnaProfile.c_str(), "DLNA.ORG_OP", op, "DLNA.ORG_PS", ps, "DLNA.ORG_CI", ci, "DLNA.ORG_FLAGS", flags, 0);
    }
    else {
        sprintf(dlna_info, "%s=%.2x;%s=%d;%s=%d;%s=%.8x%.24x", "DLNA.ORG_OP", op, "DLNA.ORG_PS", ps, "DLNA.ORG_CI", ci, "DLNA.ORG_FLAGS", flags, 0);
    }

    result = dlna_info;
    return result;
}

std::string CContentDirectory::BuildProtocolInfo(bool p_bTranscode, std::string p_sMimeType, std::string p_sProfileId, CUPnPBrowseSearchBase* pUPnPBrowse)
{
    string sTmp;
    if (pUPnPBrowse->DeviceSettings()->dlnaVersion() == CMediaServerSettings::dlna_none) {
        sTmp = "http-get:*:" + p_sMimeType + ":*";
        return sTmp;
    }

    sTmp = "http-get:*:" + p_sMimeType + ":";
    sTmp += CContentDirectory::buildDlnaInfo(p_bTranscode, p_sProfileId);
    return sTmp;
}

void CContentDirectory::writeAlbumArtUrl(xmlTextWriterPtr pWriter, CUPnPBrowseSearchBase* pAction, const Database::Item& item)
{
    if (!pAction->IncludeProperty("upnp:albumArtURI"))
        return;

    OBJECT_TYPE objectType = item.type;
    bool video = (objectType >= ITEM_VIDEO_ITEM && objectType < ITEM_VIDEO_ITEM_MAX);
    bool audio = (objectType >= ITEM_AUDIO_ITEM && objectType < ITEM_AUDIO_ITEM_MAX);
    bool image = (objectType >= ITEM_IMAGE_ITEM && objectType < ITEM_IMAGE_ITEM_MAX);
    bool container = (objectType >= CONTAINER_STORAGEFOLDER && objectType < CONTAINER_MAX);

    //SQLQuery qry;
    object_id_t albumArtId = 0;
    std::string albumArtExt;
    int albumArtWidth = 0;
    int albumArtHeight = 0;
    bool appendSize = false;

    if (audio) {
        // audio files with an album art have either the id for an image file
        // or their own object id if the file contains an image.
        // if the id is 0 there is no album art available
        if (0 == item.details->albumArtId) {
            return;
        }

        // object contains the image
        if (item.details->albumArtId == item.objectId || item.details->albumArtId == item.refId || item.details->albumArtId == item.vrefId) {

            albumArtId = item.details->albumArtId; //pSQLResult->asUInt("ALBUM_ART_ID");
            albumArtExt = item.details->albumArtExtension; //pSQLResult->asString("ALBUM_ART_EXT");
            albumArtWidth = item.details->albumArtWidth; //pSQLResult->asInt("ALBUM_ART_WIDTH");
            albumArtHeight = item.details->albumArtHeight; //pSQLResult->asInt("ALBUM_ART_HEIGHT");
            if (albumArtWidth == 0 || albumArtHeight == 0) {
                albumArtWidth = 300;
                albumArtHeight = 300;
            }
            appendSize = true;
        }
        // image file
        else {
            albumArtId = item.details->albumArtId; //pSQLResult->asUInt("ALBUM_ART_ID");
            albumArtExt = item.details->albumArtExtension; //pSQLResult->asString("ALBUM_ART_EXT");
            albumArtWidth = item.details->albumArtWidth; //pSQLResult->asInt("ALBUM_ART_WIDTH");
            albumArtHeight = item.details->albumArtHeight; //pSQLResult->asInt("ALBUM_ART_HEIGHT");
        }

    } // audio

    if (image) {

        // if we got an image and no magickWand transcoder there is no album art
        //if (0 == item.details->albumArtId && !Plugin::Manager::hasTranscoderPlugin("magickWand")) {
    	// todo
    	if (0 == 0) {
            return;
        }

        // else we use the object itself
        albumArtId = item.objectId; // pSQLResult->asUInt("OBJECT_ID");
        albumArtExt = item.extension; //ExtractFileExt(pSQLResult->asString("FILE_NAME"));
        albumArtWidth = 300;
        albumArtHeight = 300;
    } // image

    if (video) {

        if (0 == item.details->albumArtId) {
            return;
        }

        albumArtId = item.details->albumArtId;
        albumArtExt = item.details->albumArtExtension; //pSQLResult->asString("ALBUM_ART_EXT");
        albumArtWidth = item.details->albumArtWidth; //pSQLResult->asInt("ALBUM_ART_WIDTH");
        albumArtHeight = item.details->albumArtHeight;

        // no album art set and no ffmpegthumbnailer means no album art
        /*if(!Plugins::CPluginMgr::hasMetadataPlugin("ffmpegthumbnailer") && 0 == item.details->albumArtId)
         return;

         // create image from the video using ffmpegthumbnailer
         if(0 == item.details->albumArtId) {
         albumArtId = item.objectId;
         albumArtExt = "jpg";
         albumArtWidth = 300;
         albumArtHeight = 300;

         }
         // image file
         else {

         albumArtId = item.details->albumArtId;
         albumArtExt = item.details->albumArtExtension; //pSQLResult->asString("ALBUM_ART_EXT");
         albumArtWidth = item.details->albumArtWidth; //pSQLResult->asInt("ALBUM_ART_WIDTH");
         albumArtHeight = item.details->albumArtHeight; //pSQLResult->asInt("ALBUM_ART_HEIGHT");
         }*/

    } // video

    if (container) {

        if (0 == item.details->albumArtId)
            return;

        albumArtId = item.details->albumArtId;
        albumArtExt = item.details->albumArtExtension; //pSQLResult->asString("ALBUM_ART_EXT");
        albumArtWidth = item.details->albumArtWidth; //pSQLResult->asInt("ALBUM_ART_WIDTH");
        albumArtHeight = item.details->albumArtHeight; //pSQLResult->asInt("ALBUM_ART_HEIGHT");
    } // container

    std::string mimeType;
    std::string profile;
    DLNA::getImageProfile(albumArtExt, albumArtWidth, albumArtHeight, profile, mimeType);

    if (mimeType.empty()) {
        mimeType = pAction->DeviceSettings()->MimeType(albumArtExt);
    }

    char szArtId[20];
    sprintf(szArtId, "%"PRObjectId, albumArtId);
    stringstream url;
    url << "http://" << m_sHTTPServerURL << "/image/" << string(szArtId) << "." << albumArtExt << "?vfolder=none";
    if (appendSize)
        url << "&width=" << albumArtWidth << "&height=" << albumArtHeight;

    // write the xml node
    xmlTextWriterStartElement(pWriter, BAD_CAST "upnp:albumArtURI");

    if (!profile.empty()) {
        //xmlTextWriterWriteAttribute(pWriter, BAD_CAST "xmlns:dlna", BAD_CAST "urn:schemas-dlna-org:metadata-1-0/");
        xmlTextWriterWriteAttribute(pWriter, BAD_CAST "dlna:profileID", BAD_CAST profile.c_str());
    }

    xmlTextWriterWriteString(pWriter, BAD_CAST url.str().c_str());

    xmlTextWriterEndElement(pWriter);
}

inline bool convertAlias(std::string& alias)
{
    string tmp = alias;
    tmp = StringReplace(tmp, " & ", " and ");
    tmp = StringReplace(tmp, " ", "_");

    RegEx rxAlias("^[A-Z|a-z|\\d|_|-]+$");
    if (rxAlias.search(tmp)) {
        alias = tmp;
        return true;
    }
    return false;
}

std::string CContentDirectory::buildObjectAlias(std::string objectId, const Database::Item& item, CDeviceSettings* deviceSettings)
{
    /*
     if(!rewriteUrl()) {
     return objectId;
     }
     */

    /*cout << "buildObjectAlias for object id: " << objectId << endl;
     cout << "title: " << pSQLResult->asString("TITLE") << endl;  */

    string alias;
    bool valid = false;

    if (deviceSettings->MediaServerSettings()->rewriteUrls) {

        switch (item.type) {

            case ITEM_AUDIO_ITEM:
                case ITEM_AUDIO_ITEM_MUSIC_TRACK:
                // ARTIST ALBUM TRACK_NO ...
                //valid = convertAlias(alias);
                if (!valid) {
                    alias = item.title;
                    valid = convertAlias(alias);
                }
                break;

            default:
                alias = item.title;
                valid = convertAlias(alias);
                break;
        }

    }

    //cout << "alias: " << alias << " " << (valid ? "valid" : "invalid") << endl;

    return (valid ? objectId + "/" + alias : objectId);
}

inline bool envelopeStart(xmlBufferPtr* buffer, xmlTextWriterPtr* writer, const std::string action)
{
    *buffer = xmlBufferCreate();
    *writer = xmlNewTextWriterMemory(*buffer, 0);
    if (NULL == *writer) {
        xmlBufferFree(*buffer);
        return false;
    }

    xmlTextWriterStartDocument(*writer, NULL, "UTF-8", NULL);

    // root
    xmlTextWriterStartElementNS(*writer, BAD_CAST "s", BAD_CAST "Envelope", NULL);
    xmlTextWriterWriteAttributeNS(*writer, BAD_CAST "s", BAD_CAST "encodingStyle", BAD_CAST "http://schemas.xmlsoap.org/soap/envelope/", BAD_CAST "http://schemas.xmlsoap.org/soap/encoding/");

    // body
    xmlTextWriterStartElementNS(*writer, BAD_CAST "s", BAD_CAST "Body", NULL);

    // action response
    xmlTextWriterStartElementNS(*writer, BAD_CAST "u", BAD_CAST action.c_str(), BAD_CAST "urn:schemas-upnp-org:service:ContentDirectory:1");

    return true;
}

inline std::string envelopeEnd(xmlBufferPtr* buffer, xmlTextWriterPtr* writer)
{
    xmlTextWriterEndElement(*writer); // action response
    xmlTextWriterEndElement(*writer); // body
    xmlTextWriterEndElement(*writer); // envelope

    xmlTextWriterEndDocument(*writer);
    xmlFreeTextWriter(*writer);

    string result = (const char*)(*buffer)->content;
    xmlBufferFree(*buffer);

    return result;
}

upnp_error_code_t CContentDirectory::CreateObject(CUPnPAction* action, std::string* result)
{
    return upnp_401_invalid_action;

    //cout << action->GetContent() << endl;
    //cout << "CreateObject: " << action->value("ContainerID") << " " << action->value("Elements") << endl;

    /*
     unsigned int containerId;
     if(!action->valueAsUInt("ContainerID", &containerId))
     return upnp_402_invalid_args;

     DbObject* parent = DbObject::createFromObjectId(containerId);
     if(NULL == parent && containerId > 0) {
     return upnp_710_no_such_container;
     }
     delete parent;

     // parent restricted ?
     // return upnp_713_restricted_parent_object;

     // parse the elements
     std::string elements = action->value("Elements");
     xmlDocPtr doc = xmlReadMemory(elements.c_str(), elements.length(), "", NULL, 0);
     if(NULL == doc) {
     return upnp_402_invalid_args;
     }

     xmlNodePtr tmp = xmlDocGetRootElement(doc); // DIDL-Lite
     if(NULL == tmp) {
     xmlFreeDoc(doc);
     return upnp_402_invalid_args;
     }

     tmp = xmlFirstElementChild(tmp); // container | item
     if(NULL == tmp) {
     xmlFreeDoc(doc);
     return upnp_402_invalid_args;
     }

     string type = (char*)tmp->name;

     xmlChar* pid = xmlGetNoNsProp(tmp, BAD_CAST "parentID");
     if(NULL == pid) {
     xmlFreeDoc(doc);
     return upnp_712_bad_metadata;
     }

     if(HexToInt((char*)pid) != containerId) {
     xmlFreeDoc(doc);
     return upnp_712_bad_metadata;
     }

     string dc_title;
     string upnp_class;
     string* current;

     tmp = xmlFirstElementChild(tmp);
     xmlChar* content;
     while (NULL != tmp) {

     string name = (char*)tmp->name;
     current = NULL;

     if(name.compare("title") == 0)
     current = &dc_title;
     else if(name.compare("class") == 0)
     current = &upnp_class;

     content = xmlNodeGetContent(tmp);
     if(NULL != current && NULL != content)
     *current = (char*)content;

     tmp = xmlNextElementSibling(tmp);
     }

     DbObject obj;
     obj.setParentId(containerId);
     obj.setPath("*CreateObject");
     obj.setTitle(dc_title);
     obj.setType(UPnP::stringToType(upnp_class));

     bool saved = false;
     string didl;

     if(type.compare("container") == 0) {

     saved = obj.save();

     didl =
     "<DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\">"
     "  <container id=\"" + obj.objectIdAsHex() + "\" parentID=\"" + action->value("ContainerID") + "\" restricted=\"false\">"
     "    <dc:title>" + obj.title() + "</dc:title>"
     "    <upnp:class>" + UPnP::typeToString(obj.type()) + "</upnp:class>"
     "  </container>"
     "</DIDL-Lite>";
     */
    /*
     <DIDL-Lite xmlns="urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:upnp="urn:schemas-upnp-org:metadata-1-0/upnp/">
     <container id="" parentID="0000000001" restricted="false">
     <dc:title>test dir 123</dc:title>
     <upnp:class>object.container.storageFolder</upnp:class>
     </container>
     </DIDL-Lite>
     */

//    }
    /*else if(type.compare("item")) {
     }*/
    /*    else {
     xmlFreeDoc(doc);
     return upnp_712_bad_metadata;
     }

     if(!saved) {
     xmlFreeDoc(doc);
     return upnp_401_invalid_action;
     }

     xmlBufferPtr buffer = NULL;
     xmlTextWriterPtr writer = NULL;
     envelopeStart(&buffer, &writer, "CreateObjectResponse");

     // objectId
     xmlTextWriterStartElement(writer, BAD_CAST "ObjectID");
     xmlTextWriterWriteString(writer, BAD_CAST obj.objectIdAsHex().c_str());
     xmlTextWriterEndElement(writer);

     // result
     xmlTextWriterStartElement(writer, BAD_CAST "Result");
     xmlTextWriterWriteString(writer, BAD_CAST didl.c_str());
     xmlTextWriterEndElement(writer);

     *result = envelopeEnd(&buffer, &writer);

     xmlFreeDoc(doc);

     CContentDatabase::incSystemUpdateId();
     return upnp_200_ok;*/
}

upnp_error_code_t CContentDirectory::DestroyObject(CUPnPAction* action, std::string* result)
{
    cout << "DestroyObject: " << action->value("ObjectID") << endl;

    //CContentDatabase::incSystemUpdateId();
    return upnp_200_ok;
}

upnp_error_code_t CContentDirectory::UpdateObject(CUPnPAction* action, std::string* result)
{
    cout << "UpdateObject: " << action->value("ObjectID") << " " << action->value("CurrentTagValue") << " " << action->value("NewTagValue") << endl;

    //CContentDatabase::incSystemUpdateId();
    return upnp_200_ok;
}

upnp_error_code_t CContentDirectory::CreateReference(CUPnPAction* action, std::string* result)
{


    // get the action arguments
    unsigned int containerId;
    unsigned int objectId;

    if (!action->valueAsUInt("ContainerID", &containerId) || !action->valueAsUInt("ObjectID", &objectId))
        return upnp_402_invalid_args;
    /*
     // get the corresponding objects
     DbObject* item = DbObject::createFromObjectId(objectId);
     if(NULL == item) {
     return upnp_402_invalid_args;
     }
     DbObject* parent = DbObject::createFromObjectId(containerId);
     if(NULL == parent) {
     delete item;
     return upnp_402_invalid_args;
     }

     // create the reference
     DbObject* ref = new DbObject(item);
     ref->setParentId(parent->objectId());
     ref->setRefId(item->objectId());
     ref->save();

     delete item;
     delete parent;

     // create response
     xmlBufferPtr buffer = NULL;
     xmlTextWriterPtr writer = NULL;
     envelopeStart(&buffer, &writer, "CreateReferenceResponse");

     xmlTextWriterStartElement(writer, BAD_CAST "newID");
     xmlTextWriterWriteString(writer, BAD_CAST ref->objectIdAsHex().c_str());
     xmlTextWriterEndElement(writer);

     *result = envelopeEnd(&buffer, &writer);

     delete ref;

     CContentDatabase::incSystemUpdateId();
     return upnp_200_ok;
     */

    return upnp_402_invalid_args;
}

