/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Transcoder.h"

#include "TranscoderExternalProcess.h"

using namespace Transcoding;

#include <iostream>

Transcoder::Transcoder(Plugin::Manager& pluginManager, Transcoding::Configuration& configuration) :
		m_pluginManager(pluginManager),
		m_cache(pluginManager)
{
	m_tempDir = configuration.tempDir;
}

bool Transcoder::initializeTranscoding(Transcoding::Item& item)
{
	std::cout << "Transcoder::initializeTranscoding" << std::endl;

	// set the transcode target
	bool cached = false;
	item.transcodeTarget = Transcoding::Item::Unknown;

	for (size_t i = 0; i < item.m_transcodeSteps.size(); i++) {

		switch (item.m_transcodeSteps.at(i)) {

			case Transcoding::Item::None:
				return false;
				break;

			case Transcoding::Item::Rename:
				item.transcodeTarget = Transcoding::Item::File;
				break;

			case Transcoding::Item::ExtractImage:
				cached = false;
				item.transcodeTarget = Transcoding::Item::Memory;
				break;
			case Transcoding::Item::ConvertImage:
			case Transcoding::Item::ScaleImage:
				cached = false;
				item.transcodeTarget = Transcoding::Item::Memory;
				break;

			case Transcoding::Item::Oneshot:
			case Transcoding::Item::Threaded:
				cached = true;
				if (item.isAudioItem()) {
					item.transcodeTarget = Transcoding::Item::File;
				}
				else if (item.isImageItem()) {
					item.transcodeTarget = Transcoding::Item::Memory;
				}
				else if (item.isVideoItem()) {
					item.transcodeTarget = Transcoding::Item::File;
				}
				else {
					item.transcodeTarget = Transcoding::Item::Unknown;
					return false;
				}

				item.decoder = item.getDeviceSettings()->getDecoderName(item.originalExtension);
				item.encoder = item.getDeviceSettings()->getEncoderName(item.originalExtension);
				item.transcoder = item.getDeviceSettings()->getTranscoderName(item.originalExtension, item.getMetadata().details->audioCodec, item.getMetadata().details->videoCodec);
				item.transcoderCommand = item.getDeviceSettings()->getTranscoderCommand(item.originalExtension, item.getMetadata().details->audioCodec, item.getMetadata().details->videoCodec);
				break;
		}
	}

	if (cached) {

		// set the target path
		if (Transcoding::Item::File == item.transcodeTarget) {
			item.targetPath = m_tempDir + item.getCacheIdentifier();
		}

		// check if the file is already completely transcoded
		if (fuppes::File::exists(item.targetPath + "_complete")) {
			item.targetPath += "_complete";
			item.m_transcodeSteps.clear();
			return true;
		}

		item.cacheItem = m_cache.getItem(item.getCacheIdentifier(), item.getDeviceSettings()->ReleaseDelay(item.originalExtension));
		if (item.cacheItem->m_initialized) {
			return true;
		}

		std::string error;

		if (!item.decoder.empty() && !item.encoder.empty()) {

			item.cacheItem->m_decoder = m_pluginManager.createDecoderPlugin(item.decoder);
			item.cacheItem->m_encoder = m_pluginManager.createEncoderPlugin(item.encoder);

			if (NULL == item.cacheItem->m_decoder || NULL == item.cacheItem->m_encoder) {
				error = "failed to create decoder and/or encoder";
			}
		}
		else if (!item.transcoder.empty()) {

			if (0 == item.transcoder.compare("external") || 0 == item.transcoder.compare("cmd")) {
				item.cacheItem->m_transcoder = new TranscoderExternalProcess();
			}
			else {
				item.cacheItem->m_transcoder = m_pluginManager.createTranscoderPlugin(item.transcoder);
			}

			if (NULL == item.cacheItem->m_transcoder) {
				error = "failed to create transcoder";
			}
		}

		if (!error.empty()) {
			m_cache.releaseItem(item.cacheItem);
			item.cacheItem = NULL;
			return false;
		}

		if (!item.cacheItem->init(item)) {
			m_cache.releaseItem(item.cacheItem);
			item.cacheItem = NULL;
			return false;
		}
	}

	return true;
}

bool Transcoder::executeTranscoding(Transcoding::Item& item) const
{

	std::cout << "Transcoder::executeTranscoding" << std::endl;

	Threading::MutexLocker locker(&item.m_transcodeStepsMutex);

	while (!item.m_transcodeSteps.empty()) {

		switch (item.m_transcodeSteps.front()) {

			case Transcoding::Item::None:
			case Transcoding::Item::Rename:
				break;

			case Transcoding::Item::ExtractImage:
				std::cout << "Transcoder::ExtractImage" << std::endl;
				if (false == extractImageFromAudio(item)) {
					return false;
				}
				break;

			case Transcoding::Item::ConvertImage:
				std::cout << "Transcoder::ConvertImage" << std::endl;
				if (false == convertImage(item)) {
					return false;
				}
				break;

			case Transcoding::Item::ScaleImage:
				std::cout << "Transcoder::ScaleImage" << std::endl;
				if (false == scaleImage(item)) {
					return false;
				}
				break;

			case Transcoding::Item::Oneshot:
				case Transcoding::Item::Threaded:
				item.cacheItem->transcode();
				break;
		}

		item.m_transcodeSteps.pop_front();
	}

	return true;
}

void Transcoder::release(Transcoding::Item& item)
{
	if (NULL == item.cacheItem) {
		return;
	}

	m_cache.releaseItem(item.cacheItem);
	item.cacheItem = NULL;
}

bool Transcoder::extractImageFromAudio(Transcoding::Item& item) const
{
	Plugin::Metadata* plugin = m_pluginManager.createMetadataPlugin("taglib");
	if (NULL == plugin) {
		return false;
	}

	if (false == plugin->openFile(item.originalPath)) {
		delete plugin;
		return false;
	}

	char tmpMime[100];
	item.bufferSize = 0;
	item.buffer = (unsigned char*)malloc(1);

	bool gotImage = plugin->readImage(&tmpMime[0], &item.buffer, &item.bufferSize);
	plugin->closeFile();
	delete plugin;

	return gotImage;
}

bool Transcoder::convertImage(Transcoding::Item& item) const
{
	return false;
}

bool Transcoder::scaleImage(Transcoding::Item& item) const
{
	return true;

	if (NULL == item.buffer) {
		fuppes::File file(item.targetPath);
		if (!file.open(fuppes::File::Read)) {
			return false;
		}

		item.buffer = (unsigned char*)malloc(file.size());
		item.bufferSize = file.read((char*)item.buffer, file.size());
		file.close();
	}

	Plugin::AbstractTranscoder* plugin = m_pluginManager.createTranscoderPlugin("magickWand");
	if (NULL == plugin) {
		return false;
	}

	//plugin->TranscodeMem()

	delete plugin;

	return false;
}
