/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Image.h"

using namespace Plugin;

Image::Image() :
        Plugin(),
        AbstractFile(),
        AbstractMetadata()
{

}

Image::Image(Image* plugin) :
    Plugin(plugin),
    AbstractFile(plugin, &m_data),
    AbstractMetadata(plugin, &m_data)
{

}

bool Image::init(LibraryHandle handle)
{
    if (!AbstractFile::init(handle, &m_data) || !AbstractMetadata::init(handle, &m_data)) {
        return false;
    }

    return true;
}
