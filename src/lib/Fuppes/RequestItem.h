/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _FUPPES_REQUESTITEM_H
#define _FUPPES_REQUESTITEM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Database/Item.h"
#include "../Dlna/Item.h"
#include "../Transcoding/Item.h"

#include <fuppes_plugin.h>

namespace Fuppes
{

    class RequestItem: public Database::Item, public Dlna::Item, public Transcoding::Item
    {
        public:

            RequestItem();

            void clear();

            std::string mimeType;
            std::string uri;
            CDeviceSettings* deviceSettings;

            bool isAudioItem() const
            {
                return isAudio();
            }

            bool isVideoItem() const
            {
                return isVideo();
            }

            bool isImageItem() const
            {
                return isImage();
            }

            // Transcoding::AbstractItem
            ::Database::AbstractItem&	getMetadata()
            {
            	return *this;
            }

        protected:

            void setMimeType(const std::string mimeType);

            object_type_t getType();
            std::string getExtension();

            int getWidth();
            int getHeight();

            std::string getAudioCodec();
            std::string getVideoCodec();

            int getAudioChannels();
            int getAudioBitrate();

            bool isTranscoded();

            CDeviceSettings* getDeviceSettings();
            std::string getCacheIdentifier();
            std::string getPath();
            std::string getFilename();

            std::string getTitle();
    };

}

#endif // _FUPPES_REQUESTITEM_H
