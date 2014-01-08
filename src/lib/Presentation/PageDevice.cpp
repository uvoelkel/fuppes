/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            PageDevice.cpp
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

#include "PageDevice.h"

#include "../UPnP/UPnPDevice.h"
#include <list>

using namespace std;

string PageDevice::content()
{
  stringstream result;

  // detected devices
  result << "<h1>detected render devices</h1>" << endl;  
  
  // sort the devices
  list<CUPnPDevice*> devices;
  /*for(unsigned int i = 0; i < CSharedConfig::Shared()->GetFuppesInstance(0)->GetRemoteDevices().size(); i++) {

    CUPnPDevice* pDevice = CSharedConfig::Shared()->GetFuppesInstance(0)->GetRemoteDevices()[i];
    if(pDevice->GetUPnPDeviceType() != UPNP_DEVICE_MEDIA_RENDERER) 
      continue;

    if(pDevice->descriptionAvailable())
      devices.push_front(pDevice);
    else
      devices.push_back(pDevice);
  }*/


  list<CUPnPDevice*>::iterator iter;
  int count = 0;
  for(iter = devices.begin(); iter != devices.end(); iter++) {
    
    CUPnPDevice* pDevice = *iter;
    
    result << "<div class=\"remote-device\">" << endl;
    
    // icon
    result << "<div class=\"remote-device-icon\">";
      if(pDevice->descriptionAvailable()) {
        stringstream url;
        if(pDevice->iconCount() == 0) {
          url << "/presentation/fuppes-icon-48x48.png"; //generic icon";
        }
        else {
          UPnPDeviceIcon icon = pDevice->icon(0);
          url << "http://" << pDevice->ipAddress() << ":" << pDevice->port() << icon.url;
        }
        result << "<img src=\"" << url.str() << "\" alt=\"" << "UPnP Renderer Icon" << "\"/>";
      }
      else {
       result << "icon loading";
      }
    result << "</div>" << endl;

 
    // friendly name    
    result << "<div class=\"remote-device-friendly-name\">";
    result << pDevice->GetFriendlyName();
    result << "</div>" << endl;


    result << "<div class=\"remote-device-info\">";
    result << pDevice->GetUPnPDeviceTypeAsString() << " : " << pDevice->GetUPnPDeviceVersion() << "<br />";    
    
    result << "<a href=\"javascript:deviceDetails(" << count << ");\">details</a>" << endl;
    result << "</div>" << endl;
    
    result << "<div class=\"clear\"></div>";
    
    // details
    result << "<div class=\"remote-device-details\" id=\"remote-device-details-" << count << "\">";


    result << "<table>" << endl;

    result << "<tr><th colspan=\"2\">details</th></tr>" << endl;
    
    // uuid
    result << "<tr><td>uuid</td><td>";
    result << pDevice->GetUUID();
    result << "</td></tr>" << endl;
    
    // timeout
    result << "<tr><td>timeout</td><td>";
    result << pDevice->GetTimer()->GetCount() / 60 << "min. " << pDevice->GetTimer()->GetCount() % 60 << "sec.";
    result << "</td></tr>" << endl;
    
    // ip : port

    // presentation url
    result << "<tr><td>presentation</td><td>";
    result << pDevice->presentationUrl() << "<br />";
    result << "</td></tr>" << endl;
    
    // manufacturer
    result << "<tr><td>manufacturer</td><td>";
    result << pDevice->manufacturer() << "<br />";
    result << "</td></tr>" << endl;
    
    // manufacturerUrl
    result << "<tr><td>manufacturerUrl</td><td>";
    result << pDevice->manufacturerUrl() << "<br />";
    result << "</td></tr>" << endl;



    // descriptionUrl
    result << "<tr><td>descriptionUrl</td><td>";
    result << pDevice->descriptionUrl() << "<br />";
    result << "</td></tr>" << endl;


    // mac
    result << "<tr><td>mac</td><td>";
    result << pDevice->macAddress();
    result << "</td></tr>" << endl;


    // device settings
    result << "<tr><td>device settings</td><td>";

    /*fuppes::StringList list = PathFinder::GetDevicesList();
    fuppes::StringListIterator iter;
    string selected = "";
    
    result << "<select id=\"device-" << pDevice->GetUUID() << "\" onchange=\"setDevice('" << pDevice->GetUUID() << "')\">";

    for(iter = list.begin(); iter != list.end(); iter++) {
      selected = "";
      if(pDevice->deviceSettings()->name() == *iter) {
        selected = " selected=\"selected\" ";
      }
      result << "<option" << selected << ">" << *iter << "</option>";
    }
    result << "</select>";*/
    
    /*
    static fuppes::StringList GetVfoldersList();*/

    result << "</td></tr>" << endl;


    // vfolder settings
    result << "<tr><td>vfolder settings</td><td>";

    // CSharedConfig::Shared()->virtualFolders()->getEnabledFolders();
    
    /*list = PathFinder::GetVfoldersList();
    result << "<select id=\"vfolder-" << pDevice->GetUUID() << "\"  onchange=\"setVfolder('" << pDevice->GetUUID() << "')\">";

    for(iter = list.begin(); iter != list.end(); iter++) {
      selected = "";
      if(pDevice->vfolderLayout() == *iter) {
        selected = " selected=\"selected\" ";
      }
      result << "<option" << selected << ">" << *iter << "</option>";
    }
    result << "</select>";*/
    
    /*
    static fuppes::StringList GetVfoldersList();*/

    result << "</td></tr>" << endl;
    
    
    result << "</table>" << endl;
    
    result << "</div>" << endl; //details
    

    result << "</div>" << endl; // remote-device

    count++;
  }

  result << "<div id=\"remote-device-count\" style=\"display: none;\">" << count << "</div>";


  

  result << "<h1>" << title() << "</h1>" << endl;  

  // device settings
  string deviceSettings;
  //CDeviceIdentificationMgr::Shared()->PrintSettings(&deviceSettings);
  //result << deviceSettings;
  
  return result.str();  
}

