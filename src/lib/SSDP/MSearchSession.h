/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            MSearchSession.h
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

#ifndef _MSEARCHSESSION_H
#define _MSEARCHSESSION_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>

#include "UDPSocket.h"
#include "../Common/Timer.h"
#include "SSDPMessage.h"
#include "NotifyMsgFactory.h"

class CMSearchSession;

class IMSearchSession
{
    public:
        virtual ~IMSearchSession() { }

        virtual void OnSessionReceive(CSSDPMessage* pMessage) = 0;
        virtual void OnSessionTimeOut(CMSearchSession* pSender) = 0;
};

class CMSearchSession: public IUDPSocket, fuppes::ITimer
{

    public:
        CMSearchSession(std::string p_sIPAddress, IMSearchSession* pReceiveHandler, Ssdp::NotifyMsgFactory* pNotifyMsgFactory);
        virtual ~CMSearchSession();

        void OnUDPSocketStarted();
        void OnUDPSocketReceive(CSSDPMessage* pMessage);
        void OnTimer();

        void Start();
        void Stop();

        void send_multicast(std::string);
        void send_unicast(std::string);

        void begin_receive_unicast();
        void end_receive_unicast();

        sockaddr_in GetLocalEndPoint();

    private:
       // int m_nTimeout;
        CUDPSocket m_UdpSocket;
        std::string m_sIPAddress;
        IMSearchSession* m_pEventHandler;
        fuppes::Timer m_Timer;
        Ssdp::NotifyMsgFactory* m_pNotifyMsgFactory;
};

class CHandleMSearchSession;

class IHandleMSearchSession
{
    public:
        virtual ~IHandleMSearchSession() { }
        virtual void OnSessionEnd(CHandleMSearchSession* pSender) = 0;
};

class CHandleMSearchSession: public Threading::Thread
{
    public:
        CHandleMSearchSession(CSSDPMessage* pSSDPMessage, std::string uuid, std::string p_sIPAddress, std::string p_sHTTPServerURL, Ssdp::NotifyMsgFactory* pNotifyMsgFactory);
        ~CHandleMSearchSession();

        std::string m_uuid;
        std::string m_sIPAddress;
        std::string m_sHTTPServerURL;
        Ssdp::NotifyMsgFactory* m_pNotifyMsgFactory;

        CSSDPMessage* GetSSDPMessage()
        {
            return m_pSSDPMessage;
        }

    private:
        void run();

        CSSDPMessage* m_pSSDPMessage;
};

#endif // _MSEARCHSESSION_H
