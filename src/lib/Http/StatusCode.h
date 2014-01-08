/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _HTTP_STATUSCODE_H
#define _HTTP_STATUSCODE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <map>
#include <string>

namespace Http
{
    typedef enum HttpStatusCode
    {
        OK = 200,
        PARTIAL_CONTENT = 206,

        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        NOT_ACCEPTABLE = 406,

        REQUEST_TIMEOUT = 408,

        REQUEST_ENTITY_TOO_LARGE = 413,
        REQUEST_URI_TOO_LONG = 414,
        UNSUPPORTED_MEDIA_TYPE = 415,
        REQUEST_RANGE_NOT_SATISFIABLE = 416,

        IM_A_TEAPOT = 418,

        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        SERVICE_UNAVAILABLE = 503,
        HTTP_VERSION_NOT_SUPPORTED = 505

    } HttpStatusCode;

    struct HttpStatusText
    {
            static std::map<Http::HttpStatusCode, std::string> create()
            {

                std::map<Http::HttpStatusCode, std::string> map;

                map[Http::OK] = "OK";
                map[Http::PARTIAL_CONTENT] = "Partial Content";

                map[Http::BAD_REQUEST] = "Bad Request";
                map[Http::UNAUTHORIZED] = "Unauthorized";
                map[Http::FORBIDDEN] = "Forbidden";
                map[Http::NOT_FOUND] = "Not Found";
                map[Http::METHOD_NOT_ALLOWED] = "Method Not Allowed";
                map[Http::NOT_ACCEPTABLE] = "Not Acceptable";

                map[Http::REQUEST_TIMEOUT] = "Request Timeout";

                map[Http::REQUEST_ENTITY_TOO_LARGE] = "Request Entity Too Large";
                map[Http::REQUEST_URI_TOO_LONG] = "Request-URI Too Long";
                map[Http::UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
                map[Http::REQUEST_RANGE_NOT_SATISFIABLE] = "Requested Range Not Satisfiable";

                map[Http::IM_A_TEAPOT] = "I'm a Teapot";

                map[Http::INTERNAL_SERVER_ERROR] = "Internal Server Error";
                map[Http::NOT_IMPLEMENTED] = "Not Implemented";
                map[Http::SERVICE_UNAVAILABLE] = "Service Unavailable";
                map[Http::HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported";

                return map;
            }
            static const std::map<Http::HttpStatusCode, std::string> m_map;

            static std::string get(Http::HttpStatusCode code)
            {
                std::map<Http::HttpStatusCode, std::string>::const_iterator iter;
                if ((iter = HttpStatusText::m_map.find(code)) != HttpStatusText::m_map.end()) {
                    return iter->second;
                }
                return "";
            }

    };

}

#endif // _HTTP_STATUSCODE_H
