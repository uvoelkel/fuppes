/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Encoder.h"

using namespace Plugin;

Encoder::Encoder() :
        Plugin()
{
	m_encodeSetup = NULL;
	m_guessContentLength = NULL;

	m_encode = NULL;
	m_encodeFlush = NULL;

	m_getEndianness = NULL;
	m_getBuffer = NULL;

	m_encodeTeardown = NULL;
}

Encoder::Encoder(Encoder* plugin) :
        Plugin(plugin)
{
	m_encodeSetup = plugin->m_encodeSetup;
	m_guessContentLength = plugin->m_guessContentLength;

	m_encode = plugin->m_encode;
	m_encodeFlush = plugin->m_encodeFlush;

	m_getEndianness = plugin->m_getEndianness;
	m_getBuffer = plugin->m_getBuffer;

	m_encodeTeardown = plugin->m_encodeTeardown;
}

bool Encoder::init(LibraryHandle handle)
{
	m_encodeSetup = (encodeSetup_t)Plugin::getProcedure(handle, "fuppes_encoder_setup");
	m_guessContentLength = (guessContentLength_t)Plugin::getProcedure(handle, "fuppes_encoder_guess_content_length");

	m_encode = (encode_t)Plugin::getProcedure(handle, "fuppes_encoder_interleaved");
	m_encodeFlush = (encodeFlush_t)Plugin::getProcedure(handle, "fuppes_encoder_flush");

	m_getEndianness = (getEndianness_t)Plugin::getProcedure(handle, "fuppes_get_endianness");
	m_getBuffer = (getBuffer_t)Plugin::getProcedure(handle, "fuppes_get_buffer");

	m_encodeTeardown = (encodeTeardown_t)Plugin::getProcedure(handle, "fuppes_encoder_teardown");

    return true;
}


Endianness Encoder::getEndianness()
{
	return m_getEndianness(&m_data);
}

bool Encoder::setup(::Transcoding::AbstractItem& item)
{
    return (0 == m_encodeSetup(item, &m_data));
}

fuppes_off_t Encoder::guessContentLength(fuppes_int64_t samples)
{
	if (NULL == m_guessContentLength) {
		return 0;
	}
	return m_guessContentLength(samples, &m_data);
}

size_t Encoder::encode(unsigned char* buffer, size_t size, int samples)
{
	return m_encode(buffer, size, samples, &m_data);
}

size_t Encoder::encodeFlush()
{
    if (NULL == m_encodeFlush) {
        return 0;
    }
    return m_encodeFlush(&m_data);
}

unsigned char* Encoder::getBuffer()
{
    return m_getBuffer(&m_data);
}

void Encoder::teardown()
{
    m_encodeTeardown(&m_data);
}
