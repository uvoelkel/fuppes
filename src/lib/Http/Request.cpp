/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "../Common/Socket.h"

#include "Request.h"

using namespace Http;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

Http::Request* Request::create(const char* data, Http::Request* request /*= NULL*/) // static
{
    // try to find the end of the header
    char* end = strstr((char*)data, "\r\n\r\n");
    if (NULL == end) {
        return NULL;
    }

    if (NULL == request) {
        request = new Http::Request();
    }

    int length = 0;
    int offset = 0;
    char tmp[256];
    bool valid = true;

    // get the method
    char* pos = strchr((char*)data + offset, ' ');
    if (NULL != pos) {
        length = pos - (data + offset);
        if (length < (int)sizeof(tmp) - 1) {
            strncpy(tmp, data + offset, length);
            tmp[length] = '\0';
            request->m_method = tmp;
        }
        offset += length + 1;
    }
    else {
        valid = false;
    }

    // get the uri
    pos = strchr((char*)data + offset, ' ');
    if (NULL != pos) {
        length = pos - (data + offset);
        if (length < (int)sizeof(tmp) - 1) {
            strncpy(tmp, data + offset, length);
            tmp[length] = '\0';
            request->m_uri = tmp;
        }
        offset += length + 1;
    }
    else {
        valid = false;
    }

    // get the version
    pos = strchr((char*)data + offset, '\r');
    if (NULL != pos) {
        length = pos - (data + offset);
        if (length == 8 && length < (int)sizeof(tmp) - 1) {  // 8 = "HTTP/1.x"
            strncpy(tmp, data + offset + 5, length);        // 5 = "HTTP/"
            tmp[length - 5] = '\0';
            request->m_version = tmp;
        }
        offset += length + 2; // \r\n
    }
    else {
        valid = false;
    }

    // parse the get vars
    size_t qm = request->m_uri.find("?");
    if (std::string::npos != qm) {
        std::string params = request->m_uri.substr(qm + 1);
        request->m_uri = request->m_uri.substr(0, qm);

        size_t eq;
        size_t amp;
        std::string chunk;
        std::string key;
        std::string value;

        while (params.length() > 0) {

            amp = params.find("&");
            if (std::string::npos == amp) {
                chunk = params;
                params = "";
            }
            else {
                chunk = params.substr(0, amp);
                params = params.substr(amp + 1);
            }

            eq = chunk.find("=");
            if (std::string::npos == eq) {
                valid = false;
                break;
            }
            request->m_get[chunk.substr(0, eq)] = chunk.substr(eq + 1);
        }
    }

    // extract the extension, ...
    size_t dot = request->m_uri.find_last_of(".");
    if (std::string::npos != dot) {
        request->m_extension = request->m_uri.substr(dot);
    }

    // ... path and ...
    size_t slash = request->m_uri.find_last_of("/");
    if (std::string::npos == slash) {
        valid = false;
    }
    request->m_path = request->m_uri.substr(0, slash);

    // ... filename
    if (slash < request->m_uri.size()) {
        if (std::string::npos != dot) {
            request->m_file = request->m_uri.substr(slash, dot - slash);
        }
        else {
            request->m_file = request->m_uri.substr(slash);
        }
    }

    if (!valid) {
        delete request;
        return NULL;
    }

    // parse the rest of the header
    request->m_header.parse(data + offset, end);

    return request;
}

Request::Request()
{
    m_buffer = NULL;
    m_bufferSize = 0;
    m_bufferFill = 0;
    m_headerSize = 0;
    m_remoteSocket = NULL;
}

Request::~Request()
{
    if (NULL != m_buffer) {
        free(m_buffer);
    }
}

void Request::setBuffer(const char* buffer, fuppes_off_t size, fuppes_off_t fill, fuppes_off_t headerSize)
{
    m_buffer = (char*)buffer;
    m_bufferSize = size;
    m_bufferFill = fill;
    m_headerSize = headerSize;

    if (m_bufferFill < m_bufferSize) {
        memset((char*)&m_buffer[m_bufferFill], '\0', m_bufferSize - m_bufferFill);
    }
}

std::string Request::getRawHeader()
{
    std::string result;
    char* buffer = new char[m_headerSize + 1];
    memcpy(buffer, m_buffer, m_headerSize);
    buffer[m_headerSize] = '\0';
    result = buffer;
    delete[] buffer;
    return result;
}

bool Request::getGetVar(const std::string key, std::string &value)
{
    std::map<std::string, std::string>::iterator iter = m_get.find(key);
    if (m_get.end() == iter) {
        return false;
    }
    value = iter->second;
    return true;
}
