/*
 * This file is part of fuppes
 *
 * (c) 2012-2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _TEST_HTTP_FILE_SESSION_H
#define _TEST_HTTP_FILE_SESSION_H

#include <Http/HttpSession.h>
#include <Http/ResponseFile.h>

class TestHttpFileSession: public Http::HttpSession
{
    public:
        TestHttpFileSession(fuppes::TCPRemoteSocket* remoteSocket, const std::string filename) :
                Http::HttpSession(remoteSocket)
        {
            m_filename = filename;
        }

    protected:
        Http::Response* handleRequest(Http::Request* request)
        {
            Http::Response* response;
            if(!fuppes::File::exists(m_filename)) {
                response = new Http::Response(Http::SERVICE_UNAVAILABLE);
            }
            else {
                response = new Http::ResponseFile(Http::OK, "text/plain", m_filename);
            }

            response->getHeader()->setValue("Server", "FUPPES");
            return response;
        }

    private:
        std::string m_filename;
};

#endif // _TEST_HTTP_FILE_SESSION_H
