/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "HttpHeader.h"

using namespace Http;

#include <string.h>
#include <ctype.h>

#include "../Common/Common.h"
#include "../Common/RegEx.h"

#include <iostream>

void HttpHeader::parse(const char* data, const char* end)
{
    m_valid = false;

    int lineLength = 0;
    int offset = 0;
    int length = 0;

    char key[256];
    char value[512];
    char* colon;

    char* lineEnd = strstr((char*)data + offset, "\r\n");
    while (lineEnd < end + 2) {

        lineLength = lineEnd - (data + offset);

        colon = strchr((char*)data + offset, ':');
        if(NULL == colon || colon > data + offset + lineLength) {
            std::cout << "HTTP HEADER PARSE ERROR" << std::endl;
            return;
        }

        // get the key
        length = colon - (data + offset);
        if(length > 0 && length < (int)sizeof(key) - 1) {
            strncpy(key, data + offset, length);
            key[length] = '\0';
            // transform to lowercase
            for(size_t i = 0; i < strlen(key); i++) {
                key[i] = tolower(key[i]);
            }
        }
        else {
            std::cout << "HTTP HEADER PARSE ERROR :: KEY TOO SHORT/LONG" << std::endl;
            return;
        }

        // get the value
        colon++; // move cursor behind the colon
        length = lineLength - (colon - (data + offset));
        if(length > 0 && length < (int)sizeof(value) - 1) {

            // trim leading whitespace
            while (' ' == colon[0]) {
                colon++;
                length--;
                /*if(colon > lineEnd || 0 == length) {
                 std::cout << "HTTP HEADER PARSE ERROR :: WHITESPACE ERROR" << std::endl;
                 return;
                 }*/
            }

            if(length > 0) {

                // trim trailing whitespace
                while (' ' == colon[length - 1]) {
                    length--;
                    if(0 == length) {
                        std::cout << "HTTP HEADER PARSE ERROR :: WHITESPACE ERROR" << std::endl;
                        return;
                    }
                }
                strncpy(value, colon, length);
            }
            value[length] = '\0';
        }
        else {
            std::cout << "HTTP HEADER PARSE ERROR :: KEY TOO SHORT/LONG" << std::endl;
            return;
        }

        m_headerBag[key] = value;

        offset += lineLength + 2;
        lineEnd = strstr((char*)data + offset, "\r\n");

        // get range request values
        if(strcmp(key, "range") == 0) {

            for(size_t i = 0; i < strlen(value); i++) {
                value[i] = tolower(value[i]);
            }

//std::cout << "RANGE 1: " << value << "*" << std::endl;

            RegEx rxRange("bytes=(\\d*)-(\\d*)");
            if(!rxRange.search(value)) {
                m_valid = false;

                std::cout << "RANGE SEARCH FAILED" << std::endl;

                return;
            }

            if(rxRange.match(1).length() > 0)
                m_rangeStart = strToOffT(rxRange.match(1));

            if(rxRange.match(2).length() > 0)
                m_rangeEnd = strToOffT(rxRange.match(2));

            // validate range values
            if(-1 == m_rangeStart && -1 == m_rangeEnd) {
                m_valid = false;
                return;
            }

            if(-1 != m_rangeEnd && m_rangeStart >= m_rangeEnd) {
                m_valid = false;
                return;
            }

            //std::cout << "RANGE 2: " <<  m_rangeStart << " TO " << m_rangeEnd << "*" << std::endl;

            /*
             http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html#sec14.27

             - The first 500 bytes (byte offsets 0-499, inclusive):  bytes=0-499
             - The second 500 bytes (byte offsets 500-999, inclusive): bytes=500-999
             - The final 500 bytes (byte offsets 9500-9999, inclusive): bytes=-500
             - Or bytes=9500-
             - The first and last bytes only (bytes 0 and 9999):  bytes=0-0,-1
             */

        }
    }

    m_valid = true;
}

fuppes_off_t HttpHeader::getValueAsOffT(std::string key)
{
    return strToOffT(this->getValue(key).c_str());
}

std::string HttpHeader::getValueAsLower(std::string key)
{
    std::map<std::string, std::string>::iterator iter;
    iter = m_headerBag.find(key);
    if(m_headerBag.end() == iter) {
        return "";
    }
    return ToLower(iter->second);
}

std::string HttpHeader::toString()
{
    std::string result;
    std::map<std::string, std::string>::iterator iter;
    for(iter = m_headerBag.begin(); iter != m_headerBag.end(); iter++) {
        result += iter->first + ": " + iter->second + "\r\n";
    }
    return result;
}
