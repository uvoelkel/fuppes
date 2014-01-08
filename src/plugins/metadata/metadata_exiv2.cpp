/***************************************************************************
 *            metadata_exiv2.cpp
 *
 *  Exif metadata extraction for Fuppes using Exiv2 library.
 *
 *  Copyright (C) 2008 Ben Rees
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

#include "../../include/fuppes_plugin.h"

#include <exiv2/image.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/iptc.hpp>
#include <iostream>
#include <iomanip>

using namespace Exiv2;
using namespace std;

static const ExifKey ExifDateTimeCreated("Exif.Photo.DateTimeOriginal");
static const ExifKey ExifDateTimeDigitized("Exif.Photo.DateTimeDigitized");

static const ExifKey ExifHeight("Exif.Photo.PixelYDimension");
static const ExifKey ExifWidth("Exif.Photo.PixelXDimension");
	
int exiv2_metadata_file_open(plugin_info* plugin, const char* fileName)
{
	try
	{
		Image::AutoPtr image = Exiv2::ImageFactory::open(fileName);
		
		if(!image->good())
		{
//		cerr << "Image could not be read by Exiv2: " << fileName << endl;
			return -1;
		}
		
		image->readMetadata();
		
		if(image->exifData().empty()) // && image->iptcData().empty())
		{
//		cout << "Image had no EXIF/IPTC metadata: " << fileName << endl;
			return -1;
		}
		
	    plugin->user_data = new Image::AutoPtr(image);	
	}
	catch(...)
	{
		cerr << "Exception in Exiv2 Metadata plugin" << endl;
		return -1;
	}
	
	return 0;
}

void exiv2_dump_tags(const ExifData& exif)
{
	cout <<  "exiv2_dump_tags: " << endl;
	
	Exiv2::ExifData::const_iterator end = exif.end();
	
    for (Exiv2::ExifData::const_iterator i = exif.begin(); i != end; ++i)
    {
        std::cout << std::setw(44) << std::setfill(' ') << std::left
                  << i->key() << " "
                  << "0x" << std::setw(4) << std::setfill('0') << std::right
                  << std::hex << i->tag() << " "
                  << std::setw(9) << std::setfill(' ') << std::left
                  << i->typeName() << " "
                  << std::dec << std::setw(3)
                  << std::setfill(' ') << std::right
                  << i->count() << "  "
                  << std::dec << i->value()
                  << "\n";
    }
}

int set_date(const ExifData& exif, char date[])
{
	const ExifData::const_iterator end = exif.end();
	ExifData::const_iterator value = exif.findKey(ExifDateTimeCreated);
	
    if(value == end)
		value = exif.findKey(ExifDateTimeDigitized);
       
	if(value == end)
	    return 1;
	    
	const std::string dateTime = value->value().toString();
	
	if(!dateTime.empty())
	{
		//*date = (char*)realloc(*date, (dateTime.length() + 1) * sizeof(char));
		strcpy(date, const_cast<char*>(dateTime.c_str()));  
	
	    // format gets returned as 'YYYY:MM:DD HH:MM:SS' when it
	    // needs to be 'YYYY-MM-DDTHH:MM:SS' so fix separators
	    char* dateStr = date;
	    
	    if(dateTime.length() > 4 && dateStr[4] != '-')
		    dateStr[4] = '-';
		    
	    if(dateTime.length() > 6 && dateStr[7] != '-')
            dateStr[7] = '-';
            
	    if(dateTime.length() > 10 && dateStr[10] != 'T')
            dateStr[10] = 'T';
	}
		
	return 0;
}

int set_long_value(const ExifData& exif, const ExifKey& key, unsigned int& field)
{
	const ExifData::const_iterator end = exif.end();
	const ExifData::const_iterator value = exif.findKey(key);
	
    if(value == end)
        return -1;
        
	field = value->value().toLong();
	return 0;
}

int set_height(const ExifData& exif, unsigned int& height)
{
	return set_long_value(exif, ExifHeight, height);
}

int set_width(const ExifData& exif, unsigned int& width)
{
	return set_long_value(exif, ExifWidth, width);
}

/* Read the EXIF metadata into the struct
 * A non-zero return code indicates some data could not be read,
 * which may require further action.
 */
int exiv2ReadExif(plugin_info* plugin, metadata_t* metadata)
{
    int errors = 0;
    
    try
    {	
	    Image::AutoPtr image = *(Image::AutoPtr*)(plugin->user_data);
	    
		const ExifData& exif = image->exifData();
		
	    const ExifData::const_iterator noValue = exif.end();
	    ExifData::const_iterator value;
	
		errors += set_width(exif, metadata->width);
		errors += set_height(exif, metadata->height); 
		errors += set_date(exif, metadata->date); 
    }
    catch(Exiv2::Error& err)
    {
    	cerr << "Exiv2::Error: " << err.what() << endl;
    	return 1;
    }
    catch(...)
    {
    	cerr << "Exception in Exiv2 metadata plugin" << endl;
    	return 1;
    } 
    
	return errors;  // Non-zero indicates at least one field could not be read
}

void exiv2_metadata_file_close(plugin_info* plugin)
{
	delete (Image::AutoPtr*)plugin->user_data;
}


#ifdef __cplusplus
extern "C" {
#endif

void register_fuppes_plugin(plugin_info* info)
{
	strcpy(info->plugin_name, "exiv2");
	strcpy(info->plugin_author, "Ben Rees");
	info->plugin_type = PT_METADATA;
}

int fuppes_metadata_file_open(plugin_info* plugin, const char* fileName)
{
	if(exiv2_metadata_file_open(plugin, fileName) != 0)
		return -1;
		
	return 0;
}

int fuppes_metadata_read(plugin_info* plugin, metadata_t* metadata)
{
	metadata->type = MD_IMAGE;
	
    return exiv2ReadExif(plugin, metadata);
    
#warning todo IPTC metadata  	
/* Should check to see all fields are complete - if not
 * could try looking them up in the IPTC data instead?
 */
}

void fuppes_metadata_file_close(plugin_info* plugin)
{
	exiv2_metadata_file_close(plugin);
}

void unregister_fuppes_plugin(plugin_info* plugin __attribute__((unused)))
{
}

#ifdef __cplusplus
}
#endif
