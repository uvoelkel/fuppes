/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _TRANSCODING_ITEM_H
#define _TRANSCODING_ITEM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <deque>

#include "CacheItem.h"
#include "../DeviceIdentification/DeviceSettings.h"

namespace Transcoding
{
    class Item: public Transcoding::AbstractItem
    {
        public:
            enum TranscodeType
            {
                None = 0,
                Rename = 1 << 0,
                ExtractImage = 1 << 1,
                ConvertImage = 1 << 2,
                ScaleImage = 1 << 3,
                Oneshot = 1 << 4,
                Threaded = 1 << 5
            };

            enum TranscodeTarget
            {
                Unknown = 0,
                Memory = 1,
                File = 2,
            };

            Item();
            virtual ~Item();

            virtual void clear();

            virtual CDeviceSettings* getDeviceSettings() = 0;

            virtual std::string getCacheIdentifier() = 0;
            virtual std::string getPath() = 0;
            virtual std::string getFilename() = 0;

            virtual std::string getAudioCodec() = 0;
            virtual std::string getVideoCodec() = 0;

            virtual std::string getTitle() = 0;
            //virtual std::string getArtist() = 0;

            virtual bool isAudioItem() const = 0;
            virtual bool isVideoItem() const = 0;
            virtual bool isImageItem() const = 0;

            bool isThreaded() {
            	return m_threaded;
            }

            Transcoding::Item::TranscodeTarget transcodeTarget;

            std::string decoder;
            std::string encoder;
            std::string transcoder;
            std::string transcoderCommand;

            std::string originalExtension;
            std::string originalPath;
            std::string targetPath;

            std::string getSourcePath() {
            	return originalPath;
            }
			std::string getTargetPath() {
				return targetPath;
			}
			std::string getTranscoderCommand() {
				return transcoderCommand;
			}

			void appendTranscodeStep(Transcoding::Item::TranscodeType step);

            bool							m_threaded;
            std::deque<TranscodeType>    	m_transcodeSteps;
            Threading::Mutex				m_transcodeStepsMutex;

            int width;
            int height;

            Transcoding::CacheItem* cacheItem;
            unsigned char* buffer;
            fuppes_off_t bufferSize;


    };
}

#endif // _TRANSCODING_ITEM_H
