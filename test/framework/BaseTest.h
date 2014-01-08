/*
 * This file is part of fuppes
 *
 * (c) 2012-2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef FUPTTESTEST_BASE_TEST_H
#define FUPTTESTEST_BASE_TEST_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <list>

#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>

#include <curl/curl.h>
#include <libxml/parser.h>

#include <Plugin/Manager.h>

namespace Test
{
    class Exception: public std::exception
    {
        public:
            Exception(const std::string msg)
            {
                m_msg = msg;
            }
            ~Exception() throw ()
            {
            }

            const char* what() const throw ()
            {
                return m_msg.c_str();
            }

        private:
            std::string m_msg;
    };

    class SetupException: public Exception
    {
        public:
            SetupException(const std::string msg) :
                    Exception(msg)
            {
            }
    };

    class TeardownException: public Exception
    {
        public:
            TeardownException(const std::string msg) :
                    Exception(msg)
            {
            }
    };

    class App;

    class BaseTest
    {
            friend class App;

        public:
            virtual ~BaseTest()
            {
            }

            virtual void setup() throw (Test::SetupException)
            {
            }
            virtual void run() throw (Test::Exception) = 0;
            virtual void teardown() throw (Test::TeardownException)
            {
            }

            std::string getName()
            {
                return m_name;
            }

            std::string getUrl(std::string uri = "");

        protected:
            BaseTest(std::string name);
            std::string m_name;

            int serverPort;
            std::string serverIp;

            std::string appDir;
            std::string testDir;
            std::string dataDir;
            std::string pluginDir;
            Plugin::Manager* pluginManager;

            void msleep(const int milliseconds);
    };

    class BaseTestCurl: public BaseTest
    {

        public:
            BaseTestCurl(std::string name);
            virtual void setup() throw (Test::SetupException);
            virtual struct curl_slist* buildRequestHeader(struct curl_slist* slist)
            {
                return slist;
            }
            virtual void teardown() throw (Test::TeardownException);

            static size_t curlHeaderFunction(void *ptr, size_t size, size_t nmemb, void *userdata);
            static size_t curlWriteFunction(void *ptr, size_t size, size_t nmemb, void *userdata);

        protected:
            CURL* m_curl;
            struct curl_slist* m_slist;
    };

    class App
    {
        public:

            enum AppFlags
            {
                TA_NONE = 0 << 0,
                TA_NETWORK = 1 << 1,   // app needs ip and port parameters
                TA_XML = 1 << 2,   // app needs libxml (needs to be initialized and freed after usage)
                TA_PLUGINS = 1 << 3,   // app loads plugins
                TA_DATABASE = 1 << 4 | TA_PLUGINS,   // app needs a database connection (implies TA_PLUGINS)
            };

            App(int argc, char* argv[], int flags = TA_NONE);
            int run();

            std::string getAppDir()
            {
                return m_appDir;
            }
            std::string getTestDir()
            {
                return m_appDir + "testdata/";
            }

            std::list<BaseTest*> m_tests;

            // private:
            int m_flags;

            int m_serverPort;
            std::string m_serverIp;

            std::string m_appDir;
            std::string m_dataDir;
            std::string m_pluginDir;

    };

}

#endif // FUPTTESTEST_BASE_TEST_H
