/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "CacheItem.h"

#include "Item.h"

#include <iostream>

using namespace Transcoding;

CacheItem::CacheItem() :
	Threading::Thread("transcoding cache item")
{
	m_initialized = false;
	m_item = NULL;

	m_completed = false;
	m_error = false;
	m_decoder = NULL;
	m_encoder = NULL;
	m_transcoder = NULL;

	m_refCount = 0;
	m_relCount = 0;

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

bool CacheItem::init()
{
	if (m_initialized) {
		return false;
	}

	// open output file
	if (Transcoding::Item::File == m_item->transcodeTarget && 0 != m_item->targetPath.compare(m_item->originalPath)) {
		m_file.setFileName(m_item->targetPath);
		if (!m_file.open(fuppes::File::Write | fuppes::File::Truncate)) {
			return false;
		}
	}


	if (NULL != m_transcoder) {

		if (Transcoding::Item::File == m_item->transcodeTarget) {
			if (!m_file.isOpen()) {
				return false;
			}
			m_file.close();
			fuppes::File::remove(m_item->targetPath);
		}

		if (!m_transcoder->setup(*m_item)) {
			return false;
		}
	}
	else if (NULL != m_decoder && NULL != m_encoder) {

		if (!m_decoder->openFile(m_item->originalPath)) {
			return false;
		}
		m_item->totalSamples = m_decoder->getTotalSamples();

		if (!m_encoder->setup(*m_item)) {
			m_decoder->closeFile();
			return false;
		}
		m_item->guessedContentLength = m_encoder->guessContentLength(m_item->totalSamples);

		m_bufferSize = m_decoder->getBufferSize();
		m_buffer = new unsigned char[m_bufferSize];

	}
	else {
		return false;
	}

	m_initialized = true;
	return true;
}

bool CacheItem::transcode()
{
	if (m_item->isThreaded()) {
		this->start();
	}
	else {
		this->doTranscode();
	}

	return true;
}

void CacheItem::run()
{
	this->doTranscode();
}

void CacheItem::doTranscode()
{

	m_error = false;

	std::cout << "CacheItem :: start transcoding loop: " << threadId() << std::endl;

	if (NULL != m_transcoder) {

		if (Transcoding::Item::File == m_item->transcodeTarget) {

			int ret = 0;
			while (!stopRequested()) {

				ret = m_transcoder->transcode();
				if (0 >= ret) { // done or error
					m_error = (0 != ret);
					break;
				}

				if (m_item->isThreaded()) {
					this->msleep(200);
				}
				m_dataSize = fuppes::File::getSize(m_item->targetPath);
			}

		}
		else if (Transcoding::Item::Memory == m_item->transcodeTarget) {

		}
		else {
			m_error = true;
		}

	}
	else if (NULL != m_decoder && NULL != m_encoder) {

		int samples = 0;
		size_t bytes = 0;

		int encRet = 0;

		while (!stopRequested()) {

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

	if (m_completed && Transcoding::Item::File == m_item->transcodeTarget && m_item->isThreaded()) {
		if (fuppes::File::move(m_item->targetPath, m_item->targetPath + "_complete")) {
			m_item->targetPath += "_complete";
		}
	}

	std::cout << "CacheItem :: exit transcoding loop: " << (m_completed ? "completed" : "not completed") << " " << (m_error  ? "error" : "ok") << std::endl;

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
