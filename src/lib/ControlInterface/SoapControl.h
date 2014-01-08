/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            SoapControl.h
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2008-2011 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#ifndef _SOAPCONTROL_H
#define _SOAPCONTROL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ControlInterface.h"
#include "../UPnP/UPnPService.h"

//#include "../HTTP/HTTPMessage.h"

class SoapControl: public UPnPService
{
    public:
        SoapControl(std::string HTTPServerURL, ControlInterface* controlInterface);

        // from CUPnPDervice
        std::string getServiceDescription();
        //void HandleUPnPAction(CUPnPAction* pUPnPAction, CHTTPMessage* pMessageOut);
        Fuppes::HttpResponse* handleAction(CUPnPAction* upnpAction, std::string action, Fuppes::HttpRequest* request);

    private:
        void paramsToXml(ControlActionParams params, std::stringstream& result);
        void paramToXml(ControlActionParam param, std::stringstream& result);

        ControlInterface* m_controlInterface;

        /*
         void	setDeviceConfig(CXMLNode* request, std::stringstream& result);
         void	setVfolderConfig(CXMLNode* request, std::stringstream& result);
         void  setDeviceConfigValue(CXMLNode* request, std::string value, std::stringstream& result);
         */
};

#endif // _SOAPCONTROL_H
