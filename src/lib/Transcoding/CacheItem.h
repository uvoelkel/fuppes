/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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
			CacheItem();
			~CacheItem();

			bool init();
			bool transcode();

			bool						m_initialized;
			Transcoding::Item*			m_item;

			bool						m_completed;
			bool						m_error;
			Plugin::AbstractDecoder*	m_decoder;
			Plugin::AbstractEncoder*	m_encoder;
			Plugin::AbstractTranscoder*	m_transcoder;

			int	m_refCount;
			int m_relCount;

			// the buffer holding the decoder result
			unsigned char*				m_buffer;
			size_t						m_bufferSize;

			// the buffer holding the final encoded/transcoded result
			unsigned char*				m_data;
			fuppes_off_t				m_dataSize;

			fuppes::File				m_file;

		private:
			void run();
			void doTranscode();
	};

}

#endif // _TRANSCODING_CACHEITEM_H
