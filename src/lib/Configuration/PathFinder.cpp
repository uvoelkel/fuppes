/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            PathFinder.cpp
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

#include "PathFinder.h"
#include "../Common/Directory.h"

#include <iostream>
#include <string.h>
#include <stdlib.h>

using namespace std;
using namespace fuppes;

//PathFinder* PathFinder::m_instance = NULL;

/*
 void PathFinder::init(std::string applicationDir) // static
 {
 assert(m_instance == NULL);
 m_instance = new PathFinder(applicationDir);
 }


 void PathFinder::uninit() // static
 {
 assert(m_instance != NULL);
 delete m_instance;
 m_instance = NULL;
 }

 PathFinder* PathFinder::instance() // static
 {
 assert(m_instance != NULL);
 return m_instance;
 }
 */

void PathFinder::init(std::string applicationDir)
{

    m_applicationDir = applicationDir;

    /*
     * http://standards.freedesktop.org/basedir-spec/latest/ar01s03.html
     *
     * $XDG_DATA_HOME defines the base directory relative to which user specific data files should be stored. If $XDG_DATA_HOME is either not set or empty, a default equal to $HOME/.local/share should be used.
     * $XDG_CONFIG_HOME defines the base directory relative to which user specific configuration files should be stored. If $XDG_CONFIG_HOME is either not set or empty, a default equal to $HOME/.config should be used.
     *
     */

#ifdef WIN32
    string data = StringReplace(string(getenv("APPDATA")), "\\", "/") + "/fuppes/";
    m_dataPaths.push_back(data);

    string config = StringReplace(string(getenv("APPDATA")), "\\", "/") + "/fuppes/";
    m_configPaths.push_back(config);
#else

    string xdgData = PathFinder::getXdgDataHome();
    string xdgConfig = PathFinder::getXdgConfigHome();

    // old .fuppes dir
    string oldData = string(getenv("HOME")) + "/.fuppes/";
    string oldConfig = string(getenv("HOME")) + "/.fuppes/";

    // $(localstatedir)/lib/fuppes  (this directory is created by make install but it may not be writable)
    string lsData = Directory::appendTrailingSlash(FUPPES_LOCALSTATEDIR);
    string lsConfig = Directory::appendTrailingSlash(FUPPES_LOCALSTATEDIR);

    // check if $(localstatedir)/lib/fuppes is writable and create
    // xdg paths if it is not
    if (!Directory::writable(lsData)) {

        //log(Log::config, Log::normal | Log::warning)<< lsData << "not writable using" << xdgData << "instead";
        //log(Log::config, Log::normal | Log::warning) << lsConfig << "not writable using" << xdgConfig << "instead";

        if(!Directory::exists(xdgData)) {
            Directory::create(xdgData);
        }

        if(!Directory::exists(xdgConfig)) {
            Directory::create(xdgConfig);
        }
    }

    // if the xdg paths exist they have the highest priority
    if (Directory::exists(xdgData) && Directory::exists(xdgConfig)) {
        m_dataPaths.push_back(xdgData);
        m_configPaths.push_back(xdgConfig);
    }

    // check if the old .fuppes directory still exists, throw a warning
    // and add it to the config and data search paths
    if (Directory::exists(oldData)) {
        m_dataPaths.push_back(oldData);
        m_configPaths.push_back(oldConfig);

        //log(Log::config, Log::normal | Log::warning)<< "old ~/.fuppes config directory found.";
    }

    // finally add the $(localstatedir)/lib/fuppes paths
    m_dataPaths.push_back(lsData);
    m_configPaths.push_back(lsConfig);

#endif

    devicesPath = DEVICE_DIRECTORY;
    vfolderPath = VFOLDER_DIRECTORY;
}

/*
 string PathFinder::defaultPath(void)
 {
 assert(m_paths.size() > 0);
 return m_paths.front();
 }
 */

void PathFinder::addConfigPath(std::string path) // static
{
    if (!path.empty()) {
        m_configPaths.insert(m_configPaths.begin(), path);
    }
}

string PathFinder::findInDataPath(std::string fileName, int flags /*= File::Readable*/, std::string extra /*= ""*/)
{
    bool found = false;
    string tempName = "";
    vector<string>::const_iterator it;
    for (it = m_dataPaths.begin(); it != m_dataPaths.end(); ++it) {
        tempName = *it;
        tempName += extra;
        tempName += fileName;
        if (File::exists(tempName)) {

            found = true;

            if ((flags & File::Readable) && !File::readable(tempName))
                found = false;
            if ((flags & File::Writable) && !File::writable(tempName))
                found = false;
            if ((flags & File::Executable) && !File::executable(tempName))
                found = false;

            if (found)
                break;
        }
    }

    return (found ? tempName : "");
}

string PathFinder::findInConfigPath(std::string fileName, int flags /*= File::Readable*/, std::string extra /*= ""*/)
{
    bool found = false;
    string tempName = "";
    vector<string>::const_iterator it;
    for (it = m_configPaths.begin(); it != m_configPaths.end(); ++it) {
        tempName = *it;
        tempName += extra;
        tempName += fileName;
        if (File::exists(tempName)) {

            found = true;

            if ((flags & File::Readable) && !File::readable(tempName))
                found = false;
            if ((flags & File::Writable) && !File::writable(tempName))
                found = false;
            if ((flags & File::Executable) && !File::executable(tempName))
                found = false;

            if (found)
                break;
        }
    }

    return (found ? tempName : "");
}

std::string PathFinder::findDataPath(int flags)
{
    bool found = false;
    string tempName = "";
    vector<string>::const_iterator it;
    for (it = m_dataPaths.begin(); it != m_dataPaths.end(); ++it) {
        tempName = *it;

        if (Directory::exists(tempName)) {

            found = true;

            if ((flags & Directory::Readable) && !Directory::readable(tempName))
                found = false;
            if ((flags & Directory::Writable) && !Directory::writable(tempName))
                found = false;

            if (found)
                break;
        }
    }

    return (found ? tempName : "");
}

string PathFinder::findDeviceInPath(string device)
{
    return findInConfigPath(device + DEVICE_EXT, File::Readable, Directory::appendTrailingSlash(devicesPath));
}

string PathFinder::findVFolderInPath(string device)
{
    return findInConfigPath(device + VFOLDER_EXT, File::Readable, Directory::appendTrailingSlash(vfolderPath));
}

std::string PathFinder::findDataDir() // static
{
    if (!m_dataDir.empty())
        return m_dataDir;

    string tempName = "";
    vector<string>::const_iterator it;
    for (it = m_dataPaths.begin(); it != m_dataPaths.end(); ++it) {
        tempName = *it;

        if (!Directory::exists(tempName) && Directory::writable(*it) && Directory::create(tempName)) {
            m_dataDir = tempName;
            break;
        }

        if (Directory::exists(tempName) && Directory::writable(tempName)) {
            m_dataDir = tempName;
            break;
        }
    }

    return m_dataDir;
}

std::string PathFinder::findThumbnailsDir() // static
{
    if (!m_thumbnailsDir.empty())
        return m_thumbnailsDir;

    string tempName = "";
    vector<string>::const_iterator it;
    for (it = m_dataPaths.begin(); it != m_dataPaths.end(); ++it) {
        tempName = *it;
        tempName += "thumbnails/";

        if (!Directory::exists(tempName) && Directory::writable(*it) && Directory::create(tempName)) {
            m_thumbnailsDir = tempName;
            break;
        }

        if (Directory::exists(tempName) && Directory::writable(tempName)) {
            m_thumbnailsDir = tempName;
            break;
        }
    }

    return m_thumbnailsDir;
}

StringList PathFinder::GetDevicesList() // static
{
    StringList result;

    Directory dir;
    DirEntryList entries;
    DirEntryListIterator entriesIter;

    string tmp;
    string ext;

    vector<string>::const_iterator it;
    for (it = m_configPaths.begin(); it != m_configPaths.end(); ++it) {

        if (!dir.open(*it + DEVICE_DIRECTORY))
            continue;
        entries = dir.dirEntryList(DirEntry::File);
        dir.close();

        for (entriesIter = entries.begin(); entriesIter != entries.end(); entriesIter++) {

            tmp = entriesIter->name();
            if (tmp.length() < 5)
                continue;

            ext = tmp.substr(tmp.length() - 4, 4);
            tmp = tmp.substr(0, tmp.length() - 4);

            if (result.indexOf(tmp) >= 0)
                continue;
            result.push_back(tmp);
        }
    }

    return result;
}

StringList PathFinder::GetVfoldersList() // static
{
    StringList result;

    result.push_back("none");

    Directory dir;
    DirEntryList entries;
    DirEntryListIterator entriesIter;

    string tmp;
    string ext;

    vector<string>::const_iterator it;
    for (it = m_configPaths.begin(); it != m_configPaths.end(); ++it) {

        if (!dir.open(*it + VFOLDER_DIRECTORY))
            continue;
        entries = dir.dirEntryList(DirEntry::File);
        dir.close();

        for (entriesIter = entries.begin(); entriesIter != entries.end(); entriesIter++) {

            tmp = entriesIter->name();
            if (tmp.length() < 5)
                continue;

            ext = tmp.substr(tmp.length() - 4, 4);
            tmp = tmp.substr(0, tmp.length() - 4);

            if (result.indexOf(tmp) >= 0)
                continue;
            result.push_back(tmp);
        }
    }

    return result;
}

#ifndef WIN32
std::string PathFinder::getXdgConfigHome() // static
{
    char* env = getenv("XDG_CONFIG_HOME");

    // XDG_CONFIG_HOME :: if $XDG_CONFIG_HOME is either not set or empty, a default equal to $HOME/.config should be used.
    if (NULL == env || 0 == strlen(env)) {
        return string(getenv("HOME")) + "/.config/fuppes/";
    }
    else {
        return string(env) + "/fuppes/";
    }
}

std::string PathFinder::getXdgDataHome() // static
{
    char* env = getenv("XDG_DATA_HOME");

    // XDG_DATA_HOME :: if $XDG_DATA_HOME is either not set or empty, a default equal to $HOME/.local/share should be used.
    if (NULL == env || 0 == strlen(env)) {
        return string(getenv("HOME")) + "/.local/share/fuppes/";
    }
    else {
        return string(env) + "/fuppes/";
    }
}
#endif

/*
 void PathFinder::walker(bool (*step)(string)) {
 Directory::walk(&m_paths, step);
 }
 */
