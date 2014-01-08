/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PLUGIN_IMAGE_H
#define _PLUGIN_IMAGE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Plugin.h"
#include "AbstractFile.h"
#include "AbstractMetadata.h"

namespace Plugin
{
    class Image: public Plugin, public AbstractFile, public AbstractMetadata
    {
        public:
            Image();
            Image(Image* plugin);
            bool init(LibraryHandle handle);

            // scale
            // convert
    };
}

#endif // _PLUGIN_IMAGE_H
