/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            HTTPMessage.h
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _HTTPMESSAGE_H
#define _HTTPMESSAGE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Http/Request.h"
#include "../Http/Response.h"

#include "../Common/Common.h"
#include "../Common/File.h"
//#include "../DeviceSettings/DeviceSettings.h"
//#include "../UPnPActions/UPnPAction.h"
//#include "../Transcoding/TranscodingCache.h"
//#include "../ContentDirectory/FileDetails.h"
//#include "../ContentDirectory/DatabaseObject.h"
//#include "../ContentDirectory/DatabaseConnection.h"

#include <string>
#include <iostream>
#include <fstream>
#include <map>

#ifndef WIN32
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif

typedef enum tagHTTP_VERSION
{
    HTTP_VERSION_UNKNOWN = 0, HTTP_VERSION_1_0 = 1, HTTP_VERSION_1_1 = 2
} HTTP_VERSION;

typedef enum tagHTTP_MESSAGE_TYPE
{
    HTTP_MESSAGE_TYPE_UNKNOWN = 0,

    /* HTTP 1.0 and 1.1 message types */
    HTTP_MESSAGE_TYPE_GET = 1, HTTP_MESSAGE_TYPE_HEAD = 2, HTTP_MESSAGE_TYPE_POST = 3, HTTP_MESSAGE_TYPE_200_OK = 4, HTTP_MESSAGE_TYPE_206_PARTIAL_CONTENT = 5,

    HTTP_MESSAGE_TYPE_400_BAD_REQUEST = 6, HTTP_MESSAGE_TYPE_403_FORBIDDEN = 7, HTTP_MESSAGE_TYPE_404_NOT_FOUND = 8, HTTP_MESSAGE_TYPE_500_INTERNAL_SERVER_ERROR = 9,

    /* SOAP */
    HTTP_MESSAGE_TYPE_POST_SOAP_ACTION = 10,

    /* GENA message types */
    HTTP_MESSAGE_TYPE_SUBSCRIBE = 11, HTTP_MESSAGE_TYPE_UNSUBSCRIBE = 12, HTTP_MESSAGE_TYPE_GENA_OK = 13, HTTP_MESSAGE_TYPE_NOTIFY = 14

} HTTP_MESSAGE_TYPE;

typedef enum tagHTTP_CONNECTION
{
    HTTP_CONNECTION_UNKNOWN, HTTP_CONNECTION_CLOSE
} HTTP_CONNECTION;

/*
 SUBSCRIBE publisher path HTTP/1.1
 HOST: publisher host:publisher port
 CALLBACK: <delivery URL>
 NT: upnp:event
 TIMEOUT: Second-requested subscription duration


 HTTP/1.1 200 OK
 DATE: when response was generated
 SERVER: OS/version UPnP/1.0 product/version
 SID: uuid:subscription-UUID
 TIMEOUT: Second-actual subscription duration
 */

/*typedef enum tagHTTP_CONTENT_TYPE
 {
 HTTP_CONTENT_TYPE_UNKNOWN       =  0,
 HTTP_CONTENT_TYPE_TEXT_HTML     =  1,
 HTTP_CONTENT_TYPE_TEXT_XML      =  2,
 HTTP_CONTENT_TYPE_AUDIO_MPEG    =  3,
 HTTP_CONTENT_TYPE_IMAGE_PNG     =  4,
 HTTP_CONTENT_TYPE_IMAGE_JPEG    =  5
 }HTTP_CONTENT_TYPE;*/

typedef enum HTTP_TRANSFER_ENCODING
{
    HTTP_TRANSFER_ENCODING_NONE, HTTP_TRANSFER_ENCODING_CHUNKED
} HTTP_TRANSFER_ENCODING;

//class CHTTPMessage;

namespace fuppes
{
    /*

    class HTTPHeader
    {

        public:
            HTTPHeader()
            {
                m_version = HTTP_VERSION_UNKNOWN;
                m_contentLength = 0;
                m_hasContentLength = false;
            }

            HTTP_VERSION version()
            {
                return m_version;
            }

            fuppes_off_t contentLength()
            {
                return m_contentLength;
            }
            bool hasContentLength()
            {
                return m_hasContentLength;
            }

        private:
            HTTP_VERSION m_version;

            fuppes_off_t m_contentLength;
            bool m_hasContentLength;

    };

*/

    typedef enum HTTP_REQUEST_TYPE
    {
        HTTP_REQUEST_TYPE_UNKNOWN = 0,

        HTTP_REQUEST_TYPE_GET = 1, HTTP_REQUEST_TYPE_HEAD = 2, HTTP_REQUEST_TYPE_POST = 3, HTTP_REQUEST_TYPE_POST_SOAP_ACTION = 4,

        HTTP_REQUEST_TYPE_SUBSCRIBE = 5, HTTP_REQUEST_TYPE_UNSUBSCRIBE = 6, HTTP_REQUEST_TYPE_NOTIFY = 7

    } HTTP_REQUEST_TYPE;

    /*
    class HTTPRequestHeader: public HTTPHeader
    {
    };
    */

    typedef enum HTTP_RESPONSE_TYPE
    {
        HTTP_RESPONSE_TYPE_UNKNOWN = 0,

        HTTP_RESPONSE_TYPE_200_OK = 1, HTTP_RESPONSE_TYPE_206_PARTIAL_CONTENT = 2,

        HTTP_RESPONSE_TYPE_400_BAD_REQUEST = 3, HTTP_RESPONSE_TYPE_403_FORBIDDEN = 4, HTTP_RESPONSE_TYPE_404_NOT_FOUND = 5, HTTP_RESPONSE_TYPE_500_INTERNAL_SERVER_ERROR = 6,

        HTTP_RESPONSE_TYPE_GENA_OK = 7

    } HTTP_RESPONSE_TYPE;
/*
    class HTTPResponseHeader: public HTTPHeader
    {
    };
*/
}

//class CUPnPBrowse;

class CHTTPMessage: public Http::Request, public Http::Response
{
        friend class CHTTPParser;

    public:
        CHTTPMessage();
        ~CHTTPMessage();

        bool SetHeader(std::string p_sHeader);
        void SetMessage(HTTP_MESSAGE_TYPE nMsgType, std::string p_sContentType);
        bool SetMessage(std::string p_sMessage);

        std::string GetContent()
        {
            return m_sContent;
        }
        std::string GetMessage()
        {
            return m_sMessage;
        }
        std::string GetHeader()
        {
            return m_sHeader;
        }

        std::string GetRequest()
        {
            return m_sRequest;
        }
        std::string GetContentType()
        {
            return m_sHTTPContentType;
        }
        HTTP_MESSAGE_TYPE GetMessageType()
        {
            return m_nHTTPMessageType;
        }
        HTTP_VERSION GetVersion()
        {
            return m_nHTTPVersion;
        }
        //std::string       GetContent()          { return m_sContent;          }
        fuppes_off_t GetBinContentLength();
        char* GetBinContent()
        {
            return m_sBuffer;
        }

        bool IsBinary()
        {
            return m_bIsBinary;
        }
        void SetIsBinary(bool p_bIsBinary)
        {
            m_bIsBinary = p_bIsBinary;
        }

        HTTP_TRANSFER_ENCODING GetTransferEncoding()
        {
            return m_nTransferEncoding;
        }
        void SetTransferEncoding(HTTP_TRANSFER_ENCODING p_nTransferEncoding)
        {
            m_nTransferEncoding = p_nTransferEncoding;
        }

        //CUPnPAction* GetAction();
        std::string GetHeaderAsString();
        std::string GetMessageAsString();

        unsigned int GetBinContentChunk(char* p_sContentChunk, unsigned int p_nSize, fuppes_off_t p_nOffset);

        fuppes_off_t GetRangeStart()
        {
            return m_nRangeStart;
        }
        fuppes_off_t GetRangeEnd()
        {
            return m_nRangeEnd;
        }
        bool hasRange()
        {
            return m_hasRange;
        }
        void SetRangeStart(fuppes_off_t p_nRangeStart)
        {
            m_nRangeStart = p_nRangeStart;
        }
        void SetRangeEnd(fuppes_off_t p_nRangeEnd)
        {
            m_nRangeEnd = p_nRangeEnd;
        }
        HTTP_CONNECTION GetHTTPConnection()
        {
            return m_nHTTPConnection;
        }

        bool PostVarExists(std::string p_sPostVarName);
        std::string GetPostVar(std::string p_sPostVarName);

        bool GetVarExists(std::string key);
        std::string getGetVar(std::string key);

        std::string getVarAsStr(std::string key);
        int getVarAsInt(std::string key);

        void SetMessageType(HTTP_MESSAGE_TYPE p_nHTTPMessageType)
        {
            m_nHTTPMessageType = p_nHTTPMessageType;
        }
        void SetVersion(HTTP_VERSION p_nHTTPVersion)
        {
            m_nHTTPVersion = p_nHTTPVersion;
        }
        void SetContentType(std::string p_sContentType)
        {
            m_sHTTPContentType = p_sContentType;
        }
        void SetContent(std::string p_sContent)
        {
            m_sContent = p_sContent;
        }
        void SetBinContent(char* p_szBinContent, fuppes_off_t p_nBinContenLength);

        std::string GetGENASubscriptionID()
        {
            return m_sGENASubscriptionID;
        }
        void SetGENASubscriptionID(std::string p_sSubscriptionID)
        {
            m_sGENASubscriptionID = p_sSubscriptionID;
        }

        std::string soapTarget()
        {
            return m_soapTarget;
        }
        std::string soapAction()
        {
            return m_soapAction;
        }

        // getcontentFeatures.dlna.org: 1
        void dlnaGetContentFeatures(bool get)
        {
            m_dlnaGetContentFeatures = get;
        }
        bool dlnaGetContentFeatures()
        {
            return m_dlnaGetContentFeatures;
        }

        // contentFeatures.dlna.org: *
        void dlnaContentFeatures(std::string features)
        {
            m_dlnaContentFeatures = features;
        }
        std::string dlnaContentFeatures()
        {
            return m_dlnaContentFeatures;
        }

        // transferMode.dlna.org: Interactive
        enum
        {
            Streaming, Interactive, Background
        } dlnaTransferMode_t;

        void dlnaTransferMode(std::string mode)
        {
            m_dlnaTransferMode = mode;
        }
        std::string dlnaTransferMode()
        {
            return m_dlnaTransferMode;
        }

        // dlna 1.0 clients may not send the transferMode header.
        // in that case we have to treat it as "Streaming" for Audio and Video objects
        // and "Interactive" for all other binaries

        // getCaptionInfo.sec: 1
        bool secGetCaptionInfo()
        {
            return m_secGetCaptionInfo;
        }

        bool LoadContentFromFile(std::string);
        //bool             TranscodeContentFromFile(std::string p_sFileName, CSQLResult* sqlResult);
        //void BreakTranscoding();
        //bool IsTranscoding();

        /*CDeviceSettings* DeviceSettings()
        {
            return m_pDeviceSettings;
        }
        void DeviceSettings(CDeviceSettings* pSettings)
        {
            m_pDeviceSettings = pSettings;
        }*/
        /*
        std::string virtualFolderLayout()
        {
            return m_virtualFolderLayout;
        }
        void setVirtualFolderLayout(std::string layout)
        {
            m_virtualFolderLayout = layout;
        }
        */

        std::string GetRemoteIPAddress();
        void SetLocalEndPoint(sockaddr_in);
        void SetRemoteEndPoint(sockaddr_in);
        sockaddr_in GetLocalEndPoint()
        {
            return m_LocalEp;
        }
        sockaddr_in GetRemoteEndPoint()
        {
            return m_RemoteEp;
        }
    public:
        // buffer
        char* m_sBuffer;
        // the buffer's size
        unsigned int m_nBufferSize;
        // buffer bytes consumed
        unsigned int m_nBytesConsumed;

        fuppes_off_t m_nBinContentLength;

        bool m_bIsBinary;

        std::string m_sUserAgent;

        /*
#ifndef DISABLE_TRANSCODING
        CTranscodeSessionInfo* m_pTranscodingSessionInfo;
        CTranscodingCacheObject* m_pTranscodingCacheObj;
#endif
*/
        //fuppesThreadMutex TranscodeMutex;

    private:
        bool BuildFromString(std::string p_sMessage);

        // Header information: [HTTP 1.0|HTTP 1.1]
        HTTP_VERSION m_nHTTPVersion;
        // Message type
        HTTP_MESSAGE_TYPE m_nHTTPMessageType;
        // Header information: Content type
        std::string m_sHTTPContentType;
        // Header information: HTTP request line
        std::string m_sRequest;
        // Header information: content length
        int m_nContentLength;
        // Header information: Connection [close|keep alive]
        HTTP_CONNECTION m_nHTTPConnection;

        // Header information: Call-Back (GENA - Request)
        std::string m_sGENACallBack;
        // Header information: NT (notification type) (GENA - Request)
        std::string m_sGENANT;
        // Header information: Timeout (GENA - Request)
        std::string m_sGENATimeout;
        // Header information: Subscription-ID (GENA - Request & Response)
        std::string m_sGENASubscriptionID;

        std::string m_soapTarget;
        std::string m_soapAction;

        bool m_dlnaGetContentFeatures;
        std::string m_dlnaContentFeatures;
        std::string m_dlnaTransferMode;

        bool m_secGetCaptionInfo;

        HTTP_TRANSFER_ENCODING m_nTransferEncoding;

        //CUPnPAction* m_pUPnPAction;
        //std::fstream       m_fsFile;
        fuppes::File m_file;
        fuppes_off_t m_nRangeStart;
        fuppes_off_t m_nRangeEnd;
        bool m_hasRange;
        fuppes_off_t m_nBinContentPosition;

        std::map<std::string, std::string> m_getVars;
        std::map<std::string, std::string>::iterator m_getVarsIter;

        //CDeviceSettings* m_pDeviceSettings;
        //std::string m_virtualFolderLayout;

        std::string m_sContent;
        std::string m_sHeader;
        std::string m_sMessage;
        sockaddr_in m_LocalEp;
        sockaddr_in m_RemoteEp;

        bool ParsePOSTMessage(std::string p_sMessage);
        bool ParseSUBSCRIBEMessage(std::string p_sMessage);

};

#endif // _HTTPMESSAGE_H
