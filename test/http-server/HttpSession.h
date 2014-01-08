/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _TEST_HTTP_SESSION_H
#define _TEST_HTTP_SESSION_H


#include "../../src/lib/Http/HttpSession.h"
#include "../../src/lib/Http/ResponseFile.h"

class TestHttpSession: public Http::HttpSession
{
    public:
        TestHttpSession(fuppes::TCPRemoteSocket* remoteSocket)
        :Http::HttpSession(remoteSocket) {
        }

    protected:
        Http::Response* handleRequest(Http::Request* request) {

            string msg = 
                "<html><head></head><body>"
                "miep moep da humm palim\r\narghhh gnarf tilt foo bar"
                "</body></html>";
            
            Http::Response* response = new Http::Response(Http::OK, msg);
            //response->getHeader()->setValue("Connection", "close");
            response->getHeader()->setValue("Content-Type", "text/html");
            response->getHeader()->setValue("Server", "FUPPES");
            return response;
        }

        void onSessionStarted(std::string ip, int port) {
            cout << "ON SESSION STARTED" << endl;
        }
        
        void onRequestStarted() {
            cout << "ON REQUEST STARTED" << endl;
        }
        
        void onRequest() {
            cout << "ON REQUEST" << endl;
        }
        
        void onValidRequest() {
            cout << "ON VALID REQUEST" << endl;
        }
        
        void onResponse() {
            cout << "ON RESPONSE" << endl;
        }
        
        void onRequestFinished() {
            cout << "ON REQUEST FINISHED" << endl;
        }
        
        void onSessionFinished() {
            cout << "ON SESSION FINISHED" << endl;
        }
           
};

#endif // _TEST_HTTP_SESSION_H
