/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            HTTPParser.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2006-2009 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#ifndef _HTTPPARSER_H
#define _HTTPPARSER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


//#include "../Http/HttpContextAware.h"

#include <string>
#include "../Common/Common.h"


class CHTTPMessage;

class CHTTPParser
{
    public:
      static bool parseHeader(std::string header, CHTTPMessage* message);
      static void ConvertURLEncodeContentToPlain(CHTTPMessage* message);

      static bool					hasContentLength(char* buffer);
      static fuppes_off_t getContentLength(char* buffer);

    private:
      static void parseCommonValues(std::string header, CHTTPMessage* message);
      static void parseGetVars(std::string header, CHTTPMessage* message);
      static void parseDlnaHeader(std::string header, CHTTPMessage* message);
      static void parseSecHeader(std::string header, CHTTPMessage* message);
};

#endif // _HTTPPARSER_H
