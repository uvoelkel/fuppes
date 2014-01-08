/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            SSDPCtrl.cpp
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
 
#include "UDPSocket.h"
#include "SSDPMessage.h"

#include "SSDPCtrl.h"
#include "../SharedLog.h"
#include <sstream>
#include <string.h>
#include "../Thread/ThreadPool.h"
#include "../Thread/Mutex.h"
#include "../Common/Exception.h"

using namespace std;
using namespace fuppes;
 
CSSDPCtrl::CSSDPCtrl(std::string p_sIPAddress, std::string p_sHTTPServerURL, ISSDPCtrl* pHandler, std::string serverSignature, std::string uuid):
m_notifyMsgFactory(p_sHTTPServerURL, serverSignature, uuid)
{
    m_uuid = uuid;
  m_sIPAddress   = p_sIPAddress;
  m_sHTTPServerURL = p_sHTTPServerURL;
  m_pReceiveHandler = pHandler;
  
  m_isStarted = false;  
  memset(&m_LastMulticastEp, 0, sizeof(sockaddr_in));  
}

CSSDPCtrl::~CSSDPCtrl()
{
  //delete m_pNotifyMsgFactory;
}

bool CSSDPCtrl::Start()
{	
	try {
    m_Listener.name("udp multicast listener");
    m_Listener.SetupSocket(true, m_sIPAddress);
    m_Listener.SetTTL(4);
	  m_Listener.SetReceiveHandler(this);
		m_Listener.SetStartedHander(this);
	  m_Listener.BeginReceive();	
  }
  catch(fuppes::Exception& ex) {
      m_pReceiveHandler->onSSDPCtrlError(this, "error starting ssdp listener on " + m_sIPAddress + " :: " + ex.what());
    //throw;
      return false;
  }
  
  CSharedLog::Log(L_EXT, __FILE__, __LINE__, "SSDPController started");
  return true;
}

void CSSDPCtrl::Stop()
{	
  CleanupSessions(true);
	m_Listener.EndReceive();  
  CSharedLog::Log(L_EXT, __FILE__, __LINE__, "SSDPController stopped");
}

CUDPSocket* CSSDPCtrl::get_socket()
{
	return &m_Listener;
}

void CSSDPCtrl::CleanupSessions(bool clearRunning /*= false*/)
{
  CSharedLog::Log(L_DBG, __FILE__, __LINE__, "CleanupSessions");
  Threading::MutexLocker locker(&m_SessionTimedOutMutex);
      
  /*if(m_HandleMSearchThreadList.size() > 0)
  {    
    for(m_HandleMSearchThreadListIterator = m_HandleMSearchThreadList.begin();
        m_HandleMSearchThreadListIterator != m_HandleMSearchThreadList.end(); )
    {     
      if(m_HandleMSearchThreadList.size() == 0)
        break;
      
      CHandleMSearchSession* pMSession = *m_HandleMSearchThreadListIterator;
      if(pMSession->finished())
      {
        std::list<CHandleMSearchSession*>::iterator tmpIt = m_HandleMSearchThreadListIterator;
        ++tmpIt;        
        m_HandleMSearchThreadList.erase(m_HandleMSearchThreadListIterator);
        m_HandleMSearchThreadListIterator = tmpIt;
        delete pMSession;        
      }
      else
      {
        ++m_HandleMSearchThreadListIterator;
      }  
    }
  }*/


  if(clearRunning && m_RunningSessionList.size() > 0)
  {  
    for(m_RunningSessionListIterator = m_RunningSessionList.begin();
        m_RunningSessionListIterator != m_RunningSessionList.end(); )
    {
      if(m_RunningSessionList.size() == 0)
        break;
                  
      std::list<CMSearchSession*>::iterator tmpIt = m_RunningSessionListIterator;
      ++tmpIt;      
      CMSearchSession* pSession = *m_RunningSessionListIterator;   
      m_RunningSessionList.erase(m_RunningSessionListIterator);
      m_RunningSessionListIterator = tmpIt;
      delete pSession;
    }  
  }
  
  if(m_SessionList.size() > 0)
  {  
    for(m_SessionListIterator = m_SessionList.begin();
        m_SessionListIterator != m_SessionList.end(); )
    {
      if(m_SessionList.size() == 0)
        break;
                  
      std::list<CMSearchSession*>::iterator tmpIt = m_SessionListIterator;
      ++tmpIt;      
      CMSearchSession* pSession = *m_SessionListIterator;   
      m_SessionList.erase(m_SessionListIterator);
      m_SessionListIterator = tmpIt;
      delete pSession;
    }  
  }
}

void CSSDPCtrl::send_msearch()
{
//#warning todo: store sessions and cleanup on shutdown

  CMSearchSession* pSession = new CMSearchSession(m_sIPAddress, this, &m_notifyMsgFactory);
  m_LastMulticastEp = pSession->GetLocalEndPoint();
  pSession->Start();

  {
    Threading::MutexLocker locker(&m_SessionTimedOutMutex);
    m_RunningSessionList.push_back(pSession);
  }
  
  CleanupSessions();
}

void CSSDPCtrl::send_alive()
{
  CleanupSessions();
  
  CUDPSocket Sock;
	Sock.SetupSocket(false, m_sIPAddress);
	Sock.SetTTL(4);
	
	//m_LastMulticastEp = Sock.GetLocalEndPoint();
	
	Sock.SendMulticast(m_notifyMsgFactory.notify_alive(Ssdp::MESSAGE_TYPE_USN));
  fuppesSleep(50);
	Sock.SendMulticast(m_notifyMsgFactory.notify_alive(Ssdp::MESSAGE_TYPE_ROOT_DEVICE));
  fuppesSleep(50);
	Sock.SendMulticast(m_notifyMsgFactory.notify_alive(Ssdp::MESSAGE_TYPE_MEDIA_SERVER));
  fuppesSleep(50);
	Sock.SendMulticast(m_notifyMsgFactory.notify_alive(Ssdp::MESSAGE_TYPE_CONTENT_DIRECTORY));
  fuppesSleep(50);
	Sock.SendMulticast(m_notifyMsgFactory.notify_alive(Ssdp::MESSAGE_TYPE_CONNECTION_MANAGER));
  fuppesSleep(50);
  Sock.SendMulticast(m_notifyMsgFactory.notify_alive(Ssdp::MESSAGE_TYPE_X_MS_MEDIA_RECEIVER_REGISTRAR));
		
	/*Sock.SendMulticast(m_pNotifyMsgFactory->notify_alive(MESSAGE_TYPE_ROOT_DEVICE));	
	Sock.SendMulticast(m_pNotifyMsgFactory->notify_alive(MESSAGE_TYPE_ROOT_DEVICE));
	fuppesSleep(200);
	
	Sock.SendMulticast(m_pNotifyMsgFactory->notify_alive(MESSAGE_TYPE_CONNECTION_MANAGER));
	Sock.SendMulticast(m_pNotifyMsgFactory->notify_alive(MESSAGE_TYPE_CONNECTION_MANAGER));
	fuppesSleep(200);
	
	Sock.SendMulticast(m_pNotifyMsgFactory->notify_alive(MESSAGE_TYPE_CONTENT_DIRECTORY));
	Sock.SendMulticast(m_pNotifyMsgFactory->notify_alive(MESSAGE_TYPE_CONTENT_DIRECTORY));
	fuppesSleep(200);
	
	Sock.SendMulticast(m_pNotifyMsgFactory->notify_alive(MESSAGE_TYPE_MEDIA_SERVER));
	Sock.SendMulticast(m_pNotifyMsgFactory->notify_alive(MESSAGE_TYPE_MEDIA_SERVER));
	fuppesSleep(200);
	
	Sock.SendMulticast(m_pNotifyMsgFactory->notify_alive(MESSAGE_TYPE_USN));
	Sock.SendMulticast(m_pNotifyMsgFactory->notify_alive(MESSAGE_TYPE_USN));*/
	
	Sock.TeardownSocket();
}

void CSSDPCtrl::send_byebye()
{
	CUDPSocket Sock;
	Sock.SetupSocket(false, m_sIPAddress);
	Sock.SetTTL(4);
  //m_LastMulticastEp = Sock.GetLocalEndPoint();	
	
	Sock.SendMulticast(m_notifyMsgFactory.notify_bye_bye(Ssdp::MESSAGE_TYPE_ROOT_DEVICE));
  fuppesSleep(50);
	/*Sock.SendMulticast(m_pNotifyMsgFactory->notify_bye_bye(MESSAGE_TYPE_ROOT_DEVICE));
	fuppesSleep(200);*/

  Sock.SendMulticast(m_notifyMsgFactory.notify_bye_bye(Ssdp::MESSAGE_TYPE_X_MS_MEDIA_RECEIVER_REGISTRAR));
	fuppesSleep(50);
  
	Sock.SendMulticast(m_notifyMsgFactory.notify_bye_bye(Ssdp::MESSAGE_TYPE_CONNECTION_MANAGER));
  fuppesSleep(50);
	/*Sock.SendMulticast(m_pNotifyMsgFactory->notify_bye_bye(MESSAGE_TYPE_CONNECTION_MANAGER));
	fuppesSleep(200);*/
	
	Sock.SendMulticast(m_notifyMsgFactory.notify_bye_bye(Ssdp::MESSAGE_TYPE_CONTENT_DIRECTORY));
  fuppesSleep(50);
	/*Sock.SendMulticast(m_pNotifyMsgFactory->notify_bye_bye(MESSAGE_TYPE_CONTENT_DIRECTORY));
	fuppesSleep(200);*/
	
	Sock.SendMulticast(m_notifyMsgFactory.notify_bye_bye(Ssdp::MESSAGE_TYPE_MEDIA_SERVER));
  fuppesSleep(50);
	/*Sock.SendMulticast(m_pNotifyMsgFactory->notify_bye_bye(MESSAGE_TYPE_MEDIA_SERVER));
	fuppesSleep(200);*/
	
	Sock.SendMulticast(m_notifyMsgFactory.notify_bye_bye(Ssdp::MESSAGE_TYPE_USN));
	//Sock.SendMulticast(m_pNotifyMsgFactory->notify_bye_bye(MESSAGE_TYPE_USN));
	
	Sock.TeardownSocket();
}

void CSSDPCtrl::OnUDPSocketStarted()
{
	m_isStarted = true;
	m_pReceiveHandler->onSSDPCtrlReady(this);
}

void CSSDPCtrl::OnUDPSocketReceive(CSSDPMessage* pSSDPMessage)
{
  Threading::MutexLocker locker(&m_SessionReceiveMutex);
	
  stringstream sLog;
  sLog << "OnUDPSocketReceive() :: " << inet_ntoa(pSSDPMessage->GetRemoteEndPoint().sin_addr) << ":" << ntohs(pSSDPMessage->GetRemoteEndPoint().sin_port) << endl;
  //sLog << inet_ntoa(m_LastMulticastEp.sin_addr) << ":" << ntohs(m_LastMulticastEp.sin_port);
 
  CSharedLog::Log(L_DBG, __FILE__, __LINE__, sLog.str().c_str());
  //log(Logging::ssdp, Logging::debug) << sLog.str();

  if((m_LastMulticastEp.sin_addr.s_addr != pSSDPMessage->GetRemoteEndPoint().sin_addr.s_addr) ||
    (m_LastMulticastEp.sin_port != pSSDPMessage->GetRemoteEndPoint().sin_port))
  {	
    switch(pSSDPMessage->GetMessageType())
    {
      case SSDP_MESSAGE_TYPE_M_SEARCH:
        HandleMSearch(pSSDPMessage);
        break;
      
      default:        
        if(m_pReceiveHandler != NULL) {
          m_pReceiveHandler->onSSDPCtrlReceiveMsg(pSSDPMessage);
        }
        break;
    }
  }
}

void CSSDPCtrl::OnSessionReceive(CSSDPMessage* pMessage)
{
  /* lock receive mutex */
  Threading::MutexLocker locker(&m_SessionReceiveMutex);
  
  /* logging */
  //CSharedLog::Log(L_DBG, __FILE__, __LINE__, pMessage->GetMessage().c_str());
	//log(Logging::Log::ssdp, Logging::Log::debug) << pMessage->GetMessage();
  
  /* pass message to the main fuppes instance */
  if(NULL != m_pReceiveHandler)
      m_pReceiveHandler->onSSDPCtrlReceiveMsg(pMessage);
}

void CSSDPCtrl::OnSessionTimeOut(CMSearchSession* pSender)
{
  CleanupSessions();
  
  /* lock timeout mutex */
  Threading::MutexLocker locker(&m_SessionTimedOutMutex);

  // remove from running session list ...
  for(m_RunningSessionListIterator = m_RunningSessionList.begin();
      m_RunningSessionListIterator != m_RunningSessionList.end(); 
      m_RunningSessionListIterator++) {
          
     CMSearchSession* tmp = *m_RunningSessionListIterator;
     if(tmp != pSender)
      continue;
          
     m_RunningSessionList.erase(m_RunningSessionListIterator);
     break;
  }

  // ... and append to timed out session list
  m_SessionList.push_back(pSender);
}


void CSSDPCtrl::HandleMSearch(CSSDPMessage* pSSDPMessage)
{
  {
    Threading::MutexLocker locker(&m_SessionTimedOutMutex);

    //stringstream sLog;
    //sLog << "received m-search from: \"" << inet_ntoa(pSSDPMessage->GetRemoteEndPoint().sin_addr) << ":" << ntohs(pSSDPMessage->GetRemoteEndPoint().sin_port) << "\"";
    //log(Logging::Log::ssdp, Logging::Log::extended) << sLog.str() << Logging::Log::debug << pSSDPMessage->GetMessage();

    
    //cout << pSSDPMessage->GetMSearchST() << " - " << M_SEARCH_ST_UNSUPPORTED << endl;
    
    if(pSSDPMessage->GetMSearchST() != M_SEARCH_ST_UNSUPPORTED) {
      CHandleMSearchSession* pHandleMSearch = new CHandleMSearchSession(pSSDPMessage, m_uuid, m_sIPAddress, m_sHTTPServerURL, &m_notifyMsgFactory);
      pHandleMSearch->start();
      Threading::ThreadPool::deleteLater(pHandleMSearch);    
    }
  }
  
  CleanupSessions();
}
