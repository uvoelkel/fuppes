/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _HTTP_RESPONSE_CALLBACK_H
#define _HTTP_RESPONSE_CALLBACK_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Response.h"

namespace Http
{

    class HttpSession;
    
    typedef fuppes_off_t (*contentCallback_t)(Http::HttpSession* session, char** buffer, fuppes_off_t offset, bool* eof);

    class BaseResponseCallback
    {
        public:
            BaseResponseCallback(contentCallback_t callback = NULL);
                
            void setCallback(contentCallback_t callback);

            fuppes_off_t    getContentSize();
            // returns the size of the buffer starting with offset
            fuppes_off_t    getContent(HttpSession* session, char** buffer, fuppes_off_t offset, fuppes_off_t size, bool* eof);

        private:
            contentCallback_t                   m_contentCallback;
    };
    
    
    class ResponseCallback: public Http::Response, public BaseResponseCallback
    {
        public:
            ResponseCallback(Http::HttpStatusCode status = Http::OK, std::string contentType = "", contentCallback_t callback = NULL):
            Http::Response(status, contentType),
            Http::BaseResponseCallback(callback) {
            }

            bool hasContentLength() { return false; }
            bool acceptRanges() { return false; }

            fuppes_off_t    getContentSize() {
                return BaseResponseCallback::getContentSize(); 
            }
            
            fuppes_off_t    getContent(Http::HttpSession* session, char** buffer, fuppes_off_t offset, fuppes_off_t size, bool* eof) {
                return BaseResponseCallback::getContent(session, buffer, offset, size, eof);
            }
    };

}

#endif // _HTTP_RESPONSE_CALLBACK_H
