/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PLUGIN_ENCODER_H
#define _PLUGIN_ENCODER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Plugin.h"
#include "AbstractEncoder.h"

namespace Plugin
{


	typedef int (*encodeSetup_t)(::Transcoding::AbstractItem& item, void** data);
	typedef fuppes_off_t (*guessContentLength_t)(fuppes_int64_t samples, void** data);

	typedef size_t (*encode_t)(unsigned char* buffer, size_t size, int samples, void** data);
	typedef size_t (*encodeFlush_t)(void** data);

	typedef Endianness (*getEndianness_t)(void** data);
	typedef unsigned char* (*getBuffer_t)(void** data);

	typedef int (*encodeTeardown_t)(void** data);

    class Encoder: public Plugin, public AbstractEncoder
    {
        public:
            Encoder();
            Encoder(Encoder* plugin);
            bool init(LibraryHandle handle);

            bool setup(::Transcoding::AbstractItem& item);
            fuppes_off_t guessContentLength(fuppes_int64_t samples);

            size_t encode(unsigned char* buffer, size_t size, int samples);
            size_t encodeFlush();

            Endianness getEndianness();
            unsigned char* getBuffer();

            void teardown();

        private:
            encodeSetup_t		m_encodeSetup;
            guessContentLength_t m_guessContentLength;

            encode_t 			m_encode;
            encodeFlush_t		m_encodeFlush;

            getEndianness_t		m_getEndianness;
            getBuffer_t			m_getBuffer;

            encodeTeardown_t	m_encodeTeardown;
    };
}

#endif // _PLUGIN_ENCODER_H
