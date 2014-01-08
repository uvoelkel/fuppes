/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <BaseTest.h>

#include <Fam/Monitor.h>
#include <Fam/EventHandler.h>

#include <Common/File.h>
#include <Common/Directory.h>

class FamTest: public Test::BaseTest, Fam::EventHandler
{
    public:
        FamTest() :
                BaseTest("")
        {
            monitor = NULL;
        }

        void setup() throw (Test::SetupException)
        {
            monitor = Fam::Monitor::createMonitor(*this, false);

            testDir += "famroot/";
            if (!fuppes::Directory::exists(testDir)) {
                fuppes::Directory::create(testDir);
            }
        }

        void run() throw (Test::Exception)
        {
            monitor->start();

            monitor->addWatch(testDir);

            msleep(1000);

            fuppes::Directory::create(testDir + "testdir1/");
            fuppes::Directory::create(testDir + "testdir1/testdir1_1");

            fuppes::File testfile(testDir + "testdir1/testfile1_1.txt");
            testfile.open(fuppes::File::Write | fuppes::File::Truncate);
            char buffer[] = "testfile1_1";
            testfile.write(buffer, sizeof(buffer));
            msleep(1000);
            testfile.write(buffer, sizeof(buffer));
            testfile.close();

            msleep(2000);

            std::cout << "stopping monitor" << std::endl;
            monitor->stop();
            std::cout << "closing monitor" << std::endl;
            monitor->close();
            std::cout << "done" << std::endl;
        }

        void teardown() throw (Test::TeardownException)
        {
            delete monitor;
        }

        void onDirectoryCreateEvent(const std::string path, const std::string name)
        {
            std::cout << "onDirectoryCreateEvent" << std::endl;
        }
        void onDirectoryDeleteEvent(const std::string path)
        {
            std::cout << "onDirectoryDeleteEvent" << std::endl;
        }
        void onDirectoryMoveEvent(const std::string oldpath, const std::string oldname, const std::string path, const std::string name)
        {
            std::cout << "onDirectoryMoveEvent" << std::endl;
        }

        void onFileCreateEvent(const std::string path, const std::string name)
        {

        }
        void onFileDeleteEvent(const std::string path, const std::string name)
        {

        }
        void onFileMoveEvent(const std::string oldpath, const std::string oldname, const std::string path, const std::string name)
        {

        }
        void onFileModifiedEvent(const std::string path, const std::string name)
        {

        }

    private:
        Fam::Monitor* monitor;
};

int main(int argc, char* argv[])
{
    Test::App app(argc, argv);

    if (!fuppes::Directory::exists(app.getTestDir() + "famroot/")) {
        fuppes::Directory::create(app.getTestDir() + "famroot/");
    }

    app.m_tests.push_back(new FamTest());
    return app.run();
}

