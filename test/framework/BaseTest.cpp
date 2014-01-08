/*
 * This file is part of fuppes
 *
 * (c) 2012-2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "BaseTest.h"
using namespace Test;

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>
#include <Log/Log.h>
#include <Common/File.h>
#include <Database/Connection.h>

BaseTest::BaseTest(std::string name)
{
    m_name = name;
    serverPort = 0;
    pluginManager = NULL;
}

std::string BaseTest::getUrl(std::string uri /*= ""*/)
{
    std::stringstream stream;
    stream << "http://" << serverIp << ":" << serverPort << "/";
    if (!uri.empty()) {
        stream << uri;
    }
    return stream.str();
}

void BaseTest::msleep(const int milliseconds)
{
#ifdef WIN32
    Sleep(milliseconds);
#else
    if (milliseconds < 1000)
        usleep(milliseconds * 1000);
    else
        sleep(milliseconds / 1000);
#endif
}

BaseTestCurl::BaseTestCurl(std::string name) :
        BaseTest(name)
{
    m_curl = NULL;
    m_slist = NULL;
}

void BaseTestCurl::setup() throw (Test::SetupException)
{
    curl_global_init(CURL_GLOBAL_ALL);

    m_curl = curl_easy_init();
    if (NULL == m_curl) {
        throw SetupException("failed to init curl");
    }

    curl_easy_setopt(m_curl, CURLOPT_HEADERDATA, this);
    curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, BaseTestCurl::curlHeaderFunction);

    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, BaseTestCurl::curlWriteFunction);

    m_slist = this->buildRequestHeader(m_slist);
    curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_slist);
}

void BaseTestCurl::teardown() throw (Test::TeardownException)
{
    if (NULL != m_slist) {
        curl_slist_free_all(m_slist);
        m_slist = NULL;
    }

    curl_easy_cleanup(m_curl);
    m_curl = NULL;

    curl_global_cleanup();
}

size_t BaseTestCurl::curlHeaderFunction(void *ptr, size_t size, size_t nmemb, void *userdata) // static
{
    BaseTestCurl* test = (BaseTestCurl*)userdata;

    size_t dataSize = size * nmemb;

    char* buffer = new char[dataSize + 1];
    strncpy(buffer, (char*)ptr, dataSize);
    buffer[dataSize] = 0;

    std::cout << "CURL HEADER: " << dataSize << " :: " << buffer; // << std::endl;

    delete buffer;

    return dataSize;
}

size_t BaseTestCurl::curlWriteFunction(void *ptr, size_t size, size_t nmemb, void *userdata) // static
{
    BaseTestCurl* test = (BaseTestCurl*)userdata;

    size_t dataSize = size * nmemb;

    char* buffer = (char*)malloc(dataSize + 1);
    memcpy(buffer, ptr, dataSize);
    buffer[dataSize] = '\0';

    std::cout << "CURL WRITE: " << dataSize << std::endl << "*" << buffer << "*" << std::endl;

    free(buffer);

    return dataSize;
}

static struct option testOptions[] = {
        { "ip", required_argument, 0, 'i' },
        { "port", required_argument, 0, 'p' },

        { "app_dir", required_argument, 0, 'a' },
        { "data_dir", required_argument, 0, 'd' },
        { "plugin_dir", required_argument, 0, 'l' },
        { 0, 0, 0, 0 }
};

App::App(int argc, char* argv[], int flags /*= TA_NONE*/)
{
    m_flags = flags;
    m_serverPort = 0;

    // parse the command line options
    int opt;
    int optIdx = -1;
    while (true) {
        opt = getopt_long(argc, argv, "i:p:a:d:l:", testOptions, &optIdx);
        if (-1 == opt) {
            break;
        }

        switch ((char)opt) {
            case 'i':
                m_serverIp = optarg;
                break;
            case 'p':
                m_serverPort = atoi(optarg);
                break;

            case 'a':
                m_appDir = optarg;
                m_appDir += "/";
                break;
            case 'd':
                m_dataDir = optarg;
                break;
            case 'l':
                m_pluginDir = optarg;
                break;

            default:
                break;
        }
    }

    //std::cout << m_serverIp << " : " << m_serverPort << std::endl;
}

int App::run()
{
    //Logging::Log::init();

    Plugin::Manager pluginMgr;
    Database::Connection databaseConnection(pluginMgr);
    Database::ConnectionParameters connectionParameters;

    if ((TA_NETWORK == (m_flags & TA_NETWORK)) && (m_serverIp.empty() || 0 == m_serverPort)) {
        std::cout << "ERROR: no ip or port given" << std::endl;
        return 1;
    }

    if (TA_XML == (m_flags & TA_XML)) {
        xmlInitParser();
    }

    if (TA_PLUGINS == (m_flags & TA_PLUGINS)) {

        if (false == pluginMgr.load(m_pluginDir)) {
            std::cout << "ERROR: loading plugins from " << m_pluginDir << std::endl;
            return 1;
        }
    }

    if (TA_DATABASE == (m_flags & TA_DATABASE)) {

        std::string filename = getTestDir() + "test.db";
        if (!fuppes::File::exists(filename)) {
            std::cout << "ERROR: test database \"" << filename << "\" not found" << std::endl;
            return 1;
        }

        connectionParameters.type = "sqlite3";
        connectionParameters.filename = filename;
        connectionParameters.readonly = false;

        databaseConnection.setParameters(connectionParameters);
        if (!databaseConnection.connect()) {
            std::cout << "ERROR: connecting to test database." << std::endl;
            return 1;
        }
        Database::Connection::setDefaultConnection(&databaseConnection);
    }

    size_t size = m_tests.size();
    int success = 0;

    while (!m_tests.empty()) {

        BaseTest* test = m_tests.front();

        test->serverIp = m_serverIp;
        test->serverPort = m_serverPort;

        test->appDir = getAppDir();
        test->testDir = getTestDir();
        test->dataDir = m_dataDir;
        test->pluginDir = m_pluginDir;

        if (TA_PLUGINS == (m_flags & TA_PLUGINS)) {
            test->pluginManager = &pluginMgr;
        }

        std::cout << "run test: " << test->getName() << std::endl;

        try {
            test->setup();
        }
        catch (Test::SetupException& ex) {
            std::cout << "[SETUP FAILED] :: " << ex.what() << std::endl;
        }

        try {
            test->run();
            success++;
            std::cout << "[OK]" << std::endl;
        }
        catch (Test::Exception& ex) {
            std::cout << "[FAILED] :: " << ex.what() << std::endl;
        }

        try {
            test->teardown();
        }
        catch (Test::TeardownException& ex) {
            std::cout << "[TEARDOWN FAILED] :: " << ex.what() << std::endl;
        }

        std::cout << std::endl;

        delete test;
        m_tests.pop_front();
    }

    std::cout << "finished " << size << " tests. " << success << " OK " << size - success << " failed " << std::endl;

    if (TA_XML == (m_flags & TA_XML)) {
        xmlCleanupParser();
    }

    //Logging::Log::uninit();

    //getchar();
    return 0;
}

