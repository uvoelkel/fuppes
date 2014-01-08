/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            Directory.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2009 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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


#include "Directory.h"
#include "Common.h"
#include "RegEx.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include <string.h>
#else
#include <unistd.h>
#endif

#include <iostream>

using namespace std;
using namespace fuppes;

#define SLASH "/"
#define BACKSLASH "\\"
    
bool Directory::create(const std::string directory) // static
{
  if(!Directory::exists(directory)) {
    #ifdef WIN32
		CreateDirectory(directory.c_str(), NULL);
    #else
		int ret = mkdir(directory.c_str(), S_IRWXU | S_IRWXG);
		return (ret == 0); // you may not have had permissions required to create it
    #endif
  }
  
  return true; // the directory exists for sure now
}

bool Directory::remove(const std::string directory, const bool recursive) // static
{
  return true;
}

bool Directory::exists(std::string directory) // static
{
#ifdef WIN32
	directory = removeTrailingSlash(directory);
	
  struct _stat info;
  memset(&info, 0, sizeof(info));

  _stat(directory.c_str(), &info);
  return ((info.st_mode & _S_IFDIR) != 0);	
#else
	directory = appendTrailingSlash(directory);
	
	struct stat Stat;  
  return (stat(directory.c_str(), &Stat) == 0 && S_ISDIR(Stat.st_mode) != 0);
#endif
}

bool Directory::readable(std::string directory) // static
{
  bool isReadable = false;

#ifdef WIN32
	directory = removeTrailingSlash(directory);
#else
	directory = appendTrailingSlash(directory);
#endif

#ifdef WIN32
  struct _stat info;
  memset(&info, 0, sizeof(info));

  _stat(directory.c_str(), &info);
  isReadable = ((info.st_mode & _S_IFDIR) != 0);	
#else
	struct stat Stat;
  if(stat(directory.c_str(), &Stat) == 0 && S_ISDIR(Stat.st_mode)) {
    // check other then group
    if(Stat.st_mode & S_IROTH) {
      isReadable = true;  // I am an other
    }
    if(!isReadable && (Stat.st_mode & S_IRGRP)) {
      if (getgid() == Stat.st_gid) {
        isReadable = true;
      }
    }
    if(!isReadable && (Stat.st_mode & S_IRUSR)) {
      if (getuid() == Stat.st_uid) {
        isReadable = true;
      }
    }
  }
#endif

  return isReadable;
}

bool Directory::writable(std::string directory) // static
{
  bool isWritable = false;

#ifdef WIN32
	directory = removeTrailingSlash(directory);
#else
	directory = appendTrailingSlash(directory);
#endif

#ifdef WIN32
  struct _stat info;
  memset(&info, 0, sizeof(info));

  _stat(directory.c_str(), &info);
  isWritable = ((info.st_mode & _S_IFDIR) != 0);	
#else
	struct stat Stat;
  if(stat(directory.c_str(), &Stat) == 0 && S_ISDIR(Stat.st_mode)) {
    // check other then group
    if(Stat.st_mode & S_IWOTH) {
      isWritable = true;  // I am an other
    }
    if(!isWritable && (Stat.st_mode & S_IWGRP)) {
      if (getgid() == Stat.st_gid) {
        isWritable = true;
      }
    }
    if(!isWritable && (Stat.st_mode & S_IWUSR)) {
      if (getuid() == Stat.st_uid) {
        isWritable = true;
      }
    }
  }
#endif

  return isWritable;
}

bool Directory::searchable(std::string directory) // static
{
  bool isExecutable = false;

#ifdef WIN32
	directory = removeTrailingSlash(directory);
#else
	directory = appendTrailingSlash(directory);
#endif

#ifdef WIN32
  struct _stat info;
  memset(&info, 0, sizeof(info));

  _stat(directory.c_str(), &info);
  isExecutable = ((info.st_mode & _S_IFDIR) != 0);	
#else
	struct stat Stat;
  if(stat(directory.c_str(), &Stat) == 0 && S_ISDIR(Stat.st_mode)) {
    // check other then group
    if(Stat.st_mode & S_IXOTH) {
      isExecutable = true;  // I am an other
    }
    if(!isExecutable && (Stat.st_mode & S_IXGRP)) {
      if (getgid() == Stat.st_gid) {
        isExecutable = true;
      }
    }
    if(!isExecutable && (Stat.st_mode & S_IXUSR)) {
      if (getuid() == Stat.st_uid) {
        isExecutable = true;
      }
    }
  }
#endif

  return isExecutable;
}

bool Directory::hidden(std::string directory) // static
{
	string folder;
	ExtractFolderFromPath(directory, &folder);
#ifndef WIN32
	if(folder.length() > 0) {
		return (folder[0] == '.');
	}
#else
	return false;
#endif
	return false;
}


std::string Directory::appendTrailingSlash(std::string directory) // static
{
	if(directory.length() <= 1)
		return directory;

	if(directory.substr(directory.length() - 1).compare(SLASH) != 0 &&
     directory.substr(directory.length() - 1).compare(BACKSLASH) != 0) {
	  directory += SLASH;
	}

	return directory;
}

std::string Directory::removeTrailingSlash(std::string directory) // static
{
	if(directory.length() <= 2)
		return directory;

	if(directory.substr(directory.length() - 1).compare(SLASH) == 0 ||
     directory.substr(directory.length() - 1).compare(BACKSLASH) == 0) {
		return directory.substr(0, directory.length() - 1);
	}

	return directory;
}


Directory::Directory(std::string path)
{
	setPath(path);
}

void Directory::setPath(std::string path)
{
	if(path.length() == 0) {
		m_path = "";
		return;
	}

	m_path = "";
	m_dir = NULL;
	path = Directory::appendTrailingSlash(path);

#ifndef WIN32
	string pattern = "/([\\s|\\w|\\d|\\.]+/\\.\\./)";
#else
#warning todo test
	string pattern = "\\([\\s|\\w|\\d|\\.]+\\\\.\\.\\)";
#endif
		
	RegEx rxClear(pattern, PCRE_CASELESS);		
	while(rxClear.search(path)) {


		size_t pos = path.find(rxClear.match(1));		
		m_path = path.substr(0, pos);
		path = path.substr(pos + rxClear.match(1).length(), path.length());
	}

	m_path += path;
}

bool Directory::open(std::string path /*= ""*/)
{
  if(path.length() > 0) {
    setPath(path);
  }
  
  m_dir = opendir(m_path.c_str());
  if(m_dir == NULL) {
    return false;
  }

#if WIN32
  // http://msdn.microsoft.com/en-us/library/aa363858(v=VS.85).aspx
  // m_handle = CreateFile();
#endif
  
  return true;
}

void Directory::close()
{
	if(m_dir == NULL)
		return;

	closedir(m_dir);
	m_dir = NULL;
}

DirEntryList Directory::dirEntryList(int filter /* = DirEntry::All*/)
{
	DirEntryList result;
	DirEntry entry;

#ifdef WIN32
	struct _stat info;
#else
	struct stat info;
#endif
	
	dirent* dirEntry;
	while((dirEntry = readdir(m_dir)) != NULL) {
		
		if((string(".").compare(dirEntry->d_name) == 0) ||
		   (string("..").compare(dirEntry->d_name) == 0)) {
			continue;
		}

		// set type of entry

#ifdef WIN32  
	  memset(&info, 0, sizeof(info));
	  string name = m_path + dirEntry->d_name;
		int ret = _stat(name.c_str(), &info);
		if(ret == -1)
		  cout << strerror(errno) << endl;
		  		
		if((info.st_mode & _S_IFDIR) != 0) {
			entry.m_type = DirEntry::Directory;
		}
		else {
			entry.m_type = DirEntry::File;
		}
#else

		string path = m_path + dirEntry->d_name;		
		if(stat(path.c_str(), &info) == -1) {
			continue;
		}
		
		if(S_ISDIR(info.st_mode) != 0) {
			entry.m_type = DirEntry::Directory;
		}
		else {
			entry.m_type = DirEntry::File;
		}
#endif	

		// filter
		if((entry.m_type & filter) == 0)
			continue;
		
		// set path and name
		if(entry.m_type == DirEntry::Directory) {
			entry.m_path = Directory::appendTrailingSlash(m_path + dirEntry->d_name);
			entry.m_name = dirEntry->d_name;
		}
		else {
			entry.m_path = m_path;
			entry.m_name = dirEntry->d_name;
		}

		result.push_back(entry);
	} // while readdir
				
	
	return result;
}




std::list<std::string> Directory::getDriveLetters() // static
{
  std::list<std::string> result;

  #ifdef WIN32
	DWORD drives = GetLogicalDrives();
		
	DWORD bit = 0x01;
	char  letter = 'A';
  string tmp;
	while(bit != 0x8000000) {
  	if(drives & bit) {
      tmp = letter;
  		result.push_back(tmp);
		}
		bit = bit << 1;
		letter++;
	}
  #endif

	return result;
}
