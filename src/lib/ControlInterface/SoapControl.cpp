/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            SoapControl.cpp
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

#include "SoapControl.h"

#include "ControlInterface.h"
#include "../Configuration/Config.h"
#include "../Common/XMLParser.h"

#include <iostream>
#include <sstream>
using namespace std;

#include "SoapControlDescription.cpp"

SoapControl::SoapControl(std::string HTTPServerURL, ControlInterface* controlInterface) :
        UPnPService(FUPPES_SOAP_CONTROL, 1, HTTPServerURL)
{
    m_controlInterface = controlInterface;
}

std::string SoapControl::getServiceDescription()
{
    return soapControlDescription;
}

Fuppes::HttpResponse* SoapControl::handleAction(CUPnPAction* upnpAction, std::string action, Fuppes::HttpRequest* request)
{

    //cout << "SoapControl:: handleAction : " << pUPnPAction->GetContent() << endl;

    CXMLDocument xml;
    xml.LoadFromString(upnpAction->GetContent());

    CXMLNode* tmp = xml.RootNode()->FindNodeByName("Body");
    if (tmp == NULL)
        return new Fuppes::HttpResponse(Http::BAD_REQUEST);

    tmp = tmp->ChildNode(0);

    stringstream content;
    string response;

    ControlAction* ctrlAction = m_controlInterface->getAction(upnpAction->getAction());
    if (NULL == ctrlAction) {

        content << "<Code>" << ControlInterface::Error << "</Code><Message>function not implemented</Message>";

        response = "<c:Error xmlns:c=\"urn:fuppesControl\">";
        response += "<Result>" + content.str() + "</Result>";
        response += "</c:Error>";
    }
    else {

        // create input params
        ControlActionParams params;
        for (int i = 0; i < tmp->ChildCount(); i++) {
            ControlActionParam param;
            param.name = tmp->ChildNode(i)->name();
            param.value = tmp->ChildNode(i)->value();
            tmp->ChildNode(i)->attributes(param.attributes);
            params.push_back(param);
        }

        // call the action
        ControlActionParam resultParam;
        ControlActionParams resultParams;
        ControlInterface::ErrorCode error = ControlInterface::Error;

        if (ctrlAction->exec_single) {
            error = (m_controlInterface->*(ctrlAction->exec_single))(params, resultParam);
            paramToXml(resultParam, content);
        }
        else if (ctrlAction->exec_multi) {
            error = (m_controlInterface->*(ctrlAction->exec_multi))(params, resultParams);
            paramsToXml(resultParams, content);
        }

        if (0 == content.str().length()) {

        	string msg;
        	switch (error) {
        		case ControlInterface::Ok:
        			msg = "OK";
        			break;
        		case ControlInterface::RestartRequired:
					msg = "RestartRequired";
					break;
        		case ControlInterface::Unchanged:
					msg = "Unchanged";
					break;

        		case ControlInterface::Error:
					msg = "Error";
					break;
        		case ControlInterface::MissingInputParam:
					msg = "MissingInputParam";
					break;
        		case ControlInterface::MissingAttribute:
					msg = "MissingAttribute";
					break;
        		case ControlInterface::NotAllowed:
					msg = "NotAllowed";
					break;
        	}

        	content << "<Result><Code>" << error << "</Code><Message>" << msg << "</Message></Result>";
        }

        if (ControlInterface::Error > error) {
            response = "<u:" + tmp->name() + "Response xmlns:u=\"urn:fupppes:service:SoapControl:1\">";
            response += content.str();
            response += "</u:" + tmp->name() + "Response>";
        }
        else if (ControlInterface::Error <= error) {
            response = "<c:Error xmlns:c=\"urn:fuppesControl\">";
            response += content.str();
            response += "</c:Error>";
        }

    }

    /*
     switch(action->type()) {

     case FUPPES_CTRL_SET_LOGBUFFER_ACTIVE: {
     content << "active";
     fuppes::LogMgr::instance()->setActive(true);
     }
     break;

     case FUPPES_CTRL_SET_DEVICE_CONFIG:
     setDeviceConfig(tmp, content);
     break;
     case FUPPES_CTRL_SET_VFOLDER_CONFIG:
     setVfolderConfig(tmp, content);
     break;


     default:
     break;
     }
     */

    string result =
            "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                    "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                    "  <s:Body>"
                    + response +
                    "  </s:Body>"
                            "</s:Envelope>";

    /*
     pMessageOut->SetMessage(HTTP_MESSAGE_TYPE_200_OK, "text/xml; charset=\"utf-8\"");
     pMessageOut->SetContent(result);*/

    return new Fuppes::HttpResponse(Http::OK, "text/xml; charset=\"utf-8\"", result);

    //return NULL;
}

void SoapControl::paramsToXml(ControlActionParams params, std::stringstream& result)
{
    ControlActionParamsIter iter = params.begin();
    for (; iter != params.end(); iter++) {
        paramToXml(*iter, result);
    }
}

void SoapControl::paramToXml(ControlActionParam param, std::stringstream& result)
{
    if (param.name.length() == 0)
        return;

    // opening tag
    result << "<" << param.name;

    // attributes
    std::map<std::string, std::string>::const_iterator attrsIter;
    for (attrsIter = param.attributes.begin(); attrsIter != param.attributes.end(); attrsIter++)
        result << " " << attrsIter->first << "=\"" << attrsIter->second << "\"";

    // finish opening tag
    result << ">";

    // write value
    result << param.value;
    // add child nodes
    paramsToXml(param.children, result);

    // closing tag
    result << "</" << param.name << ">";
}

/*
 void SoapControl::setDeviceConfig(CXMLNode* request, std::stringstream& result)
 {
 setDeviceConfigValue(request, "device", result);
 }

 void SoapControl::setVfolderConfig(CXMLNode* request, std::stringstream& result)
 {
 setDeviceConfigValue(request, "vfolder", result);
 }

 void SoapControl::setDeviceConfigValue(CXMLNode* request, std::string value, std::stringstream& result)
 {
 CXMLNode* uuidNode = request->FindNodeByName("uuid", true);
 CXMLNode* valueNode = request->FindNodeByName(value, true);
 if(uuidNode == NULL || valueNode == NULL) {
 return;
 }

 // get the remote device that belongs to uuid
 CUPnPDevice* device = NULL;
 for(unsigned int i = 0; i < CSharedConfig::Shared()->GetFuppesInstance(0)->GetRemoteDevices().size(); i++) {

 device = CSharedConfig::Shared()->GetFuppesInstance(0)->GetRemoteDevices()[i];
 if(device->GetUUID() == uuidNode->value()) {
 break;
 }
 device = NULL;
 }

 if(!device) {
 result << "<Code>123</Code><Message>unknown device</Message>";
 return;
 }


 fuppes::EntryList entries;
 fuppes::EntryListIterator iter;
 fuppes::ConfigEntry entry;
 fuppes::Config::getEntries("device_mapping", "", entries);

 bool found = false;
 for(iter = entries.begin(); iter != entries.end(); iter++) {

 entry = *iter;

 // found by mac
 if(entry.key.compare("mac") == 0 && entry.attributes["value"].compare(device->macAddress()) == 0) {
 found = true;
 break;
 }
 else if(entry.key.compare("ip") == 0 && entry.attributes["value"].compare(device->ipAddress()) == 0) {
 found = true;
 break;
 }

 }

 // create a new entry if none found
 if(!found) {
 entry.clear();
 entry.key = "mac";
 entry.attributes["value"] = device->macAddress();
 }

 // set the new device value
 entry.attributes[value] = valueNode->value();

 fuppes::Config::setEntry("device_mapping", "", entry);


 result << "<Code>0</Code><Message>OK</Message>";
 }
 */
