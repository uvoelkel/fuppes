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
    transcodeSteps.clear();

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

bool Item::isThreaded()
{
    for(size_t i = 0; i < transcodeSteps.size(); i++) {
        if(Transcoding::Item::Threaded == transcodeSteps.at(i)) {
            return true;
        }
    }
    return false;
}
