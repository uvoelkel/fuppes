/*
 * This file is part of fuppes
 *
 * (c) 2010-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _DLNA_DLNA_H
#define _DLNA_DLNA_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include "Item.h"

#include "../Fuppes/HttpRequest.h"
#include "../Fuppes/HttpResponse.h"

namespace Dlna
{
    class DLNA
    {
        public:

            static void buildProfile(Dlna::Item& item);
            static void build4thField(Dlna::Item& item);
            
          	static bool getImageProfile(std::string ext, int width, int height, std::string& dlnaProfile, std::string& mimeType);
            static bool getAudioProfile(std::string ext, int channels, int bitrate, std::string& dlnaProfile, std::string& mimeType);
            static bool getVideoProfile(std::string ext, std::string vcodec, std::string acodec, std::string& dlnaProfile, std::string& mimeType);

            static fuppes::StringList getImageProfiles(std::string ext, std::string& mimeType);
            static fuppes::StringList getAudioProfiles(std::string ext, std::string& mimeType);
            static fuppes::StringList getVideoProfiles(std::string ext, std::string& mimeType);


            //static void create4thField(UPnP::UPnPObject* item, Http::HttpRequest* request, Http::HttpResponse* response);
            static void setHttpHeaderValues(Dlna::Item& item, Fuppes::HttpRequest* request, Fuppes::HttpResponse* response);
    };

}

#endif // _DLNA_DLNA_H
