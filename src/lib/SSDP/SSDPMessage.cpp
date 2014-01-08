/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            SSDPMessage.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005 - 2007 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
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

#include "SSDPMessage.h"
#include "../Common/Common.h"
#include "../Common/RegEx.h"
//#include "../SharedLog.h"
#include <sstream>
#include <iostream>





using namespace std;

//const std::string LOGNAME = "SSDPMessage";

CSSDPMessage::CSSDPMessage()
{
  m_nMessageType = SSDP_MESSAGE_TYPE_UNKNOWN;
  m_nMSearchST = M_SEARCH_ST_UNSUPPORTED;
  m_nMX = -1;

	memset(&m_LocalEp, '\0', sizeof(struct sockaddr_in));
	memset(&m_RemoteEp, '\0', sizeof(struct sockaddr_in));
}

CSSDPMessage::~CSSDPMessage()
{
}

bool CSSDPMessage::SetMessage(std::string p_sMessage)
{
	m_sMessage = p_sMessage;
  
  /* some sample messages */
  
  /* notify-alive
  
  NOTIFY * HTTP/1.1
  HOST: 239.255.255.250:1900
  CACHE-CONTROL: max-age=1800
  LOCATION: http://192.168.0.22:80/description.xml
  NT: upnp:rootdevice
  NTS: ssdp:alive
  SERVER: ThreadX/1.0 UPnP/1.0 Product/1.0
  USN: uuid:00000000-0000-0000-0000-08000E200000::upnp:rootdevice*/

  /* m-search response
  
  HTTP/1.1 200 OK
  CACHE-CONTROL: max-age=1800
  EXT:
  LOCATION: http://192.168.0.2:47224/
  SERVER: Free UPnP Entertainment Service/0.1.1 UPnP/1.0 libfuppes/0.1
  ST: urn:schemas-upnp-org:service:ContentDirectory:1
  NTS: ssdp:alive
  USN: uuid:45645678-aabb-0000-ccdd-1234eeff0000::urn:schemas-upnp-org:service:ContentDirectory:1
  Content-Length: 0 */
  
  /* notify-byebye
  
  NOTIFY * HTTP/1.1
  HOST: 239.255.255.250:1900
  CACHE-CONTROL: max-age=1800
  CONTENT-LENGTH: 0
  LOCATION: http://192.168.0.22:80/description.xml
  NT: urn:schemas-upnp-org:service:HtmlPageHandler:1
  NTS: ssdp:byebye
  USN: uuid:00000000-0000-0000-0000-08000E200000::urn:schemas-upnp-org:service:HtmlPageHandler:1
  */
  
  /* m-search
  
  M-SEARCH * HTTP/1.1
  MX: 10
  ST: urn:schemas-upnp-org:service:ContentDirectory:1
  HOST: 239.255.255.250:1900
  MAN: "ssdp:discover"
  Content-Length: 0 
  */
  
  /* let's find out the message type */  
  m_nMessageType = SSDP_MESSAGE_TYPE_UNKNOWN;
  RegEx rxMSearch("M-SEARCH +\\* +HTTP/1\\.[1|0]", PCRE_CASELESS);
  if(rxMSearch.Search(m_sMessage.c_str()))
  {
    m_nMessageType = SSDP_MESSAGE_TYPE_M_SEARCH;
  }
  else
  {
    /* check if it's a notify message */
    RegEx rxNotify("NOTIFY", PCRE_CASELESS);
    if(rxNotify.Search(m_sMessage.c_str()))
    {
      /* found notify. check whether it's alive or byebye */
      RegEx rxNTS("NTS: *ssdp:(\\w+)", PCRE_CASELESS);
      if(rxNTS.Search(m_sMessage.c_str()))
      {        
        if(ToLower(rxNTS.Match(1)).compare("alive") == 0)        
          m_nMessageType = SSDP_MESSAGE_TYPE_NOTIFY_ALIVE;        
        else if(ToLower(rxNTS.Match(1)).compare("byebye") == 0)        
          m_nMessageType = SSDP_MESSAGE_TYPE_NOTIFY_BYEBYE;        
      }
    }  
    /* otherwise check if we have a m-search response */
    else
    {
      /* m-search response */
      RegEx rxHTTP("HTTP */* *1.\\d +(\\d+) +(\\w+)", PCRE_CASELESS);
      if(rxHTTP.Search(m_sMessage.c_str()))              
        m_nMessageType = SSDP_MESSAGE_TYPE_M_SEARCH_RESPONSE;
      else      
        return false;
    }    
  }
  /* end find out message type */
  
  
  if(m_nMessageType != SSDP_MESSAGE_TYPE_UNKNOWN)
  {
    
    if(m_nMessageType != SSDP_MESSAGE_TYPE_M_SEARCH)
    {
      /* Location */
      RegEx rxLocation("LOCATION: *(http://.+)", PCRE_CASELESS);
      if(rxLocation.Search(m_sMessage.c_str()))
        m_sLocation = rxLocation.Match(1);
      
      /* Server */
      RegEx rxServer("SERVER: *(.*)", PCRE_CASELESS);
      if(rxServer.Search(m_sMessage.c_str()))      
        m_sServer = rxServer.Match(1);
      
      /* USN */
      RegEx rxUSN("USN: *(.*)\r\n", PCRE_CASELESS);
      if(rxUSN.Search(m_sMessage.c_str()))
      {
        m_sUSN = rxUSN.Match(1);        
        if(m_sUSN.find("::") != std::string::npos) {
          m_sUSN = m_sUSN.substr(0, m_sUSN.find("::"));
        }        
        
        RegEx rxUUID("uuid:(.*)", PCRE_CASELESS);        
        if(rxUUID.Search(m_sUSN.c_str()))
          m_sUUID = rxUUID.Match(1);
      }
      
    } /* if(m_nMessageType != SSDP_MESSAGE_TYPE_M_SEARCH)  */
    else if(m_nMessageType == SSDP_MESSAGE_TYPE_M_SEARCH)
    {
      /* MX (required) */
      RegEx rxMX("MX: *([0-9]+)", PCRE_CASELESS);
      if(rxMX.Search(m_sMessage))      
        m_nMX = atoi(rxMX.Match(1).c_str());
      else
        return false;
      
      /* ST (required) */
      m_nMSearchST = M_SEARCH_ST_UNSUPPORTED;
      RegEx rxST("\r\nST: *(.*)\r\n", PCRE_CASELESS);
      if(rxST.Search(m_sMessage))
      {
        m_sST = ToLower(rxST.Match(1));        

        if(m_sST.compare("ssdp:all") == 0)
          m_nMSearchST = M_SEARCH_ST_ALL;        
        else if(m_sST.compare("upnp:rootdevice") == 0)
          m_nMSearchST = M_SEARCH_ST_ROOT;
        else if(m_sST.substr(0, 5).compare("uuid:") == 0)
          m_nMSearchST = M_SEARCH_ST_UUID;        
        else if(m_sST.compare("urn:schemas-upnp-org:device:mediaserver:1") == 0)
          m_nMSearchST = M_SEARCH_ST_DEVICE_MEDIA_SERVER;        
        else if(m_sST.compare("urn:schemas-upnp-org:service:contentdirectory:1") == 0)
          m_nMSearchST = M_SEARCH_ST_SERVICE_CONTENT_DIRECTORY;
        else if(m_sST.compare("urn:schemas-upnp-org:service:connectionmanager:1") == 0)
          m_nMSearchST = M_SEARCH_ST_SERVICE_CONNECTION_MANAGER;
        else if(m_sST.compare("urn:schemas-upnp-org:service:avtransport:1") == 0)
		  m_nMSearchST = M_SEARCH_ST_SERVICE_AV_TRANSPORT;
        else if(m_sST.compare("urn:microsoft.com:service:x_ms_mediareceiverregistrar:1") == 0)
		  m_nMSearchST = M_SEARCH_ST_SERVICE_X_MS_MEDIA_RECEIVER_REGISTRAR;
        else
          m_nMSearchST = M_SEARCH_ST_UNSUPPORTED;        
      }

      /* MAN (required) */
      RegEx rxMAN("MAN: *(.*)", PCRE_CASELESS);
      if(rxMAN.Search(m_sMessage))      
        m_sMAN = rxMAN.Match(1);
      else
        return false;

    } /* if(m_nMessageType == SSDP_MESSAGE_TYPE_M_SEARCH) */
    
  } /* if(m_nMessageType != SSDP_MESSAGE_TYPE_UNKNOWN) */

  return true;
}

void CSSDPMessage::Assign(CSSDPMessage* pSSDPMessage)
{
  //cout << "ASSIGN: " << m_sMessage << endl;
  pSSDPMessage->SetMessage(m_sMessage);
  pSSDPMessage->SetLocalEndPoint(this->GetLocalEndPoint());
  pSSDPMessage->SetRemoteEndPoint(this->GetRemoteEndPoint());  
}

void CSSDPMessage::SetLocalEndPoint(sockaddr_in p_EndPoint)
{	
	m_LocalEp.sin_family	= p_EndPoint.sin_family;
	m_LocalEp.sin_addr 		= p_EndPoint.sin_addr;
	m_LocalEp.sin_port		= p_EndPoint.sin_port;
	memset(&m_LocalEp.sin_zero, 0, sizeof(m_LocalEp.sin_zero));	
}

void CSSDPMessage::SetRemoteEndPoint(sockaddr_in p_EndPoint)
{
	m_RemoteEp.sin_family	= p_EndPoint.sin_family;
	m_RemoteEp.sin_addr		= p_EndPoint.sin_addr;
	m_RemoteEp.sin_port		= p_EndPoint.sin_port;
	memset(&m_RemoteEp.sin_zero, 0, sizeof(m_RemoteEp.sin_zero));	
}
