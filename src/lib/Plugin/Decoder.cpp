/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Decoder.h"

using namespace Plugin;

#define DEFAULT_BUFFER_SIZE 32768

Decoder::Decoder() :
        Plugin()
{
    m_openFile = NULL;
    m_decode = NULL;
    m_closeFile = NULL;

    m_readMetadata = NULL;
    m_getTotalSamples = NULL;

    m_getBufferSize = NULL;
    m_setEndianness = NULL;
}

Decoder::Decoder(Decoder* plugin) :
        Plugin(plugin)
{
    m_openFile = plugin->m_openFile;
    m_decode = plugin->m_decode;
    m_closeFile = plugin->m_closeFile;

    m_readMetadata = plugin->m_readMetadata;
    m_getTotalSamples = plugin->m_getTotalSamples;

    m_getBufferSize = plugin->m_getBufferSize;
    m_setEndianness = plugin->m_setEndianness;
}

bool Decoder::init(LibraryHandle handle)
{
    m_openFile = (openFile_t)Plugin::getProcedure(handle, "fuppes_open_file");
    m_decode = (decode_t)Plugin::getProcedure(handle, "fuppes_decode_interleaved");
    m_closeFile = (closeFile_t)Plugin::getProcedure(handle, "fuppes_close_file");

    m_readMetadata = (readMetadata_t)Plugin::getProcedure(handle, "fuppes_read_metadata");
    m_getTotalSamples = (getTotalSamples_t)Plugin::getProcedure(handle, "fuppes_get_total_samples");

    m_getBufferSize = (getBufferSize_t)Plugin::getProcedure(handle, "fuppes_get_buffersize");
    m_setEndianness = (setEndianness_t)Plugin::getProcedure(handle, "fuppes_set_endianness");

    return true;
}

bool Decoder::openFile(const std::string filename)
{
    if (NULL == m_openFile) {
        return false;
    }
    return 0 == m_openFile(filename.c_str(), &m_data);
}

int Decoder::decode(unsigned char* buffer, size_t size, size_t* bytesConsumed)
{
    if (NULL == m_decode) {
        return -1;
    }
    return m_decode(buffer, size, bytesConsumed, &m_data);
}

void Decoder::closeFile()
{
    if (NULL == m_closeFile) {
        return;
    }
    m_closeFile(&m_data);
}

fuppes_int64_t Decoder::getTotalSamples()
{
    if (NULL == m_getTotalSamples) {
        return 0;
    }

    return m_getTotalSamples(&m_data);
}

bool Decoder::readMetadata(Database::AbstractItem& item)
{
    if (NULL == m_readMetadata || NULL == item.details) {
        return false;
    }

    return (0 == m_readMetadata(item, &m_data));
}

size_t Decoder::getBufferSize()
{
    if (NULL == m_getBufferSize) {
        return DEFAULT_BUFFER_SIZE;
    }

    size_t size = m_getBufferSize(&m_data);
    return (0 == size) ? DEFAULT_BUFFER_SIZE : size;
}

bool Decoder::setEndianness(Endianness endianness)
{
    if (NULL == m_setEndianness) {
        return false;
    }
	return 0 == m_setEndianness(endianness, &m_data);
}


