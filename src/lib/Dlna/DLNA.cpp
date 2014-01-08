/*
 * This file is part of fuppes
 *
 * (c) 2010-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */


#include "DLNA.h"

#include "dlna_image_profiles.h"
#include "dlna_audio_profiles.h"
#include "dlna_video_profiles.h"

#include <stdio.h>

using namespace Dlna;

void DLNA::buildProfile(Dlna::Item& item) // static
{
    std::string ext = item.getExtension();
    std::string dlnaProfile;
    std::string mimeType;
    bool ret = false;
    
    if(item.getType() >= ITEM_IMAGE_ITEM && item.getType() < ITEM_IMAGE_ITEM_MAX) {

        int width = item.getWidth();
        int height = item.getHeight();

        ret = DLNA::getImageProfile(ext, width, height, dlnaProfile, mimeType);
    }
    else if(item.getType() >= ITEM_VIDEO_ITEM && item.getType() < ITEM_VIDEO_ITEM_MAX) {

        std::string acodec = item.getAudioCodec();
        std::string vcodec = item.getVideoCodec();

        ret = DLNA::getVideoProfile(ext, vcodec, acodec, dlnaProfile, mimeType);
    }
    else if(item.getType() >= ITEM_AUDIO_ITEM && item.getType() < ITEM_AUDIO_ITEM_MAX) {

        int channels = item.getAudioChannels();
        int bitrate = item.getAudioBitrate();
        ret = DLNA::getAudioProfile(ext, channels, bitrate, dlnaProfile, mimeType);
    }

    if(false == ret) {
        return;
    }

    item.profile = dlnaProfile;
    item.setMimeType(mimeType);

    //item->set("dlna_profile", dlnaProfile);
    //item->setMimeType(mimeType);
}



/* enums stolen from libdlna :: copyright (C) 2007-2008 Benjamin Zores */


/*
# Play speed
#    1 normal
#    0 invalid
DLNA_ORG_PS = 'DLNA.ORG_PS'
DLNA_ORG_PS_VAL = '1'
*/

enum dlna_org_playSpeed {
  ps_invalid  = 0,
  ps_normal   = 1
};

/*
# Conversion Indicator
#    1 transcoded
#    0 not transcoded
DLNA_ORG_CI = 'DLNA.ORG_CI'
DLNA_ORG_CI_VAL = '0'
*/

enum dlna_org_conversionIndicator {
  ci_none = 0,
  ci_transcoded = 1
};


/*
# Operations
#    00 not time seek range, not range
#    01 range supported
#    10 time seek range supported
#    11 both supported
DLNA_ORG_OP = 'DLNA.ORG_OP'
DLNA_ORG_OP_VAL = '01'
*/

enum dlna_org_operations {
  op_none   = 0x00,
  op_range  = 0x01,
  op_time   = 0x10,
  op_both   = 0x11
};


/*
# Flags
#    senderPaced                      80000000  31
#    lsopTimeBasedSeekSupported       40000000  30
#    lsopByteBasedSeekSupported       20000000  29
#    playcontainerSupported           10000000  28
#    s0IncreasingSupported            08000000  27
#    sNIncreasingSupported            04000000  26
#    rtspPauseSupported               02000000  25
#    streamingTransferModeSupported   01000000  24
#    interactiveTransferModeSupported 00800000  23
#    backgroundTransferModeSupported  00400000  22
#    connectionStallingSupported      00200000  21
#    dlnaVersion15Supported           00100000  20
DLNA_ORG_FLAGS = 'DLNA.ORG_FLAGS'
DLNA_ORG_FLAGS_VAL = '01500000000000000000000000000000'
*/

enum dlna_org_flags {
  flag_senderPaced                      = (1 << 31),
  flag_lsopTimeBasedSeekSupported       = (1 << 30),
  flag_lsopByteBasedSeekSupported       = (1 << 29),
  flag_playcontainerSupported           = (1 << 28),
  flag_s0IncreasingSupported            = (1 << 27),
  flag_sNIncreasingSupported            = (1 << 26),
  flag_rtspPauseSupported               = (1 << 25),
  flag_streamingTransferModeSupported   = (1 << 24),
  flag_interactiveTransferModeSupported = (1 << 23),
  flag_backgroundTransferModeSupported  = (1 << 22),
  flag_connectionStallingSupported      = (1 << 21),
  flag_dlnaVersion15Supported           = (1 << 20)
};



// dlna Volume 1, 7.3.32.1

void DLNA::build4thField(Dlna::Item& item) // static
{
    // play speed
    dlna_org_playSpeed ps = ps_normal;

    // conversion indicator
    dlna_org_conversionIndicator ci = ci_none;
    if(item.isTranscoded())
        ci = ci_transcoded;

    // operations
    dlna_org_operations op = op_range;
    if(item.isTranscoded())
        op = op_none;

    // flags
    int flags = 0;
    flags = 
        flag_streamingTransferModeSupported |
        flag_backgroundTransferModeSupported |
        flag_connectionStallingSupported |  
        flag_dlnaVersion15Supported;

    if(!item.isTranscoded())
        flags |= flag_lsopByteBasedSeekSupported;
    

	char info[448];
	if(!item.profile.empty()) {
		sprintf(info, "%s=%s;%s=%.2x;%s=%d;%s=%d;%s=%.8x%.24x",
				  "DLNA.ORG_PN", item.profile.c_str(), "DLNA.ORG_OP", op,
          "DLNA.ORG_PS", ps, "DLNA.ORG_CI", ci, 
          "DLNA.ORG_FLAGS", flags, 0);
	}
	else {
		sprintf(info, "%s=%.2x;%s=%d;%s=%d;%s=%.8x%.24x",
				  "DLNA.ORG_OP", op, "DLNA.ORG_PS", ps,
          "DLNA.ORG_CI", ci, "DLNA.ORG_FLAGS", flags, 0);
	}


	item.dlna4thfield = info;
    //item->setDlna4thField(info);
}


bool DLNA::getImageProfile(std::string ext, int width, int height, std::string& dlnaProfile, std::string& mimeType) // static
{
	if(width == 0 || height == 0) {
		return false;
	}

    bool result = false;
	if((ext.compare("jpeg") == 0) || (ext.compare("jpg") == 0)) {
		result = dlna_get_image_profile_jpeg(width, height, dlnaProfile, mimeType);
	}	
	else if(ext.compare("png") == 0) {
		result = dlna_get_image_profile_png(width, height, dlnaProfile, mimeType);
	}
  
    return result;
}

fuppes::StringList DLNA::getImageProfiles(std::string ext, std::string& mimeType) // static
{
    fuppes::StringList result;

	if((ext.compare("jpeg") == 0) || (ext.compare("jpg") == 0)) {

        mimeType = "image/jpeg";
    
		result.push_back("JPEG_SM_ICO");
		result.push_back("JPEG_LRG_ICO");
		result.push_back("JPEG_TN");
		result.push_back("JPEG_SM");
		result.push_back("JPEG_MED");
		result.push_back("JPEG_LRG");
	}	
	else if(ext.compare("png") == 0) {

        mimeType = "image/png";
        
        result.push_back("PNG_SM_ICO");
        result.push_back("PNG_LRG_ICO");
        result.push_back("PNG_TN");
        result.push_back("PNG_LRG");
	}

    return result;
}


bool DLNA::getAudioProfile(std::string ext, int channels, int bitrate, std::string& dlnaProfile, std::string& mimeType) // static
{
    bitrate /= 1024; // kbits

    bool result = false;
  
	if(ext.compare("mp3") == 0) {
		result = dlna_get_audio_profile_mp3(channels, bitrate, dlnaProfile, mimeType);
	}	
	else if(ext.compare("wma") == 0) {
		result = dlna_get_audio_profile_wma(channels, bitrate, dlnaProfile, mimeType);
	}
	else if(ext.compare("m4a") == 0) {
        result = dlna_get_audio_profile_mpeg4(channels, bitrate, dlnaProfile, mimeType);
	}
	else if(ext.compare("ac3") == 0) {
		result = dlna_get_audio_profile_ac3(channels, bitrate, dlnaProfile, mimeType);
	}
  
    return result;
}

fuppes::StringList DLNA::getAudioProfiles(std::string ext, std::string& mimeType) // static
{
    fuppes::StringList result;

	if(ext.compare("mp3") == 0) {
        mimeType = "audio/mpeg";
		result.push_back("MP3");
	}
	else if(ext.compare("wma") == 0) {
        mimeType = "audio/x-ms-wma";
		result.push_back("WMABASE");
		result.push_back("WMAFULL");
		result.push_back("WMAPRO");    
	}
	else if(ext.compare("m4a") == 0) {
	}
	else if(ext.compare("ac3") == 0) {
	}  
  
    return result;
}

bool DLNA::getVideoProfile(std::string ext, std::string vcodec, std::string acodec, std::string& dlnaProfile, std::string& mimeType) // static
{
    bool result = false;

	if((ext.compare("mpg") == 0) || (ext.compare("mpeg") == 0)) {

    if(vcodec.compare("mpeg1video") == 0) {    
        result = dlna_get_video_profile_mpeg1(vcodec, acodec, dlnaProfile, mimeType);
    }

    // mpeg2video
    // mpeg4
    // msmpeg4
    // msmpeg4v1
    // msmpeg4v2
    // h264
	}	
	else if(ext.compare("avi") == 0) {		
	}
	else if(ext.compare("wmv") == 0) {		
    // wmv1, wmv2, wmv3
	}
	else if(ext.compare("mp4") == 0) {
	}
	else if(ext.compare("mkv") == 0) {
	}

    return result;
}

fuppes::StringList DLNA::getVideoProfiles(std::string ext, std::string& mimeType) // static
{
    fuppes::StringList result;

	if((ext.compare("mpg") == 0) || (ext.compare("mpeg") == 0)) {
        mimeType = "video/mpeg";
		result.push_back("MPEG1");
	}	
	else if(ext.compare("avi") == 0) {
	}
	else if(ext.compare("wmv") == 0) {
	}
	else if(ext.compare("mp4") == 0) {
	}
	else if(ext.compare("mkv") == 0) {
	}
  
    return result;
}




void DLNA::setHttpHeaderValues(Dlna::Item& item, Fuppes::HttpRequest* request, Fuppes::HttpResponse* response) // static
{
    if(CMediaServerSettings::dlna_none == request->getDeviceSettings()->MediaServerSettings()->DlnaVersion) {
        return;
    }

    // dlna volume 1, 7.4.26
    if(request->getHeader()->keyExists("getcontentFeatures.dlna.org")) {

        // value has to be "1" otherwise return a "400 Bad Request"
        if(0 != request->getHeader()->getValue("getcontentFeatures.dlna.org").compare("1")) {
            delete response;
            response = new Fuppes::HttpResponse(Http::BAD_REQUEST);
        }

        response->getHeader()->setValue("contentFeatures.dlna.org", item.dlna4thfield);
    }
    
    
    if(request->getHeader()->keyExists("transferMode.dlna.org")) {
        response->getHeader()->setValue("transferMode.dlna.org", request->getHeader()->getValue("transferMode.dlna.org"));
    }

}
