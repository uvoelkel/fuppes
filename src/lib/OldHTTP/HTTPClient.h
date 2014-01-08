/***************************************************************************
 *            HTTPClient.h
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
 
#ifndef _HTTPCLIENT_H
#define _HTTPCLIENT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Common/Socket.h"
#include "HTTPMessage.h"
#include "../GENA/EventNotification.h"
#include "../Common/Common.h"
#include "../Thread/Thread.h"

#ifndef WIN32
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

class IHTTPClient
{
  public:
    virtual ~IHTTPClient() {};
  
	  virtual void OnAsyncReceiveMsg(CHTTPMessage* pMessage) = 0;
};

class CHTTPClient: public Threading::Thread
{
  public:
    CHTTPClient(std::string ip, IHTTPClient* pAsyncReceiveHandler = NULL, std::string name = "");
    ~CHTTPClient();

    bool AsyncGet(std::string p_sGetURL);
    void AsyncNotify(CEventNotification* pNotification);
 
    std::string  m_sMessage;
    bool         m_bAsyncDone;
    bool         m_bIsAsync;
 	  IHTTPClient* m_pAsyncReceiveHandler;


    fuppes::TCPSocket* socket() { return &m_socket; }

  private:
		void run();
		
    std::string BuildGetHeader(std::string  p_sGet,
                               std::string  p_sTargetIPAddress,
                               unsigned int p_nTargetPort); 

    fuppes::TCPSocket m_socket;
};

#endif // _HTTPCLIENT_H
