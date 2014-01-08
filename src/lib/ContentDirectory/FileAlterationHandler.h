/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            FileAlterationHandler.h
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

#ifndef _FILEALTERATIONHANDLER_H
#define _FILEALTERATIONHANDLER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Common/Common.h"
#include "../Thread/Thread.h"
#include "FileAlterationMonitor.h"

class CContentDatabase;

class FileAlterationHandler: public IFileAlterationMonitor
{
  public:
    FileAlterationHandler(CContentDatabase* contentDb);
    void setMonitor(CFileAlterationMonitor* fileAlterationMonitor);
    
    void famEvent(CFileAlterationEvent* event);

    fuppes::DateTime    lastEventTime() { return m_lastEventTime; }
    
  private:
    CContentDatabase*         m_contentDb;
    
    Threading::Mutex          m_mutex;
    CFileAlterationMonitor*   m_fileAlterationMonitor;
    fuppes::DateTime          m_lastEventTime;

    void    createDirectory(CFileAlterationEvent* event);
    void    deleteDirectory(CFileAlterationEvent* event);
    void    moveDirectory(CFileAlterationEvent* event);


    void    createFile(CFileAlterationEvent* event);
    void    deleteFile(CFileAlterationEvent* event);
    void    moveFile(CFileAlterationEvent* event);
    void    modifyFile(CFileAlterationEvent* event);
};


#endif // _FILEALTERATIONHANDLER_H
