/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _HTTP_HTTP_SERVER_H
#define _HTTP_HTTP_SERVER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Thread/EventLoopThread.h"
#include "../Thread/IoWatcher.h"
#include "../Thread/IoEventReceiverInterface.h"
#include "HttpSession.h"

namespace fuppes
{
    class TCPServer;
}

namespace Http
{
    class HttpServer;
    
    class HttpServerEventReceiver
    {
        public:
            virtual ~HttpServerEventReceiver() { }
            virtual void onHttpServerReady(Http::HttpServer* server) = 0;
            virtual void onHttpServerError(Http::HttpServer* server, std::string error) = 0;
    };
    
    typedef Http::HttpSession* (*createSessionCallback_t)(fuppes::TCPRemoteSocket* socket, void* userData);

    class HttpServer: public Threading::EventLoopThread, public Threading::IoReadEventReceiverInterface
    {
        public:
            HttpServer(std::string ip, int port = 0, Http::HttpServerEventReceiver* eventReceiver = NULL);
            void setCreateSessionCallback(createSessionCallback_t callback, void* userData) {
                m_createSessionCallback = callback;
                m_createSessionUserData = userData;
            }

            int getPort() { return m_port; }
            std::string getUrl() { return m_url; }
            
            void ioReadEvent(Threading::IoReadWatcher* watcher);
            
        protected:
            void run();

        private:
            std::string                     m_ip;
            int                             m_port;
            std::string                     m_url;
            
            createSessionCallback_t         m_createSessionCallback;
            void*                           m_createSessionUserData;
            
            fuppes::TCPServer*              m_listener;
            Threading::IoReadWatcher*       m_ioReadWatcher;

            HttpServerEventReceiver*        m_eventReceiver;
    };

}

#endif // _HTTP_HTTP_SERVER_H
