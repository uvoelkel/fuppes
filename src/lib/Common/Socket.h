/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            Socket.h
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

#ifndef _SOCKET_H
#define _SOCKET_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef WIN32
/*
 #pragma comment(lib,"Wsock32.lib")
 #pragma comment(lib,"Ws2_32.lib")
 #pragma comment(lib,"shlwapi.lib")
 */
#include <winsock2.h>
#include <ws2tcpip.h>
#include <shlwapi.h>

#else

#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#endif

//#include "Common.h"
#include "../../../include/fuppes_types.h"
#include <string>
#include <unistd.h>

#include "Exception.h"

namespace fuppes
{

    class TCPServer;

    class SocketBase
    {
            friend class TCPServer;

        protected:
            SocketBase();
            virtual ~SocketBase();

        public:
            bool setNonBlocking();
            bool setBlocking();
            bool isBlocking()
            {
                return !m_nonBlocking;
            }
            bool close();
            bool isConnected()
            {
                return m_connected;
            }

            fuppes_off_t send(std::string message, bool loop = true);
            fuppes_off_t send(const char* buffer, fuppes_off_t size, bool loop = true);
            // timeout works only on nonblocking sockets and if "select()" is available
            fuppes_off_t receive(int timeout = 0);
            fuppes_off_t oldreceive(int timeout = 0);

            char* buffer()
            {
                return m_buffer;
            }
            fuppes_off_t bufferSize()
            {
                return m_bufferSize;
            }
            fuppes_off_t bufferFill()
            {
                return m_bufferFill;
            }
            void bufferClear()
            {
                m_bufferFill = 0;
            }
            void bufferDetach()
            {
                m_buffer = NULL;
                m_bufferSize = 0;
                m_bufferFill = 0;
            }

            std::string localAddress()
            {
                return inet_ntoa(m_localEndpoint.sin_addr);
            }
            int localPort()
            {
                return ntohs(m_localEndpoint.sin_port);
            }

#ifdef WIN32
            SOCKET socket() {return m_socket;}
#else
            int socket()
            {
                return m_socket;
            }
#endif

            sockaddr_in remoteEndpoint()
            {
                return m_remoteEndpoint;
            }
            std::string remoteAddress()
            {
                return inet_ntoa(m_remoteEndpoint.sin_addr);
            }
            int remotePort()
            {
                return ntohs(m_remoteEndpoint.sin_port);
            }

        protected:
#ifdef WIN32
            SOCKET m_socket;
#else
            int m_socket;
#endif

            sockaddr_in m_localEndpoint;
            sockaddr_in m_remoteEndpoint;

        private:
            bool m_nonBlocking;
            bool m_connected;

            char* m_buffer;
            fuppes_off_t m_bufferSize;
            fuppes_off_t m_bufferFill;
    };

    class UDPSocket: private SocketBase
    {
        public:
            UDPSocket()
            {
            }
            ~UDPSocket()
            {
            }
    };

    class TCPSocket: public SocketBase
    {
        public:
            TCPSocket(std::string ipv4Address = "");
            virtual ~TCPSocket();

            bool connect() throw (fuppes::Exception);

            std::string remoteAddress()
            {
                return m_remoteAddress;
            }
            void remoteAddress(std::string address)
            {
                m_remoteAddress = address;
            }
            uint16_t remotePort()
            {
                return m_remotePort;
            }
            void remotePort(const uint16_t port)
            {
                m_remotePort = port;
            }
            sockaddr_in remoteEndpoint()
            {
                return m_remoteEndpoint;
            }

        private:
            std::string m_remoteAddress;
            uint16_t m_remotePort;
    };

    class TCPRemoteSocket: public SocketBase
    {
            friend class TCPServer;

        public:
            TCPRemoteSocket() :
                    SocketBase()
            {
            }

    };

    class TCPServer: public SocketBase
    {
        public:
            TCPServer() :
                    SocketBase()
            {
            }
            virtual ~TCPServer()
            {
            }

            bool init(std::string ip, int port, bool nonBlocking = true);
            bool listen(int backlog = SOMAXCONN);

            // caller has to free object instance
            TCPRemoteSocket* accept();

    };

}

#endif // _SOCKET_H
