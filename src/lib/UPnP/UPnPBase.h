/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            UPnPBase.h
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
 
#ifndef _UPNPBASE_H
#define _UPNPBASE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>

#include "UPnPErrorCodes.h"

typedef enum {
  UPNP_DEVICE_UNKNOWN,
  //UPNP_DEVICE_TYPE_ROOT_DEVICE,
  UPNP_DEVICE_MEDIA_SERVER,
  UPNP_DEVICE_MEDIA_RENDERER,
  UPNP_SERVICE_CONTENT_DIRECTORY,
  UPNP_SERVICE_RENDERING_CONTROL,
  UPNP_SERVICE_CONNECTION_MANAGER,
  UPNP_SERVICE_AV_TRANSPORT,
  UPNP_SERVICE_X_MS_MEDIA_RECEIVER_REGISTRAR,
  FUPPES_SOAP_CONTROL
}UPNP_DEVICE_TYPE;

class CUPnPBase
{

  protected:

    /** constructor
     *  @param  nType  the device type
     *  @param  version  the device version
     *  @param  p_sHTTPServerURL  URL of the HTTP server
     */
    CUPnPBase(UPNP_DEVICE_TYPE nType, int version, std::string p_sHTTPServerURL);  


  public:

    void setHttpServerUrl(const std::string httpServerUrl) { m_sHTTPServerURL = httpServerUrl; }
    
    /** returns the device type
     *  @return device type as string
     */
    std::string GetUPnPDeviceTypeAsString();
	  UPNP_DEVICE_TYPE GetUPnPDeviceType() { return m_nUPnPDeviceType; }

    int GetUPnPDeviceVersion() { return m_UPnPDeviceVersion; }
  
  protected:
    std::string       m_sHTTPServerURL;
    UPNP_DEVICE_TYPE  m_nUPnPDeviceType;
    int               m_UPnPDeviceVersion;

};

#endif // _UPNPBASE_H 
