/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <BaseTest.h>

#include <Log/Log.h>

class LogTest: public Test::BaseTest
{
    public:
        LogTest() :
                BaseTest("simple log test")
        {
        }

        void run() throw (Test::Exception)
        {
            Log::Log::setType(Log::Logger::Null);

            Log::Log("logtest", Log::Log::normal, __FILE__, __LINE__)  << "foo" << "bar";

            Log::Log::setType(Log::Logger::Stdout);

            log("logtest") <<  Log::Log::normal << "foo" << "bar";

            Log::Log::setType(Log::Logger::Null);

            log("logtest") << Log::Log::normal << "foo" << "bar";

            Log::Log::setType(Log::Logger::Stdout);

            log("logtest") << Log::Log::normal << "foo" << "bar";
        }
};


class LogPrintfTest: public Test::BaseTest
{
    public:
		LogPrintfTest() :
                BaseTest("printf log test")
        {
        }

        void run() throw (Test::Exception)
        {
            Log::Log::setType(Log::Logger::Stdout);

            log("logtest") << Log::Log::normal << "printf" << Log::Log::printf("test %s %d", "string", 123);
        }
};

int main(int argc, char* argv[])
{
    Test::App app(argc, argv);

    app.m_tests.push_back(new LogTest());
    app.m_tests.push_back(new LogPrintfTest());

    return app.run();
}
