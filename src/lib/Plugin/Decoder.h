/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PLUGIN_DECODER_H
#define _PLUGIN_DECODER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Plugin.h"
#include "AbstractDecoder.h"

namespace Plugin
{
    typedef int (*openFile_t)(const char* filename, void** data);
    typedef int (*decode_t)(unsigned char* buffer, size_t size, size_t* bytesConsumed, void** data);
    typedef void (*closeFile_t)(void** data);

    typedef fuppes_int64_t (*getTotalSamples_t)(void** data);
    typedef int (*readMetadata_t)(::Database::AbstractItem& item, void** data);

    typedef fuppes_int64_t (*getTotalSamples_t)(void** data);

    typedef size_t (*getBufferSize_t)(void** data);
    typedef int (*setEndianness_t)(Endianness endianness,void** data);

    class Decoder: public Plugin, public AbstractDecoder
    {
        public:
            Decoder();
            Decoder(Decoder* plugin);
            bool init(LibraryHandle handle);

            bool openFile(const std::string filename);
            int decode(unsigned char* buffer, size_t size, size_t* bytesConsumed);
            void closeFile();

            fuppes_int64_t getTotalSamples();
            bool readMetadata(::Database::AbstractItem& item);

            size_t getBufferSize();
            bool setEndianness(Endianness endianness);

        private:
            openFile_t m_openFile;
            decode_t m_decode;
            closeFile_t m_closeFile;

            getTotalSamples_t m_getTotalSamples;
			readMetadata_t	m_readMetadata;

            getBufferSize_t	m_getBufferSize;
            setEndianness_t m_setEndianness;
    };
}

#endif // _PLUGIN_DECODER_H
