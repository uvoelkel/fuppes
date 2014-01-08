/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Response.h"
#include "Request.h"

using namespace Http;

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

const std::map<Http::HttpStatusCode, std::string>HttpStatusText::m_map = HttpStatusText::create();


Response::Response(Http::HttpStatusCode status /*= Http::OK*/, std::string contentType /*= ""*/, std::string content /*= ""*/)
{
    m_statusCode = status;

    m_headerBuffer = NULL;
    m_headerBufferSize = 0;
    
    m_contentBuffer = NULL;
    m_contentBufferSize = 0;
    if(content.length() > 0) {
        this->setContent(content);
    }

    if(contentType.length() > 0) {
        this->getHeader()->setValue("Content-Type", contentType);
    }
}

Response::~Response()
{
    if(NULL != m_headerBuffer) {
        free(m_headerBuffer);
    }

    if(NULL != m_contentBuffer) {
        free(m_contentBuffer);
    }
}

void Response::setContent(std::string content)
{
    this->setContent(content.c_str(), content.length());    
}

void Response::setContent(const char* buffer, fuppes_off_t size)
{
    if(0 == size && 0 == m_contentBufferSize) {
        return;
    }
    else if(0 == size && 0 < m_contentBufferSize) {
        m_contentBufferSize = 0;
        free(m_contentBuffer);
        m_contentBuffer = NULL;
        return;
    }
    else if(0 < size && 0 == m_contentBufferSize) {
        m_contentBufferSize = size;
        m_contentBuffer = (char*)malloc(m_contentBufferSize + 1);
    }
    else if(0 < size && 0 < m_contentBufferSize) {
        m_contentBufferSize = size;
        m_contentBuffer = (char*)realloc(m_contentBuffer, m_contentBufferSize + 1);
    }

    memcpy(m_contentBuffer, buffer, size);
    m_contentBuffer[size] = '\0';
}

void Response::prepare(Http::Request* request)
{
    if(NULL != m_headerBuffer) {
        free(m_headerBuffer);
        m_headerBuffer = NULL;
        m_headerBufferSize = 0;
    }

    
    if(NULL != request) {
        
        this->setVersion(request->getVersion());

        if("HEAD" == request->getMethod()) {
            this->setContent(NULL, 0);
        }
        
    }

    // append default server signature
    if(!this->getHeader()->keyExists("Server")) {

        std::string signature = "fuppes/";
        signature += FUPPES_VERSION;
            
        this->getHeader()->setValue("Server", signature);
    }

    std::string statusText = HttpStatusText::get(this->m_statusCode);
    std::string headerFields = this->m_header.toString() + "\r\n";
    
    // calc the length of the first line
    fuppes_off_t size = 9 + 4 + statusText.length() + 2; // 9 = "HTTP/1.x " 4 = "200 " 2 = "\r\n";

    m_headerBufferSize = size + headerFields.length();
    m_headerBuffer = (char*)malloc(m_headerBufferSize + 1);
    
    sprintf(m_headerBuffer, "HTTP/%s %d %s\r\n", this->m_version.c_str(), this->m_statusCode, statusText.c_str());

    memcpy(&m_headerBuffer[size], headerFields.c_str(), m_headerBufferSize - size);
    m_headerBuffer[m_headerBufferSize] = '\0';

    //std::cout << m_headerBuffer << std:: endl;
}


fuppes_off_t Response::getContentSize()
{
    return m_contentBufferSize;
}


fuppes_off_t Response::getContent(HttpSession* session, char** buffer, fuppes_off_t offset, fuppes_off_t size, bool* eof)
{
    if(offset >= m_contentBufferSize) {
        *eof = true;
        return 0;
    }

    *buffer = &m_contentBuffer[offset];

    if(0 == size || offset + size >= m_contentBufferSize) {
        size = m_contentBufferSize - offset;
        *eof = true;
    }

    return size;          
}
