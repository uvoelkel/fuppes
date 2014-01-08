/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            File.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2009-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#include "File.h"
#include "Common.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>
#include <stdio.h>

#include <iostream>

#define SLASH "/"
#define BACKSLASH "\\"

using namespace fuppes;
using namespace std;

File::File(std::string fileName)
{
    m_openMode = Closed;
    m_file = NULL;
    setFileName(fileName);
}

File::~File()
{
    if (isOpen())
        close();
}

void File::setFileName(std::string fileName)
{
#ifdef WIN32
    fileName = StringReplace(fileName, BACKSLASH, SLASH);
#endif
    m_fileName = fileName;
}

bool File::open(int mode) // File::OpenMode
{
    std::string openmode;

    if (mode & Read)
        openmode += "r";
    if (mode & Write)
        openmode += "w";
    /*if(mode & Truncate)
     openmode |= "w";
     if(mode & Append)
     openmode += "a";*/
    if (!(mode & Text))
        openmode += "b";

    /*
     "r"	Open a file for reading. The file must exist.
     "w"	Create an empty file for writing. If a file with the same name already exists its content is erased and the file is treated as a new empty file.
     "a"	Append to a file. Writing operations append data at the end of the file. The file is created if it does not exist.
     "r+"	Open a file for update both reading and writing. The file must exist.
     "w+"	Create an empty file for both reading and writing. If a file with the same name already exists its content is erased and the file is treated as a new empty file.
     "a+"	Open a file for reading and appending. All writing operations are performed at the end of the file, protecting the previous content to be overwritten. You can reposition (fseek, rewind) the internal pointer to anywhere in the file for reading, but writing operations will move it back to the end of file. The file is created if it does not exist.
     */

    m_file = fopen(m_fileName.c_str(), openmode.c_str());
    return isOpen();
}

bool File::isOpen()
{
    return (m_file != NULL);
}

void File::close()
{
    fclose(m_file);
    m_file = NULL;
}

std::string File::path()
{
    size_t pos = m_fileName.find_last_of(SLASH);
    if (pos == string::npos)
        return m_fileName;

    return m_fileName.substr(0, pos + 1);
}

std::string File::ext()
{
    size_t pos = m_fileName.find_last_of(".");
    if (pos == string::npos)
        return "";

    return ToLower(m_fileName.substr(pos + 1));
}

bool File::seek(fuppes_off_t offset)
{
    if (!isOpen())
        return false;

#ifndef WIN32
    return (fseeko(m_file, offset, SEEK_SET) == 0);
#else
    return (fseeko64(m_file, offset, SEEK_SET) == 0);
    //return (_fseeki64(m_file, offset, SEEK_SET) == 0);
#endif
}

fuppes_off_t File::read(char* buffer, fuppes_off_t length)
{
    if (!isOpen())
        return false;

    return fread(buffer, 1, length, m_file);
}

fuppes_off_t File::write(const char* buffer, fuppes_off_t length)
{
    if (!isOpen())
        return false;

    return fwrite(buffer, 1, length, m_file);
}

fuppes_off_t File::size()
{
    struct stat Stat;
    if (stat(m_fileName.c_str(), &Stat) != 0)
        return 0;

    return Stat.st_size;
}

bool File::getline(std::string& line)
{
    if (!isOpen())
        return false;

    fuppes_off_t start;
#ifndef WIN32
    start = ftello(m_file);
#else
    start = ftello64(m_file);
#endif
    int c;
    do {
        c = fgetc(m_file);
        if (c == 10 || c == 13) {
            break;
        }
    } while (c != EOF);

    fuppes_off_t end;
#ifndef WIN32
    end = ftello(m_file);
#else
    end = ftello64(m_file);
#endif

    if (end > start) {
        seek(start);

        char* buffer = new char[end - start + 1];
        read(buffer, end - start);
        buffer[end - start] = '\0';
        line = buffer;
        delete[] buffer;
        return true;
    }

    return false;
}

bool File::remove(const std::string fileName) // static
{
#ifndef WIN32
	return (0 == ::remove(fileName.c_str()));
#else
	return false;
#endif
}

bool File::move(const std::string source, const std::string target) // static
{
#ifndef WIN32
	return (0 == rename(source.c_str(), target.c_str()));
#else
	return false;
#endif
}

bool File::exists(std::string fileName) // static
{
    struct stat Stat;
    return (stat(fileName.c_str(), &Stat) == 0 && S_ISREG(Stat.st_mode) != 0);
}

bool File::readable(std::string fileName) // static
{
    struct stat Stat;
    bool isReadable = false;

    if (stat(fileName.c_str(), &Stat) == 0 && S_ISREG(Stat.st_mode)) {
#ifndef WIN32
        // check other then group
        if (Stat.st_mode & S_IROTH) {
            isReadable = true;  // I am an other
        }
        if (!isReadable && (Stat.st_mode & S_IRGRP)) {
            if (getgid() == Stat.st_gid) {
                isReadable = true;
            }
        }
        if (!isReadable && (Stat.st_mode & S_IRUSR)) {
            if (getuid() == Stat.st_uid) {
                isReadable = true;
            }
        }
#else
        // todo
        isReadable = true;
#endif
    }

    return isReadable;
}

bool File::writable(std::string fileName) // static
{
    struct stat Stat;
    bool isWritable = false;

    if (stat(fileName.c_str(), &Stat) == 0 && S_ISREG(Stat.st_mode)) {
#ifndef WIN32
        // check other then group
        if (Stat.st_mode & S_IWOTH) {
            isWritable = true;  // I am an other
        }
        if (!isWritable && (Stat.st_mode & S_IWGRP)) {
            if (getgid() == Stat.st_gid) {
                isWritable = true;
            }
        }
        if (!isWritable && (Stat.st_mode & S_IWUSR)) {
            if (getuid() == Stat.st_uid) {
                isWritable = true;
            }
        }
#else
        // todo
        isWritable = true;
#endif
    }

    return isWritable;
}

bool File::executable(std::string fileName) // static
{
    struct stat Stat;
    bool isExecutable = false;

    if (stat(fileName.c_str(), &Stat) == 0 && S_ISREG(Stat.st_mode)) {
#ifndef WIN32
        // check other then group
        if (Stat.st_mode & S_IXOTH) {
            isExecutable = true;  // I am an other
        }
        if (!isExecutable && (Stat.st_mode & S_IXGRP)) {
            if (getgid() == Stat.st_gid) {
                isExecutable = true;
            }
        }
        if (!isExecutable && (Stat.st_mode & S_IXUSR)) {
            if (getuid() == Stat.st_uid) {
                isExecutable = true;
            }
        }
#else
        // todo
        isExecutable = true;
#endif
    }

    return isExecutable;
}

std::string File::extension(const std::string filename) // static
{
    size_t pos = filename.find_last_of(".");
    if (string::npos == pos) {
        return "";
    }

    return ToLower(filename.substr(pos + 1));
}

time_t File::lastModified(const std::string filename) // static
{
    struct stat Stat;
    if (stat(filename.c_str(), &Stat) == 0 && S_ISREG(Stat.st_mode)) {
        return Stat.st_ctime;
    }

    return 0;
}

fuppes_off_t File::getSize(const std::string filename) // static
{
    struct stat Stat;
    if (0 == stat(filename.c_str(), &Stat)) {
        return Stat.st_size;
    }

    return 0;
}
