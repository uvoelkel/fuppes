/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _HTTP_REQUEST_H
#define _HTTP_REQUEST_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Message.h"
#include "HttpHeader.h"
#include "Message.h"

#include <string.h>
#include <map>

namespace fuppes
{
    class TCPRemoteSocket;
}

namespace Http
{    
    class Request: protected Message
    {
        public:
            // make sure data is null terminated
            static Http::Request* create(const char* data, Http::Request* request = NULL);

            Request();
            virtual ~Request();

            fuppes::TCPRemoteSocket* getRemoteSocket() { return m_remoteSocket; }
            void setRemoteSocket(fuppes::TCPRemoteSocket* socket) { m_remoteSocket = socket; }

            void setBuffer(const char* buffer, fuppes_off_t size, fuppes_off_t fill, fuppes_off_t headerSize);
            
            Http::HttpHeader* getHeader() { return &m_header; }

            std::string getRawHeader();
            
            const char* getContent() { return m_buffer + m_headerSize; }
            fuppes_off_t getContentSize() { return m_bufferFill - m_headerSize; }
            
            std::string getVersion() { return m_version; }
            std::string getMethod() { return m_method; }

            std::string getUri() { return m_uri; }
            std::string getPath() { return m_path; }
            std::string getFile() { return m_file; }
            std::string getExtension() { return m_extension; }

            bool getGetVar(const std::string key, std::string &value);

        private:
            Http::HttpHeader    m_header;

            std::string         m_version;
            std::string         m_method;

            std::string         m_uri;
            std::string         m_path;
            std::string         m_file;
            std::string         m_extension;

            std::map<std::string, std::string> m_get;
            
            char*               m_buffer;
            fuppes_off_t        m_bufferSize;  // the size of the buffer (-1 for \0)
            fuppes_off_t        m_bufferFill;  // the bytes actually filled
            fuppes_off_t        m_headerSize;
            
            fuppes::TCPRemoteSocket*            m_remoteSocket;
    };

}

#endif // _HTTP_REQUEST_H
