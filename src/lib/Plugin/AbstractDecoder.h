/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PLUGIN_ABSTRACTDECODER_H
#define _PLUGIN_ABSTRACTDECODER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_plugin.h>

namespace Plugin
{

	class AbstractDecoder
	{
		public:
			virtual ~AbstractDecoder() { }

			virtual bool openFile(const std::string filename) = 0;
			virtual int decode(unsigned char* buffer, size_t size, size_t* bytesConsumed) = 0;
			virtual void closeFile() = 0;

			virtual bool readMetadata(::Database::AbstractItem& item) = 0;
			virtual fuppes_int64_t getTotalSamples() = 0;

			virtual size_t getBufferSize() = 0;
			virtual bool setEndianness(Endianness endianness) = 0;
	};
}


#endif // _PLUGIN_ABSTRACTDECODER_H
