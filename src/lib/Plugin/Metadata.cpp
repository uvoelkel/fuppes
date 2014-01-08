/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Metadata.h"

using namespace Plugin;

Metadata::Metadata() :
        Plugin()
{
    m_openFile = NULL;
    m_closeFile = NULL;

    m_readImage = NULL;
    m_readMetadata = NULL;
}

Metadata::Metadata(Metadata* plugin) :
        Plugin(plugin)
{
    m_openFile = plugin->m_openFile;
    m_closeFile = plugin->m_closeFile;

    m_readImage = plugin->m_readImage;
    m_readMetadata = plugin->m_readMetadata;
}

bool Metadata::init(LibraryHandle handle)
{
    m_openFile = (openFile_t)Plugin::getProcedure(handle, "fuppes_open_file");
    m_closeFile = (closeFile_t)Plugin::getProcedure(handle, "fuppes_close_file");

    m_readImage = (readImage_t)Plugin::getProcedure(handle, "fuppes_read_image");
    m_readMetadata = (readMetadata_t)Plugin::getProcedure(handle, "fuppes_read_metadata");

    return true;
}

