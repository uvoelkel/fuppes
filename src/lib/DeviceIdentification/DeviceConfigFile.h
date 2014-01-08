/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            ConfigFile.h
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2007-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#ifndef _CONFIGFILE_H
#define _CONFIGFILE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//#include "../Configuration/PathFinder.h"

#include <vector>
#include <string>

#define SETUP_DEVICE_SUCCESS      0
#define SETUP_DEVICE_NOT_FOUND    1
#define SETUP_DEVICE_LOAD_FAILED  2

typedef enum CONFIG_FILE_ERROR
{
    CF_OK, CF_FILE_NOT_FOUND, CF_PARSE_ERROR, CF_CONFIG_DEPRECATED
} CONFIG_FILE_ERROR;

class CXMLNode;
class CDeviceSettings;
struct CFileSettings;

class CDeviceConfigFile
{
    public:
        CDeviceConfigFile();
        ~CDeviceConfigFile();

        int setupDevice(const std::string filename, CDeviceSettings* pSettings, const std::string deviceName);

    private:
        //CXMLDocument* m_pDeviceDoc;

        void ParseDeviceSettings(CXMLNode* pDevice, CDeviceSettings* pSettings);
        void ParseFileSettings(CXMLNode* pFileSettings, CDeviceSettings* pDevSet);
        void ParseTranscodingSettings(CXMLNode* pTCNode, CFileSettings* pFileSet);
        void ParseImageSettings(CXMLNode* pISNode, CFileSettings* pFileSet);
        void ParseDescriptionValues(CXMLNode* pDescrValues, CDeviceSettings* pDevSet);
};

#endif // _CONFIGFILE_H
