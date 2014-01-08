/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "FuppesHttpSession.h"

#include "../Log/Log.h"

#include "../Fuppes/Core.h"

#include "../UPnPActions/UPnPActionFactory.h"

#include "../ContentDirectory/ContentDirectory.h"
#include "../ConnectionManager/ConnectionManager.h"
#include "../XMSMediaReceiverRegistrar/XMSMediaReceiverRegistrar.h"
#include "../ControlInterface/SoapControl.h"

#include "../Dlna/DLNA.h"

#include "../Common/RegEx.h"

#include "../Database/ObjectManager.h"

using namespace Fuppes;

#include <iostream>
using namespace std;

FuppesHttpSession::FuppesHttpSession(fuppes::TCPRemoteSocket* remoteSocket, Fuppes::FuppesConfig& fuppesConfig, Security::AccessControl& accessControl, DeviceIdentification::IdentificationMgr& deviceIdentMgr, CSubscriptionMgr& subscriptionMgr, MediaServer& mediaServer,
        Transcoding::Transcoder& transcoder, Presentation::PresentationHandler& presentationHandler, ControlInterface& controlInterface) :
        Http::HttpSession(remoteSocket),

        m_fuppesConfig(fuppesConfig),
        m_accessControl(accessControl),
        m_deviceIdentMgr(deviceIdentMgr),
        m_subscriptionMgr(subscriptionMgr),
        m_mediaServer(mediaServer),
        m_transcoder(transcoder),
        m_presentationHandler(presentationHandler),
        m_controlInterface(controlInterface)
{
    //m_transcodingBuffer = NULL;
    //m_transcodingBufferSize = 0;
    //m_transcodingSessionInfo = NULL;

	m_fileTranscodingBuffer = NULL;
}

FuppesHttpSession::~FuppesHttpSession()
{
    m_transcoder.release(m_item);

    if (NULL != m_fileTranscodingBuffer) {
    	//delete[] m_fileTranscodingBuffer;
    	free(m_fileTranscodingBuffer);
    }

    if (m_fileTranscodingFile.isOpen()) {
    	m_fileTranscodingFile.close();
    }

    /*if (NULL != m_transcodingBuffer) {
        free(m_transcodingBuffer);
        m_transcodingBuffer = NULL;
    }*/

    /*if (NULL != m_transcodingSessionInfo) {
        delete m_transcodingSessionInfo;
        m_transcodingSessionInfo = NULL;
    }*/
}

Http::Request* FuppesHttpSession::createRequest(const char* buffer, fuppes::TCPRemoteSocket* remoteSocket)
{
    Fuppes::HttpRequest* tmp = new Fuppes::HttpRequest();
    Fuppes::HttpRequest* result = (Fuppes::HttpRequest*)Http::Request::create(buffer, tmp);
    if (NULL == result) {
        delete tmp;
    }
    else {
        result->setRemoteSocket(remoteSocket);
        m_deviceIdentMgr.identifyDevice(result);
    }
    return result;
}

Http::Response* FuppesHttpSession::createResponse(Http::HttpStatusCode status, std::string content /*= ""*/)
{
    Fuppes::HttpResponse* response = new Fuppes::HttpResponse(status, content);

    Fuppes::HttpRequest* request = (Fuppes::HttpRequest*)this->getHttpRequest();
    if (NULL != request) {
        response->setDeviceSettings(request->getDeviceSettings());
        response->getHeader()->setValue("Server", request->getDeviceSettings()->getHttpServerSignature());
    }
    return response;
}

Http::Response* FuppesHttpSession::handleRequest(Http::Request* req)
{
    Fuppes::HttpRequest* request = (Fuppes::HttpRequest*)req;
    Fuppes::HttpResponse* response = NULL;

    /*
     cout << "HTTP REQUEST: " << request->getRemoteSocket()->remoteAddress() << endl;
     cout << request->getMethod() << endl;
     cout << request->getUri() << endl;

     std::map<std::string, std::string>* bag = request->getHeader()->getHeaderBag();
     std::map<std::string, std::string>::iterator iter;
     for(iter = bag->begin(); iter != bag->end(); iter++) {
     cout << iter->first << " = " << iter->second << "*" << endl;
     }
     cout << endl;
     */

    std::string ip = request->getRemoteSocket()->remoteAddress();
    if (!m_accessControl.isAllowed(ip)) {
        logExt("http") << "request from:" << ip << "forbidden";
        return this->createResponse(Http::FORBIDDEN);
    }

    if (request->getMethod().compare("GET") == 0 || request->getMethod().compare("HEAD") == 0) {
        response = this->handleGet(request);
    }
    else if (request->getMethod().compare("POST") == 0) {
        response = this->handlePost(request);
    }
    else if (request->getMethod().compare("SUBSCRIBE") == 0) {
        response = this->handleSubscribe(request);
    }
    else if (request->getMethod().compare("UNSUBSCRIBE") == 0) {
        response = this->handleUnsubscribe(request);
    }

    if (NULL == response) {
        response = new HttpResponse(Http::NOT_IMPLEMENTED);
    }

    // set the device settings
    response->setDeviceSettings(request->getDeviceSettings());

    // set the server signature
    response->getHeader()->setValue("Server", request->getDeviceSettings()->getHttpServerSignature());

    return response;
}

Fuppes::HttpResponse* FuppesHttpSession::handleGet(Fuppes::HttpRequest* request)
{
    Fuppes::HttpResponse* response = NULL;

    if ((request->getUri().length() > 7) && (request->getUri().substr(0, 7).compare("/video/") == 0)) {
        response = this->handleItemRequest(request);
    }
    else if ((request->getUri().length() > 7) && (request->getUri().substr(0, 7).compare("/audio/") == 0)) {
        response = this->handleItemRequest(request);
    }
    else if ((request->getUri().length() > 7) && (request->getUri().substr(0, 7).compare("/image/") == 0)) {
        response = this->handleItemRequest(request);
    }
    else if (request->getUri().compare("/description.xml") == 0) {

        response = new HttpResponse(Http::OK, "text/xml");
        response->setContent(m_mediaServer.localDeviceDescription(request->getDeviceSettings()));
    }
    else if (request->getUri().compare("/UPnPServices/ContentDirectory/description.xml") == 0) {

        response = new HttpResponse(Http::OK, "text/xml");

        CContentDirectory dir;
        dir.setHttpServerUrl(this->getHttpServer()->getUrl());
        response->setContent(dir.getServiceDescription());
    }
    else if (request->getUri().compare("/UPnPServices/ConnectionManager/description.xml") == 0) {

        response = new HttpResponse(Http::OK, "text/xml");

        CConnectionManager mgr;
        mgr.setHttpServerUrl(this->getHttpServer()->getUrl());
        response->setContent(mgr.getServiceDescription());
    }
    else if (request->getUri().compare("/UPnPServices/XMSMediaReceiverRegistrar/description.xml") == 0) {

        response = new HttpResponse(Http::OK, "text/xml");

        XMSMediaReceiverRegistrar reg;
        reg.setHttpServerUrl(this->getHttpServer()->getUrl());
        response->setContent(reg.getServiceDescription());
    }
    else if (request->getUri().compare("/UPnPServices/SoapControl/description.xml") == 0) {

        response = new HttpResponse(Http::OK, "text/xml");

        SoapControl soap(this->getHttpServer()->getUrl(), &m_controlInterface);
        response->setContent(soap.getServiceDescription());
    }

    else if (request->getUri().compare("/") == 0 || request->getUri().compare("/index.html") == 0 || request->getUri().compare("/favicon.ico") == 0 || (request->getUri().length() > 14 && request->getUri().substr(0, 14).compare("/presentation/") == 0)) {
        response = m_presentationHandler.handleRequest(request);
    }

    if (NULL != response) {
        return response;
    }
    else {
        return new Fuppes::HttpResponse(Http::NOT_FOUND);
    }
}

Fuppes::HttpResponse* FuppesHttpSession::handlePost(Fuppes::HttpRequest* request)
{
    Fuppes::HttpResponse* response = NULL;

    if (request->getHeader()->keyExists("soapaction")) {

        size_t pos;
        string tmp = request->getHeader()->getValue("soapaction");
        if ((pos = tmp.find("#")) == std::string::npos) {
            return new Fuppes::HttpResponse(Http::BAD_REQUEST);
        }

        string target = tmp.substr(1, pos - 1);
        string action = tmp.substr(pos + 1);
        action.erase(action.length() - 1);

        response = this->handleSoap(request, target, action);
    }

    if (0 == request->getUri().substr(0, 14).compare("/presentation/")) {
        response = m_presentationHandler.handleRequest(request);
    }

    if (NULL != response) {
        return response;
    }
    else {
        return new Fuppes::HttpResponse(Http::NOT_FOUND);
    }
}

Fuppes::HttpResponse* FuppesHttpSession::handleSoap(Fuppes::HttpRequest* request, std::string target, std::string action)
{
    //cout << "SOAP: " << target << " ### " << action << "*" << endl;

    CUPnPAction* upnpAction = CUPnPActionFactory::buildActionFromString(request->getContent(), request->getDeviceSettings(), request->getVfolderLayout(), request->getVfoldersEnabled());
    if (NULL == upnpAction) {
        return new Fuppes::HttpResponse(Http::NOT_IMPLEMENTED);
    }

    Fuppes::HttpResponse* response = NULL;

    // content directory
    if (target.compare("urn:schemas-upnp-org:service:ContentDirectory:1") == 0) {

        CContentDirectory cdir;
        cdir.setHttpServerUrl(this->getHttpServer()->getUrl());
        response = cdir.handleAction(upnpAction, action, request);
    }
    // connection manager
    else if (target.compare("urn:schemas-upnp-org:service:ConnectionManager:1") == 0) {

        CConnectionManager cman;
        cman.setHttpServerUrl(this->getHttpServer()->getUrl());
        response = cman.handleAction(upnpAction, action, request);
    }
    // XMSMediaReceiverRegistrar
    else if (target.compare("urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1") == 0) {

        XMSMediaReceiverRegistrar xreg;
        xreg.setHttpServerUrl(this->getHttpServer()->getUrl());
        response = xreg.handleAction(upnpAction, action, request);
    }
    // SoapControl
    else if (target.compare("urn:fuppes:service:SoapControl:1") == 0) {

        SoapControl soap(this->getHttpServer()->getUrl(), &m_controlInterface);
        response = soap.handleAction(upnpAction, action, request);
    }

    delete upnpAction;

    if (NULL != response) {
        return response;
    }
    else {
        return new Fuppes::HttpResponse(Http::NOT_IMPLEMENTED);
    }
}

Fuppes::HttpResponse* FuppesHttpSession::handleItemRequest(Fuppes::HttpRequest* request)
{
    logExt("http") << "FuppesHttpSession::handleItemRequest: " << request->getUri();

    Fuppes::HttpResponse* response = NULL;

    if (0 == request->getUri().compare(m_item.uri)) {

        response = this->createItemResponse(m_item);
        if (Transcoding::Item::Memory == m_item.transcodeTarget && !m_item.isThreaded()) {
            response->setContent((const char*)m_item.buffer, m_item.bufferSize);
        }
        return response;
    }
    else {
    	m_transcoder.release(m_item);
        m_item.clear();
    }

    // parse the url
    RegEx rxUri("/(audio|video|image)/([0-9|A-F|a-f]+)/*[\\w|%20|-]*\\.(\\w+)");
    if (!rxUri.search(request->getUri())) {
        return NULL;
    }

    object_id_t objectId = HexToInt(rxUri.match(2));
    string uriType = rxUri.match(1);
    string uriExt = rxUri.match(3);

    string vfolder = request->getVfolderLayout();
    if (request->getGetVar("vfolder", vfolder)) {
        if (0 == vfolder.compare("none")) {
            vfolder = "";
        }
    }

    // get the object from the db
    Database::ObjectManager mgr;
    if (false == mgr.findOne(objectId, vfolder, m_item, true)) {
        return new Fuppes::HttpResponse(Http::NOT_FOUND);
    }

    // check if the file exists
    string path = m_item.path + m_item.filename;
    if (!fuppes::File::exists(path)) {
        return new Fuppes::HttpResponse(Http::NOT_FOUND);
    }

    // save the original extension and path
    m_item.originalExtension = m_item.extension;
    m_item.originalPath = path;
    m_item.targetPath = path;
    m_item.uri = request->getUri();
    m_item.deviceSettings = request->getDeviceSettings();

    // check for an album art request
    response = this->checkAlbumArtRequest(m_item, uriType, request);
    if (NULL != response) {
        return response;
    }

    // check for transcoding by url parameters
    response = this->checkUriTranscoding(m_item, uriType, uriExt, request);
    if (NULL != response) {
        return response;
    }

    // check for transcoding by device settings
    if (0 == m_item.transcodeSteps.size() && request->getDeviceSettings()->DoTranscode(m_item.extension, m_item.details->audioCodec, m_item.details->videoCodec)) {

        TRANSCODING_TYPE tt = request->getDeviceSettings()->GetTranscodingType(m_item.extension);
        switch (tt) {
            case TT_NONE:
            case TT_RENAME:
                break;
            case TT_THREADED_TRANSCODER:
            case TT_THREADED_DECODER_ENCODER:
                m_item.transcodeSteps.push_back(Transcoding::Item::Threaded);
                break;
            case TT_TRANSCODER:
                m_item.transcodeSteps.push_back(Transcoding::Item::Oneshot);
                break;
        }

        // get target mime type and extension
        m_item.mimeType = request->getDeviceSettings()->MimeType(m_item.extension, m_item.details->audioCodec, m_item.details->videoCodec);
        m_item.extension = request->getDeviceSettings()->Extension(m_item.extension, m_item.details->audioCodec, m_item.details->videoCodec);
    }

    // get device specific mime type and extension
    if (0 == m_item.transcodeSteps.size()) {
        m_item.mimeType = request->getDeviceSettings()->MimeType(m_item.extension);
        m_item.extension = request->getDeviceSettings()->Extension(m_item.extension);
    }

    // initialize transcoding
    if (0 != m_item.transcodeSteps.size()) {
        if (false == m_transcoder.initializeTranscoding(m_item)) {
            return new Fuppes::HttpResponse(Http::INTERNAL_SERVER_ERROR);
        }
    }

    // apply dlna values
    if (CMediaServerSettings::dlna_none != request->getDeviceSettings()->MediaServerSettings()->DlnaVersion) {
        Dlna::DLNA::buildProfile(m_item);
        Dlna::DLNA::build4thField(m_item);
        Dlna::DLNA::setHttpHeaderValues(m_item, request, response);
    }

    // create the response
    response = this->createItemResponse(m_item);

    if (0 == request->getMethod().compare("GET") && 0 < m_item.transcodeSteps.size()) {

        // execute non threaded resp. start threaded transcoding
        if (false == m_transcoder.executeTranscoding(m_item)) {
            delete response;
            return new Fuppes::HttpResponse(Http::INTERNAL_SERVER_ERROR);
        }

        // set the transcoded content for non threaded transcoding to memory
        if (Transcoding::Item::Memory == m_item.transcodeTarget && !m_item.isThreaded()) {
            response->setContent((const char*)m_item.buffer, m_item.bufferSize);
        }

        /*if (RESPONSE_CHUNKED == request->getDeviceSettings()->TranscodingHTTPResponse(ext)) {
            response->getHeader()->setValue("Transfer-Encoding", "Chunked");
        }*/
    }

    // done
    return response;

}

Fuppes::HttpResponse* FuppesHttpSession::createItemResponse(Fuppes::RequestItem& item)
{
    // no transcoding
    if (item.transcodeSteps.empty()) {
        return new Fuppes::HttpResponseFile(Http::OK, m_item.mimeType, m_item.targetPath);
    }

    // one shot transcoding to a (temp) file or renaming/mime type change
    else if (Transcoding::Item::File == item.transcodeTarget && !item.isThreaded()) {
        return new Fuppes::HttpResponseFile(Http::OK, m_item.mimeType, m_item.targetPath);
    }

    // one shot transcoding (conversion/rescaling/extraction) into memory
    else if (Transcoding::Item::Memory == item.transcodeTarget && !item.isThreaded()) {
        return new Fuppes::HttpResponse(Http::OK, m_item.mimeType);
    }

    // threaded transcoding (to a file or memory)
    else if (item.isThreaded()) {
        return new Fuppes::HttpResponseCallback(Http::OK, m_item.mimeType, &FuppesHttpSession::transcodeContentCallback);
    }

    return new Fuppes::HttpResponse(Http::INTERNAL_SERVER_ERROR);
}

Fuppes::HttpResponse* FuppesHttpSession::checkAlbumArtRequest(Fuppes::RequestItem& item, const std::string uriType, Fuppes::HttpRequest* request)
{
    if (0 != uriType.compare("image")) {
        return NULL;
    }

    if (item.isAudio()) {

        if (0 == item.details->albumArtId || item.objectId != item.details->albumArtId) {
            return new Fuppes::HttpResponse(Http::NOT_FOUND);
        }

        item.type = ITEM_IMAGE_ITEM;
        item.extension = item.details->albumArtExtension;
        item.mimeType = item.details->albumArtMimeType;
        item.transcodeSteps.push_back(Transcoding::Item::ExtractImage);
    }

    else if (item.isVideo()) {

        if (0 == item.details->albumArtId || item.objectId != item.details->albumArtId) {
            return new Fuppes::HttpResponse(Http::NOT_FOUND);
        }

        item.type = ITEM_IMAGE_ITEM;
        item.extension = item.details->albumArtExtension;
        item.mimeType = item.details->albumArtMimeType;

        std::stringstream path;
        path << m_fuppesConfig.contentDatabaseSettings.getThumbnailDirectory();
        path << item.objectId << "." << item.extension;
        item.targetPath = path.str();
    }

    return NULL;
}

Fuppes::HttpResponse* FuppesHttpSession::checkUriTranscoding(Fuppes::RequestItem& item, const std::string uriType, const std::string uriExt, Fuppes::HttpRequest* request)
{
    // check for image conversion / rescaling
    if (uriType.compare("image") == 0 && item.isImage()) {

        // convert image
        if (item.extension != uriExt) {
            item.extension = uriExt;
            item.transcodeSteps.push_back(Transcoding::Item::ConvertImage);
        }

        // scale image
        int width = 0;
        int height = 0;
        std::string value;

        if (request->getGetVar("width", value)) {
            width = atoi(value.c_str());
        }

        if (request->getGetVar("height", value)) {
            height = atoi(value.c_str());
        }

        if ((0 < width && width != item.details->iv_width) || (0 < height && item.details->iv_height)) {
            item.width = (0 < width ? width : item.details->iv_width);
            item.height = (0 < height ? height : item.details->iv_height);
            item.transcodeSteps.push_back(Transcoding::Item::ScaleImage);
        }
    }

    return NULL;
}

fuppes_off_t FuppesHttpSession::transcodeContentCallback(Http::HttpSession* httpSession, char** buffer, fuppes_off_t offset, bool* eof)
{
    Fuppes::FuppesHttpSession* session = (Fuppes::FuppesHttpSession*)httpSession;

    bool loop = true;
    fuppes_off_t rest = 0;
    do {
        rest = session->m_item.cacheItem->m_dataSize - offset;

        //std::cout << "TC: size: " <<  session->m_item.cacheItem->m_dataSize << " offset: " << offset << " rest: " << rest << std::endl;

        if (0 >= rest) {
            session->msleep(50);
        }

        if (rest > 0 || session->m_item.cacheItem->m_completed || session->m_item.cacheItem->m_error) {
            break;
        }

    } while (loop && !session->stopRequested());

    rest = session->m_item.cacheItem->m_dataSize - offset;
    if (offset < session->m_item.cacheItem->m_dataSize && rest > 0) {

    	if (Transcoding::Item::Memory == session->m_item.transcodeTarget) {
			*buffer = (char*)&session->m_item.cacheItem->m_data[offset];
			return rest;
    	}
    	else if (Transcoding::Item::File == session->m_item.transcodeTarget) {

    		if (!session->m_fileTranscodingFile.isOpen()) {
    			session->m_fileTranscodingFile.setFileName(session->m_item.targetPath);
    			session->m_fileTranscodingFile.open(fuppes::File::Read);
    		}

			session->m_fileTranscodingFile.seek(offset);

			fuppes_off_t size = 1024 * 10;
			if (rest > size) {
				rest = size;
			}
			if (NULL == session->m_fileTranscodingBuffer) {
				session->m_fileTranscodingBuffer = new unsigned char[size];
			}

			*buffer = (char*)session->m_fileTranscodingBuffer;
			return session->m_fileTranscodingFile.read((char*)session->m_fileTranscodingBuffer, rest);
    	}
    }

    *eof = ((rest == 0 && session->m_item.cacheItem->m_completed) || session->m_item.cacheItem->m_error);
    return 0;
}

/*
 Fuppes::FuppesHttpResponse* FuppesHttpSession::extractImageFromAudio(UPnP::UPnPObject& item)
 {
 Plugins::MetadataPlugin* plugin = m_pluginMgr.createMetadataPlugin("taglib");
 if(NULL == plugin) {
 return new Fuppes::FuppesHttpResponse(Http::SERVICE_UNAVAILABLE);
 }

 if(false == plugin->openFile(item.getOriginalPath())) {
 delete plugin;
 return new Fuppes::FuppesHttpResponse(Http::NOT_FOUND);
 }

 char tmpMime[100];
 size_t size = 0;
 unsigned char* buffer = (unsigned char*)malloc(1);

 bool gotImage = plugin->readImage(&tmpMime[0], &buffer, &size);
 plugin->closeFile();
 delete plugin;

 Fuppes::FuppesHttpResponse* response = NULL;

 if(gotImage) {
 response = new Fuppes::FuppesHttpResponse(Http::OK, item.getMimeType());
 response->setContent((const char*)buffer, size);
 }
 else {
 response = new Fuppes::FuppesHttpResponse(Http::NOT_FOUND);
 }

 free(buffer);
 return response;
 }
 */

Fuppes::HttpResponse* FuppesHttpSession::handleSubscribe(Fuppes::HttpRequest* request)
{
    return m_subscriptionMgr.handleSubscription(request);
}

Fuppes::HttpResponse* FuppesHttpSession::handleUnsubscribe(Fuppes::HttpRequest* request)
{
    return m_subscriptionMgr.handleSubscription(request);
}

void FuppesHttpSession::onSessionStarted(std::string ip, int port)
{
    //cout << "FuppesHttpSession::onSessionStarted" << endl;
    //Logging::Profiler::sessionStarted(ip, port);
}

void FuppesHttpSession::onRequestStarted()
{
    //cout << "FuppesHttpSession::onRequestStarted" << endl;
    //Logging::Profiler::requestStarted();
}

void FuppesHttpSession::onRequest(Http::Request* request)
{
    return;

    Fuppes::HttpRequest* req = (Fuppes::HttpRequest*)request;

    char* pos = strstr(this->getRemoteSocket()->buffer(), "\r\n\r\n");
    size_t size = pos - this->getRemoteSocket()->buffer();
    char* header = new char[size + 1];
    memcpy(header, this->getRemoteSocket()->buffer(), size);
    header[size] = '\0';

    cout << "FuppesHttpSession::onRequest" << endl;
    cout << "ip: " << this->getRemoteSocket()->remoteAddress() << " : " << this->getRemoteSocket()->remotePort() << endl;
    cout << "device settings: " << req->getDeviceSettings()->name() << endl;
    cout << "device vfolder: " << req->getVfolderLayout() << endl;
    cout << "HEADER: " << endl;
    cout << header << endl;
    cout << endl;

    /*
     std::string ip = this->getRemoteSocket()->remoteAddress();
     log(Logging::Log::http, Logging::Log::extended) << "request from: " << ip << "\r\n" << header;

     Logging::Profiler::requestSetValue("header", header);
     */

    delete[] header;
}

void FuppesHttpSession::onValidRequest(Http::Request* request)
{
	//log("http") << Log::Log::debug << "valid request:" << request->getRawHeader();


    return;
    //cout << "FuppesHttpSession::onValidRequest: " << m_request->getContentSize() << endl;

    char* content = new char[m_request->getContentSize() + 1];
    memcpy(content, m_request->getContent(), m_request->getContentSize());
    content[m_request->getContentSize()] = '\0';

    //Fuppes::HttpRequest* req = (HttpRequest*)request;

    //Logging::Profiler::requestSetValue("content", content);
    //Logging::Profiler::requestSetValue("vfolder", req->getVfolderLayout());
    //Logging::Profiler::requestSetValue("device", req->getDeviceSettings()->name());

    delete[] content;
}

void FuppesHttpSession::onResponse()
{
	//logDbg("http") << "response:" << m_response->getHeaderBuffer();
    return;
    //cout << "FuppesHttpSession::onResponse" << endl;

    cout << "FuppesHttpSession::onResponse" << endl;
    cout << m_response->getHeaderBuffer() << endl;
    cout << endl;

    /* char* content = NULL;
     bool  eof = false;
     fuppes_off_t size = m_response->getContent(this, &content, 0, 0, &eof);

     cout << content << endl;
     cout << endl;*/

    /*
     Logging::Profiler::requestSetValue("response_header", m_response->getHeaderBuffer());

     string contentType = m_response->getHeader()->getValue("Content-Type");
     if(contentType.length() > 5 && contentType.substr(0, 5).compare("text/") == 0) {
     char* content = NULL;
     bool  eof = false;
     fuppes_off_t size = m_response->getContent(this, &content, 0, 0, &eof);
     if(size > 0)
     Logging::Profiler::requestSetValue("response_content", content);
     }
     */
}

void FuppesHttpSession::onRequestFinished()
{
    return;
    cout << "FuppesHttpSession::onRequestFinished" << endl;
    //Logging::Profiler::requestFinished();
}

void FuppesHttpSession::onSessionFinished()
{
    return;
    cout << "FuppesHttpSession::onSessionFinished" << endl;
    //Logging::Profiler::sessionFinished();
}

