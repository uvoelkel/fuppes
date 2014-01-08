/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            PageConfig.cpp
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

#include "PageConfig.h"

#include "../Common/Common.h"

std::string PageConfig::content()
{
  std::stringstream result;
  
  result << "<div class=\"grid_12\"><p id=\"cfg_file_name\"></p></div>";

  result << "<div class=\"grid_12\"><h1>shared objects</h1></div>";
  result << "<div class=\"clear\"></div>";

  result << "<div id=\"shared-objects-list\"></div>";
    

  result << "<div id=\"dlg-shared-object\">";

  result << "<div class=\"framehead\">add shared object</div>" << std::endl;

  result << "<div id=\"dlg-shared-object-content\">";
  
  result << "<div id=\"dlg-shared-object-types\">";
  result << "</div>";
  
  result << "<div id=\"object-list\"></div>";
  result << "<div id=\"selected-object-type\"></div>";
  result << "<div id=\"selected-object-name\"></div>";
  result << "<div id=\"selected-object-path\"></div>";

  result << "<span><input type=\"button\" id=\"submit\" onclick=\"submit();\" name=\"submit\" value=\"submit\" /></span> ";
  result << "<span><input type=\"button\" id=\"cancel\" onclick=\"cancel();\" name=\"cancel\" value=\"cancel\" /></span>";
  
  result << "</div>";
  result << "</div>";


  result << "<form method=\"post\">";


  result << "<div class=\"grid_12\"><h1>network settings</h1></div>";
  result << "<div class=\"clear\"></div>";

  result << "<div class=\"grid_3\"><label for=\"cfg_interface\">";
#ifdef WIN32
  result << "IP-address";
#else
  result << "Interface name or IP-address";
#endif
  result << "</label></div>";
  result << "<div class=\"grid_9\"><input type=\"text\" name=\"cfg_interface\" id=\"cfg_interface\" class=\"cfg_input\" data-function=\"Interface\" /></div>";
  result << "<div class=\"clear\"></div>";


  result << "<div class=\"grid_3\"><label for=\"cfg_httpport\">HTTP port</label></div>";
  result << "<div class=\"grid_9\"><input type=\"text\" name=\"cfg_httpport\" id=\"cfg_httpport\" class=\"cfg_input\" data-function=\"HttpPort\" /></div>";
  result << "<div class=\"clear\"></div>";



  result << "<div class=\"grid_12\"><h1>system settings</h1></div>";
  result << "<div class=\"clear\"></div>";

  result << "<div class=\"grid_3\"><label for=\"cfg_charset\">charset</label></div>";
  result << "<div class=\"grid_9\"><input type=\"text\" name=\"cfg_charset\" id=\"cfg_charset\" class=\"cfg_input\" data-function=\"Charset\" /></div>";
  result << "<div class=\"clear\"></div>";


  result << "<div class=\"grid_3\"><label for=\"cfg_charset\">uuid</label></div>";
  result << "<div class=\"grid_9\"><input type=\"text\" name=\"cfg_uuid\" id=\"cfg_uuid\" class=\"cfg_input\" data-function=\"Uuid\" /></div>";
  result << "<div class=\"clear\"></div>";

  result << "<div class=\"grid_3\"><label for=\"cfg_charset\">fixed uuid</label></div>";
  result << "<div class=\"grid_9\"><input type=\"text\" name=\"cfg_fixeduuid\" id=\"cfg_fixeduuid\" class=\"cfg_input\" data-function=\"IsFixedUuid\" /></div>";
  result << "<div class=\"clear\"></div>";

  result << "</form>";

  // <network> / 
    // <interface />
    // <http_port />
    // <allowed_ips>

  // <database type="sqlite3"> <!-- sqlite3 | mysql -->
    // <file />  <!-- if empty default path will be used -->
    //<hostname />    <username />    <password />    <dbname />
    //readonly>false</readonly>

  // <global_settings>
    // <local_charset>UTF-8</local_charset>
    // <temp_dir/>
    // <use_fixed_uuid>false</use_fixed_uuid>

//	<vfolders enabled="false">
		//<vfolder name="default" enabled="true" />
		//<vfolder name="xbox" enabled="false" />
	
  //<device_mapping>
    //<!--ip value="123.123.123.123" device="xbox" vfolder="xbox" /-->
    //<!--mac value="00:12:5a:f0:59:3f" device="xbox" vfolder="xbox"/-->
  
  return result.str();
}


