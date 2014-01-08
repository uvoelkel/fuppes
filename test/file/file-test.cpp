/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <BaseTest.h>
#include <Common/File.h>

class FileTest: public Test::BaseTest
{
    public:
		FileTest() : BaseTest("file test")
        {
        }

        void setup() throw (Test::SetupException)
        {
        }

        void run() throw (Test::Exception)
        {
        	fuppes::File in("/tmp/in.bin");
        	in.open(fuppes::File::Read);

        	fuppes::File out("/tmp/out.bin");
			out.open(fuppes::File::Write | fuppes::File::Truncate);

			size_t size = 1024 * 10;
			char* buffer = (char*)malloc(size);
			fuppes_off_t ret = 0;
			while (0 < (ret = in.read(buffer, size))) {
				out.write(buffer, ret);
			}

			in.close();
			out.close();

        }

        void teardown() throw (Test::TeardownException)
        {
        }

};

int main(int argc, char* argv[])
{
    Test::App app(argc, argv, Test::App::TA_NONE);
    app.m_tests.push_back(new FileTest());
    return app.run();
}
