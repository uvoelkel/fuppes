/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "../../src/lib/Threading/ThreadPool.h"
#include "../../src/lib/Http/HttpServer.h"
#include "../../src/lib/Logging/Log.h"

#include "HttpSession.h"
#include "HttpFileSession.h"
#include "HttpCallbackSession.h"

#include <sstream>
#include <iostream>
#include <stdio.h>
using namespace std;

Http::HttpSession* createSessionCallback(fuppes::TCPRemoteSocket* socket, void* userData) {
        //cout << "createSessionCallback" << endl;
    //return new TestHttpSession(socket);
    return new TestHttpFileSession(socket);
    //return new TestHttpCallbackSession(socket);
}

int main(int argc, char* argv[])
{
    Logging::Log::init();
    Threading::ThreadPool::init();

 
    Http::HttpServer* server = new Http::HttpServer("192.168.0.7", 5080);
    server->setCreateSessionCallback(&createSessionCallback, NULL);
    server->start();

    getchar();

    cout << "stopping server" << endl;
    server->stop();
    cout << "closing server" << endl;
    server->close();
    cout << "deleting server" << endl;
    delete server;


    Threading::ThreadPool::uninit();
    Logging::Log::uninit();
    
    return 0;
}


