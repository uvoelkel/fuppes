/***************************************************************************
 *            SSDPCtrl.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2008 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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
 
#ifndef _SSDPCTRL_H
#define _SSDPCTRL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Common/Common.h"
#include "../Thread/Thread.h"

#ifndef WIN32
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif

#include <string>
#include <list>

#include "MSearchSession.h"
#include "NotifyMsgFactory.h"

class CSSDPCtrl;
class CSSDPMessage;

class ISSDPCtrl
{
    public:
        virtual ~ISSDPCtrl() {};

        virtual void onSSDPCtrlReady(CSSDPCtrl* ssdpCtrl) = 0;
        virtual void onSSDPCtrlError(CSSDPCtrl* ssdpCtrl, std::string error) = 0;

        virtual void onSSDPCtrlReceiveMsg(CSSDPMessage*) = 0;
};

class CUDPSocket;

class CSSDPCtrl: public IUDPSocket, IMSearchSession
{
  public:
		CSSDPCtrl(std::string p_sIPAddress, std::string p_sHTTPServerURL, ISSDPCtrl* pHandler, std::string serverSignature, std::string uuid);
		virtual ~CSSDPCtrl();

		bool Start();
    void Stop();

		CUDPSocket* get_socket();

		void send_msearch();
	  void send_alive();
	  void send_byebye();

		bool isStarted() { return m_isStarted; }
		void OnUDPSocketStarted();
	  void OnUDPSocketReceive(CSSDPMessage* pSSDPMessage);
   	void OnSessionReceive(CSSDPMessage* pMessage);
    void OnSessionTimeOut(CMSearchSession* pSender);

	private:

    void HandleMSearch(CSSDPMessage* pSSDPMessage);  
  
    void CleanupSessions(bool clearRunning = false);
  
		bool							 m_isStarted;
    CUDPSocket         m_Listener;	
    Ssdp::NotifyMsgFactory  m_notifyMsgFactory;
    sockaddr_in        m_LastMulticastEp;
    std::string         m_uuid;
    std::string        m_sIPAddress;    
    std::string        m_sHTTPServerURL;
    ISSDPCtrl*         m_pReceiveHandler;
    Threading::Mutex      m_SessionReceiveMutex;
    Threading::Mutex      m_SessionTimedOutMutex;

    std::list<CMSearchSession*> m_RunningSessionList;    
    std::list<CMSearchSession*>::iterator m_RunningSessionListIterator;
    
    std::list<CMSearchSession*> m_SessionList;    
    std::list<CMSearchSession*>::iterator m_SessionListIterator;    
};

#endif // _SSDPCTRL_H
