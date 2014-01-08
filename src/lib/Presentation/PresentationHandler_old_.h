/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            PresentationHandler.h
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
 
#ifndef _PRESENTATIONHANDLER_H
#define _PRESENTATIONHANDLER_H

#ifdef HAVE_CONFIG_H
#include "../../config.h"
#endif

#include "../UPnPDevice.h"
#include "../HTTP/HTTPMessage.h"
#include "../Fuppes.h"
#include "PresentationPage.h"

#include <string>
#include <vector>


typedef enum tagPRESENTATION_PAGE
{
  PRESENTATION_PAGE_UNKNOWN,
  PRESENTATION_BINARY_IMAGE,
	PRESENTATION_STYLESHEET,
  PRESENTATION_JAVASCRIPT,
  PRESENTATION_PAGE_INDEX,
  //PRESENTATION_PAGE_ABOUT,
  //PRESENTATION_PAGE_HELP,
  PRESENTATION_PAGE_OPTIONS,
  //PRESENTATION_PAGE_STATUS,
  //PRESENTATION_PAGE_JSTEST
}PRESENTATION_PAGE;

class CPresentationHandler
{
  public:
    CPresentationHandler(std::string httpServerUrl);
    virtual ~CPresentationHandler();


    /** handles HTTP messages add returns a corresponding message
     *  @param pSender  sender of the incoming message
     *  @param pMessage  the incoming message
     *  @param pResult  the outgoing message
     */
    void OnReceivePresentationRequest(CHTTPMessage* pMessage, CHTTPMessage* pResult, bool& isImage, int &width, int &height);
  
  private:

    /** handles HTTP requests
     *  @param p_sRequest  the message to handle
     */
    std::string HandleRequest(std::string p_sRequest);  


    std::string GetPageHeader(PRESENTATION_PAGE p_nPresentationPage, std::string p_sPageName, std::string js = "");
    std::string GetPageFooter(PRESENTATION_PAGE p_nPresentationPage);

/*
    std::string getPageHeader(PresentationPage* page);
    std::string getPageFooter(PresentationPage* page);
  */  
    std::string m_httpServerUrl;

    //std::list<PresentationPage*>  m_pages;
};

#endif // _PRESENTATIONHANDLER_H
