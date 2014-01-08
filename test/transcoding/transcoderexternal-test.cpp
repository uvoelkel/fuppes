/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <BaseTest.h>

#include <Plugin/Transcoder.h>
#include <Transcoding/TranscoderExternalProcess.h>

#include <Common/File.h>

class TestDetails: public Database::AbstractItemDetails
{
	public:
		void clear()
		{
		}
};

class TestItem: public Database::AbstractItem, public Transcoding::AbstractItem
{
	public:
		TestItem()
		{
			details = new TestDetails();
		}
		~TestItem() {
			delete details;
		}

		void clear()
		{
			details->clear();
		}

		Database::AbstractItem&	getMetadata()
		{
			return *this;
		}


        std::string getSourcePath() {
        	return "/home/ulrich/projects/fuppes-test/video/Gavin_Harrison_19_Days_live_Modern_Drummer.flv";
        }
		std::string getTargetPath() {
			return "/tmp/fuppes/23_cmd_default-transcoding-enabled.mp4";
		}
		std::string getTranscoderCommand() {
			return "/usr/bin/ffmpeg -y -i %in% %out%";
		}
};

class TranscoderTest: public Test::BaseTest
{
    public:
		TranscoderTest() : BaseTest("transcoder test") { }

        void setup() throw (Test::SetupException) { }

        void run() throw (Test::Exception)
        {
        	TestItem item;

        	Transcoding::TranscoderExternalProcess transcoder;
        	transcoder.setup(item);

			int ret = 0;
			bool error = false;
			do {
				ret = transcoder.transcode();
				if (0 >= ret) { // done or error
					error = (0 != ret);
					break;
				}

				this->msleep(200);
			} while (ret > 0);


        }

        void teardown() throw (Test::TeardownException) { }
};


int main(int argc, char* argv[])
{
    Test::App app(argc, argv, Test::App::TA_PLUGINS);
    app.m_tests.push_back(new TranscoderTest());
    return app.run();
}
