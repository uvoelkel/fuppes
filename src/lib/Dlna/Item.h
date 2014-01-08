/*
 * This file is part of fuppes
 *
 * (c) 2010-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _DLNA_ITEM_H
#define _DLNA_ITEM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_upnp_object_types.h>
#include <string>

namespace Dlna
{
    class Item
    {
        public:
            virtual ~Item() { }

            virtual void clear() {
                profile = "";
                dlna4thfield = "";
            }

            std::string profile;
            std::string dlna4thfield;

            virtual void setMimeType(std::string mimeType) = 0;

            virtual object_type_t   getType() = 0;
            virtual std::string getExtension() = 0;

            virtual int getWidth() = 0;
            virtual int getHeight() = 0;

            virtual std::string getAudioCodec() = 0;
            virtual std::string getVideoCodec() = 0;

            virtual int getAudioChannels() = 0;
            virtual int getAudioBitrate() = 0;

            virtual bool isTranscoded() = 0;
    };
}

#endif // _DLNA_ITEM_H
