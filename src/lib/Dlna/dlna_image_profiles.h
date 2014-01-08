/***************************************************************************
 *            dlna_image_profiles.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2008-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

bool dlna_get_image_profile_jpeg(int width, int height, std::string& profile, std::string& mimeType)
{
  if(width <= 0 && height <= 0) {
    return false;
  }
  
	mimeType = "image/jpeg";

  if(width <= 48 && height <= 48) {
		profile = "JPEG_SM_ICO";
		return true;
  }
  else if(width <= 120 && height <= 120) {
		profile = "JPEG_LRG_ICO";
		return true;
  }
  else if(width <= 160 && height <= 160) {
		profile = "JPEG_TN";
		return true;
  }
  else if(width <= 640 && height <= 480) {
		profile = "JPEG_SM";
		return true;
  }
  else if(width <= 1024 && height <= 768) {
    profile = "JPEG_MED";
		return true;
  }
  else if(width <= 4096 && height <= 4096) {
    profile = "JPEG_LRG";
		return true;
  }

  return false;
}

bool dlna_get_image_profile_png(int width, int height, std::string& profile, std::string& mimeType)
{
  if(width <= 0 && height <= 0) {
    return false;
  }

  mimeType = "image/png";

  if(width <= 48 && height <= 48) {
		profile = "PNG_SM_ICO";
		return true;
  }
  else if(width <= 120 && height <= 120) {
		profile = "PNG_LRG_ICO";
		return true;
  }
  else if(width <= 160 && height <= 160) {
		profile = "PNG_TN";
		return true;
  }
  else if(width <= 4096 && height <= 4096) {
    profile = "PNG_LRG";
		return true;
  }

	return false;
}
