/***************************************************************************
 *            UDPSocket.cpp
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
#include "../Common/Exception.h"

#include <sstream>
#include <string.h>
#include <fcntl.h>


#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif


#ifndef WIN32
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#endif


#if (defined(__unix__) || defined(unix)) && !defined(USG)
#include <sys/param.h>
#endif


#ifndef WIN32
#define INVALID_SOCKET -1
#endif

using namespace std;
using namespace fuppes;

#define MULTICAST_PORT 1900
#define MULTICAST_IP   "239.255.255.250"

//fuppesThreadCallback UDPReceiveLoop(void *arg);

CUDPSocket::CUDPSocket()
:Thread("udpsocket")
{	
	/* Init members */
  m_LocalEndpoint.sin_port = 0;
	//m_ReceiveThread          = (fuppesThread)NULL;
  m_pReceiveHandler        = NULL;
	m_pStartedHandler				 = NULL;
	m_Socket = INVALID_SOCKET;
}	
	
CUDPSocket::~CUDPSocket()
{
  /* Cleanup */
  TeardownSocket();

  stop();
  close();
}

/* SetupSocket */
bool CUDPSocket::SetupSocket(bool p_bDoMulticast, std::string p_sIPAddress /* = "" */)
{
  /* Create socket */
	m_Socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(m_Socket == INVALID_SOCKET) {
    throw fuppes::Exception(__FILE__, __LINE__, "failed to create socket");
  }
  
  /* Set socket options */
  int ret  = 0;
  #ifdef WIN32  
  bool bOptVal = true;
  ret = setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&bOptVal, sizeof(bool));
  #else
  int flag = 1;
  ret = setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
  #endif
  if(ret == -1) {
    throw fuppes::Exception(__FILE__, __LINE__, "failed to setsockopt: SO_REUSEADDR");
  }

	#if defined(BSD) || defined(__APPLE__)
	// bug #1864791 - avoid locking the multicast port on OS X
	flag = 1;
	ret = setsockopt(m_Socket, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));
	if(ret == -1) {
		throw fuppes::Exception(__FILE__, __LINE__, "failed to setsockopt: SO_REUSEPORT");
	}
	#endif
	
	#if defined(BSD) || HAVE_SELECT
	/* OS X does not support pthread_cancel
	   so we need to set the socket to non blocking and
		 constantly poll the cancellation state.
		 otherwise fuppes will hang on shutdown
		 same for HTTPServer */
	//fuppesSocketSetNonBlocking(m_Socket);
     #ifdef WIN32     
    int nonblocking = 1;
    if(ioctlsocket(m_Socket, FIONBIO, (unsigned long*) &nonblocking) != 0)
      return false;
    #else     
    int opts;
	  opts = fcntl(m_Socket, F_GETFL);
	  if (opts < 0) {
      return false;
	  }
	  opts = (opts | O_NONBLOCK);
	  if (fcntl(m_Socket, F_SETFL,opts) < 0) {		
      return false;
	  } 
	  #endif
  
	#endif
	
	/* Set local endpoint */
  m_LocalEndpoint.sin_family = AF_INET;	
	if(p_bDoMulticast) {
    m_LocalEndpoint.sin_addr.s_addr  = INADDR_ANY;
    m_LocalEndpoint.sin_port		     = htons(MULTICAST_PORT);
	}
  else {
    m_LocalEndpoint.sin_addr.s_addr  = inet_addr(p_sIPAddress.c_str());		
    m_LocalEndpoint.sin_port		     = htons(0); /* use random port */
	}	
	memset(&(m_LocalEndpoint.sin_zero), '\0', 8); // fill the rest of the structure with zero
	
	/* Bind socket */
	ret = bind(m_Socket, (struct sockaddr*)&m_LocalEndpoint, sizeof(m_LocalEndpoint)); 
  if(ret == -1) {
    throw fuppes::Exception(__FILE__, __LINE__, "failed to bind udp socket %s", p_sIPAddress.c_str());
  }
	
	/* Get random port */
	socklen_t size = sizeof(m_LocalEndpoint);
	getsockname(m_Socket, (struct sockaddr*)&m_LocalEndpoint, &size);
	
	/* Join multicast group */
	m_bDoMulticast = p_bDoMulticast;
	if(m_bDoMulticast)
	{	
		struct ip_mreq stMreq; /* Multicast interface structure */
			
		stMreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_IP); 
		stMreq.imr_interface.s_addr = inet_addr(p_sIPAddress.c_str()); //INADDR_ANY; 	
		ret = setsockopt(m_Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&stMreq,sizeof(stMreq)); 	
		if(ret == -1) {      
      throw fuppes::Exception(__FILE__, __LINE__, "failed to setsockopt: multicast");
    }
	}
  
  return true;
}	

bool CUDPSocket::SetTTL(int p_nTTL)
{
  return -1 != setsockopt(m_Socket, IPPROTO_IP, IP_TTL, (char *)&p_nTTL, sizeof(p_nTTL));
}

/* TeardownSocket */
void CUDPSocket::TeardownSocket()
{

  if(m_Socket == INVALID_SOCKET)
	  return;

    /* Exit thread */  
  /*if(m_ReceiveThread != (fuppesThread)NULL)
    EndReceive();*/
	stop();

	/* Leave multicast group */
	if(m_bDoMulticast)
	{
		struct ip_mreq stMreq;
		
	  stMreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_IP); 
		stMreq.imr_interface.s_addr = INADDR_ANY; 	
		setsockopt(m_Socket, IPPROTO_IP, IP_DROP_MEMBERSHIP,(char *)&stMreq,sizeof(stMreq)); 		
	}

  /* Close socket */
  #ifdef WIN32
  ::closesocket(m_Socket);
  #else
  shutdown(m_Socket, SHUT_RDWR);
  ::close(m_Socket);
  #endif
  
  m_Socket = INVALID_SOCKET;
}

/* SendMulticast */
void CUDPSocket::SendMulticast(std::string p_sMessage)
{
/*
  #ifdef WIN32
  const char loop = 0;
  #else
  u_char loop = 0; 
  #endif
  setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)); 
*/
	
	/* Create remote end point */
	sockaddr_in remote_ep;	
	remote_ep.sin_family      = AF_INET;
  remote_ep.sin_addr.s_addr = inet_addr(MULTICAST_IP);
  remote_ep.sin_port				= htons(MULTICAST_PORT);
	memset(&(remote_ep.sin_zero), '\0', 8);
		
  /* Send message */
	sendto(m_Socket, p_sMessage.c_str(), (int)strlen(p_sMessage.c_str()), 0, (struct sockaddr*)&remote_ep, sizeof(remote_ep));
}

/* SendUnicast */
void CUDPSocket::SendUnicast(std::string p_sMessage, sockaddr_in p_RemoteEndPoint)
{
  /* Send message */
  sendto(m_Socket, p_sMessage.c_str(), (int)strlen(p_sMessage.c_str()), 0, (struct sockaddr*)&p_RemoteEndPoint, sizeof(p_RemoteEndPoint));  
}

/* BeginReceive */
void CUDPSocket::BeginReceive()
{
  /* Start thread */  
	m_bBreakReceive = false;
  //fuppesThreadStart(m_ReceiveThread, UDPReceiveLoop);
	start();
}

/* EndReceive */
void CUDPSocket::EndReceive()
{
  /* Exit thread */	 
  m_bBreakReceive = true;
	stop();
	/*if(m_ReceiveThread) {
    fuppesThreadCancel(m_ReceiveThread);    
    fuppesThreadClose(m_ReceiveThread);
    m_ReceiveThread = (fuppesThread)NULL;
  }*/
}


/* CallOnReceive */
void CUDPSocket::CallOnReceive(CSSDPMessage* pSSDPMessage)
{ 
  if(m_pReceiveHandler != NULL)
	  m_pReceiveHandler->OnUDPSocketReceive(pSSDPMessage);
}

void CUDPSocket::CallOnStarted()
{
	if(m_pReceiveHandler != NULL) {
		m_pReceiveHandler->OnUDPSocketStarted();
	}
}

/* GetSocketFd */
fuppesSocket CUDPSocket::GetSocketFd()
{
	return m_Socket;
}

/* GetPort */
int CUDPSocket::GetPort()
{
	return ntohs(m_LocalEndpoint.sin_port);
}

/* GetIPAddress */
std::string CUDPSocket::GetIPAddress()
{
	return inet_ntoa(m_LocalEndpoint.sin_addr);
}

/* GetLocalEp */
sockaddr_in CUDPSocket::GetLocalEndPoint()
{
	return m_LocalEndpoint;
}

//fuppesThreadCallback UDPReceiveLoop(void *arg)
void CUDPSocket::run()
{
/* 
  #ifndef FUPPES_TARGET_WIN32                     
  //set thread cancel state
  int nRetVal = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  if (nRetVal != 0) {
    perror("Thread pthread_setcancelstate Failed...\n");
    exit(EXIT_FAILURE);
  }

  // set thread cancel type
  nRetVal = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
  if (nRetVal != 0) {
    perror("Thread pthread_setcanceltype failed...");
    exit(EXIT_FAILURE);
  }
  #endif // FUPPES_TARGET_WIN32
*/

  CUDPSocket* udp_sock = this; //(CUDPSocket*)arg;

	/*CSharedLogging::Log::Log(L_EXT, __FILE__, __LINE__, 
      "listening on %s:%d", udp_sock->GetIPAddress().c_str(), udp_sock->GetPort());*/
  
	char buffer[4096];	
	int  bytes_received = 0;
	  
	sockaddr_in remote_ep;	
	socklen_t   size = sizeof(remote_ep);
  
	udp_sock->CallOnStarted();
	
	#ifdef HAVE_SELECT
	fd_set fds;	
	struct timeval timeout;
	#endif
	
  while(!this->stopRequested() && !udp_sock->m_bBreakReceive) {
		#ifdef HAVE_SELECT
		FD_ZERO(&fds);
		FD_SET(udp_sock->GetSocketFd(), &fds);
		
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;		
		
 		int sel = select(udp_sock->GetSocketFd() + 1, &fds, NULL, NULL, &timeout);
		//cout << "SELECT: " << sel << endl;
		if(sel <= 0)
			continue;
		#endif
		
    bytes_received = recvfrom(udp_sock->GetSocketFd(), buffer, sizeof(buffer), 0, (struct sockaddr*)&remote_ep, &size);
    if(bytes_received <= 0) {
		  /*#if defined(BSD)
			pthread_testcancel();
			fuppesSleep(100);
			#else */
      //CSharedLogging::Log::Log(L_DBG, __FILE__, __LINE__, "error :: recvfrom()");
      //#endif
			continue;
    }
		buffer[bytes_received] = '\0';

    
    // ensure we don't receive our own message
		if((remote_ep.sin_addr.s_addr != udp_sock->GetLocalEndPoint().sin_addr.s_addr) || 
			 (remote_ep.sin_port != udp_sock->GetLocalEndPoint().sin_port))	
    {
			CSSDPMessage pSSDPMessage;
			pSSDPMessage.SetRemoteEndPoint(remote_ep);
	  	pSSDPMessage.SetLocalEndPoint(udp_sock->GetLocalEndPoint());
			
      if(pSSDPMessage.SetMessage(buffer) && !this->stopRequested()) {
		  	udp_sock->CallOnReceive(&pSSDPMessage);
      }
      else {
        //CSharedLogging::Log::Log(L_DBG, __FILE__, __LINE__, "error parsing UDP-message");
      }
		}
	}
	
  //CSharedLogging::Log::Log(L_EXT, __FILE__, __LINE__, "exiting ReceiveLoop()");
  //fuppesThreadExit();
}
