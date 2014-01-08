/***************************************************************************
 *            ConnectionManager.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2006-2008 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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
 
#include "ConnectionManager.h"
#include "ConnectionManagerDescription.cpp"

using namespace std;

CConnectionManagerCore* CConnectionManagerCore::m_instance = 0;


void CConnectionManagerCore::init() // static
{
	if(m_instance == 0) {
		m_instance = new CConnectionManagerCore();
	}
}

void CConnectionManagerCore::uninit() // static
{
	if(m_instance == 0)
		return;
  delete m_instance;
  m_instance = NULL;
}


CConnectionManager::CConnectionManager():
	UPnPService(UPNP_SERVICE_CONNECTION_MANAGER, 1, "")
{
}

std::string CConnectionManager::getServiceDescription()
{  
  return sConnectionManagerDescription;
}

Fuppes::HttpResponse* CConnectionManager::handleAction(CUPnPAction* upnpAction, std::string action, Fuppes::HttpRequest* request)
{
	CM_ERROR ret = ERR_INVALID_ACTION;
	string   res;
    
	switch(upnpAction->GetActionType()) {
		case CMA_GET_PROTOCOL_INFO:
			ret = getProtocolInfo(upnpAction, res);
			break;
		/*case CMA_PREPARE_FOR_CONNECTION:
			ret = prepareForConnection(pUPnPAction, &res);
			break;
		case CMA_CONNECTION_COMPLETE:
			ret = connectionComplete(pUPnPAction, &res);
			break;*/
		case CMA_GET_CURRENT_CONNECTION_IDS:
			ret = getCurrentConnectionIds(upnpAction, res);
			break;
		case CMA_GET_CURRENT_CONNECTION_INFO:
			ret = getCurrentConnectionInfo(upnpAction, res);
			break;
	}


    Fuppes::HttpResponse* response = NULL;
    
	if(ret == ERR_NONE) {

        response = new Fuppes::HttpResponse(Http::OK, "text/xml; charset=\"utf-8\"", res);
        //response->getHeader()->setValue("Content-Type", "text/xml; charset=\"utf-8\"");        
	}
	else {
		std::string sContent = 
			"<?xml version=\"1.0\" encoding=\"utf-8\"?>"  
			"<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
			"  <s:Body>"
			"    <s:Fault>"
			"      <faultcode>s:Client</faultcode>"
			"      <faultstring>UPnPError</faultstring>"
			"      <detail>"
			"        <UPnPError xmlns=\"urn:schemas-upnp-org:control-1-0\">"
			"          <errorCode>401</errorCode>"
			"          <errorDescription>Invalid Action</errorDescription>"
			"        </UPnPError>"
			"      </detail>"
			"    </s:Fault>"
			"  </s:Body>"
			"</s:Envelope>";

        response = new Fuppes::HttpResponse(Http::INTERNAL_SERVER_ERROR, "text/xml; charset=\"utf-8\"", sContent);
        //response->getHeader()->setValue("Content-Type", "text/xml; charset=\"utf-8\"");
	}

    return response;  
}



CM_ERROR CConnectionManager::getProtocolInfo(CUPnPAction* action, std::string& result)
{
/*
POST /ConnectionManager/Control HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:ConnectionManager:1#GetProtocolInfo"
Content-Type: text/xml; charset=utf-8

<?xml version="1.0"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
  <s:Body>
    <u:GetProtocolInfo xmlns:u="urn:schemas-upnp-org:service:ConnectionManager:1">
    </u:GetProtocolInfo>
  </s:Body>
</s:Envelope>

HTTP/1.1 200 OK
Content-Type:  text/xml; charset="utf-8"

<?xml version="1.0" encoding="utf-8"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
  <s:Body>
    <u:GetProtocolInfoResponse xmlns:u="urn:schemas-upnp-org:service:ConnectionManager:1">
      <Source>http-get:*:audio/mpeg:DLNA.ORG_PN=MP3;DLNA.ORG_OP=01;DLNA.ORG_FLAGS=01700000000000000000000000000000,http-get:*:video/flv:*,http-get:*:application/ogg:*,http-get:*:audio/x-flac:*</Source>
      <Sink></Sink>
    </u:GetProtocolInfoResponse>
  </s:Body>
</s:Envelope>
*/

  
  xmlTextWriterPtr writer;
	xmlBufferPtr buf;
	
	buf    = xmlBufferCreate();   
	writer = xmlNewTextWriterMemory(buf, 0);    
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
  
  // root
  xmlTextWriterStartElementNS(writer, BAD_CAST "s", BAD_CAST "Envelope", NULL);    
  xmlTextWriterWriteAttributeNS(writer, BAD_CAST "s", 
    BAD_CAST "encodingStyle", 
    BAD_CAST  "http://schemas.xmlsoap.org/soap/envelope/", 
    BAD_CAST "http://schemas.xmlsoap.org/soap/encoding/");
   
    // body
    xmlTextWriterStartElementNS(writer, BAD_CAST "s", BAD_CAST "Body", NULL);    
  
      // GetProtocolInfo response
      xmlTextWriterStartElementNS(writer, BAD_CAST "u",        
        BAD_CAST "GetProtocolInfoResponse", 
        BAD_CAST "urn:schemas-upnp-org:service:ConnectionManager:1");

        // Source
        xmlTextWriterStartElement(writer, BAD_CAST "Source");
        xmlTextWriterWriteString(writer, BAD_CAST action->DeviceSettings()->protocolInfo().c_str());
        xmlTextWriterEndElement(writer);

        // Sink
        xmlTextWriterStartElement(writer, BAD_CAST "Sink");
        xmlTextWriterWriteString(writer, BAD_CAST "");
        xmlTextWriterEndElement(writer);
                                
      // end GetProtocolInfo response
      xmlTextWriterEndElement(writer);
      
    // end body
    xmlTextWriterEndElement(writer);
   
	// end root
	xmlTextWriterEndElement(writer);
  xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);
	
  result = (const char*)buf->content;
	xmlBufferFree(buf);

 	return ERR_NONE;
}

CM_ERROR CConnectionManager::prepareForConnection(CUPnPAction* /*action*/, std::string* /*result*/)
{
	return ERR_INVALID_ACTION;
}

CM_ERROR CConnectionManager::connectionComplete(CUPnPAction* /*action*/, std::string* /*result*/)
{
	return ERR_INVALID_ACTION;
}

CM_ERROR CConnectionManager::getCurrentConnectionIds(CUPnPAction* /*action*/, std::string& result)
{
/*
POST /ConnectionManager/Control HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:ConnectionManager:1#GetCurrentConnectionIDs"
Content-Type: text/xml; charset=utf-8

<?xml version="1.0"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
  <s:Body>
    <u:GetCurrentConnectionIDs xmlns:u="urn:schemas-upnp-org:service:ConnectionManager:1">
    </u:GetCurrentConnectionIDs>
  </s:Body>
</s:Envelope>


HTTP/1.1 200 OK
Content-Type:  text/xml; charset="utf-8"

<?xml version="1.0" encoding="utf-8"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
  <s:Body>
    <u:GetCurrentConnectionIDsResponse xmlns:u="urn:schemas-upnp-org:service:ConnectionManager:1">
      <ConnectionIDs>0</ConnectionIDs>
    </u:GetCurrentConnectionIDsResponse>
  </s:Body>
</s:Envelope>
*/

  xmlTextWriterPtr writer;
	xmlBufferPtr buf;
	
	buf    = xmlBufferCreate();   
	writer = xmlNewTextWriterMemory(buf, 0);    
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
  
  // root
  xmlTextWriterStartElementNS(writer, BAD_CAST "s", BAD_CAST "Envelope", NULL);    
  xmlTextWriterWriteAttributeNS(writer, BAD_CAST "s", 
    BAD_CAST "encodingStyle", 
    BAD_CAST  "http://schemas.xmlsoap.org/soap/envelope/", 
    BAD_CAST "http://schemas.xmlsoap.org/soap/encoding/");
   
    // body
    xmlTextWriterStartElementNS(writer, BAD_CAST "s", BAD_CAST "Body", NULL);    
  
      // GetCurrentConnectionIDs response
      xmlTextWriterStartElementNS(writer, BAD_CAST "u",        
        BAD_CAST "GetCurrentConnectionIDsResponse", 
        BAD_CAST "urn:schemas-upnp-org:service:ConnectionManager:1");

        // ConnectionIDs
        xmlTextWriterStartElement(writer, BAD_CAST "ConnectionIDs");
        xmlTextWriterWriteString(writer, BAD_CAST "0");
        xmlTextWriterEndElement(writer);
                                
      // end GetCurrentConnectionIDs response
      xmlTextWriterEndElement(writer);
      
    // end body
    xmlTextWriterEndElement(writer);
   
	// end root
	xmlTextWriterEndElement(writer);
  xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);
	
  result = (const char*)buf->content;
	xmlBufferFree(buf);
  
	return ERR_NONE;
}

CM_ERROR CConnectionManager::getCurrentConnectionInfo(CUPnPAction* /*action*/, std::string& result)
{
/*
POST /ConnectionManager/Control HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:ConnectionManager:1#GetCurrentConnectionInfo"
Content-Type: text/xml; charset=utf-8

<?xml version="1.0"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
  <s:Body>
    <u:GetCurrentConnectionInfo xmlns:u="urn:schemas-upnp-org:service:ConnectionManager:1">
      <ConnectionID>0</ConnectionID>
    </u:GetCurrentConnectionInfo>
  </s:Body>
</s:Envelope>

HTTP/1.1 200 OK
Content-Type:  text/xml; charset="utf-8"

<?xml version="1.0" encoding="utf-8"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
  <s:Body>
    <u:GetCurrentConnectionInfoResponse xmlns:u="urn:schemas-upnp-org:service:ConnectionManager:1">
      <RcsID>-1</RcsID>
      <AVTransportID>-1</AVTransportID>
      <ProtocolInfo></ProtocolInfo>
      <PeerConnectionManager></PeerConnectionManager>
      <PeerConnectionID>0</PeerConnectionID>
      <Direction>Output</Direction>
      <Status>Unknown</Status>
    </u:GetCurrentConnectionInfoResponse>
  </s:Body>
</s:Envelope>
*/
 
  xmlTextWriterPtr writer;
	xmlBufferPtr buf;
	
	buf    = xmlBufferCreate();   
	writer = xmlNewTextWriterMemory(buf, 0);    
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
  
  // root
  xmlTextWriterStartElementNS(writer, BAD_CAST "s", BAD_CAST "Envelope", NULL);    
  xmlTextWriterWriteAttributeNS(writer, BAD_CAST "s", 
    BAD_CAST "encodingStyle", 
    BAD_CAST  "http://schemas.xmlsoap.org/soap/envelope/", 
    BAD_CAST "http://schemas.xmlsoap.org/soap/encoding/");
   
    // body
    xmlTextWriterStartElementNS(writer, BAD_CAST "s", BAD_CAST "Body", NULL);    
  
      // GetCurrentConnectionInfo response
      xmlTextWriterStartElementNS(writer, BAD_CAST "u",        
        BAD_CAST "GetCurrentConnectionInfoResponse", 
        BAD_CAST "urn:schemas-upnp-org:service:ConnectionManager:1");

        // RcsID
        xmlTextWriterStartElement(writer, BAD_CAST "RcsID");
        xmlTextWriterWriteString(writer, BAD_CAST "-1");
        xmlTextWriterEndElement(writer);

        // AVTransportID
        xmlTextWriterStartElement(writer, BAD_CAST "AVTransportID");
        xmlTextWriterWriteString(writer, BAD_CAST "-1");
        xmlTextWriterEndElement(writer);

        // ProtocolInfo
        xmlTextWriterStartElement(writer, BAD_CAST "ProtocolInfo");
        xmlTextWriterWriteString(writer, BAD_CAST "");
        xmlTextWriterEndElement(writer);

        // PeerConnectionManager
        xmlTextWriterStartElement(writer, BAD_CAST "PeerConnectionManager");
        xmlTextWriterWriteString(writer, BAD_CAST "");
        xmlTextWriterEndElement(writer);
                                
        // PeerConnectionID
        xmlTextWriterStartElement(writer, BAD_CAST "PeerConnectionID");
        xmlTextWriterWriteString(writer, BAD_CAST "-1");
        xmlTextWriterEndElement(writer);

        // Direction
        xmlTextWriterStartElement(writer, BAD_CAST "Direction");
        xmlTextWriterWriteString(writer, BAD_CAST "Output");
        xmlTextWriterEndElement(writer);

        // Status
        xmlTextWriterStartElement(writer, BAD_CAST "Status");
        xmlTextWriterWriteString(writer, BAD_CAST "Unknown");
        xmlTextWriterEndElement(writer);
                                
      // end GetCurrentConnectionInfo response
      xmlTextWriterEndElement(writer);
      
    // end body
    xmlTextWriterEndElement(writer);
   
	// end root
	xmlTextWriterEndElement(writer);
  xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);
	
  result = (const char*)buf->content;
	xmlBufferFree(buf);

  return ERR_NONE;
}

