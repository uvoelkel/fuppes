/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            Common.h
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#ifndef _COMMON_H
#define _COMMON_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>

#include <string>
#include <sstream>
#include <assert.h>
#include <vector>
#include <unistd.h>

//#include <libxml/xmlwriter.h>


#ifdef WIN32

/* This must be defined to use InitializeCriticalSectionAndSpinCount() */
//#define _WIN32_WINNT 0x0410 // Windows 98 or later

/*
#pragma comment(lib,"Wsock32.lib") 
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"shlwapi.lib")
*/

//#include <winsock2.h>
//#include <ws2tcpip.h>
#include <shlwapi.h>

#else

#include <pthread.h>

#endif


#include "File.h"
#include "Directory.h"
#include "DateTime.h"




/* 
moved to fuppes_types.h because it's needed by some plugins

#ifdef WIN32
  #if SIZEOF_LONG_INT == 8
  typedef long int fuppes_off_t;  
	#elif SIZEOF_LONG_LONG_INT == 8
	typedef long long int fuppes_off_t;  
	#endif	
#else
  typedef off_t fuppes_off_t;
#endif
*/

#include <fuppes_types.h>
 

#define ASSERT assert

const std::string MIME_TYPE_TEXT_HTML = "text/html";

std::string StringReplace(std::string p_sIn, std::string p_sSearch, std::string p_sReplace);
std::string ExtractFileExt(std::string p_sFileName);
std::string ExtractFilePath(std::string p_sFileName);
std::string TruncateFileExt(std::string p_sFileName);
bool ExtractFolderFromPath(std::string p_sPath, std::string* p_sFolder);
std::string TrimFileName(std::string p_sFileName, unsigned int p_nMaxLength);
std::string TrimWhiteSpace(std::string s);
std::string MD5Sum(std::string p_sFileName);

/*
void        appendTrailingSlash(std::string* value);
std::string appendTrailingSlash(std::string value);
*/

std::string ToLower(std::string p_sInput);
std::string ToUpper(std::string p_sInput);
bool SplitURL(std::string p_sURL, std::string* p_sIPAddress, unsigned int* p_nPort);
int  Base64Decode(const std::string p_sInputString, char* p_szOutBuffer, int p_nBufSize);

unsigned int HexToInt(std::string sHex);
object_id_t HexToObjectId(std::string hex);
std::string SQLEscape(std::string p_sValue);

std::string ToUTF8(std::string p_sValue, std::string p_sEncoding);

std::string URLEncodeValueToPlain(std::string p_sValue);

void fuppesSleep(unsigned int p_nMilliseconds);

//fuppes_off_t getFileSize(std::string fileName);

fuppes_off_t strToOffT(std::string value);

#ifdef WIN32
  typedef SOCKET fuppesSocket;  
#else
  typedef int fuppesSocket;
#endif


#ifdef WIN32
  typedef HINSTANCE  fuppesLibHandle;
  typedef FARPROC    fuppesProcHandle;
#else
  #include <dlfcn.h>
  
  typedef void*      fuppesLibHandle;
  typedef void*      fuppesProcHandle;
#endif

fuppesLibHandle   FuppesLoadLibrary(std::string p_sLibName);
fuppesProcHandle  FuppesGetProcAddress(fuppesLibHandle p_LibHandle, std::string p_sProcName);
bool              FuppesCloseLibrary(fuppesLibHandle p_LibHandle);

/*
#ifdef WIN32
#define upnpPathDelim           "\\"
#else*/
#define upnpPathDelim           "/"
//#endif

namespace fuppes
{

class FormatHelper
{
  public:
    static std::string msToUpnpDuration(int ms);
    static std::string fileNameToTitle(std::string fileName, std::string localCharset);
};


/*typedef             StringList;*/

typedef std::vector<std::string>::iterator  StringListIterator;

class StringList
{
  public:

    unsigned int size() {
      return m_list.size();
    }
    
    std::string at(int idx) { 
      return m_list.at(idx); 
    }
    
    void push_back(std::string value) {
      m_list.push_back(value);
    }

    int indexOf(std::string value) {

      for(unsigned int i = 0; i < m_list.size(); i++) {
        if(m_list.at(i) == value) {
          return (int)i;
        }
      }
      
      return -1;
    }

    StringListIterator begin() {
      return m_list.begin();
    }
    
    StringListIterator end() {
      return m_list.end();
    }

    void clear() {
      m_list.clear();
    }
    
  private:
    std::vector<std::string>  m_list;
};

class String
{
  public:
    static StringList split(std::string in, std::string delimiter);
};

}


#endif // _COMMON_H
