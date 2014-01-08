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
        	return "";
        }
		std::string getTargetPath() {
			return "";
		}
		std::string getTranscoderCommand() {
			return "";
		}
};

class DecoderTest: public Test::BaseTest
{
    public:
        DecoderTest() : BaseTest("decoder test")
        {
        }

        void setup() throw (Test::SetupException)
        {
        }

        void run() throw (Test::Exception)
        {
        	// create the decoder
            Plugin::AbstractDecoder* decoder = pluginManager->createDecoderPlugin(getPluginName());
            if (NULL == decoder) {
                throw Test::Exception("error loading plugin: " + getPluginName());
            }

			// open input file
            std::string filename = testDir + "test." + getFileExtension();
            if (!decoder->openFile(filename)) {
                delete decoder;
                throw Test::Exception("error opening file " + filename);
            }
            std::cout << "file: " << filename << " opened. samples: " << decoder->getTotalSamples() << std::endl;

            // open output file
            filename += ".wav";
            fuppes::File file(filename);
            if (!file.open(fuppes::File::Write | fuppes::File::Truncate)) {
                delete decoder;
                throw Test::Exception("error opening file " + filename);
            }

			// create encoder
            Plugin::AbstractEncoder* encoder = pluginManager->createEncoderPlugin("wav");
		    if (NULL == encoder) {
			    delete decoder;
			    throw Test::Exception("error loading plugin: wav");
		    }


            TestItem item;
            decoder->readMetadata(item);
            item.totalSamples = decoder->getTotalSamples();
            encoder->setup(item);

            fuppes_off_t lengthGuess = encoder->guessContentLength(item.totalSamples);
            fuppes_off_t lengthActual = 0;

            std::cout << "encoder setup. guessed content length: " << lengthGuess << std::endl;

            unsigned char* buffer = new unsigned char[decoder->getBufferSize()];
            int samplesDecoded = 0;
            size_t bytesConsumed = 0;
            size_t bytesEncoded = 0;
            while (0 < (samplesDecoded = decoder->decode(&buffer[0], sizeof(buffer), &bytesConsumed))) {

                //std::cout << bytesConsumed << " bytes consumed. " << samplesDecoded << " samples decoded." << std::endl;
            	bytesEncoded = encoder->encode(buffer, bytesConsumed, samplesDecoded);
                file.write((const char*)encoder->getBuffer(), bytesEncoded);

                lengthActual += bytesEncoded;
            }

            delete[] buffer;
            delete encoder;

            file.close();
            decoder->closeFile();
            delete decoder;

            std::cout << "done. length - guessed: " << lengthGuess << " actual: " << lengthActual << std::endl;
        }

        void teardown() throw (Test::TeardownException)
        {
        }

    protected:
        virtual const std::string getPluginName() const = 0;
        virtual const std::string getFileExtension() const = 0;
};


class DecoderVorbisTest: public DecoderTest
{
    protected:
        const std::string getPluginName() const { return "vorbis"; }
        const std::string getFileExtension() const { return "ogg"; }
};

class DecoderFlacTest: public DecoderTest
{
    protected:
        const std::string getPluginName() const { return "flac"; }
        const std::string getFileExtension() const { return "flac"; }
};

class DecoderWavTest: public DecoderTest
{
    protected:
        const std::string getPluginName() const { return "wav"; }
        const std::string getFileExtension() const { return "wav"; }
};

int main(int argc, char* argv[])
{
    Test::App app(argc, argv, Test::App::TA_PLUGINS);
    app.m_tests.push_back(new DecoderVorbisTest());
    app.m_tests.push_back(new DecoderFlacTest());
    //app.m_tests.push_back(new DecoderWavTest());
    return app.run();
}
