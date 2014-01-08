/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <BaseTest.h>

#include <Thread/ThreadPool.h>
#include <Http/HttpServer.h>

#include "HttpFileSession.h"

#define LARGE_FILE_SIZE 10737418242ul
#define GiB 1073741824ul

class ChunkedRequestTest: public Test::BaseTestCurl
{

    public:

        ChunkedRequestTest() :
                BaseTestCurl("chunked http request")
        {
        }


        struct curl_slist* buildRequestHeader(struct curl_slist* slist)
        {
            slist = curl_slist_append(slist, "transfer-encoding=chunked");
            return slist;
        }

        void run() throw (Test::Exception)
        {
            curl_easy_setopt(m_curl, CURLOPT_URL, getUrl().c_str());

            CURLcode res = curl_easy_perform(m_curl);
            if(res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }
        }

};



Http::HttpSession* createSessionCallback(fuppes::TCPRemoteSocket* socket, void* userData)
{
    Test::App* app = (Test::App*)userData;
    return new TestHttpFileSession(socket, app->getTestDir() + "largefile.bin");
}

int main(int argc, char* argv[])
{
    Test::App app(argc, argv, Test::App::TA_NETWORK);

    std::string filename = app.getTestDir() + "largefile.bin";
    if(!fuppes::File::exists(filename)) {
        return 1;
    }

    app.m_tests.push_back(new ChunkedRequestTest());


    //Logging::Log::init();
    Threading::ThreadPool::init();

    // create and start http server
    Http::HttpServer server(app.m_serverIp, app.m_serverPort);
    server.setCreateSessionCallback(&createSessionCallback, &app);
    server.start();

    // run the tests
    int ret = app.run();

    // close the server and cleanup
    std::cout << "stopping server" << std::endl;
    server.stop();
    std::cout << "closing server" << std::endl;
    server.close();
    std::cout << "deleting server" << std::endl;


    Threading::ThreadPool::uninit();

    return ret;
}

