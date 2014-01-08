/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            MSearchSession.cpp
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2011 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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
 
#include "MSearchSession.h"
#include "NotifyMsgFactory.h"
#include "../SharedLog.h"

#include <iostream>
#include <sstream>
#include <time.h>
#include <stdlib.h>

#define MX_MIN 1
#define MX_MAX 120

using namespace std;

CMSearchSession::CMSearchSession(std::string p_sIPAddress, IMSearchSession* pReceiveHandler, Ssdp::NotifyMsgFactory* pNotifyMsgFactory)
  :m_Timer(this)
{
  ASSERT(NULL != pReceiveHandler);
  ASSERT(NULL != pNotifyMsgFactory);
  
  m_sIPAddress        = p_sIPAddress;
  m_pEventHandler     = pReceiveHandler;
  m_pNotifyMsgFactory = pNotifyMsgFactory;
  
  m_Timer.SetInterval(30);
  m_UdpSocket.SetupSocket(false, m_sIPAddress);	
  m_UdpSocket.SetTTL(4);
}

CMSearchSession::~CMSearchSession()
{
  m_UdpSocket.close();
  m_UdpSocket.TeardownSocket();
}

void CMSearchSession::OnUDPSocketStarted()
{
}

void CMSearchSession::OnUDPSocketReceive(CSSDPMessage* pSSDPMessage)
{
  if(m_pEventHandler != NULL)
    m_pEventHandler->OnSessionReceive(pSSDPMessage);
}

void CMSearchSession::OnTimer()
{
  Stop();
  if(m_pEventHandler != NULL)
  {
    m_pEventHandler->OnSessionTimeOut(this);
  }  
}

void CMSearchSession::Start()
{
	begin_receive_unicast();
	fuppesSleep(200);
	send_multicast(m_pNotifyMsgFactory->msearch());
  m_Timer.start();
}

void CMSearchSession::Stop()
{
  m_Timer.stop();
	end_receive_unicast();
}

void CMSearchSession::send_multicast(std::string a_message)
{
	/* Send message twice */
  m_UdpSocket.SendMulticast(a_message);
	fuppesSleep(200);
	m_UdpSocket.SendMulticast(a_message);	
}

void CMSearchSession::send_unicast(std::string)
{
}

void CMSearchSession::begin_receive_unicast()
{	
	/* Start receiving messages */
  m_UdpSocket.SetReceiveHandler(this);
	m_UdpSocket.BeginReceive();
}

void CMSearchSession::end_receive_unicast()
{
  /* End receiving messages */
  m_UdpSocket.EndReceive();
}

sockaddr_in CMSearchSession:: GetLocalEndPoint()
{
	return m_UdpSocket.GetLocalEndPoint();
}

CHandleMSearchSession::CHandleMSearchSession(CSSDPMessage* pSSDPMessage, std::string uuid, std::string p_sIPAddress, std::string p_sHTTPServerURL, Ssdp::NotifyMsgFactory* pNotifyMsgFactory)
:Threading::Thread("m-search session")
{
    m_uuid = uuid;
  m_sIPAddress        = p_sIPAddress;
  m_sHTTPServerURL    = p_sHTTPServerURL;
  m_pSSDPMessage      = new CSSDPMessage();
  pSSDPMessage->Assign(m_pSSDPMessage);
  m_pNotifyMsgFactory = pNotifyMsgFactory; //new CNotifyMsgFactory(m_sHTTPServerURL);
}
   
CHandleMSearchSession::~CHandleMSearchSession()
{
  delete m_pSSDPMessage;
  //delete m_pNotifyMsgFactory;
}

void CHandleMSearchSession::run()
{
  CHandleMSearchSession* pSession = this; 
  if(pSession->GetSSDPMessage()->GetMSearchST() == M_SEARCH_ST_UNSUPPORTED) {
    return;  
  }
   
  CUDPSocket Sock;
  Sock.SetupSocket(false, pSession->m_sIPAddress);


  // calculate mx delay
  int mx = pSession->GetSSDPMessage()->GetMX();
  if(mx < MX_MIN)
    mx = MX_MIN;
  else if(mx > MX_MAX)
    mx = MX_MAX;

  // get a random value between 0 an MX
  srand(time(NULL));
  int rnd = (rand() % (mx + 1));
  
  int ms = rnd * 1000;
  
  //std::cout << "MX: " << mx << " :: " << pSession->GetSSDPMessage()->GetMSearchST() << " :: " << rnd << " = " << ms << std::endl;

  // if a search results in multiple responses spread them with
  // random intervals through the period from 0 to MX
  // todo: actually use random intervals
  if(pSession->GetSSDPMessage()->GetMSearchST() == M_SEARCH_ST_ALL)
    ms /= 7;
  
	std::string sUUID = "";
	
  if(pSession->GetSSDPMessage()->GetMSearchST() == M_SEARCH_ST_ALL) {
    fuppesSleep(ms);
    Sock.SendUnicast(pSession->m_pNotifyMsgFactory->GetMSearchResponse(Ssdp::MESSAGE_TYPE_USN), pSession->GetSSDPMessage()->GetRemoteEndPoint());
		fuppesSleep(ms);
    Sock.SendUnicast(pSession->m_pNotifyMsgFactory->GetMSearchResponse(Ssdp::MESSAGE_TYPE_ROOT_DEVICE), pSession->GetSSDPMessage()->GetRemoteEndPoint());
    fuppesSleep(ms);
    Sock.SendUnicast(pSession->m_pNotifyMsgFactory->GetMSearchResponse(Ssdp::MESSAGE_TYPE_MEDIA_SERVER), pSession->GetSSDPMessage()->GetRemoteEndPoint());
		fuppesSleep(ms);
    Sock.SendUnicast(pSession->m_pNotifyMsgFactory->GetMSearchResponse(Ssdp::MESSAGE_TYPE_CONTENT_DIRECTORY), pSession->GetSSDPMessage()->GetRemoteEndPoint());
		fuppesSleep(ms);
    Sock.SendUnicast(pSession->m_pNotifyMsgFactory->GetMSearchResponse(Ssdp::MESSAGE_TYPE_CONNECTION_MANAGER), pSession->GetSSDPMessage()->GetRemoteEndPoint());
    fuppesSleep(ms);
    Sock.SendUnicast(pSession->m_pNotifyMsgFactory->GetMSearchResponse(Ssdp::MESSAGE_TYPE_X_MS_MEDIA_RECEIVER_REGISTRAR), pSession->GetSSDPMessage()->GetRemoteEndPoint());
  }
  else {
          
      msleep(ms);
		switch(pSession->GetSSDPMessage()->GetMSearchST()) {
      case M_SEARCH_ST_ROOT:          
        Sock.SendUnicast(pSession->m_pNotifyMsgFactory->GetMSearchResponse(Ssdp::MESSAGE_TYPE_ROOT_DEVICE), pSession->GetSSDPMessage()->GetRemoteEndPoint());          
        break;
      case M_SEARCH_ST_DEVICE_MEDIA_SERVER:
        Sock.SendUnicast(pSession->m_pNotifyMsgFactory->GetMSearchResponse(Ssdp::MESSAGE_TYPE_MEDIA_SERVER), pSession->GetSSDPMessage()->GetRemoteEndPoint());          
        break;
      case M_SEARCH_ST_SERVICE_CONTENT_DIRECTORY:
        Sock.SendUnicast(pSession->m_pNotifyMsgFactory->GetMSearchResponse(Ssdp::MESSAGE_TYPE_CONTENT_DIRECTORY), pSession->GetSSDPMessage()->GetRemoteEndPoint());          
        break;
      case M_SEARCH_ST_SERVICE_CONNECTION_MANAGER:
        Sock.SendUnicast(pSession->m_pNotifyMsgFactory->GetMSearchResponse(Ssdp::MESSAGE_TYPE_CONNECTION_MANAGER), pSession->GetSSDPMessage()->GetRemoteEndPoint());             
        break;
      /*case Ssdp::MESSAGE_TYPE_X_MS_MEDIA_RECEIVER_REGISTRAR:
        Sock.SendUnicast(pSession->m_pNotifyMsgFactory->GetMSearchResponse(Ssdp::MESSAGE_TYPE_X_MS_MEDIA_RECEIVER_REGISTRAR), pSession->GetSSDPMessage()->GetRemoteEndPoint());             
        break;*/
        
      case M_SEARCH_ST_UUID:
        sUUID = pSession->GetSSDPMessage()->GetSTAsString().substr(5);
        if(sUUID.compare(ToLower(m_uuid)) == 0) {
          Sock.SendUnicast(pSession->m_pNotifyMsgFactory->GetMSearchResponse(Ssdp::MESSAGE_TYPE_USN), pSession->GetSSDPMessage()->GetRemoteEndPoint());            
        }
        break;
			default:
			  break;
     }
  }
    
  Sock.TeardownSocket();
}
