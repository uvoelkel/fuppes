/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Item.h"

using namespace Transcoding;

Item::Item()
{
	cacheItem = NULL;
    buffer = NULL;
    clear();
}

Item::~Item()
{
    if (NULL != buffer) {
        free(buffer);
    }
}

void Item::clear()
{
    transcodeTarget = Item::Unknown;
    m_transcodeSteps.clear();
    m_threaded = false;

    decoder = "";
    encoder = "";
    transcoder = "";

    bufferSize = 0;
    if (NULL != buffer) {
        free(buffer);
        buffer = NULL;
    }

    totalSamples = 0;
    guessedContentLength = 0;

    width = 0;
    height = 0;
}


void Item::appendTranscodeStep(Transcoding::Item::TranscodeType step)
{
	Threading::MutexLocker locker(&m_transcodeStepsMutex);
	m_transcodeSteps.push_back(step);

	if (Transcoding::Item::Threaded == step) {
		m_threaded = true;
	}
}
