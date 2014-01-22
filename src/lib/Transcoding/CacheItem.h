/*
 * This file is part of fuppes
 *
 * (c) 2013-2014 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _TRANSCODING_CACHEITEM_H
#define _TRANSCODING_CACHEITEM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Common/File.h"
#include "../Thread/Thread.h"
#include "../Plugin/AbstractDecoder.h"
#include "../Plugin/AbstractEncoder.h"
#include "../Plugin/AbstractTranscoder.h"

namespace Transcoding
{
	class Item;

	class CacheItem: public Threading::Thread
	{
		public:

			enum State
			{
				Invalid,
				Idle,
				Running,
				Paused,
				Completed,
				Error
			};

			CacheItem(const std::string identifier);
			~CacheItem();

			bool init(Transcoding::Item& item);
			bool transcode();

			bool canPause() const;
			bool isPaused() const;
			void pause();
			void resume();

			std::string m_identifier;
			bool m_initialized;

			bool m_threaded;
			bool m_completed;
			bool m_error;
			State m_state;

			bool m_transcodeToFile;
			Plugin::AbstractDecoder* m_decoder;
			Plugin::AbstractEncoder* m_encoder;
			Plugin::AbstractTranscoder* m_transcoder;

			unsigned int m_referenceCount;
			unsigned int m_releaseCount;
			unsigned int m_releaseDelay;

			// the buffer holding the decoder result
			unsigned char* m_buffer;
			size_t m_bufferSize;

			// the buffer holding the final encoded/transcoded result
			unsigned char* m_data;
			fuppes_off_t m_dataSize;

			fuppes::File m_file;
			std::string m_targetPath;

		private:
			void run();
			void doTranscode();
	};

}

#endif // _TRANSCODING_CACHEITEM_H
