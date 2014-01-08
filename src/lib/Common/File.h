/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            File.h
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

#ifndef _FILE_H
#define _FILE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_types.h>

#include <string>

namespace fuppes
{

    class File
    {
        public:

            enum OpenMode
            {
                Closed = 0,
                Read = 1,
                Write = 2,
                ReadWrite = Read | Write,
                Truncate = 4,
                Append = 8,
                Text = 16
            };

            enum Flags
            {
                None = 0,
                Readable = 1,
                Writable = 2,
                Executable = 4
            };

            File(std::string fileName = "");
            ~File();
            void setFileName(std::string fileName);
            bool open(int mode); // File::OpenMode mode
            void close();
            fuppes_off_t size();
            bool isOpen();

            std::string path();
            std::string ext();

            bool seek(fuppes_off_t);
            fuppes_off_t read(char* buffer, fuppes_off_t length);
            bool getline(std::string& line);
            fuppes_off_t write(const char* buffer, fuppes_off_t length);

            static bool remove(const std::string fileName);
            static bool move(const std::string source, const std::string target);

            static bool exists(std::string fileName);
            static bool readable(std::string fileName);
            static bool writable(std::string fileName);
            static bool executable(std::string fileName);
            static std::string extension(const std::string filename);

            static time_t lastModified(const std::string filename);
            static fuppes_off_t getSize(const std::string filename);

        private:
            std::string m_fileName;
            OpenMode m_openMode;
            FILE* m_file;

    };

}

#endif // _FILE_H
