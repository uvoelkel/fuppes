/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <BaseTest.h>

#include <Plugin/Decoder.h>
#include <Plugin/Encoder.h>
#include <Common/File.h>

class TestItem: public Database::AbstractItem, public Transcoding::AbstractItem
{
	public:
		void clear()
		{
		}

		Database::AbstractItem&	getMetadata()
		{
			return *this;
		}



        std::string getSourcePath() {
        	return "";
        }
		std::string getTargetPath() {
			return "";
		}
		std::string getTranscoderCommand() {
			return "";
		}
};

class EncoderTest: public Test::BaseTest
{
    public:
		EncoderTest() : BaseTest("encoder test")
        {
        }

        void setup() throw (Test::SetupException)
        {
        }

        void run() throw (Test::Exception)
        {
            Plugin::AbstractDecoder* decoder = pluginManager->createDecoderPlugin("wav");
            if (NULL == decoder) {
                throw Test::Exception("error loading decoder plugin: wav");
            }

            Plugin::AbstractEncoder* encoder = pluginManager->createEncoderPlugin(getPluginName());
            if (NULL == encoder) {
            	delete decoder;
                throw Test::Exception("error loading plugin: " + getPluginName());
            }


            std::string filename = testDir + "test.wav";
            if (!decoder->openFile(filename)) {
                delete decoder;
                delete encoder;
                throw Test::Exception("error opening source file " + filename);
            }

            filename += "." + getFileExtension();
            fuppes::File file(filename);
            if (!file.open(fuppes::File::Write | fuppes::File::Truncate)) {
            	decoder->closeFile();
                delete decoder;
                delete encoder;
                throw Test::Exception("error opening target file " + filename);
            }

            TestItem item;
            item.totalSamples = decoder->getTotalSamples();
            encoder->setup(item);

            unsigned char buffer[32768];
            int samplesDecoded = 0;
            size_t bytesConsumed = 0;

            int encRet = 0;

            while (0 < (samplesDecoded = decoder->decode(&buffer[0], sizeof(buffer), &bytesConsumed))) {

            	std::cout << "samples: " << samplesDecoded << " bytes: " << bytesConsumed << std::endl;

            	encRet = encoder->encode(buffer, bytesConsumed, samplesDecoded);
                file.write((const char*)encoder->getBuffer(), encRet);
            }

            encRet = encoder->encodeFlush();
			file.write((const char*)encoder->getBuffer(), encRet);


            file.close();

            delete encoder;
        	decoder->closeFile();
            delete decoder;
        }

        void teardown() throw (Test::TeardownException)
        {
        }

    protected:
        virtual const std::string getPluginName() const = 0;
        virtual const std::string getFileExtension() const = 0;
};


class EncoderLameTest: public EncoderTest
{
    protected:
        const std::string getPluginName() const { return "lame"; }
        const std::string getFileExtension() const { return "mp3"; }
};


int main(int argc, char* argv[])
{
    Test::App app(argc, argv, Test::App::TA_PLUGINS);
    app.m_tests.push_back(new EncoderLameTest());
    return app.run();
}
