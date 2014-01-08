/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            PathFinder.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2010-2011 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *  Copyright (C) 2010 Robert Massaioli <robertmassaioli@gmail.com>
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

#ifndef __PATHFINDER_H
#define __PATHFINDER_H


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Common/Common.h"

#include <string>
#include <vector>

#define DEVICE_EXT ".cfg"
#define VFOLDER_EXT ".cfg"

#define DEVICE_DIRECTORY string("devices/")
#define VFOLDER_DIRECTORY string("vfolders/")


class PathFinder
{
  public:
        //PathFinder(std::string applicationDir);
    void init(std::string applicationDir);
    /*static void uninit();
    static PathFinder* instance();*/

    std::string findInDataPath(std::string fileName, int flags = fuppes::File::Readable, std::string extra = "");
    std::string findInConfigPath(std::string fileName, int flags = fuppes::File::Readable, std::string extra = "");

    std::string findDataPath(int flags);
    
    std::string findDeviceInPath(std::string device);
    std::string findVFolderInPath(std::string device);

    void addConfigPath(std::string path);

    std::string findDataDir();
    std::string findThumbnailsDir();
    
    fuppes::StringList GetDevicesList();
    fuppes::StringList GetVfoldersList();
    
#ifndef WIN32
    static std::string getXdgConfigHome();
    static std::string getXdgDataHome();
#endif

  private:
    //static PathFinder* m_instance;

    std::string m_applicationDir;
    
    std::string devicesPath, vfolderPath; // the extra paths for device files and vfolder files
    
    std::vector<std::string> m_dataPaths;
    std::vector<std::string> m_configPaths;

    std::string         m_dataDir;
    std::string         m_thumbnailsDir;
};

#endif
