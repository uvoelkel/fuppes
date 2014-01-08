/***************************************************************************
 *            UUID.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2008 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#include "UUID.h"
#include "Common.h"

#ifdef WIN32
#include <objbase.h> /* for CoCreateGuid() */
#else

#ifndef HAVE_CONFIG_H
#undef HAVE_UUID
#endif

#ifdef HAVE_UUID
#include <uuid/uuid.h>
#endif
#endif

#ifndef WC_NO_BEST_FIT_CHARS
#define WC_NO_BEST_FIT_CHARS        0x00000400
#endif

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

#define UUID_LENGTH 36

bool readFromFile(std::string fileName, std::string* p_uuid)
{
	std::fstream fsUUID;
	fsUUID.open(fileName.c_str(), ios::in);

  if(fsUUID.fail() != 1) {
		
		// check length
		fsUUID.seekg(0, ios::end);
		int length = fsUUID.tellg();
		fsUUID.seekg(0, ios::beg);

		if(length != UUID_LENGTH) {
			fsUUID.close();
			return false;
		}

		// read uuid
		char uuid[UUID_LENGTH + 1];
		fsUUID.read((char*)&uuid, UUID_LENGTH);
		fsUUID.close();
		
		uuid[UUID_LENGTH] = '\0';	
		*p_uuid = uuid;
		
    return true;
  } 
  else {
    return false;
  }	
		
}

void writeToFile(std::string fileName, std::string uuid)
{
	std::fstream fsUUID;
	
	fsUUID.open(fileName.c_str(), ios::out | ios::trunc);
  if(fsUUID.fail() != 1) {
		fsUUID.write(uuid.c_str(), uuid.length());
		fsUUID.close();
  }
}

std::string GenerateUUID(std::string fileName)
{
  stringstream sResult;
	string result;

	// read uuid from file
	if(!fileName.empty()) {
		if(readFromFile(fileName, &result)) {
			return result;
		}
	}		 
	
#ifdef WIN32

  /* Generate GUID */
  GUID guid;
  CoCreateGuid(&guid);
  wchar_t szTemp[64];
  
  /* Get UUID as string */
  StringFromGUID2(guid, szTemp, sizeof(szTemp));
  
  /* Convert wide char string to ansi string */
  char szUUID[64];
  WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, szTemp, sizeof(szTemp),
    szUUID, sizeof(szUUID), NULL, NULL);

  /* Set result */
  string sTmp = szUUID;
  /* remove leading and trailing brackets */
  sResult << sTmp.substr(1, sTmp.length() - 2);

#else  
  
  #ifdef HAVE_UUID
  uuid_t uuid;
  char   szUUID[37];
  
  uuid_generate(uuid);  
  uuid_unparse(uuid, (char*)&szUUID);
  
  sResult << szUUID; 
  //delete[] szUUID;
  #else  
  srand(time(0));

  int nRandom;
  stringstream sRandom;

  do {
    nRandom = (rand() % 10000) + 1;
    sRandom << nRandom;
  } while (sRandom.str().length() < 8);

  sResult << sRandom.str().substr(0, 8) << "-aabb-dead-beef-1234eeff0000";  
  #endif // HAVE_UUID
  
#endif
  
	result = sResult.str();
		
	// write uuid to file
	if(!fileName.empty()) {
		writeToFile(fileName, result);
	}
	
  return result;
}
