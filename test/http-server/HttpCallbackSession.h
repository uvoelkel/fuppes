/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _TEST_HTTP_CALLBACK_SESSION_H
#define _TEST_HTTP_CALLBACK_SESSION_H


#include "../../src/lib/Http/HttpSession.h"
#include "../../src/lib/Http/ResponseCallback.h"

class TestHttpCallbackSession: public Http::HttpSession
{
    public:
        TestHttpCallbackSession(fuppes::TCPRemoteSocket* remoteSocket)
        :Http::HttpSession(remoteSocket) {

            m_bufferSize = 4;
            m_buffer = (char*)malloc(m_bufferSize +1);
            strcpy(m_buffer, "abcd");
        }

        virtual ~TestHttpCallbackSession() {
            free(m_buffer);
        }

        static fuppes_off_t contentCallback(Http::HttpSession* session, char** buffer, fuppes_off_t bufferSize, bool* eof) {
            //cout << "TestHttpCallbackSession::contentCallback" << endl;

            TestHttpCallbackSession* that = (TestHttpCallbackSession*)session;
            
            *buffer = that->m_buffer;
            *eof = true;
            return that->m_bufferSize;
        }
        
    protected:
        Http::Response* handleRequest(Http::Request* request) {
            
            Http::Response* response = new Http::ResponseCallback(Http::OK, "text/plain", &TestHttpCallbackSession::contentCallback);

            response->getHeader()->setValue("Connection", "close");
            //response->getHeader()->setValue("Content-Type", "text/plain");
            
            return response;
        }    

    private:
        char*           m_buffer;
        fuppes_off_t    m_bufferSize;

};

#endif // _TEST_HTTP_CALLBACK_SESSION_H
