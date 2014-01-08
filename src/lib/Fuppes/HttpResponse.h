/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _FUPPES_HTTP_RESPONSE_H
#define _FUPPES_HTTP_RESPONSE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Http/Response.h"
#include "../Http/ResponseFile.h"
#include "../Http/ResponseCallback.h"

#include "../DeviceIdentification/DeviceSettings.h"

namespace Fuppes
{

    class FuppesHttpSession;

    class HttpResponse: public Http::Response
    {
        public:
            HttpResponse(Http::HttpStatusCode status = Http::OK, std::string contentType = "", std::string content = "") :
                    Http::Response(status, contentType, content)
            {
                m_deviceSettings = NULL;
            }

            CDeviceSettings* getDeviceSettings()
            {
                return m_deviceSettings;
            }
            void setDeviceSettings(CDeviceSettings* settings)
            {
                m_deviceSettings = settings;
            }

        private:
            CDeviceSettings* m_deviceSettings;
    };

    class HttpResponseFile: public HttpResponse, public Http::BaseResponseFile
    {
        public:
            HttpResponseFile(Http::HttpStatusCode status = Http::OK, std::string contentType = "", fuppes::File* file = NULL) :
                    HttpResponse(status, contentType),
                    Http::BaseResponseFile(file)
            {

            }

            HttpResponseFile(Http::HttpStatusCode status, std::string contentType, std::string filename) :
                    HttpResponse(status, contentType),
                    Http::BaseResponseFile(filename)
            {

            }

            bool hasContentLength()
            {
                return true;
            }
            bool acceptRanges()
            {
                return true;
            }

            fuppes_off_t getContentSize()
            {
                return BaseResponseFile::getContentSize();
            }

            fuppes_off_t getContent(Http::HttpSession* session, char** buffer, fuppes_off_t offset, fuppes_off_t size, bool* eof)
            {
                return BaseResponseFile::getContent(session, buffer, offset, size, eof);
            }
    };

    class HttpResponseCallback: public HttpResponse, public Http::BaseResponseCallback
    {
        public:
            HttpResponseCallback(Http::HttpStatusCode status = Http::OK, std::string contentType = "", Http::contentCallback_t callback = NULL) :
                    HttpResponse(status, contentType),
                    Http::BaseResponseCallback(callback)
            {

            }

            bool hasContentLength()
            {
                return false;
            }
            bool acceptRanges()
            {
                return false;
            }

            fuppes_off_t getContentSize()
            {
                return BaseResponseCallback::getContentSize();
            }

            fuppes_off_t getContent(Http::HttpSession* session, char** buffer, fuppes_off_t offset, fuppes_off_t size, bool* eof)
            {
                return BaseResponseCallback::getContent(session, buffer, offset, size, eof);
            }
    };

}

#endif // _FUPPES_HTTP_RESPONSE_H
