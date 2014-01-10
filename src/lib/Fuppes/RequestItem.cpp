/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "RequestItem.h"

using namespace Fuppes;

RequestItem::RequestItem() :
        Database::Item(),
        Transcoding::Item()
{
	deviceSettings = NULL;
}

void RequestItem::clear()
{
    Database::Item::clear();
    Transcoding::Item::clear();
    Dlna::Item::clear();

    mimeType = "";
    uri = "";
    deviceSettings = NULL;
}

void RequestItem::setMimeType(const std::string _mimeType)
{
    mimeType = _mimeType;
}

object_type_t RequestItem::getType()
{
    return type;
}

std::string RequestItem::getExtension()
{
    return extension;
}

int RequestItem::getWidth()
{
    return details->iv_width;
}

int RequestItem::getHeight()
{
    return details->iv_height;
}

std::string RequestItem::getAudioCodec()
{
    return details->audioCodec;
}

std::string RequestItem::getVideoCodec()
{
    return details->videoCodec;
}

int RequestItem::getAudioChannels()
{
    return details->a_channels;
}

int RequestItem::getAudioBitrate()
{
    return details->a_bitrate;
}

bool RequestItem::isTranscoded()
{
    if (!m_transcodeSteps.empty()) {
        return (Transcoding::Item::Threaded == m_transcodeSteps.back());
    }
    else {
        return false;
    }
}

CDeviceSettings* RequestItem::getDeviceSettings()
{
	return deviceSettings;
}

std::string RequestItem::getCacheIdentifier()
{
    std::string value = getObjectIdAsHex() + "_" +
    		decoder + encoder + transcoder + "_" +
    		deviceSettings->name() + "." + extension;

	return value;
}

std::string RequestItem::getPath()
{
    return path;
}

std::string RequestItem::getFilename()
{
    return filename;
}



std::string RequestItem::getTitle()
{
    return title;
}
