/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PLUGIN_ABSTRACTENCODER_H
#define _PLUGIN_ABSTRACTENCODER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_plugin.h>

namespace Plugin
{

	class AbstractEncoder
	{
		public:
			virtual ~AbstractEncoder() {}

			virtual bool setup(::Transcoding::AbstractItem& item) = 0;
			virtual fuppes_off_t guessContentLength(fuppes_int64_t samples) = 0;

			virtual size_t encode(unsigned char* buffer, size_t size, int samples) = 0;
			virtual size_t encodeFlush() = 0;

			virtual Endianness getEndianness() = 0;
			virtual unsigned char* getBuffer() = 0;

			virtual void teardown() = 0;
	};
}


#endif // _PLUGIN_ABSTRACTENCODER_H
