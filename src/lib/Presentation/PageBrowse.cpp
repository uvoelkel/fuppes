/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            PageBrowse.cpp
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

#include "PageBrowse.h"

#include "../Common/Common.h"

std::string PageBrowse::content()
{
  std::stringstream result;
  
  result << "<h1>browse data</h1>";
  result << 
    "<div>" <<
    "<form>" <<
      "virtual folder layout: " <<
      "<select id=\"virtual-layout\">" << 
        "<option selected=\"selected\">none</option>\n";
  /*fuppes::StringList enabled = CSharedConfig::Shared()->virtualFolders()->getEnabledFolders();
  for(unsigned int i = 0; i < enabled.size(); i++) {
    result << "<option>" << enabled.at(i) << "</option>\n";
  }*/
  result << 
      "</select> " <<
      "<input type=\"submit\" value=\"browse\" onclick=\"browseDirectChildren(0, 0, 0, $('virtual-layout').value); return false;\" />" <<
    "</form>" <<
    "</div>";
  
  result << "<div style=\"margin-top: 20px;\" id=\"browse-result\"></div>";
  
  return result.str();
}


