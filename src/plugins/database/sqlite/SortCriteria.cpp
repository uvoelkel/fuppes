/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "SortCriteria.h"

using namespace Sqlite;

#include <iostream>

const std::string SortCriteria::getSortCapabilities()
{
    // The property names returned MUST include the appropriate namespace prefixes, except for the DIDL-Lite namespace.
    // Properties in the DIDL-Lite namespace MUST always be returned without the prefix.
    return "dc:title,upnp:artist,upnp:genre,upnp:album,upnp:originalTrackNumber";
}

const std::string SortCriteria::getSortExtensionCapabilities()
{
    return "+,-";
}

bool SortCriteria::processField(const std::string direction, const std::string property, std::string& result)
{

    //std::cout << "process field: " << direction << " " << property << "*" << std::endl;

    if(0 == property.compare("dc:title")) {
        result = "o.TITLE";
    }
    else if(0 == property.compare("upnp:artist")) {
        result = "d.AV_ARTIST";
    }
    else if(0 == property.compare("upnp:genre")) {
        result = "d.AV_GENRE";
    }
    else if(0 == property.compare("upnp:album")) {
        result = "d.AV_ALBUM";
    }
    else if(0 == property.compare("upnp:originalTrackNumber")) {
        result = "coalesce(d.A_TRACK_NUMBER, 0)";
    }
    else {
        return false;
    }

    if(0 == direction.compare("+")) {
        result += " ASC";
    }
    else if(0 == direction.compare("-")) {
        result += " DESC";
    }
    else {
        return false;
    }

    return true;
}
