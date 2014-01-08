/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            PageLog.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2011 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#include "PageLog.h"

#include "../UPnP/UPnPDevice.h"

//#include "../Logging/Log.h"
#include "../SharedLog.h"

using namespace std;
using namespace fuppes;
//using namespace Logging;

string PageLog::content()
{
  stringstream result;

  result << "<h1>" << title() << "</h1>" << endl;

  result << buildLogSelection();

  result << "<h2>" << "log to webinterface" << "</h2>" << endl;
  
  result << "<a href=\"#\" id=\"btn-start\">start</a> ";
  result << "<a href=\"#\" id=\"btn-stop\">stop</a> ";
  result << "<a href=\"#\" id=\"btn-clear\">clear</a> ";

  result << "<div style=\"height: 200px\">";
  result << "<div id=\"log-output\"></div>";
  result << "</div>";

    result << "<p>note: to use this function you need a webbrowser that supports \"workers\".<br />any current Firefox or Chrome Version should work</p>" << endl;
  return result.str();  
}


string PageLog::buildLogSelection()
{
  stringstream result;
  //result << "<h1>logging</h1>";

  int level = CSharedLog::Shared()->GetLogLevelInt();
  
  result << "<input type=\"radio\" name=\"log-level\" id=\"log-level-0\" value=\"none\" " << 
    "onclick=\"logLevel(0)\" " << (level == 0 ? "checked=\"checked\"" : "") << " />" <<
    "<label for=\"log-level-0\">none</label>";

  result << "<input type=\"radio\" name=\"log-level\" id=\"log-level-1\" value=\"normal\" " << 
    "onclick=\"logLevel(1)\" " << (level == 1 ? "checked=\"checked\"" : "") << " />" <<
    "<label for=\"log-level-1\">normal</label>";
  
  result << "<input type=\"radio\" name=\"log-level\" id=\"log-level-2\" value=\"extended\" " << 
    "onclick=\"logLevel(2)\" " << (level == 2 ? "checked=\"checked\"" : "") << " />" <<
    "<label for=\"log-level-2\">extended</label>";
  
  result << "<input type=\"radio\" name=\"log-level\" id=\"log-level-3\" value=\"debug\" " << 
    "onclick=\"logLevel(3)\" " << (level == 3 ? "checked=\"checked\"" : "") << " />" <<
    "<label for=\"log-level-3\">debug</label>";

  result << "<br />";

/*
contentdir = 1 << 5,
contentdb  = 1 << 6,
sql        = 1 << 7,
plugin     = 1 << 8,
config     = 1 << 9,
hotplug    = 1 << 10,
*/


  /*

  result << "<input type=\"checkbox\" name=\"log-sender\" id=\"log-sender-http\" value=\"http\" " << 
    (Log::isActiveSender(Log::http) ? "checked=\"checked\"" : "") << " onclick=\"logSender('http')\" />" <<
    "<label for=\"log-sender-http\">http</label>";

  result << "<input type=\"checkbox\" name=\"log-sender\" id=\"log-sender-soap\" value=\"soap\" " << 
    (Log::isActiveSender(Log::soap) ? "checked=\"checked\"" : "") << " onclick=\"logSender('soap')\" />" <<
    "<label for=\"log-sender-soap\">soap</label>";

  result << "<input type=\"checkbox\" name=\"log-sender\" id=\"log-sender-gena\" value=\"gena\" " << 
    (Log::isActiveSender(Log::gena) ? "checked=\"checked\"" : "") << " onclick=\"logSender('gena')\" />" <<
    "<label for=\"log-sender-gena\">gena</label>"; 

  result << "<input type=\"checkbox\" name=\"log-sender\" id=\"log-sender-ssdp\" value=\"ssdp\" " << 
    (Log::isActiveSender(Log::ssdp) ? "checked=\"checked\"" : "") << " onclick=\"logSender('ssdp')\" />" <<
    "<label for=\"log-sender-ssdp\">ssdp</label>";

  result << "<input type=\"checkbox\" name=\"log-sender\" id=\"log-sender-fam\" value=\"fam\" " << 
    (Log::isActiveSender(Log::fam) ? "checked=\"checked\"" : "") << " onclick=\"logSender('fam')\" />" <<
    "<label for=\"log-sender-fam\">fam</label>";
  */
  return result.str();
}



