/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "ItemDetails.h"

using namespace Database;

ItemDetails::ItemDetails()
{
    clear();
}

void ItemDetails::clear()
{
    id = 0;

    publisher = "";
    date = "";
    description = "";
    longDescription = "";

    // audio / video properties
    av_genre = "";
    av_language = "";
    av_artist = "";
    av_album = "";
    av_contributor = "";
    av_producer = "";
    av_duration = 0;

    // image / video properties
    iv_width = 0;
    iv_height = 0;
    iv_depth = 0;

    // audio properties
    a_trackNumber = 0;
    a_channels = 0;
    a_bitrate = 0;
    a_bitsPerSample = 0;
    a_samplerate = 0;
    a_year = 0;
    a_composer = "";

    // video properties
    v_actors = "";
    v_director = "";
    v_seriesTitle = "";
    v_programTitle = "";
    v_episodeNumber = 0;
    v_episodeCount = 0;
    v_hasSubtitles = false;
    v_bitrate = 0;

    audioCodec = "";
    videoCodec = "";

    // album art
    albumArtId = 0;
    albumArtExtension = "";
    albumArtMimeType = "";
    albumArtWidth = 0;
    albumArtHeight = 0;

    // other sources
    //source = 0;
    //streamMimeType = "";

}
