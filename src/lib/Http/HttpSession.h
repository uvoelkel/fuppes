/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _HTTP_HTTP_SESSION_H
#define _HTTP_HTTP_SESSION_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_types.h>
#include "StatusCode.h"

#include "../Thread/EventLoopThread.h"
#include "../Thread/IoWatcher.h"
#include "../Thread/IoEventReceiverInterface.h"

namespace fuppes
{
    class TCPRemoteSocket;
}

namespace Http
{
    class HttpServer;
    class Response;
    class Request;

    class HttpSession:
        public Threading::EventLoopThread,
        public Threading::IoReadEventReceiverInterface,
        public Threading::IoWriteEventReceiverInterface
    {
        public:
            HttpSession(fuppes::TCPRemoteSocket* remoteSocket);
            virtual ~HttpSession();

            void stop();

            void ioReadEvent(Threading::IoReadWatcher* watcher);
            void ioWriteEvent(Threading::IoWriteWatcher* watcher);

            Http::Request* getHttpRequest() { return m_request; }
            //Http::HttpResponse

            void setHttpServer(Http::HttpServer* server) { m_server = server; }
            Http::HttpServer* getHttpServer() { return m_server; }

            fuppes::TCPRemoteSocket* getRemoteSocket() { return m_remoteSocket; }

        protected:
            virtual Http::Response* handleRequest(Http::Request* request);

            // make sure buffer is null terminated
            virtual Http::Request* createRequest(const char* buffer, fuppes::TCPRemoteSocket* remoteSocket);
            virtual Http::Response* createResponse(Http::HttpStatusCode status, std::string content = "");

            virtual void onSessionStarted(std::string ip, int port) { }
            virtual void onRequestStarted() { }
            virtual void onRequest(Http::Request* request) { }
            virtual void onValidRequest(Http::Request* request) { }
            virtual void onResponse() { }
            virtual void onRequestFinished() { }
            virtual void onSessionFinished() { }

        protected:
            Http::HttpServer*               m_server;
            Http::Request*                  m_request;
            Http::Response*                 m_response;
            
        private:
            void run();
            Http::Request* readChunkedMessage(Http::Request* request, const char* buffer, size_t bufferSize, char* headerOffset);
            int parseChunkSize(const char* buffer);
                
            // returns true if the message is completely send or an error occurred
            // returns false if there is still data left to send
            bool sendResponse(bool firstTime = true);
            bool sendContent();
            bool sendContentChunked();

            fuppes::TCPRemoteSocket*            m_remoteSocket;
            bool                                m_newRequest;
            Threading::IoReadWatcher*           m_ioReadWatcher;
            Threading::IoWriteWatcher*          m_ioWriteWatcher;
            bool                                m_keepAlive;
            Http::Request*                      m_tmpRequest;

            // send offset for the whole message including header
            fuppes_off_t                        m_responseOffset;

            // send offset and size for the content
            fuppes_off_t                        m_contentOffset;
            fuppes_off_t                        m_contentSize;
    };

}

#endif // _HTTP_HTTP_SESSION_H
