/***************************************************************************
 *            PlaylistParser.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2007-2008 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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
 
#ifndef _PLAYLISTPARSER_H
#define _PLAYLISTPARSER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Common/Common.h"

#include <string>
#include <list>

struct PlaylistEntry_t
{
  std::string   sFileName;
  bool          bIsLocalFile;
	std::string		sMimeType;
	fuppes_off_t	nSize;
  fuppes_off_t  length;
		
	std::string		sTitle;
};

class BasePlaylistParser
{
  public:
    BasePlaylistParser(std::string p_sFileName);
    virtual ~BasePlaylistParser();
    
    // Common functions
    static BasePlaylistParser* Load(std::string p_sFileName);
  
    bool              Eof() { return m_bEof; }
    PlaylistEntry_t*  Entry();
    void              Next();

    // abstract functions
    virtual bool Parse(std::string p_sContent) = 0;
    virtual int GetLengthSeconds(void) { return -1; }   // by default say that you don't know what the length is (no setter allowed)

  protected:
    bool                        m_bEof;
    int                         m_nPosition;
    std::string                 m_sListPath;

    std::list<PlaylistEntry_t*>            m_lEntries;
    std::list<PlaylistEntry_t*>::iterator  m_lEntriesIterator;

    std::string FormatFileName(std::string p_sValue);

    bool IsURL(std::string p_sValue);
    bool IsFile(std::string p_sValue);
};

class M3U_PlaylistParser : public BasePlaylistParser 
{
  public:
    M3U_PlaylistParser(std::string p_sFileName) : BasePlaylistParser(p_sFileName) {}
    virtual bool Parse(std::string p_sContent);
    
    virtual int GetLengthSeconds(void) { return playlistLength; }
  private:
    int playlistLength;
};

class PLS_PlaylistParser : public BasePlaylistParser 
{
  public:
    PLS_PlaylistParser(std::string p_sFileName) : BasePlaylistParser(p_sFileName) {}
    virtual bool Parse(std::string p_sContent);
};

class RSS_PlaylistParser : public BasePlaylistParser 
{
  public:
    RSS_PlaylistParser(std::string p_sFileName) : BasePlaylistParser(p_sFileName) {}
    virtual bool Parse(std::string p_sContent);
};

class WPL_PlaylistParser : public BasePlaylistParser 
{
  public:
    WPL_PlaylistParser(std::string p_sFileName) : BasePlaylistParser(p_sFileName) {}
    virtual bool Parse(std::string p_sContent);
};

#endif // _PLAYLISTPARSER_H
