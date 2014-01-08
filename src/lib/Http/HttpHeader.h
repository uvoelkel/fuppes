/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _HTTP_HEADER_H
#define _HTTP_HEADER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_types.h>

#include <map>
#include <string>
#include <sstream>

namespace Http
{
    class HttpHeader
    {
        public:
            HttpHeader() {
                m_valid = true;
                m_rangeStart = -1;
                m_rangeEnd = -1;
            }
            
            // make sure data is null terminated
            // data = the header data after the first line
            // end  = the position of the closing \r\n\r\n
            void parse(const char* data, const char* end);

            bool isValid() { return m_valid; }

            bool keyExists(std::string key) {
                std::map<std::string, std::string>::iterator iter;
                iter = m_headerBag.find(key);
                return (iter != m_headerBag.end());
            }

            std::string getValue(std::string key) {
                std::map<std::string, std::string>::iterator iter;
                iter = m_headerBag.find(key);
                if(m_headerBag.end() == iter)
                    return "";
                return iter->second;
            }

            fuppes_off_t getValueAsOffT(std::string key);

            std::string getValueAsLower(std::string key);
            
            void setValue(const std::string key, const std::string value) {
                m_headerBag[key] = value;
            }

            void setValueAsOffT(const std::string key, const fuppes_off_t value) {
                std::stringstream stream;
                stream << value;
                m_headerBag[key] = stream.str();
            }

            fuppes_off_t getRangeStart() const { return m_rangeStart; }
            fuppes_off_t getRangeEnd() const { return m_rangeEnd; }
            
            std::map<std::string, std::string>* getHeaderBag() {
                return &m_headerBag;
            }

            std::string toString();
                
        private:
            bool            m_valid;
            
            fuppes_off_t    m_rangeStart;
            fuppes_off_t    m_rangeEnd;
            
            std::map<std::string, std::string>  m_headerBag;  
    };

}

#endif // _HTTP_HEADER_H
