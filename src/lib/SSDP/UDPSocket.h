/***************************************************************************
 *            UDPSocket.h
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
 
#ifndef _UDPSOCKET_H
#define _UDPSOCKET_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SSDPMessage.h"

#include "../Common/Common.h"
#include "../Thread/Thread.h"

#include <string>

class CUDPSocket;

class IUDPSocket
{
  public:
    virtual ~IUDPSocket() {};
  
		virtual void OnUDPSocketStarted() = 0;
    virtual void OnUDPSocketReceive(CSSDPMessage* pMessage) = 0;
};

class CUDPSocket: public Threading::Thread
{

  public:
    /** constructor
     */
    CUDPSocket();

    /** destructor
     */
    ~CUDPSocket();

    /** initializes the socket
     *  @param  p_bDoMulticast  indicates whether to multicast or not
     *  @param  p_sIPAddress    the ip address. irrelevant if p_bDoMulticast is true
     *  @return returns true on success otherwise false
     */
    bool SetupSocket(bool p_bDoMulticast, std::string p_sIPAddress = "");

    /** set TTL
     *  @param p_nTTL Time to live
     */
     bool SetTTL(int p_nTTL);

    /** finalizes and closes the socket
     */
    void TeardownSocket();

    /** multicasts a message
     *  @param  p_sMessage  the message to send
     */
    void SendMulticast(std::string p_sMessage);

    /** unicasts a message
     *  @param  p_sMessage  the message to send
     *  @param  p_RemoteEndPoint  the receiver
     */
    void SendUnicast(std::string p_sMessage, sockaddr_in p_RemoteEndPoint);


    /** starts the receive thread
     */
    void BeginReceive();

    /** ends the receive thread
     */
    void EndReceive();

    /** sets the receive handler
     *  @param  pISocket  the receiver handler
     */	
    void SetReceiveHandler(IUDPSocket* pISocket)
			{ m_pReceiveHandler = pISocket; }

    /** lets the receiver handler handle a message
     *  @param  pMessage  the message to handle
     */	    
    void CallOnReceive(CSSDPMessage* pMessage);

		void SetStartedHander(IUDPSocket* pISocket)
			{ m_pStartedHandler = pISocket; }
	
		void CallOnStarted();
	
    /** returns the socket's descriptor
     *  @return the descriptor
     */
    fuppesSocket GetSocketFd();

    /** returns the socket's port
     *  @return the port number
     */
    int  GetPort();

    /** returns the socket's IP-Address as string
     *  @return the address
     */
    std::string GetIPAddress();

    /** returns the local end point
     *  @return the end point structure
     */
    sockaddr_in GetLocalEndPoint();	
 
    bool m_bBreakReceive;
  
  private:
		void run();
		
    /* Flags */
    bool          m_bDoMulticast;	

    /* Socket */
    fuppesSocket  m_Socket;					
    sockaddr_in   m_LocalEndpoint;

    /* Message handling */
    //fuppesThread  m_ReceiveThread;
    IUDPSocket*   m_pReceiveHandler;
		IUDPSocket*   m_pStartedHandler;
};

#endif // _UDPSOCKET_H
