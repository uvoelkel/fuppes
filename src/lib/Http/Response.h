/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _HTTP_RESPONSE_H
#define _HTTP_RESPONSE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_types.h>

#include "StatusCode.h"
#include "HttpHeader.h"
#include "Message.h"

namespace Http
{
    class HttpSession;
    class Request;

    class Response: protected Message
    {
        public:           
            Response(Http::HttpStatusCode status = Http::OK, std::string contentType = "", std::string content = "");
            virtual ~Response();

            Http::HttpStatusCode getStatusCode() { return m_statusCode; }
            void setStatusCode(Http::HttpStatusCode status) { m_statusCode = status; }

            std::string getVersion() { return m_version; }
            void setVersion(std::string version) { m_version = version; }

            Http::HttpHeader* getHeader() { return &m_header; }
            
            void setContent(std::string content);
            void setContent(const char* buffer, fuppes_off_t size);
            
            void prepare(Http::Request* request);
            
            
            fuppes_off_t    getHeaderSize() { return m_headerBufferSize; }
            const char*     getHeaderBuffer() { return m_headerBuffer; }

            
            virtual bool            hasContentLength() { return true; }
            virtual bool            acceptRanges() { return true; }
            virtual fuppes_off_t    getContentSize();
            // returns the size of the buffer starting with offset
            virtual fuppes_off_t    getContent(Http::HttpSession* session, char** buffer, fuppes_off_t offset, fuppes_off_t size, bool* eof);

            
            //fuppes_off_t    size() { return m_headerBufferSize + m_contentBufferSize; }
            // the size of the complete message including the full header
            //fuppes_off_t    sizeTotal(); // { return m_headerBufferSize + m_contentBufferSize; }
            
        private:
            HttpStatusCode  m_statusCode;
            std::string     m_version;
            HttpHeader      m_header;

            char*           m_headerBuffer;
            fuppes_off_t    m_headerBufferSize;

        protected:
            char*           m_contentBuffer;
            fuppes_off_t    m_contentBufferSize;
    };

}

#endif // _HTTP_RESPONSE_H
