/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            PresentationHandler.cpp
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

#ifdef HAVE_CONFIG_H
#include "../../config.h"
#endif
 
#include "PresentationHandler.h"
//#include "Stylesheet.h"
#include "../SharedConfig.h"
#include "../Configuration/Config.h"
#include "../SharedLog.h"
#include "../Common/Common.h"
#include "../Common/RegEx.h"
#include "../ContentDirectory/ContentDatabase.h"
#include "../ContentDirectory/DatabaseConnection.h"
#ifdef HAVE_VFOLDER
#include "../ContentDirectory/VirtualContainerMgr.h"
#endif
#include "../ContentDirectory/FileDetails.h"
#include "../Transcoding/TranscodingMgr.h"
#include "../HTTP/HTTPParser.h"
#include "../Plugins/Plugin.h"

#include <sstream>
#include <fstream>
#include <iostream>

const std::string LOGNAME = "PresentationHandler"; 


#include "PageStart.h"
#include "PageConfig.h"
#include "PageDevice.h"
#include "PageBrowse.h"
#include "PageLog.h"
#include "PagePlaylist.h"

#include "PageDebug.h"
#include "PageJsTest.h"


CPresentationHandler::CPresentationHandler(std::string httpServerUrl)
{
  m_httpServerUrl = httpServerUrl;

/*
  m_pages.push_back(new PageStart());
  m_pages.push_back(new PageConfig());
  m_pages.push_back(new PageDevice());
  m_pages.push_back(new PageBrowse());
  m_pages.push_back(new PagePlaylist());
  
  m_pages.push_back(new PageLog());
  
  m_pages.push_back(new PageDebug());
*/
  //m_pages.push_back(new PageJsTest());
}

CPresentationHandler::~CPresentationHandler()
{
  /*
  std::list<PresentationPage*>::iterator  iter;
  for(iter = m_pages.begin(); iter != m_pages.end(); ++iter) {
    delete *iter;
  } 
*/
}

void CPresentationHandler::OnReceivePresentationRequest(CHTTPMessage* pMessage, CHTTPMessage* pResult, bool& isImage, int &width, int &height)
{
  PRESENTATION_PAGE nPresentationPage = PRESENTATION_PAGE_UNKNOWN;
  string sContent;
  std::string sPageName = "undefined";


  std::string alias;
  std::string ext;
  string request = ToLower(pMessage->GetRequest());
  
  if((pMessage->GetRequest().compare("/") == 0) ||
     (request.compare("/index.html") == 0)) {    
    alias = "index";
    ext = "html";
  }
  else  {
    RegEx rxUrl("/presentation/(\\w+)\\.(html|css|png|jpg|js)");
    if(rxUrl.search(request)) {
      alias = rxUrl.match(1);
      ext = rxUrl.match(2);
    }
  }



  std::list<PresentationPage*>::iterator  iter;
  for(iter = m_pages.begin(); iter != m_pages.end(); ++iter) {
    PresentationPage* page = *iter;

    if(page->alias().compare(alias) != 0)
      continue;
      
    sPageName = page->title();

    stringstream result;    
    result << getPageHeader(page);
    result << page->content();
    result << getPageFooter(page);
    
    pResult->SetMessageType(HTTP_MESSAGE_TYPE_200_OK);
    pResult->SetContentType("text/html; charset=\"utf-8\"");
    pResult->SetContent(result.str());
    return;
  }




  if(ToLower(pMessage->GetRequest()).compare("/presentation/style.css") == 0) {
    nPresentationPage = PRESENTATION_STYLESHEET;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "style.css");
  }

  /*
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/options.html") == 0) {
    nPresentationPage = PRESENTATION_PAGE_OPTIONS;
    sContent = this->GetOptionsHTML();
    sPageName = "Options";
  }
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/options.html?db=rebuild") == 0) {
    CSharedConfig::Shared()->Refresh();
    if(!CContentDatabase::Shared()->IsRebuilding())
      CContentDatabase::Shared()->RebuildDB();

    nPresentationPage = PRESENTATION_PAGE_OPTIONS;
    sContent = this->GetOptionsHTML();
    sPageName = "Options";
  }
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/options.html?db=update") == 0) {
    CSharedConfig::Shared()->Refresh();
    if(!CContentDatabase::Shared()->IsRebuilding() 
#ifdef HAVE_VFOLDER
      && !CVirtualContainerMgr::Shared()->IsRebuilding()
#endif
      )
      CContentDatabase::Shared()->UpdateDB();

    nPresentationPage = PRESENTATION_PAGE_OPTIONS;
    sContent = this->GetOptionsHTML();
    sPageName = "Options";
  }
	else if(ToLower(pMessage->GetRequest()).compare("/presentation/options.html?vcont=rebuild") == 0) {
    CSharedConfig::Shared()->Refresh();
    if(!CContentDatabase::Shared()->IsRebuilding() 
#ifdef HAVE_VFOLDER
      && !CVirtualContainerMgr::Shared()->IsRebuilding()
#endif
      )
#ifdef HAVE_VFOLDER
      CVirtualContainerMgr::Shared()->RebuildContainerList();
#endif

    nPresentationPage = PRESENTATION_PAGE_OPTIONS;
    sContent = this->GetOptionsHTML();
    sPageName = "Options";
  }

  else if(ToLower(pMessage->GetRequest()).compare("/presentation/status.html") == 0) {
    nPresentationPage = PRESENTATION_PAGE_STATUS;
    sContent = this->GetStatusHTML();
    sPageName = "Status";
  }

  else if(ToLower(pMessage->GetRequest()).compare("/presentation/config.html") == 0) {
    nPresentationPage = PRESENTATION_PAGE_INDEX;
    sContent = this->GetConfigHTML(pMessage);
    sPageName = "Configuration";
  }
*/

  // todo: add http cache fields to the response header
  
  // device icons
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/fuppes-icon-48x48.png") == 0) {
    nPresentationPage = PRESENTATION_BINARY_IMAGE;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "fuppes-icon-48x48.png");
    pResult->SetContentType("image/png");
    width = 50;
    height = 50;
  }
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/fuppes-icon-120x120.png") == 0) {
    nPresentationPage = PRESENTATION_BINARY_IMAGE;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "fuppes-icon-120x120.png");
    pResult->SetContentType("image/png");
    width = 120;
    height = 120;
  }
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/fuppes-icon-48x48.jpg") == 0) {
    nPresentationPage = PRESENTATION_BINARY_IMAGE;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "fuppes-icon-48x48.jpg");
    pResult->SetContentType("image/jpeg");
    width = 50;
    height = 50;
  }
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/fuppes-icon-120x120.jpg") == 0) {
    nPresentationPage = PRESENTATION_BINARY_IMAGE;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "fuppes-icon-120x120.jpg");
    pResult->SetContentType("image/jpeg");
    width = 120;
    height = 120;
  }
  
  // webinterface images
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/fuppes-logo.png") == 0) {
    nPresentationPage = PRESENTATION_BINARY_IMAGE;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "fuppes-logo.png");
    pResult->SetContentType("image/png");
  }
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/header-gradient.png") == 0) {
    nPresentationPage = PRESENTATION_BINARY_IMAGE;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "header-gradient.png");
    pResult->SetContentType("image/png");
  }  
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/header-gradient-small.png") == 0) {
    nPresentationPage = PRESENTATION_BINARY_IMAGE;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "header-gradient-small.png");
    pResult->SetContentType("image/png");
  }

  // mootools
  
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/mootools-core-1.3.2-yc.js") == 0) { // mootools-1.2.4-core-yc
    nPresentationPage = PRESENTATION_JAVASCRIPT;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "mootools-core-1.3.2-yc.js");
  }
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/mootools-more-1.3.2.1-yc.js") == 0) { // mootools-1.2.4-core-yc
    nPresentationPage = PRESENTATION_JAVASCRIPT;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "mootools-more-1.3.2.1-yc.js");
  }

  
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/fuppes.js") == 0) {
    nPresentationPage = PRESENTATION_JAVASCRIPT;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "fuppes.js");
  }
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/fuppes-browse.js") == 0) {
    nPresentationPage = PRESENTATION_JAVASCRIPT;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "fuppes-browse.js");
  }
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/fuppes-control.js") == 0) {
    nPresentationPage = PRESENTATION_JAVASCRIPT;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "fuppes-control.js");
  }
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/fuppes-config.js") == 0) {
    nPresentationPage = PRESENTATION_JAVASCRIPT;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "fuppes-config.js");
  }
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/fuppes-device.js") == 0) {
    nPresentationPage = PRESENTATION_JAVASCRIPT;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "fuppes-device.js");
  }
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/fuppes-playlist.js") == 0) {
    nPresentationPage = PRESENTATION_JAVASCRIPT;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "fuppes-playlist.js");
  }
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/fuppes-log.js") == 0) {
    nPresentationPage = PRESENTATION_JAVASCRIPT;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "fuppes-log.js");
  }
  else if(ToLower(pMessage->GetRequest()).compare("/presentation/fuppes-log-worker.js") == 0) {
    nPresentationPage = PRESENTATION_JAVASCRIPT;
		pResult->LoadContentFromFile(CSharedConfig::Shared()->resourcesDir() + "fuppes-log-worker.js");
  }

  isImage = false;
  if(nPresentationPage == PRESENTATION_BINARY_IMAGE) {
    pResult->SetMessageType(HTTP_MESSAGE_TYPE_200_OK);    
    isImage = true;
  }
	else if(nPresentationPage == PRESENTATION_STYLESHEET) {
    pResult->SetMessageType(HTTP_MESSAGE_TYPE_200_OK);    
    pResult->SetContentType("text/css");
  }
  else if(nPresentationPage == PRESENTATION_JAVASCRIPT) {
    pResult->SetMessageType(HTTP_MESSAGE_TYPE_200_OK);    
    pResult->SetContentType("text/javascript");
  }
  else if((nPresentationPage != PRESENTATION_BINARY_IMAGE) && (nPresentationPage != PRESENTATION_PAGE_UNKNOWN))
  {   
    stringstream sResult;
    
    sResult << GetPageHeader(nPresentationPage, sPageName);
    sResult << sContent;    
    sResult << GetPageFooter(nPresentationPage);
    
    pResult->SetMessageType(HTTP_MESSAGE_TYPE_200_OK);    
    pResult->SetContentType("text/html; charset=\"utf-8\""); // HTTP_CONTENT_TYPE_TEXT_HTML
    pResult->SetContent(sResult.str());
  }  
  else if(nPresentationPage == PRESENTATION_PAGE_UNKNOWN) {
    stringstream sResult;
    sResult << GetPageHeader(nPresentationPage, "404");
    sResult << "<p>ERROR 404 :: page not found</p>";
    sResult << GetPageFooter(nPresentationPage);

    pResult->SetMessageType(HTTP_MESSAGE_TYPE_404_NOT_FOUND); 
    pResult->SetContentType("text/html");
    pResult->SetContent(sResult.str());    
  }
}


std::string CPresentationHandler::GetPageHeader(PRESENTATION_PAGE /*p_nPresentationPage*/, std::string p_sPageName, std::string js /*= ""*/)
{
  std::stringstream sResult; 

  sResult << "<!doctype html>" << endl;  
  sResult << "<html>\n";
  sResult << "<head>";  
  sResult << "<title>" << CSharedConfig::Shared()->GetAppName() << " - " << CSharedConfig::Shared()->GetAppFullname() << " " << CSharedConfig::Shared()->GetAppVersion();
  sResult << " (" << CSharedConfig::Shared()->networkSettings->GetHostname() << ")";
  sResult << "</title>" << endl;

	sResult << "<meta charset=\"utf-8\" />" << endl; 
	sResult << "<link href=\"/presentation/style.css\" rel=\"stylesheet\" type=\"text/css\" media=\"screen\" />" << endl;

	sResult << "<script type=\"text/javascript\" src=\"/presentation/mootools-core-1.3.2-yc.js\"></script>" << endl;
  sResult << "<script type=\"text/javascript\" src=\"/presentation/mootools-more-1.3.2.1-yc.js\"></script>" << endl;
	sResult << "<script type=\"text/javascript\" src=\"/presentation/fuppes.js\"></script>" << endl;
	sResult << "<script type=\"text/javascript\" src=\"/presentation/fuppes-control.js\"></script>" << endl;
  sResult << "<script type=\"text/javascript\" src=\"/presentation/fuppes-browse.js\"></script>" << endl;
  if(!js.empty())
  	sResult << "<script type=\"text/javascript\" src=\"/presentation/" << js << "\"></script>" << endl;
  
  sResult << "</head>";
  // html header end
  
    
  sResult << "<body>";
  
  
  // site header
  sResult << "<div id=\"header\">" << endl;

  sResult << "<div id=\"logo\"></div>" << endl;

  sResult << "<div id=\"header-text\">" << endl <<
    "FUPPES - Free UPnP Entertainment Service<br />" << endl <<
    "<span>" <<
    "Version: " << CSharedConfig::Shared()->GetAppVersion() << " &bull; " <<
    "Host: "    << CSharedConfig::Shared()->networkSettings->GetHostname() << " &bull; " <<
    "Address: " << CSharedConfig::Shared()->networkSettings->GetIPv4Address() <<
    "</span>"   << endl <<
    "</div>" << endl;
  
  sResult << "</div>" << endl;
  // site header end


  // wrapper
  sResult << "<div id=\"wrapper\">" << endl;
  
  
  // js check
  sResult << "<div id=\"js-check\">" << endl;
  sResult << "<span>Please enable Java Script</span>" << endl;
  sResult << "</div>" << endl;  
  // js check end

  
  // menu
  sResult << "<div id=\"menu\">" << endl;
  sResult << "<div class=\"framehead\">Menu</div>" << endl;    


  sResult << "<ul>";
  sResult << "<li><a href=\"/index.html\">start</a></li>";

  /*
  std::list<PresentationPage*>::iterator  iter;
  for(iter = m_pages.begin(); iter != m_pages.end(); ++iter) {
    PresentationPage* page = *iter;
    if(page->alias().compare("index") == 0)
      continue;
    sResult << "<li><a href=\"/presentation/" << page->alias() << ".html\">" << page->title() << "</a></li>";
  }
*/
  sResult << "</ul>";



/*
  sResult << 
    "<ul>" <<
      "<li><a href=\"/presentation/options.html\">Options</a></li>" <<
      //"<li><a href=\"/presentation/config.html\">Configuration</a></li>" <<
    "</ul>";
  */
  
  sResult << "</div>" << endl;  
  // menu end
  

  sResult << "<div id=\"mainframe\">" << endl;
  sResult << "<div class=\"framehead\">" << p_sPageName << "</div>" << endl;
  
  sResult << "<div id=\"content\">" << endl;
  
  return sResult.str().c_str();
}

/*
std::string CPresentationHandler::getPageHeader(PresentationPage* page)
{ 
  return GetPageHeader(PRESENTATION_PAGE_UNKNOWN, page->title(), page->js());
}

std::string CPresentationHandler::GetPageFooter(PRESENTATION_PAGE /*p_nPresentationPage*/)
{
  std::stringstream sResult;
  
  sResult << "</div>" << endl; // #content
  sResult << "</div>" << endl; // #mainframe

  sResult << "<div class=\"clear\"></div>" << endl;
  
  sResult << "</div>" << endl; // wrapper
  
  sResult << "<div id=\"footer\">" << endl;
  sResult << "<span>copyright &copy; 2005-2011 Ulrich V&ouml;lkel</span>";
  sResult << "</div>" << endl; // footer

  
  sResult << "</body>";
  sResult << "</html>";
  
  return sResult.str().c_str();
}

std::string CPresentationHandler::getPageFooter(PresentationPage* page)
{
  return GetPageFooter(PRESENTATION_PAGE_UNKNOWN);
}
*/