/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _HTTP_RESPONSE_FILE_H
#define _HTTP_RESPONSE_FILE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Response.h"
#include "../Common/File.h"

namespace Http
{
    class BaseResponseFile
    {
        public:
            BaseResponseFile(fuppes::File* file = NULL);
            BaseResponseFile(std::string filename);

            virtual ~BaseResponseFile();
                
            void setFile(fuppes::File* file);
            void setFilename(std::string filename);

            fuppes_off_t    getContentSize();
            
            // returns the size of the buffer starting with offset
            fuppes_off_t    getContent(Http::HttpSession* session, char** buffer, fuppes_off_t offset, fuppes_off_t size, bool* eof);

        private:
            fuppes::File*       m_file;
            std::string         m_filename;

            char*           m_contentBuffer;
            fuppes_off_t    m_contentBufferSize;
    };

    class ResponseFile: public Http::Response, public BaseResponseFile
    {
        public:
            ResponseFile(Http::HttpStatusCode status = Http::OK, std::string contentType = "", fuppes::File* file = NULL):
            Http::Response(status, contentType),
            Http::BaseResponseFile(file) {
            }
            
            ResponseFile(Http::HttpStatusCode status, std::string contentType, std::string filename):
            Http::Response(status, contentType),
            Http::BaseResponseFile(filename) {
            }

            bool            hasContentLength() { return true; }
            bool            acceptRanges() { return true; }

            fuppes_off_t    getContentSize() {
                return BaseResponseFile::getContentSize(); 
            }
            
            fuppes_off_t    getContent(Http::HttpSession* session, char** buffer, fuppes_off_t offset, fuppes_off_t size, bool* eof) {
                return BaseResponseFile::getContent(session, buffer, offset, size, eof);
            }
    };

}

#endif // _HTTP_RESPONSE_FILE_H
