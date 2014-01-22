/*
 * This file is part of fuppes
 *
 * (c) 2013-2014 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "CacheItem.h"

#include "Item.h"

#include <iostream>

using namespace Transcoding;

CacheItem::CacheItem(const std::string identifier) :
		Threading::Thread("transcoding cache item")
{
	m_identifier = identifier;
	m_initialized = false;

	m_threaded = false;
	m_completed = false;
	m_error = false;
	m_state = CacheItem::Invalid;

	m_transcodeToFile = false;
	m_decoder = NULL;
	m_encoder = NULL;
	m_transcoder = NULL;

	m_referenceCount = 0;
	m_releaseCount = 0;
	m_releaseDelay = 0;

	m_buffer = NULL;
	m_bufferSize = 0;

	m_data = NULL;
	m_dataSize = 0;
}

CacheItem::~CacheItem()
{

	if (NULL != m_decoder) {
		delete m_decoder;
	}

	if (NULL != m_encoder) {
		delete m_encoder;
	}

	if (NULL != m_transcoder) {
		delete m_transcoder;
	}

	if (m_file.isOpen()) {
		m_file.close();
	}

	if (NULL != m_buffer) {
		delete[] m_buffer;
	}

	if (NULL != m_data) {
		free(m_data);
	}
}

bool CacheItem::init(Transcoding::Item& item)
{
	if (m_initialized) {
		return false;
	}

	if (Transcoding::Item::Unknown == item.transcodeTarget) {
		return false;
	}

	m_threaded = item.isThreaded();
	m_transcodeToFile = (Transcoding::Item::File == item.transcodeTarget);

	// open output file
	if (Transcoding::Item::File == item.transcodeTarget && 0 != item.targetPath.compare(item.originalPath)) {
		m_file.setFileName(item.targetPath);
		m_targetPath = item.targetPath;
		if (!m_file.open(fuppes::File::Write | fuppes::File::Truncate)) {
			return false;
		}
	}

	if (NULL != m_transcoder) {

		if (Transcoding::Item::File == item.transcodeTarget) {
			if (!m_file.isOpen()) {
				return false;
			}
			m_file.close();
			fuppes::File::remove(item.targetPath);
		}

		if (!m_transcoder->setup(item)) {
			return false;
		}
	}
	else if (NULL != m_decoder && NULL != m_encoder) {

		if (!m_decoder->openFile(item.originalPath)) {
			return false;
		}
		item.totalSamples = m_decoder->getTotalSamples();

		if (!m_encoder->setup(item)) {
			m_decoder->closeFile();
			return false;
		}
		item.guessedContentLength = m_encoder->guessContentLength(item.totalSamples);

		m_bufferSize = m_decoder->getBufferSize();
		m_buffer = new unsigned char[m_bufferSize];

	}
	else {
		return false;
	}

	m_initialized = true;
	m_state = CacheItem::Idle;
	return true;
}

bool CacheItem::transcode()
{
	if (CacheItem::Idle != m_state) {
		return false;
	}

	if (m_threaded) {
		this->start();
	}
	else {
		this->doTranscode();
	}

	return true;
}

bool CacheItem::canPause() const
{
	if (!m_threaded) {
		return false;
	}

	if (m_completed || m_error) {
		return false;
	}

	// the ability to pause depends on the transcoder
	if (NULL != m_transcoder) {
		return m_transcoder->canPause();
	}
	// a combination of de-/encoder can always be paused
	else if (NULL != m_decoder && NULL != m_encoder) {
		return true;
	}

	return false;
}

bool CacheItem::isPaused() const
{
	return (CacheItem::Paused == m_state);
}

void CacheItem::pause()
{
	if (!canPause()) {
		return;
	}

	m_state = CacheItem::Paused;
}

void CacheItem::resume()
{
	if (!isPaused()) {
		return;
	}

	m_state = CacheItem::Running;
}

void CacheItem::run()
{
	this->doTranscode();
}

void CacheItem::doTranscode()
{
	m_error = false;
	m_state = CacheItem::Running;

	std::cout << "CacheItem :: start transcoding loop: " << threadId() << std::endl;

	if (NULL != m_transcoder) {

		if (m_transcodeToFile) {

			int ret = 0;
			while (!stopRequested()) {

				ret = m_transcoder->transcode();
				if (0 >= ret) { // done or error
					m_error = (0 != ret);
					break;
				}

				if (m_threaded) {
					this->msleep(200);
				}
				m_dataSize = fuppes::File::getSize(m_targetPath);
			}

		}
		else {
			// todo
		}

	}
	else if (NULL != m_decoder && NULL != m_encoder) {

		int samples = 0;
		size_t bytes = 0;

		int encRet = 0;

		while (!stopRequested()) {

			if (CacheItem::Paused == m_state) {
				this->msleep(200);
				continue;
			}

			samples = m_decoder->decode(m_buffer, m_bufferSize, &bytes);

			// eof
			if (0 == samples) {
				break;
			}
			// error
			else if (0 > samples) {
				m_error = true;
				break;
			}

			encRet = m_encoder->encode(m_buffer, bytes, samples);

			// may return 0. this is not an error
			if (0 == encRet) {
				continue;
			}
			// less than 0 however is
			else if (0 > encRet) {
				m_error = true;
				break;
			}

			if (m_file.isOpen()) {
				m_dataSize += m_file.write((const char*)m_encoder->getBuffer(), encRet);
			}
			else {
				m_data = (unsigned char*)((NULL == m_data) ? malloc(encRet) : realloc(m_data, m_dataSize + encRet));
				memcpy(&m_data[m_dataSize], m_encoder->getBuffer(), encRet);
				m_dataSize += encRet;
			}
		}

		encRet = m_encoder->encodeFlush();
		if (0 < encRet) {
			if (m_file.isOpen()) {
				m_dataSize += m_file.write((const char*)m_encoder->getBuffer(), encRet);
			}
			else {
				m_data = (unsigned char*)((NULL == m_data) ? malloc(encRet) : realloc(m_data, m_dataSize + encRet));
				memcpy(&m_data[m_dataSize], m_encoder->getBuffer(), encRet);
				m_dataSize += encRet;
			}
		}
	}

	m_completed = !stopRequested() && !m_error;
	m_state = m_completed ? CacheItem::Completed : CacheItem::Error;

	// move completed files for reuse
	if (m_completed && m_transcodeToFile && m_threaded) {
		if (fuppes::File::move(m_targetPath, m_targetPath + "_complete")) {
			m_targetPath += "_complete";
		}
	}

	std::cout << "CacheItem :: exit transcoding loop: " << (m_completed ? "completed" : "not completed") << " " << (m_error ? "error" : "ok") << std::endl;

	if (NULL != m_decoder) {
		m_decoder->closeFile();
	}

	if (NULL != m_encoder) {
		m_encoder->teardown();
	}

	if (NULL != m_transcoder) {
		m_transcoder->teardown();
	}
}
