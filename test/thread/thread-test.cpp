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
#include <Thread/EventLoopThread.h>

class TestThread: public Threading::EventLoopThread
{
    public:
        TestThread() :
                Threading::EventLoopThread("test thread")
        {

        }

        void run() {

            while(!stopRequested()) {
                std::cout << "thread is running" << std::endl;
                this->msleep(1000);
            }

            std::cout << "thread is exiting" << std::endl;
        }
};

class ThreadTest: public Test::BaseTest
{
    public:
		ThreadTest() :
                BaseTest("thread test")
        {
        }

        void run() throw (Test::Exception)
        {
            std::cout << "starting thread" << std::endl;
            m_thread.start();

            this->msleep(5000);

            std::cout << "stopping thread" << std::endl;
            m_thread.stop();
            std::cout << "closing thread" << std::endl;
            m_thread.close();

        }

    private:
        TestThread m_thread;
};

int main(int argc, char* argv[])
{
    Threading::ThreadPool::init();

    Test::App app(argc, argv);
    app.m_tests.push_back(new ThreadTest());

    int ret = app.run();

    Threading::ThreadPool::uninit();

    return ret;
}
