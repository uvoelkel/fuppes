/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            SSDPMessage.h
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005, 2006 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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
 
#ifndef _SSDPMESSAGE_H
#define _SSDPMESSAGE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef WIN32
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#else
#include <winsock2.h>
#endif

#include <string>

typedef enum tagSSDP_MESSAGE_TYPE
{
  SSDP_MESSAGE_TYPE_UNKNOWN,
  SSDP_MESSAGE_TYPE_M_SEARCH,
  SSDP_MESSAGE_TYPE_M_SEARCH_RESPONSE,
  SSDP_MESSAGE_TYPE_NOTIFY_ALIVE,
  SSDP_MESSAGE_TYPE_NOTIFY_BYEBYE
}SSDP_MESSAGE_TYPE;

typedef enum tagM_SEARCH_ST
{
  M_SEARCH_ST_ALL,
  M_SEARCH_ST_ROOT,
  M_SEARCH_ST_UUID,
  M_SEARCH_ST_DEVICE_MEDIA_SERVER,  
  M_SEARCH_ST_SERVICE_CONTENT_DIRECTORY,
  M_SEARCH_ST_SERVICE_CONNECTION_MANAGER,
  M_SEARCH_ST_SERVICE_AV_TRANSPORT,
  M_SEARCH_ST_SERVICE_X_MS_MEDIA_RECEIVER_REGISTRAR,
  M_SEARCH_ST_UNSUPPORTED  
}M_SEARCH_ST;

class CSSDPMessage
{
  public:
    CSSDPMessage();    
  	~CSSDPMessage();
	  void Assign(CSSDPMessage* pSSDPMessage);

		std::string GetContent()         { return m_sContent;                     }
    std::string	GetMessage()         { return m_sMessage;                     }
    std::string GetHeader()          { return m_sHeader;                      }
		
    bool SetMessage(std::string p_sMessage);

    std::string GetLocation() { return m_sLocation; }
    std::string GetUUID()     { return m_sUUID;     }    
    //~ std::string GetDeviceID();
    SSDP_MESSAGE_TYPE GetMessageType() { return m_nMessageType; }     
    int GetMX() { return m_nMX; }
    M_SEARCH_ST GetMSearchST() { return m_nMSearchST; }
    std::string GetSTAsString() { return m_sST; }
    
		sockaddr_in GetLocalEndPoint() { return m_LocalEp; }
    sockaddr_in GetRemoteEndPoint() { return m_RemoteEp; }

	  void        SetLocalEndPoint(sockaddr_in);
	  void        SetRemoteEndPoint(sockaddr_in);		
		
  private:
    std::string m_sLocation;
    std::string m_sServer;
    std::string m_sST;
    std::string m_sNTS;
    std::string m_sUSN;
    std::string m_sUUID;
    std::string m_sMAN;  
    SSDP_MESSAGE_TYPE m_nMessageType;
    int m_nMX;
    M_SEARCH_ST m_nMSearchST;

		std::string m_sContent;
    std::string m_sHeader;
    std::string m_sMessage;	
		sockaddr_in m_LocalEp;
		sockaddr_in m_RemoteEp;
};

#endif // _SSDPMESSAGE_H
