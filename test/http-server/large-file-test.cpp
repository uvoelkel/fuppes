/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <BaseTest.h>

#include <stdlib.h>
#include <string.h>
#include <sstream>

#include <Common/File.h>

#include <Thread/ThreadPool.h>
#include <Http/HttpServer.h>

#include "HttpFileSession.h"

#define LARGE_FILE_SIZE 10737418242ul
#define GiB 1073741824ul

class LargeFileTest: public Test::BaseTestCurl
{
    protected:
        LargeFileTest() :
                BaseTestCurl("large file test")
        {
        }

        virtual fuppes_off_t getRangeStart() = 0;
        virtual fuppes_off_t getRangeEnd() = 0;

    public:

        struct curl_slist* buildRequestHeader(struct curl_slist* slist)
        {
            std::stringstream range;
            range << "range:bytes=";

            fuppes_off_t value = getRangeStart();
            if(-1 < value) {
                range << value;
            }
            range << "-";
            if(-1 < (value = getRangeEnd())) {
                range << value;
            }

            std::cout << range.str() << std::endl;

            slist = curl_slist_append(slist, range.str().c_str());
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

class LargeFileTestFirstTenBytes: public LargeFileTest
{
    public:
        fuppes_off_t getRangeStart()
        {
            return 0;
        }

        fuppes_off_t getRangeEnd()
        {
            return 9;
        }
};

class LargeFileTestLastTenBytesFullRange: public LargeFileTest
{
    public:
        fuppes_off_t getRangeStart()
        {
            return LARGE_FILE_SIZE - 10;
        }

        fuppes_off_t getRangeEnd()
        {
            return LARGE_FILE_SIZE - 1;
        }
};

class LargeFileTestLastTenBytesShortRange: public LargeFileTest
{
    public:
        fuppes_off_t getRangeStart()
        {
            return -1;
        }

        fuppes_off_t getRangeEnd()
        {
            return 10;
        }
};


class LargeFileTestGibBoundary: public LargeFileTest
{
    public:
        LargeFileTestGibBoundary(int gib): LargeFileTest()
        {
            m_gib = gib;
        }

        fuppes_off_t getRangeStart()
        {
            fuppes_off_t range = (GiB * m_gib) - 1;
            //std::cout << range << std::endl;
            return range;
        }

        fuppes_off_t getRangeEnd()
        {
            fuppes_off_t range = (GiB * m_gib);
            //std::cout << range << std::endl;
            return range;
        }

        int m_gib;
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

    app.m_tests.push_back(new LargeFileTestFirstTenBytes());
    app.m_tests.push_back(new LargeFileTestLastTenBytesFullRange());
    app.m_tests.push_back(new LargeFileTestLastTenBytesShortRange());

    app.m_tests.push_back(new LargeFileTestGibBoundary(1));
    app.m_tests.push_back(new LargeFileTestGibBoundary(3));
    app.m_tests.push_back(new LargeFileTestGibBoundary(5));
    app.m_tests.push_back(new LargeFileTestGibBoundary(9));



    //Logging::Log::init();
    Threading::ThreadPool::init();

    // create and start http server
    Http::HttpServer* server = new Http::HttpServer(app.m_serverIp, app.m_serverPort);
    server->setCreateSessionCallback(&createSessionCallback, &app);
    server->start();

    // run the tests
    int ret = app.run();

    // close the server and cleanup
    std::cout << "stopping server" << std::endl;
    server->stop();
    std::cout << "closing server" << std::endl;
    server->close();
    std::cout << "deleting server" << std::endl;
    delete server;

    Threading::ThreadPool::uninit();
    //Logging::Log::uninit();

    return ret;
}

