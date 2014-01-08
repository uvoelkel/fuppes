/***************************************************************************
 *            dlna_audio_profiles.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


bool dlna_get_audio_profile_ac3(int channels, int bitrate, std::string& profile, std::string& mimeType)
{
  mimeType = "audio/vnd.dolby.dd-raw";
  profile = "AC3";
  return true;
}

bool dlna_get_audio_profile_atrac3plus(int channels, int bitrate, std::string& profile, std::string& mimeType)
{
  mimeType = "audio/x-sony-oma";
  profile = "ATRAC3plus";
  return true;
}

bool dlna_get_audio_profile_lpcm(int channels, int bitrate, std::string& profile, std::string& mimeType)
{
  mimeType = "audio/L16";
  profile = "LPCM";
  return true;
}

bool dlna_get_audio_profile_mp3(int channels, int bitrate, std::string& profile, std::string& mimeType)
{
  mimeType = "audio/mpeg";
  profile = "MP3";
  return true;
}

bool dlna_get_audio_profile_mpeg4(int channels, int bitrate, std::string& profile, std::string& mimeType)
{
  return false;
}

bool dlna_get_audio_profile_wma(int channels, int bitrate, std::string& profile, std::string& mimeType)
{
  mimeType = "audio/x-ms-wma";
  
  if(channels == 2 && bitrate < (193 * 1024)) {
    profile = "WMABASE";
    return true;
  }
  else if(channels == 2) {
    profile = "WMAFULL";
    return true;
  }
  else if(channels > 2) {
    profile = "WMAPRO";
    return true;
  }
  
  return false;
}

