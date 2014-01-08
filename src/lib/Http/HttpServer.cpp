/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "../Common/Socket.h"

#include "HttpServer.h"

using namespace std;
using namespace Http;

#include <iostream>
#include <sstream>

HttpServer::HttpServer(std::string ip, int port /*= 0*/, HttpServerEventReceiver* eventReceiver /*= NULL*/)
:Threading::EventLoopThread("http server")
{
    m_ip = ip;
    m_port = port;
    m_eventReceiver = eventReceiver;

    m_listener = NULL;
    m_ioReadWatcher = NULL;
    m_createSessionCallback = NULL;
    m_createSessionUserData = NULL;
}

void HttpServer::run()
{
    // create the listener socket
    m_listener = new fuppes::TCPServer();
    m_listener->init(m_ip, m_port, true);
    if(!m_listener->listen()) {
        if(NULL != m_eventReceiver) {
            m_eventReceiver->onHttpServerError(this, "http server failed to listen");
        }
        else {
            std::cout << "http server failed to listen" << std::endl;
        }
        delete m_listener;
        return;
    }
    // get the port number
    m_port = m_listener->localPort();

    // get the base url
    stringstream tmp;
    tmp << m_listener->localAddress() << ":" << m_port;
    m_url = tmp.str(); 
   

    if(NULL != m_eventReceiver) {
        m_eventReceiver->onHttpServerReady(this);
    }

    // create and init the accept watcher
    m_ioReadWatcher = new Threading::IoReadWatcher();
    m_ioReadWatcher->init(this->getEventLoop()->getLoop(), m_listener->socket(), this);
    m_ioReadWatcher->start();
    
    // run the event loop
    //cout << "http server start event loop" << endl;
    this->startEventLoop();
    //cout << "http server exited event loop" << endl;

    
    // delete the watcher
    delete m_ioReadWatcher;

    // close and destroy the listener
    m_listener->close();
    delete m_listener;
    
    //cout << "http server exit" << endl;
}

void HttpServer::ioReadEvent(Threading::IoReadWatcher* watcher)
{
    fuppes::TCPRemoteSocket* socket = m_listener->accept();
    if(NULL == socket)
        return;

    // http session will pass itself to the thread pool which will take care of deletion
    // http session also takes ownership of the socket
    HttpSession* session;
    if(NULL == m_createSessionCallback)
        session = new HttpSession(socket);
    else
        session = m_createSessionCallback(socket, m_createSessionUserData);

    session->setHttpServer(this);
    session->start();
}
