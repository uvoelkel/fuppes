/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            Socket.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2009 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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


#include "Socket.h"
//#include "../SharedConfig.h"
using namespace fuppes;

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#ifndef WIN32
#include <errno.h>
#include <sys/errno.h>

#define INVALID_SOCKET -1
#endif

// win32 and os x have no MSG_NOSIGNAL
// mac os x uses setsockopt(SO_NOSIGPIPE) instead
// win32 does not need this at all
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

//#define INITIAL_BUFFER_SIZE 16384  // 16 Kb
#define INITIAL_BUFFER_SIZE 1024  
#define MAX_BUFFER_SIZE 1048576  // 1Mb

SocketBase::SocketBase()
{ 
    m_socket = INVALID_SOCKET;
 	m_buffer = NULL;
 	m_bufferSize = 0;
 	m_bufferFill = 0;
 	m_connected = false;
	m_nonBlocking = false;	
}

SocketBase::~SocketBase()
{
  this->close();

	if(m_buffer != NULL)
		free(m_buffer);
}

bool SocketBase::setNonBlocking() 
{
	if(m_nonBlocking)
		return true;
	
	#ifdef WIN32     
  int nonblocking = 1;
  if(ioctlsocket(m_socket, FIONBIO, (unsigned long*) &nonblocking) != 0)
    return false;
  #else     
  int opts;
	opts = fcntl(m_socket, F_GETFL);
	if (opts < 0) {
    return false;
	}
	opts |= (O_NONBLOCK);
	if (fcntl(m_socket, F_SETFL, opts) < 0) {		
    return false;
	} 
	#endif
	m_nonBlocking = true;
  return true;
}

bool SocketBase::setBlocking() 
{
	if(!m_nonBlocking)
		return true;
	
	#ifdef WIN32     
  int nonblocking = 0;
  if(ioctlsocket(m_socket, FIONBIO, (unsigned long*) &nonblocking) != 0)
    return false;
  #else     
  int opts;
	opts = fcntl(m_socket, F_GETFL);
	if (opts < 0) {
    return false;
	}
	opts &= (~O_NONBLOCK);
	if (fcntl(m_socket, F_SETFL, opts) < 0) {		
    return false;
	} 
	#endif
	m_nonBlocking = false;
  return true;
}

bool SocketBase::close()
{
  if(m_socket == INVALID_SOCKET)
		return true;
  
  #ifdef WIN32
  bool closed = (closesocket(m_socket) == 0);
  #else
  bool closed = (::close(m_socket) == 0);
  #endif
	if(closed)
    m_socket = INVALID_SOCKET;
  else
    throw fuppes::Exception(__FILE__, __LINE__, "error closing socket %d - %s\n", errno, strerror(errno));  

	return closed;
}


fuppes_off_t SocketBase::send(std::string message, bool loop /* = true*/)
{
	return send(message.c_str(), message.length(), loop);
}

static inline void socketSleep(unsigned int milliseconds)
{
  #ifdef WIN32
  Sleep(milliseconds);
  #else
  if(milliseconds < 1000)
    usleep(milliseconds * 1000);
  else
    sleep(milliseconds / 1000);  
  #endif
}

fuppes_off_t SocketBase::send(const char* buffer, fuppes_off_t size, bool loop /*= true*/)
{
    if(!loop) {
  
        fuppes_off_t bytesSend = ::send(m_socket, buffer, size, MSG_NOSIGNAL);
        if(-1 == bytesSend) {

            bool wouldBlock = false;
            #ifdef WIN32
            wouldBlock = (WSAGetLastError() == WSAEWOULDBLOCK);    
            #else
            wouldBlock = (errno == EAGAIN || errno == EWOULDBLOCK);
            #endif

            return (wouldBlock ? 0 : -1);
        }

        return bytesSend;

    }
      
    else {

      
	      int						lastSend = 0;
        fuppes_off_t	fullSend = 0;
        bool					wouldBlock = false; 

        do {
          
            lastSend = ::send(m_socket, &buffer[fullSend], size - fullSend, MSG_NOSIGNAL);
            
            wouldBlock = false;
            #ifdef WIN32
            wouldBlock = (WSAGetLastError() == WSAEWOULDBLOCK);    
            #else
            wouldBlock = (errno == EAGAIN);
            #endif

            // incomplete
            if(lastSend > 0)
              fullSend += lastSend; 
            
            // complete
            if(fullSend == size)
              return fullSend;    
            
            // error
            if((lastSend < 0) && !wouldBlock)
              return -1; //throw Exception(__FILE__, __LINE__, "send error %d", lastSend);

            // would block
            if(wouldBlock)
              socketSleep(10);      

        } while ((lastSend < 0) || (fullSend < size) || wouldBlock);    
        
        return fullSend;
    }

  
}
fuppes_off_t SocketBase::oldreceive(int timeout /*= 0*/)
 {

 	#ifdef HAVE_SELECT
 	if(timeout > 0) {
 		setNonBlocking();
	}
 	
	fd_set fds;	
	struct timeval tv;
 	
	FD_ZERO(&fds);
	FD_SET(m_socket, &fds);
 		
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
 	int sel = select(m_socket + 1, &fds, NULL, NULL, &tv);
 	
	if(sel < 0) 
		throw Exception(__FILE__, __LINE__, "socket errror");
 	
	if(!FD_ISSET(m_socket, &fds) || sel == 0)
		return 0;
 	#endif
 	
 	// create new buffer
 	if(m_buffer == NULL) {
 		m_buffer = (char*)::malloc(INITIAL_BUFFER_SIZE);
 		m_bufferSize = INITIAL_BUFFER_SIZE;
 		m_bufferFill = 0;
 	}
 
 	// resize buffer
 	if(m_bufferSize - m_bufferFill == 0) {
 		m_buffer = (char*)::realloc(m_buffer, m_bufferSize + INITIAL_BUFFER_SIZE);
 		m_bufferSize += INITIAL_BUFFER_SIZE;
 	}
 		
 	// calc buffer offset and size and receive
 	char* buffer = &m_buffer[m_bufferFill];
	fuppes_off_t size = (m_bufferSize - m_bufferFill) - 1;
 	fuppes_off_t bytesReceived = ::recv(m_socket, buffer, size, 0);

 	// error
	if(bytesReceived < 0)
 		throw Exception(__FILE__, __LINE__, "socket errror");
 	
 	// set new fill status and terminate buffer
 	if(bytesReceived > 0) {
 		m_bufferFill += bytesReceived;
 		m_buffer[m_bufferFill] = '\0';
 	}
 
 	return bytesReceived;
 }
 

fuppes_off_t SocketBase::receive(int timeout /*= 0*/)
{
  m_connected = true;
  
	#ifdef HAVE_SELECT
	if(timeout > 0) {
		setNonBlocking();
	
	
	  fd_set fds;	
	  struct timeval tv;
	
	  FD_ZERO(&fds);
	  FD_SET(m_socket, &fds);
		
	  tv.tv_sec = timeout;
	  tv.tv_usec = 0;
   	int sel = select(m_socket + 1, &fds, NULL, NULL, &tv);
	
	  if(sel < 0) 
		  return -1; //throw Exception(__FILE__, __LINE__, "socket errror");
	
	  if(!FD_ISSET(m_socket, &fds) || sel == 0)
		  return 0;

  }
	#endif
	
	// create new buffer
	if(m_buffer == NULL) {
    m_bufferSize = INITIAL_BUFFER_SIZE - 1; // -1 for the terminating \0
		m_buffer = (char*)::malloc(INITIAL_BUFFER_SIZE);
		m_bufferFill = 0;
	}

	// resize buffer
	if(m_bufferSize - m_bufferFill == 0) {
    m_bufferSize += INITIAL_BUFFER_SIZE;
		m_buffer = (char*)::realloc(m_buffer, m_bufferSize + 1); // +1 for the terminating \0

    if(MAX_BUFFER_SIZE < m_bufferSize) {
      return -1; //throw Exception(__FILE__, __LINE__, "socket errror: buffer size limit reached");
    }
	}

	// calc buffer offset and size and receive
	char* buffer = &m_buffer[m_bufferFill];
	fuppes_off_t size = (m_bufferSize - m_bufferFill);
	fuppes_off_t bytesReceived = ::recv(m_socket, buffer, size, 0);
  
	// error
	if(bytesReceived < 0) {
    //WSAGetLastError
		//throw Exception(__FILE__, __LINE__, "socket errror");
    return -1;
  }

  // socket gracefully closed
  if(0 == bytesReceived) {
    m_connected = false;
    return bytesReceived;
  }
	
	// set new fill status and terminate buffer
	if(bytesReceived > 0) {
		m_bufferFill += bytesReceived;
		m_buffer[m_bufferFill] = '\0';
	}

	return bytesReceived;
}







TCPSocket::TCPSocket(std::string ipv4Address /* = ""*/)
:SocketBase()
{
	m_remotePort = 0;
	// create socket
	m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
  if(m_socket == INVALID_SOCKET)
    throw Exception("failed to create socket", __FILE__, __LINE__); 
	
	// set local end point
  m_localEndpoint.sin_family      = AF_INET;
	if(ipv4Address.size() == 0)
	  m_localEndpoint.sin_addr.s_addr = INADDR_ANY;
	else
	  m_localEndpoint.sin_addr.s_addr = inet_addr(ipv4Address.c_str());
  m_localEndpoint.sin_port				= htons(0);
  ::memset(&(m_localEndpoint.sin_zero), '\0', 8);
  
  // bind the socket
  int ret = bind(m_socket, (struct sockaddr*)&m_localEndpoint, sizeof(m_localEndpoint));
  if(ret == -1)
    throw fuppes::Exception("failed to bind socket", __FILE__, __LINE__);
    
  // fetch local end point to get port number on random ports
	socklen_t size = sizeof(m_localEndpoint);
	getsockname(m_socket, (struct sockaddr*)&m_localEndpoint, &size);
}

TCPSocket::~TCPSocket()
{
}

bool TCPSocket::connect() throw (fuppes::Exception)
{  
  // set remote end point
  m_remoteEndpoint.sin_family      = AF_INET;
  m_remoteEndpoint.sin_addr.s_addr = inet_addr(m_remoteAddress.c_str());
  m_remoteEndpoint.sin_port        = htons(m_remotePort);
  ::memset(&(m_remoteEndpoint.sin_zero), '\0', 8);


  bool blocking = isBlocking();
  if(blocking)
    setNonBlocking();
  
	// connect
	int ret = ::connect(m_socket, (struct sockaddr*)&m_remoteEndpoint, sizeof(m_remoteEndpoint));
	/*if(ret == -1)
		throw fuppes::Exception(__FILE__, __LINE__, "failed to connect to %s:%d", m_remoteAddress.c_str(), m_remotePort);
*/
  
  if (ret < 0) {

#ifndef WIN32
    if (errno != EINPROGRESS) {
#else
    if (errno != WSAEWOULDBLOCK) {       
#endif
   		return false; //throw fuppes::Exception(__FILE__, __LINE__, "failed to connect to %s:%d", m_remoteAddress.c_str(), m_remotePort);
    }


    fd_set myset; 
    struct timeval tv;
    int valopt; 
    socklen_t lon;
    
    tv.tv_sec = 3; 
    tv.tv_usec = 0; 
    FD_ZERO(&myset); 
    FD_SET(m_socket, &myset); 
    ret = select(m_socket+1, NULL, &myset, NULL, &tv); 
    if (ret < 0 && errno != EINTR) { 
      throw fuppes::Exception(__FILE__, __LINE__, "Error connecting %d - %s\n", errno, strerror(errno));  
    } 
    else if (ret > 0) {
      
      // Socket selected for write 
      lon = sizeof(int); 
#ifndef WIN32
      if (getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) { 
#else
      if (getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (char*)(&valopt), &lon) < 0) { 
#endif
        throw fuppes::Exception(__FILE__, __LINE__, "Error in getsockopt() %d - %s\n", errno, strerror(errno)); 
      } 
      // Check the value returned... 
      if (valopt) { 
        throw fuppes::Exception(__FILE__, __LINE__, "Error in delayed connection() %d - %s\n", valopt, strerror(valopt)); 
      }
      
    } 
    else { 
      throw fuppes::Exception(__FILE__, __LINE__, "Timeout in select() - Cancelling!\n"); 
    }
    
  } // ret < 0
  

  


  if(blocking)
    setBlocking();

  
	return true;
}




bool TCPServer::init(std::string ip, int port, bool nonBlocking /*= true*/)
{
	// create socket
	m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
  if(m_socket == INVALID_SOCKET)
    throw Exception("failed to create socket", __FILE__, __LINE__); 

  if(nonBlocking)
    setNonBlocking();

  // set socket option SO_REUSEADDR so restarting fuppes with
	// a fixed http port will not lead to a bind error
  int ret = 0;
  #ifdef WIN32  
  bool bOptVal = true;
  ret = setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&bOptVal, sizeof(bool));
  #else
  int flag = 1;
  ret = setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
  #endif
  if(ret == -1) {
    throw fuppes::Exception(__FILE__, __LINE__, "failed to setsockopt: SO_REUSEADDR");
  }

  
  // set local end point
	m_localEndpoint.sin_family      = AF_INET;
	m_localEndpoint.sin_addr.s_addr = inet_addr(ip.c_str());
	m_localEndpoint.sin_port				 = htons(port);
	memset(&(m_localEndpoint.sin_zero), '\0', 8);
	
  // bind the socket
	ret = bind(m_socket, (struct sockaddr*)&m_localEndpoint, sizeof(m_localEndpoint));	
  if(ret == -1)
    throw fuppes::Exception(__FILE__, __LINE__, "failed to bind socket to : %s:%d", ip.c_str(), port);
  
  // fetch local end point to get port number on random ports
	socklen_t size = sizeof(m_localEndpoint);
	getsockname(m_socket, (struct sockaddr*)&m_localEndpoint, &size);

  return true;
}

bool TCPServer::listen(int backlog /*=SOMAXCONN*/)
{
  // start listening
  int ret = ::listen(m_socket, backlog);
  if(ret == -1)
    throw fuppes::Exception(__FILE__, __LINE__, "failed to listen on socket");

  return true;
}

TCPRemoteSocket* TCPServer::accept()
{
  /*
  // select
	fd_set fds;	
	struct timeval timeout;

  FD_ZERO(&fds);
	FD_SET(m_socket, &fds);
		
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	int sel = select(m_socket + 1, &fds, NULL, NULL, &timeout);

	if(!FD_ISSET(m_socket, &fds)  || sel <= 0)
		return NULL;
*/
  
  // accept
  TCPRemoteSocket* result = new TCPRemoteSocket();
  socklen_t size = sizeof(result->m_remoteEndpoint);
  
  result->m_socket = ::accept(m_socket, (struct sockaddr*)&result->m_remoteEndpoint, &size);   
 	if(result->m_socket == INVALID_SOCKET) {
    delete result;
    return NULL;
	}
    
  return result;
}
