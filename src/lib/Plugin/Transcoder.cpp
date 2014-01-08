/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Transcoder.h"

using namespace Plugin;

Transcoder::Transcoder() :
        Plugin()
{
	m_transcodeSetup = NULL;
}

Transcoder::Transcoder(Transcoder* plugin) :
        Plugin(plugin)
{
	m_transcodeSetup = plugin->m_transcodeSetup;
}

bool Transcoder::init(LibraryHandle handle)
{
	m_transcodeSetup = (transcodeSetup_t)Plugin::getProcedure(handle, "fuppes_transcoder_setup");

    return true;
}

bool Transcoder::setup(::Transcoding::AbstractItem& item)
{
    return (0 == m_transcodeSetup(item, &m_data));
}

int Transcoder::transcode()
{
	return -1;
}

bool Transcoder::canPause()
{
	return false;
}

void Transcoder::teardown()
{
	// todo
}
