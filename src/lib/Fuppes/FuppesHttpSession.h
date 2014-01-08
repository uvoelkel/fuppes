/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _FUPPES_HTTP_SESSION_H
#define _FUPPES_HTTP_SESSION_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Http/HttpServer.h"
#include "../Http/HttpSession.h"
#include "../Http/StatusCode.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

#include "../Security/AccessControl.h"
#include "../DeviceIdentification/IdentificationMgr.h"
#include "../GENA/SubscriptionMgr.h"
#include "../MediaServer/MediaServer.h"
#include "../Presentation/PresentationHandler.h"
#include "../ControlInterface/ControlInterface.h"

#include "../UPnP/UPnPObject.h"
#include "RequestItem.h"
#include "../Transcoding/Transcoder.h"

#include <iostream>

namespace Fuppes
{
    class FuppesHttpSession: public Http::HttpSession
    {        
        public:
            FuppesHttpSession(fuppes::TCPRemoteSocket*              remoteSocket,
                              Fuppes::FuppesConfig&                 fuppesConfig,
                              Security::AccessControl&              accessControl,
                              DeviceIdentification::IdentificationMgr&  deviceIdentMgr,
                              CSubscriptionMgr&                     subscriptionMgr,
                              MediaServer&                          mediaServer,
                              Transcoding::Transcoder&              transcoder,
                              Presentation::PresentationHandler&    presentationHandler,
                              ControlInterface&                     controlInterface
            );
            virtual ~FuppesHttpSession();

            static fuppes_off_t transcodeContentCallback(Http::HttpSession* httpSession, char** buffer, fuppes_off_t offset, bool* eof);
            
        protected:
            // make sure buffer is null terminated
            Http::Request* createRequest(const char* buffer, fuppes::TCPRemoteSocket* remoteSocket);
            Http::Response* createResponse(Http::HttpStatusCode status, std::string content = "");

            Http::Response* handleRequest(Http::Request* request);

            
            void onSessionStarted(std::string ip, int port);
            void onRequestStarted();
            void onRequest(Http::Request* request);
            void onValidRequest(Http::Request* request);
            void onResponse();
            void onRequestFinished();
            void onSessionFinished();
            
        private:
            Fuppes::HttpResponse* handleGet(Fuppes::HttpRequest* request);
            Fuppes::HttpResponse* handlePost(Fuppes::HttpRequest* request);
            Fuppes::HttpResponse* handleSoap(Fuppes::HttpRequest* request, std::string target, std::string action);

            Fuppes::HttpResponse* handleItemRequest(Fuppes::HttpRequest* request);
            Fuppes::HttpResponse* createItemResponse(Fuppes::RequestItem& item);

            Fuppes::HttpResponse* checkAlbumArtRequest(Fuppes::RequestItem& item, const std::string uriType, Fuppes::HttpRequest* request);
            Fuppes::HttpResponse* checkUriTranscoding(Fuppes::RequestItem& item, const std::string uriType, const std::string uriExt, Fuppes::HttpRequest* request);

            
            Fuppes::HttpResponse* handleSubscribe(Fuppes::HttpRequest* request);
            Fuppes::HttpResponse* handleUnsubscribe(Fuppes::HttpRequest* request);


            Fuppes::FuppesConfig&               m_fuppesConfig;

            Security::AccessControl&            m_accessControl;
            DeviceIdentification::IdentificationMgr&  m_deviceIdentMgr;
            CSubscriptionMgr&                   m_subscriptionMgr;
            MediaServer&                        m_mediaServer;
            Transcoding::Transcoder&            m_transcoder;
            Presentation::PresentationHandler&  m_presentationHandler;
            ControlInterface&                   m_controlInterface;
            
            Fuppes::RequestItem                 m_item;

            fuppes::File						m_fileTranscodingFile;
            unsigned char*						m_fileTranscodingBuffer;


            //char*                       m_transcodingBuffer;
            //fuppes_off_t                m_transcodingBufferSize;
            //CTranscodingCacheObject*    m_transcodingCacheObj;
            //CTranscodeSessionInfo*      m_transcodingSessionInfo;
            
    };
    

}

#endif // _FUPPES_HTTP_SESSION_H
