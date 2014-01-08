/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "../Common/Socket.h"

#include "HttpSession.h"

#include "Request.h"
#include "Response.h"

using namespace Http;

#include "../Thread/ThreadPool.h"
#include "../Common/Common.h"

#include <string.h>
#include <stdlib.h>
#include <sstream>

#include <iostream>
using namespace std;


// we use a max http header size of 16 kb
// http://stackoverflow.com/questions/686217/maximum-on-http-header-values

// dlna volume 1, 7.2.34: specifies a max header size of 4096 bytes

#define HTTP_MAX_HEADER_SIZE 16384

HttpSession::HttpSession(fuppes::TCPRemoteSocket* remoteSocket):
        Threading::EventLoopThread("http session")
{
    m_server = NULL;
    m_request = NULL;
    m_response = NULL;

    m_remoteSocket = remoteSocket;

    m_keepAlive = true;
    m_newRequest = true;
    m_responseOffset = 0;
    m_contentOffset = 0;
    m_contentSize = 0;
    m_tmpRequest = NULL;

    m_ioReadWatcher = NULL;
    m_ioWriteWatcher = NULL;
}

HttpSession::~HttpSession()
{
    m_remoteSocket->close();
    delete m_remoteSocket;

    if(NULL != m_tmpRequest && m_tmpRequest != m_request) {
        delete m_tmpRequest;
    }

    if (NULL != m_request) {
    	delete m_request;
    }

    if (NULL != m_response) {
    	delete m_response;
    }
}

void HttpSession::stop()
{
    Threading::EventLoopThread::stop();
}

void HttpSession::run()
{
    Threading::ThreadPool::deleteLater(this);

    this->onSessionStarted(m_remoteSocket->remoteAddress(), m_remoteSocket->remotePort());
    
    m_keepAlive = true;
    m_newRequest = true;
    
    m_ioReadWatcher = new Threading::IoReadWatcher();
    m_ioReadWatcher->init(this->getEventLoop()->getLoop(), m_remoteSocket->socket(), this);

    m_ioWriteWatcher = new Threading::IoWriteWatcher();
    m_ioWriteWatcher->init(this->getEventLoop()->getLoop(), m_remoteSocket->socket(), this);

    //this->eventLoop()->startWatcher(m_ioReadWatcher);
    m_ioReadWatcher->start();

    //std::cout << "HttpSession::start()" << std::endl;
    this->startEventLoop();
    //std::cout << "HttpSession::exit()" << std::endl;
    
    m_remoteSocket->close();

    delete m_ioWriteWatcher;
    delete m_ioReadWatcher;

    // delete the threads event loop so we don't need to wait
    // for the thread to be deleted and the resources to be freed    
    this->clearEventLoop();

    this->onSessionFinished();
}

void HttpSession::ioReadEvent(Threading::IoReadWatcher* watcher)
{    
    // receive the data
    fuppes_off_t size = m_remoteSocket->receive();
    
    // connection closed by the client
    if(0 == size && !m_remoteSocket->isConnected()) {
        //cout << "connection closed by client" << endl;
        m_keepAlive = false;
        m_newRequest = true;
        //m_remoteSocket->close();
        watcher->stop();
        this->unloop();
        return;
    }    


    if(m_newRequest) {
        m_newRequest = false;
        this->onRequestStarted();
    }

    
    // try to find the end of the http header
    char* pos = strstr(m_remoteSocket->buffer(), "\r\n\r\n");
    if(NULL == pos) {

        if(m_remoteSocket->bufferFill() >= HTTP_MAX_HEADER_SIZE) {
            m_keepAlive = false;
            watcher->stop(); //this->eventLoop()->stopWatcher(watcher);
            
            m_response = this->createResponse(Http::REQUEST_ENTITY_TOO_LARGE);
            this->sendResponse();
            return;
        }

        // header is not complete yet. maybe next time
        return;
    }


    
    // we got a complete header. let's parse it
    m_tmpRequest = this->createRequest(m_remoteSocket->buffer(), m_remoteSocket);
    this->onRequest(m_tmpRequest);

    // the header is unparsable
    if(NULL == m_tmpRequest || !m_tmpRequest->getHeader()->isValid()) {
        if(NULL != m_tmpRequest) {
            delete m_tmpRequest;
            m_tmpRequest = NULL;
        }

        m_keepAlive = false;
        watcher->stop();

        m_response = this->createResponse(Http::BAD_REQUEST);
        this->sendResponse();
        return;
    }

    
    // http://www.iana.org/assignments/http-parameters/http-parameters.xml#http-parameters-2
    
    // we got at least a valid header so let's check the payload
    fuppes_off_t headerSize = (pos + 4) - m_remoteSocket->buffer(); // 4 = strlen(\r\n\r\n)
    bool chunked = (m_tmpRequest->getHeader()->getValueAsLower("transfer-encoding") == "chunked");
    
    if(chunked) {
        
        m_request = this->readChunkedMessage(m_tmpRequest, m_remoteSocket->buffer(), m_remoteSocket->bufferFill(), pos + 4); // + 4 = set cursor behind the closing \r\n\rn
        // not complete yet
        if(NULL == m_request) {
            delete m_tmpRequest;
            m_tmpRequest = NULL;
            return;
        }
    }
    else {
        
        if(m_tmpRequest->getHeader()->keyExists("content-length")) {
            
            fuppes_off_t contentLength = m_tmpRequest->getHeader()->getValueAsOffT("content-length");
            fuppes_off_t contentSize   = m_remoteSocket->bufferFill() - headerSize;

            //cout << "CONTENT LENGTH: " << contentLength << " IS: " << contentSize << " FROM: " << m_remoteSocket->bufferFill() << endl;
            
            // content not yet fully received
            if(contentSize < contentLength) {
                delete m_tmpRequest;
                m_tmpRequest = NULL;
                return;
            }
            // complete content
            else if(contentSize == contentLength) {
                m_request = m_tmpRequest;
                m_tmpRequest = NULL;
            }
            // actual content size > content-length header value
            else {
                cout << "PAYLOAD > CONTENT-LENGTH" << endl;
                delete m_tmpRequest;
                m_tmpRequest = NULL;
                m_response = this->createResponse(Http::BAD_REQUEST);
                this->sendResponse();
                return;
            }
        }
        else {
 
            fuppes_off_t contentSize =  m_remoteSocket->bufferFill() - headerSize;

            if(0 == contentSize) {
                m_request = m_tmpRequest;
                m_tmpRequest = NULL;
            }
            else if(0 < contentSize) {
                cout << "ERROR: REQUEST IS NOT CHUNKED AND DOES NOT CONTAIN A CONTENT-LENGTH BUT IT DOES CONTAIN PAYLOAD" << endl;
            }
        }

    }


    // still no valid request
    if(NULL == m_request) {
        delete m_tmpRequest;
        m_tmpRequest = NULL;

        m_keepAlive = false;
        watcher->stop(); //this->eventLoop()->stopWatcher(watcher);        

        if(NULL == m_response)
            m_response = this->createResponse(Http::BAD_REQUEST);

        this->sendResponse();
        return;
    }


    // we finally got a complete and valid request
    
    if(m_request->getHeader()->getValueAsLower("transfer-encoding") != "chunked") {
        // let the request object take ownership of the socket's buffer
        m_request->setBuffer(m_remoteSocket->buffer(), m_remoteSocket->bufferSize(), m_remoteSocket->bufferFill(), headerSize);
        m_remoteSocket->bufferDetach();
    }
    else {
        m_remoteSocket->bufferClear();
    }
    
    m_keepAlive = (0 == m_request->getHeader()->getValueAsLower("connection").compare("keep-alive"));
    this->onValidRequest(m_request);

    // build the response
    m_response = this->handleRequest(m_request);
    if(NULL == m_response) {
        delete m_request;

        m_keepAlive = false;
        //watcher->stop(); //this->eventLoop()->stopWatcher(watcher);

        m_response = this->createResponse(Http::NOT_IMPLEMENTED);
    }
    

    //if(!m_keepAlive)
    watcher->stop(); //this->eventLoop()->stopWatcher(watcher);

    this->sendResponse();
}


Http::Request* HttpSession::readChunkedMessage(Http::Request* request, const char* buffer, size_t bufferSize, char* headerOffset)
{
    // size (hex) (possible whitespace) CRLF
    // data CRLF
    // size (hex) (possible whitespace) CRLF
    // data CRLF
    // ...
    // 0 (possible whitespace) CRLF
    // CRLF
    
    // check the end of the buffer

    // the content has to end with a double crlf
    const char* dataEnd = buffer + bufferSize - 5;
    if(0 != memcmp("0\r\n\r\n", dataEnd, 5)) { // TODO: check for chunk-extensions
        return NULL;
    }
    
    // create a buffer for the final message
    char* data = (char*)malloc(bufferSize);
    fuppes_off_t dataSize = 0;
    
    // copy the header
    fuppes_off_t headerSize = headerOffset - buffer;
    memcpy(data, buffer, headerSize); 
    dataSize = headerSize;

    char* pos;
    char* dataOffset = headerOffset;
    int chunkSize = 0;
    // get the chunks
    while((pos = strstr(dataOffset, "\r\n")) != NULL) {

        chunkSize = this->parseChunkSize(dataOffset);
        if(0 == chunkSize)
            break;

        dataOffset = pos + 2; // move offset behind (chunk-size CRLF)

        // copy the chunk-data
        memcpy(&data[dataSize], dataOffset, chunkSize);
        dataSize += chunkSize;
        
        dataOffset += (chunkSize + 2); // move offset behind (chunk-data CRLF)
    }

    request->setBuffer(data, bufferSize, dataSize, headerSize);    
    return request;
}

int HttpSession::parseChunkSize(const char* buffer)
{
    char    chunkSize[10];
    int     result = 0;

    memset(chunkSize, '\0', sizeof(chunkSize));
    for(unsigned int i = 0; i < (sizeof(chunkSize)-1); i++) {

        if(buffer[i] == '\r' ||
           buffer[i] == ';' ||
           buffer[i] == ' ')
            break;

        chunkSize[i] = buffer[i];            
    }
        
    result = HexToInt(chunkSize);
    return result;
}


Http::Response* HttpSession::handleRequest(Http::Request* request)
{
    cout << "default http session handle request" << endl;
    return NULL;
}



void HttpSession::ioWriteEvent(Threading::IoWriteWatcher* watcher)
{
    //cout << "HttpSession::ioWriteEvent" << endl;

    if(NULL == m_response) {
        cout << "HttpSession::ioWriteEvent :: ERROR NO RESPONSE" << endl;
        return;
    }

    if(this->sendResponse(false)) {
        watcher->stop();
    }
}

bool HttpSession::sendResponse(bool firstTime /*= true*/)
{    
    // prepare the response on the first call
    if(firstTime) {

        m_responseOffset = 0;
        m_contentOffset = 0;
        m_contentSize = 0;

        // if the response is an error we don't keep the connection alive
        if(m_keepAlive && m_response->getStatusCode() != Http::OK) {
            m_keepAlive = false;
        }

        // check the request ranges and set content offset accordingly
        if(m_request->getHeader()->keyExists("range") && m_response->acceptRanges()) {

            fuppes_off_t start = m_request->getHeader()->getRangeStart();
            fuppes_off_t end = m_request->getHeader()->getRangeEnd();

            if(-1 == start) {
                // if there is no 'start' range 'end' is the offset from the end of the file
                start = m_response->getContentSize() - end;
                end = m_response->getContentSize() - 1;
            }
            
            if(-1 == end) {
                end = m_response->getContentSize() - 1;
            }

            m_contentSize = (end - start) + 1;
            m_contentOffset = start;

            // set response header
            m_response->setStatusCode(Http::PARTIAL_CONTENT);
            m_response->getHeader()->setValueAsOffT("Content-Length", m_contentSize);
            std::stringstream sstream;
            sstream << "bytes " << start << "-" << end << "/" << m_response->getContentSize();
            m_response->getHeader()->setValue("Content-Range", sstream.str());
        }


        // set the accept ranges (if not already set and response is not chunked) 
        if(!m_response->getHeader()->keyExists("Accept-Ranges") && 
           m_response->getHeader()->getValueAsLower("Transfer-Encoding") != "chunked") {
               
            if(m_response->hasContentLength()) {
                m_response->getHeader()->setValue("Accept-Ranges", "bytes");
                m_contentSize = m_response->getContentSize();
            }
            else {
                // m_response->getHeader()->setValue("Accept-Ranges", "none");
            }
        } 


        // set the content length (if not already set and response is not chunked) 
        if(!m_response->getHeader()->keyExists("Content-Length") && 
           m_response->getHeader()->getValueAsLower("Transfer-Encoding") != "chunked") {
               
            if(m_response->hasContentLength()) {
                m_response->getHeader()->setValueAsOffT("Content-Length", m_response->getContentSize());
                m_contentSize = m_response->getContentSize();
            }
            else {
                m_keepAlive = false;
            }
        }

        
        // set the connection header
        if(!m_keepAlive) {
            m_response->getHeader()->setValue("Connection", "close");
        }
        else if(!m_response->getHeader()->keyExists("Connection")) {
            m_response->getHeader()->setValue("Connection", "keep-alive");
        }
        else {
            m_keepAlive = (m_response->getHeader()->getValueAsLower("Connection") == "keep-alive");
        }

        
        // set the date (if not already set)
        if(!m_response->getHeader()->keyExists("Date")) {
            char   date[30];
            time_t tTime = time(NULL);
            strftime(date, 30,"%a, %d %b %Y %H:%M:%S GMT" , gmtime(&tTime));
            m_response->getHeader()->setValue("Date", date);
        }



        // build the final response header
        m_response->prepare(m_request);


        // reset the new request flag so ioReadWatcher knows that the next time it is called a new request is started.
        m_newRequest = true;
        // clear the sockets buffer. it is at this point either taken over by the request object or useless
        m_remoteSocket->bufferClear();
        this->onResponse();
    }

    

    bool complete = false;
    fuppes_off_t bytesSend = 0;
    fuppes_off_t length = 0;

    // send the header
    if(m_responseOffset < m_response->getHeaderSize()) {
        const char* buffer = m_response->getHeaderBuffer();
        length = m_response->getHeaderSize() - m_responseOffset;
        
        bytesSend = m_remoteSocket->send(&buffer[m_responseOffset], length, false);

        // send error
        if(bytesSend < 0) {
            m_remoteSocket->close();
            if(!firstTime) {
                m_ioWriteWatcher->stop(); // this->eventLoop()->startWatcher(m_ioWriteWatcher);
            }
            return false;
        }

        m_responseOffset += bytesSend;
        
        // header incomplete
        if(bytesSend < length) {
            if(firstTime) {
                m_ioWriteWatcher->start(); // this->eventLoop()->startWatcher(m_ioWriteWatcher);
            }
            //cout << "header incomplete" << endl;
            return false;
        }
    }


    // we sent the complete header so let's check for a HEAD request 
    // to determine whether to send the content
    if(m_request->getMethod().compare("HEAD") == 0) {
        complete = true;
    }
    
    // send the content
    if(!complete && m_responseOffset >= m_response->getHeaderSize()) {

        if(m_response->getHeader()->getValueAsLower("transfer-encoding") != "chunked")
            complete = this->sendContent();
        else
            complete = this->sendContentChunked();

        if(!complete && firstTime) {
            m_ioWriteWatcher->start(); //this->eventLoop()->startWatcher(m_ioWriteWatcher);
        }
    }
    

    // we are done sending. let's clean up
    if(complete) {
    
        if(!m_keepAlive) {
            m_remoteSocket->close();
            this->unloop();
        }
        // keep alive is only possible with a valid request and response
        else {

            delete m_request;
            m_request = NULL;
            
            delete m_response;
            m_response = NULL;            
        }

        this->onRequestFinished();

        if(m_keepAlive) {
            m_ioReadWatcher->start();
        }
    }
    return complete;
}

bool HttpSession::sendContent()
{
    bool  eof = false;
    char* buffer;
    fuppes_off_t bytesSend = 0;
    fuppes_off_t dataOffset = (m_responseOffset - m_response->getHeaderSize()) + m_contentOffset;        
    fuppes_off_t length = m_response->getContent(this, &buffer, dataOffset, m_contentSize, &eof);

    if(length > 0) {
        bytesSend = m_remoteSocket->send(buffer, length, false);
    }
    // eof
    else if(length == 0 && eof) {
        return true;
    }
        
    // send error
    if(bytesSend < 0) {
        //m_ioWriteWatcher->stop();
        //m_remoteSocket->close();
        return true;
    }

    m_responseOffset += bytesSend;
    if(m_contentSize > 0) {
        m_contentSize -= bytesSend;
        if(m_contentSize == 0)
            eof = true;
    }
        
    // complete
    if(bytesSend == length && eof) {
        return true;
    }

    // incomplete
    return false;
}

bool HttpSession::sendContentChunked()
{
    return true;
}


Http::Request* HttpSession::createRequest(const char* buffer, fuppes::TCPRemoteSocket* remoteSocket)
{
    Http::Request* request = Http::Request::create(m_remoteSocket->buffer());
    if(NULL != request) {
        request->setRemoteSocket(remoteSocket);
    }
    return request;
}

Http::Response* HttpSession::createResponse(Http::HttpStatusCode status, std::string content /*= ""*/)
{
    return new Http::Response(status, content);
}

