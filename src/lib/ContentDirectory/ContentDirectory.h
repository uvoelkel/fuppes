/***************************************************************************
 *            ContentDirectory.h
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2010 Ulrich Völkel <fuppes@ulrich-voelkel.de>
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
 
#ifndef _CONTENTDIRECTORY_H
#define _CONTENTDIRECTORY_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../UPnP/UPnPService.h"
#include "../Fuppes/HttpRequest.h"
#include "../Fuppes/HttpResponse.h"
#include "../UPnPActions/UPnPBrowse.h"
#include "../UPnPActions/UPnPSearch.h"

#include "../Database/Item.h"

#include <map>
#include <string>

class CContentDirectory: public UPnPService
{
	
  public:

    /** constructor
     */
    CContentDirectory();

    /** destructor     
     */
    ~CContentDirectory();


    std::string getServiceDescription();

    /** handles a UPnP action and creates the corresponding message
     *  @param  pUPnPAction  UPnP action to handle
     *  @param  pMessageOut  the message, that was created for the action   
     */
    //void HandleUPnPAction(CUPnPAction* pUPnPAction, CHTTPMessage* pMessageOut);

    Fuppes::HttpResponse* handleAction(CUPnPAction* upnpAction, std::string action, Fuppes::HttpRequest* request);


    static std::string buildDlnaInfo(bool transcode, std::string dlnaProfile);
    

  private:

    /** scans a specific directory
     *  @param  p_sDirectory  path to the directory to scan
     *  @param  p_pnCount  count of found objects
     *  @param  pParentFolder  the parent folder
    */
    //void DbScanDir(std::string p_sDirectory, long long int p_nParentId);
       
    
    
    /** handles a UPnP browse action
     *  @param  pBrowse  the browse action to handle
     *  @return string with the message content to send for the browse action
     */  
    upnp_error_code_t HandleUPnPBrowse(CUPnPBrowse* pBrowse, std::string* p_psResult);

    upnp_error_code_t BrowseMetadata(xmlTextWriterPtr pWriter, 
                        unsigned int* p_pnTotalMatches,
                        unsigned int* p_pnNumberReturned,
                        CUPnPBrowse*  pUPnPBrowse);
                        
    upnp_error_code_t BrowseDirectChildren(xmlTextWriterPtr pWriter, 
                              unsigned int* p_pnTotalMatches,
                              unsigned int* p_pnNumberReturned,
                              CUPnPBrowse*  pUPnPBrowse);

    void BuildDescription(xmlTextWriterPtr pWriter,
                          const Database::Item& item,
                          CUPnPBrowseSearchBase*  pUPnPBrowse);
  
    void BuildContainerDescription(xmlTextWriterPtr pWriter,
                                const Database::Item& item,
                                   CUPnPBrowseSearchBase*  pUPnPBrowse);
    void BuildItemDescription(xmlTextWriterPtr pWriter,
                                const Database::Item& item,
                              CUPnPBrowseSearchBase*  pUPnPBrowse,
                              OBJECT_TYPE p_nObjectType,
                              std::string p_sParentId);
    void BuildAudioItemDescription(xmlTextWriterPtr pWriter,
            const Database::Item& item,
                                   CUPnPBrowseSearchBase*  pUPnPBrowse,
                                   std::string p_sObjectID);      
    void BuildAudioBroadcastItemDescription(xmlTextWriterPtr pWriter,
            const Database::Item& item,
                                   CUPnPBrowseSearchBase*  pUPnPBrowse,
                                   std::string p_sObjectID);                                    
    void BuildImageItemDescription(xmlTextWriterPtr pWriter,
            const Database::Item& item,
                                   CUPnPBrowseSearchBase*  pUPnPBrowse,
                                   std::string p_sObjectID);
    void BuildVideoItemDescription(xmlTextWriterPtr pWriter,
            const Database::Item& item,
                                   CUPnPBrowseSearchBase*  pUPnPBrowse,
                                   std::string p_sObjectID); 
    void BuildVideoBroadcastItemDescription(xmlTextWriterPtr pWriter,
            const Database::Item& item,
                                   CUPnPBrowseSearchBase*  pUPnPBrowse,
                                   std::string p_sObjectID);   
    void BuildPlaylistItemDescription(xmlTextWriterPtr pWriter,
            const Database::Item& item,
                                   CUPnPBrowseSearchBase*  pUPnPBrowse,
                                   std::string p_sObjectID);                                    


    void HandleUPnPGetSearchCapabilities(CUPnPAction* pAction, std::string* p_psResult);

    void HandleUPnPGetSortCapabilities(CUPnPAction* pAction, std::string* p_psResult);
    
		void HandleUPnPGetSortExtensionCapabilities(CUPnPAction* pAction, std::string* p_psResult);
	
    void HandleUPnPGetSystemUpdateID(CUPnPAction* pAction, std::string* p_psResult);    
		
    upnp_error_code_t HandleUPnPSearch(CUPnPSearch* pSearch, std::string* p_psResult);

    void HandeUPnPDestroyObject(CUPnPAction* pAction, std::string* p_psResult);  	
    
    std::string BuildProtocolInfo(bool p_bTranscode,
                                  std::string p_sMimeType,
                                  std::string p_sProfileId,
                                  CUPnPBrowseSearchBase*  pUPnPBrowse);

    void writeAlbumArtUrl(xmlTextWriterPtr pWriter, CUPnPBrowseSearchBase* pAction, const Database::Item& item);
    std::string buildObjectAlias(std::string objectId, const Database::Item& item, CDeviceSettings* deviceSettings);


    upnp_error_code_t CreateObject(CUPnPAction* action, std::string* result);
    upnp_error_code_t DestroyObject(CUPnPAction* action, std::string* result);
    upnp_error_code_t UpdateObject(CUPnPAction* action, std::string* result);

    upnp_error_code_t CreateReference(CUPnPAction* action, std::string* result);

    bool  m_hasSubtitles;
};

#endif // _CONTENTDIRECTORY_H
