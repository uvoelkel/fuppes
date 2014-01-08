/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            UPnPDevice.cpp
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#include "UPnPDevice.h"
#include "../OldHTTP/HTTPMessage.h"
#include "../Common/Common.h"
#include "../Common/RegEx.h"
#include "../SharedLog.h"
//#include "Security/MacAddressTable.h"

#include <sstream>
#include <libxml/xmlwriter.h>

/*#include <iostream>
#include <stdio.h>*/

using namespace std;

CUPnPDevice::CUPnPDevice(UPNP_DEVICE_TYPE nType, std::string p_sHTTPServerURL, IUPnPDevice* pEventHandler):
  CUPnPBase(nType, 1, p_sHTTPServerURL), m_timer(this)
{
  /* this constructor is for local devices only */
  m_bIsLocalDevice  = true;  
  m_pEventHandler   = pEventHandler;
  m_descriptionAvailable = true;
	m_pHTTPClient = NULL;
  m_deviceSettings = NULL;
  m_port = 0;
}


CUPnPDevice::CUPnPDevice(std::string localIpAddress, IUPnPDevice* pEventHandler, std::string p_sUUID):
  CUPnPBase(UPNP_DEVICE_UNKNOWN, 0, ""), m_timer(this)
{
  /* this constructor is for remote devices only */
    m_localIpAddress = localIpAddress;
  m_bIsLocalDevice  = false;
  m_pEventHandler   = pEventHandler;
  m_sUUID						= p_sUUID;
  m_descriptionAvailable = false;
	m_pHTTPClient = NULL;
  m_deviceSettings = NULL;
  m_port = 0;
}

CUPnPDevice::~CUPnPDevice()
{	
	m_timer.stop();
  m_mutex.lock();
	
  if(m_pHTTPClient) {
	  delete m_pHTTPClient;	
	}

  m_mutex.unlock();
}


void CUPnPDevice::OnTimer()
{
  Threading::MutexLocker locker(&m_mutex);

  //CSharedLog::Log(L_DBG, __FILE__, __LINE__, "OnTimer()");
  if(m_pEventHandler != NULL)
    m_pEventHandler->OnTimer(this);
}

void CUPnPDevice::OnAsyncReceiveMsg(CHTTPMessage* pMessage)
{
  Threading::MutexLocker locker(&m_mutex);

  // get the device's mac address
  //Security::MacAddressTable::mac(pMessage->GetRemoteIPAddress(), m_macAddress);
  // get the device settings from the message
  m_deviceSettings = NULL; //pMessage->DeviceSettings();
  // get the vfolder layout from the message
  m_vfolderLayout = ""; //pMessage->virtualFolderLayout();

  // parse the device description
  m_descriptionAvailable = ParseDescription(pMessage->GetContent());
 	if(m_descriptionAvailable) {
    GetTimer()->SetInterval(900);
		CSharedLog::Log(L_EXT, __FILE__, __LINE__, "new device %s", m_sFriendlyName.c_str());

     /*if(!m_macAddress.empty()) {
      // todo:
      // at this point we have the complete device description and the device's mac address
      // so let's make a suggestion which device setting to use.
      //cout << "FriendlyName: " << GetFriendlyName() << " MAC: " << m_macAddress << endl;
    }*/
	}

}


/* BuildFromDescriptionURL */
void CUPnPDevice::BuildFromDescriptionURL(std::string p_sDescriptionURL)
{	
  if(!m_pHTTPClient)
	  m_pHTTPClient = new CHTTPClient(m_localIpAddress, this, "CUPnPDevice");

  m_descriptionUrl = p_sDescriptionURL;
  SplitURL(m_descriptionUrl, &m_ipAddress, &m_port);
    
  
	if(m_pHTTPClient->AsyncGet(p_sDescriptionURL)) {
    stringstream url;
    url << "http://" << m_pHTTPClient->socket()->remoteAddress() << ":" << 
      m_pHTTPClient->socket()->remotePort() << "/";
    m_sHTTPServerURL = url.str();
  }
}

/* AddUPnPService */
void CUPnPDevice::AddUPnPService(UPnPService* pUPnPService)
{
	/* Add service to vector */
  m_vUPnPServices.push_back(pUPnPService);
}

/* GetUPnPServiceCount */
int CUPnPDevice::GetUPnPServiceCount()
{
	return (int)m_vUPnPServices.size();
}

/* GetUPnPService */
UPnPService* CUPnPDevice::GetUPnPService(int p_nIndex)
{
  if(p_nIndex < 0)
    return NULL;

  return m_vUPnPServices[p_nIndex];
}


std::string CUPnPDevice::localDeviceDescription(CDeviceSettings* deviceSettings)
{		
	xmlTextWriterPtr writer;
	xmlBufferPtr buf;
	
	buf    = xmlBufferCreate();   
	writer = xmlNewTextWriterMemory(buf, 0);    
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);

	/* root */
	xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "root", BAD_CAST "urn:schemas-upnp-org:device-1-0");
	
		/* specVersion */
		xmlTextWriterStartElement(writer, BAD_CAST "specVersion");
		
			/* major */
			xmlTextWriterStartElement(writer, BAD_CAST "major");  	
			xmlTextWriterWriteString(writer, BAD_CAST "1");
			xmlTextWriterEndElement(writer);		
			/* minor */
			xmlTextWriterStartElement(writer, BAD_CAST "minor");  	
			xmlTextWriterWriteString(writer, BAD_CAST "0");
			xmlTextWriterEndElement(writer);		
	
		/* end specVersion */
		xmlTextWriterEndElement(writer);

		// url base
		if(deviceSettings->MediaServerSettings()->UseURLBase) {
			string sUrl = "http://" + m_sHTTPServerURL + "/";
			xmlTextWriterStartElement(writer, BAD_CAST "URLBase");
			xmlTextWriterWriteString(writer, BAD_CAST sUrl.c_str());
			xmlTextWriterEndElement(writer);
		}

		// device
		xmlTextWriterStartElement(writer, BAD_CAST "device");

			// deviceType
			string sType = "urn:schemas-upnp-org:device:" + GetUPnPDeviceTypeAsString() + ":1";
			xmlTextWriterStartElement(writer, BAD_CAST "deviceType");
			xmlTextWriterWriteString(writer, BAD_CAST sType.c_str());			
      xmlTextWriterEndElement(writer);
  
			// UDN
			string sUDN = "uuid:" + m_sUUID;
			xmlTextWriterStartElement(writer, BAD_CAST "UDN");      
      xmlTextWriterWriteString(writer, BAD_CAST sUDN.c_str());
			xmlTextWriterEndElement(writer);
		  
			// friendlyName
			xmlTextWriterStartElement(writer, BAD_CAST "friendlyName");
			xmlTextWriterWriteString(writer, BAD_CAST deviceSettings->MediaServerSettings()->FriendlyName.c_str());
      xmlTextWriterEndElement(writer);
			
			// manufacturer
			xmlTextWriterStartElement(writer, BAD_CAST "manufacturer");
			xmlTextWriterWriteString(writer, BAD_CAST deviceSettings->MediaServerSettings()->Manufacturer.c_str());	
			xmlTextWriterEndElement(writer);
			
      // manufacturerURL
			xmlTextWriterStartElement(writer, BAD_CAST "manufacturerURL");
			xmlTextWriterWriteString(writer, BAD_CAST deviceSettings->MediaServerSettings()->ManufacturerURL.c_str());
			xmlTextWriterEndElement(writer);
			
			// modelName
			xmlTextWriterStartElement(writer, BAD_CAST "modelName");
			xmlTextWriterWriteString(writer, BAD_CAST deviceSettings->MediaServerSettings()->ModelName.c_str());
			xmlTextWriterEndElement(writer);
		
			// modelNumber
			xmlTextWriterStartElement(writer, BAD_CAST "modelNumber");
			xmlTextWriterWriteString(writer, BAD_CAST deviceSettings->MediaServerSettings()->ModelNumber.c_str());
			xmlTextWriterEndElement(writer);
      
			// modelURL
			xmlTextWriterStartElement(writer, BAD_CAST "modelURL");
			xmlTextWriterWriteString(writer, BAD_CAST deviceSettings->MediaServerSettings()->ModelURL.c_str());
			xmlTextWriterEndElement(writer);

      // modelDescription
			if(deviceSettings->MediaServerSettings()->UseModelDescription) {
				xmlTextWriterStartElement(writer, BAD_CAST "modelDescription");      
				xmlTextWriterWriteString(writer, BAD_CAST deviceSettings->MediaServerSettings()->ModelDescription.c_str());
				xmlTextWriterEndElement(writer);
			}
  
			// serialNumber
		  if(deviceSettings->MediaServerSettings()->UseSerialNumber) {
				xmlTextWriterStartElement(writer, BAD_CAST "serialNumber");
				xmlTextWriterWriteString(writer, BAD_CAST deviceSettings->MediaServerSettings()->SerialNumber.c_str());
				xmlTextWriterEndElement(writer);
			}
		
			// UPC
		  if(deviceSettings->MediaServerSettings()->UseUPC) {
				xmlTextWriterStartElement(writer, BAD_CAST "UPC");
				xmlTextWriterWriteString(writer, BAD_CAST deviceSettings->MediaServerSettings()->UPC.c_str());
				xmlTextWriterEndElement(writer);
			}

      // dlna volume 1, 7.2.10
  
			// DLNA 1.0
		  if(deviceSettings->MediaServerSettings()->DlnaVersion == CMediaServerSettings::dlna_1_0) {
				xmlTextWriterStartElementNS(writer, BAD_CAST "dlna", BAD_CAST "X_DLNADOC", BAD_CAST "urn:schemas-dlna-org:device-1-0");
      	xmlTextWriterWriteString(writer, BAD_CAST "DMS-1.00");
				xmlTextWriterEndElement(writer);
			}
 			// DLNA 1.5
		  else if(deviceSettings->MediaServerSettings()->DlnaVersion == CMediaServerSettings::dlna_1_5) {
				xmlTextWriterStartElementNS(writer, BAD_CAST "dlna", BAD_CAST "X_DLNADOC", BAD_CAST "urn:schemas-dlna-org:device-1-0");
      	xmlTextWriterWriteString(writer, BAD_CAST "DMS-1.50");
				xmlTextWriterEndElement(writer);

        /*xmlTextWriterStartElementNS(writer, BAD_CAST "dlna", BAD_CAST "X_DLNADOC", BAD_CAST "urn:schemas-dlna-org:device-1-0");
      	xmlTextWriterWriteString(writer, BAD_CAST "M-DMS-1.50");
				xmlTextWriterEndElement(writer);*/
        //<dlna:X_DLNACAP xmlns:dlna="urn:schemas-dlna-org:device-1-0">av-upload,image-upload,audio-upload</dlna:X_DLNACAP>
			}
  
      // presentationURL
      xmlTextWriterStartElement(writer, BAD_CAST "presentationURL");
      xmlTextWriterWriteString(writer, BAD_CAST m_sPresentationURL.c_str());
			xmlTextWriterEndElement(writer);

      // iconList
      if(deviceSettings->EnableDeviceIcon()) {
  
				xmlTextWriterStartElement(writer, BAD_CAST "iconList");
        
          // 48x48 png
					xmlTextWriterStartElement(writer, BAD_CAST "icon");
						xmlTextWriterStartElement(writer, BAD_CAST "mimetype");
						xmlTextWriterWriteString(writer, BAD_CAST "image/png");
						xmlTextWriterEndElement(writer);
						xmlTextWriterStartElement(writer, BAD_CAST "width");
						xmlTextWriterWriteString(writer, BAD_CAST "48");
						xmlTextWriterEndElement(writer);
						xmlTextWriterStartElement(writer, BAD_CAST "height");
						xmlTextWriterWriteString(writer, BAD_CAST "48");
						xmlTextWriterEndElement(writer);
						xmlTextWriterStartElement(writer, BAD_CAST "depth");
						xmlTextWriterWriteString(writer, BAD_CAST "24");
						xmlTextWriterEndElement(writer);        
						xmlTextWriterStartElement(writer, BAD_CAST "url");
						xmlTextWriterWriteString(writer, BAD_CAST "/presentation/fuppes-icon-48x48.png");
						xmlTextWriterEndElement(writer);
					xmlTextWriterEndElement(writer);

          // 120x120 png
					xmlTextWriterStartElement(writer, BAD_CAST "icon");
						xmlTextWriterStartElement(writer, BAD_CAST "mimetype");
						xmlTextWriterWriteString(writer, BAD_CAST "image/png");
						xmlTextWriterEndElement(writer);
						xmlTextWriterStartElement(writer, BAD_CAST "width");
						xmlTextWriterWriteString(writer, BAD_CAST "120");
						xmlTextWriterEndElement(writer);
						xmlTextWriterStartElement(writer, BAD_CAST "height");
						xmlTextWriterWriteString(writer, BAD_CAST "120");
						xmlTextWriterEndElement(writer);
						xmlTextWriterStartElement(writer, BAD_CAST "depth");
						xmlTextWriterWriteString(writer, BAD_CAST "24");
						xmlTextWriterEndElement(writer);        
						xmlTextWriterStartElement(writer, BAD_CAST "url");
						xmlTextWriterWriteString(writer, BAD_CAST "/presentation/fuppes-icon-120x120.png");
						xmlTextWriterEndElement(writer);
					xmlTextWriterEndElement(writer);
        
          // 48x48 jpeg
					xmlTextWriterStartElement(writer, BAD_CAST "icon");
						xmlTextWriterStartElement(writer, BAD_CAST "mimetype");
						xmlTextWriterWriteString(writer, BAD_CAST "image/jpeg");
						xmlTextWriterEndElement(writer);
						xmlTextWriterStartElement(writer, BAD_CAST "width");
						xmlTextWriterWriteString(writer, BAD_CAST "48");
						xmlTextWriterEndElement(writer);
						xmlTextWriterStartElement(writer, BAD_CAST "height");
						xmlTextWriterWriteString(writer, BAD_CAST "48");
						xmlTextWriterEndElement(writer);
						xmlTextWriterStartElement(writer, BAD_CAST "depth");
						xmlTextWriterWriteString(writer, BAD_CAST "24");
						xmlTextWriterEndElement(writer);        
						xmlTextWriterStartElement(writer, BAD_CAST "url");
						xmlTextWriterWriteString(writer, BAD_CAST "/presentation/fuppes-icon-48x48.jpg");
						xmlTextWriterEndElement(writer);
					xmlTextWriterEndElement(writer);

          // 120x120 jpeg
					xmlTextWriterStartElement(writer, BAD_CAST "icon");
						xmlTextWriterStartElement(writer, BAD_CAST "mimetype");
						xmlTextWriterWriteString(writer, BAD_CAST "image/jpeg");
						xmlTextWriterEndElement(writer);
						xmlTextWriterStartElement(writer, BAD_CAST "width");
						xmlTextWriterWriteString(writer, BAD_CAST "120");
						xmlTextWriterEndElement(writer);
						xmlTextWriterStartElement(writer, BAD_CAST "height");
						xmlTextWriterWriteString(writer, BAD_CAST "120");
						xmlTextWriterEndElement(writer);
						xmlTextWriterStartElement(writer, BAD_CAST "depth");
						xmlTextWriterWriteString(writer, BAD_CAST "24");
						xmlTextWriterEndElement(writer);        
						xmlTextWriterStartElement(writer, BAD_CAST "url");
						xmlTextWriterWriteString(writer, BAD_CAST "/presentation/fuppes-icon-120x120.jpg");
						xmlTextWriterEndElement(writer);
					xmlTextWriterEndElement(writer);
        
				xmlTextWriterEndElement(writer);
        
      } // icon list
  
  
		  // serviceList
			UPnPService* pTmp;			
			xmlTextWriterStartElement(writer, BAD_CAST "serviceList");			
      for(unsigned int i = 0; i < m_vUPnPServices.size(); i++)
			{
				pTmp = m_vUPnPServices[i];				
				

			  if(pTmp->GetUPnPDeviceType() == UPNP_SERVICE_X_MS_MEDIA_RECEIVER_REGISTRAR)
				{
				  if(!deviceSettings->MediaServerSettings()->UseXMSMediaReceiverRegistrar)
			      continue;
				
				  stringstream sDescription;
					sDescription << 
						"<service>" <<
						"<serviceType>urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1</serviceType>" <<
						"<serviceId>urn:microsoft.com:serviceId:X_MS_MediaReceiverRegistrar</serviceId>" <<
						"<SCPDURL>/UPnPServices/" << pTmp->GetUPnPDeviceTypeAsString() << "/description.xml</SCPDURL>" <<
						"<controlURL>/UPnPServices/" << pTmp->GetUPnPDeviceTypeAsString() << "/control/</controlURL>" <<
						"<eventSubURL>/UPnPServices/" << pTmp->GetUPnPDeviceTypeAsString() << "/event/</eventSubURL>" <<
						"</service>";
												
					xmlTextWriterWriteRaw(writer, BAD_CAST sDescription.str().c_str());
					continue;
				}


				/* service */
				xmlTextWriterStartElement(writer, BAD_CAST "service");
				
					// serviceType
					string sTmp = "urn:schemas-upnp-org:service:" + pTmp->GetUPnPDeviceTypeAsString() + ":1";
					xmlTextWriterStartElement(writer, BAD_CAST "serviceType");
      		xmlTextWriterWriteString(writer, BAD_CAST sTmp.c_str());
					xmlTextWriterEndElement(writer);
        
					// serviceId
					sTmp = "urn:upnp-org:serviceId:" + pTmp->GetUPnPDeviceTypeAsString();
					xmlTextWriterStartElement(writer, BAD_CAST "serviceId");
      		xmlTextWriterWriteString(writer, BAD_CAST sTmp.c_str());					
					xmlTextWriterEndElement(writer);
				
					// SCPDURL
					sTmp = "/UPnPServices/" + pTmp->GetUPnPDeviceTypeAsString() + "/description.xml";
					xmlTextWriterStartElement(writer, BAD_CAST "SCPDURL");
      		xmlTextWriterWriteString(writer, BAD_CAST sTmp.c_str());					
					xmlTextWriterEndElement(writer);					

					// controlURL
					sTmp = "/UPnPServices/" + pTmp->GetUPnPDeviceTypeAsString() + "/control/";
					xmlTextWriterStartElement(writer, BAD_CAST "controlURL");
      		xmlTextWriterWriteString(writer, BAD_CAST sTmp.c_str());					
					xmlTextWriterEndElement(writer);

					// eventSubURL
					sTmp = "/UPnPServices/" + pTmp->GetUPnPDeviceTypeAsString() + "/event/";
					xmlTextWriterStartElement(writer, BAD_CAST "eventSubURL");
      		xmlTextWriterWriteString(writer, BAD_CAST sTmp.c_str());					
					xmlTextWriterEndElement(writer);
				
				/* end service */
				xmlTextWriterEndElement(writer);				
			}

      // fuppes soap control
      stringstream sDescription;
			sDescription << 
				"<service>" <<
				  "<serviceType>urn:fuppes:service:SoapControl:1</serviceType>" <<
				  "<serviceId>urn:fuppes:serviceId:SoapControl</serviceId>" <<
				  "<SCPDURL>/UPnPServices/SoapControl/description.xml</SCPDURL>" <<
				  "<controlURL>/UPnPServices/SoapControl/control/</controlURL>" <<
			  	"<eventSubURL>/UPnPServices/SoapControl/event/</eventSubURL>" <<
				"</service>";
			xmlTextWriterWriteRaw(writer, BAD_CAST sDescription.str().c_str());
  
			
			/* end serviceList */
			xmlTextWriterEndElement(writer);
      
		/* end device */
		xmlTextWriterEndElement(writer);

	/* end root */
	xmlTextWriterEndElement(writer);	
	xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);
	
	string output;
	output = (const char*)buf->content;
	
	xmlBufferFree(buf);
	
	//cout << "upnp description: " << output.str() << endl;
	return output;
}


xmlNode* FindNode(std::string p_sNodeName, xmlNode* pParentNode = NULL, bool p_bWalkSubnodes = false)
{
  if(!pParentNode || !pParentNode->children)
	  return NULL;

	xmlNode* pTmpNode = NULL;
	xmlNode* pSubNode = NULL;
	string   sNodeName;
	
	pTmpNode = pParentNode->children;
	do {
	  sNodeName = (char*)pTmpNode->name;
		//cout << "search: " << sNodeName << endl;
		
		if(ToLower(sNodeName).compare(ToLower(p_sNodeName)) == 0) {
		  return pTmpNode;
		}
		
		if(p_bWalkSubnodes && pTmpNode->children) {
		  pSubNode = FindNode(p_sNodeName, pTmpNode, true);
			if(pSubNode)
			  return pSubNode;
		}
		
	  pTmpNode = pTmpNode->next;
	}
	while(pTmpNode);
	
	
	return NULL;
}

/* ParseDescription */
bool CUPnPDevice::ParseDescription(std::string p_sDescription)
{
  //cout << __FILE__ << " parse: " << p_sDescription << endl;
	
  xmlDocPtr pDoc = NULL;
  pDoc = xmlReadMemory(p_sDescription.c_str(), p_sDescription.length(), "", NULL, XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NONET);
  if(!pDoc) {
    CSharedLog::Log(L_DBG, __FILE__, __LINE__, "xml parser error");
    return false;    
  }


  bool result = true;
  
  xmlNode* pRootNode = NULL;  
  xmlNode* pTmpNode  = NULL;   
  pRootNode = xmlDocGetRootElement(pDoc);  
  
	// friendlyName
	pTmpNode = FindNode("friendlyName", pRootNode, true);
	if(pTmpNode && pTmpNode->children) {
	  m_sFriendlyName = (char*)pTmpNode->children->content;
  }
  else {
    result = false;
  }
  
	// UDN
	pTmpNode = FindNode("UDN", pRootNode, true);
	if(pTmpNode && pTmpNode->children) {
	  //m_sUUID = 
	}
	
	// deviceType
	pTmpNode = FindNode("deviceType", pRootNode, true);
	if(pTmpNode && pTmpNode->children) {
	  string sDevType = ToLower((char*)pTmpNode->children->content);

    size_t pos = sDevType.find_last_of(":");    
    if(pos != string::npos) {
      string tmp = sDevType.substr(pos + 1, sDevType.length());
      sDevType = sDevType.substr(0, pos);
      m_UPnPDeviceVersion = atoi(tmp.c_str());
    }
    
    if(sDevType.compare("urn:schemas-upnp-org:device:mediarenderer") == 0)    
      m_nUPnPDeviceType = UPNP_DEVICE_MEDIA_RENDERER;
    else if(sDevType.compare("urn:schemas-upnp-org:device:mediaserver") == 0)    
      m_nUPnPDeviceType = UPNP_DEVICE_MEDIA_SERVER;
	}
  else {
    result = false;
  }
		
			  
				
	// presentationURL
	pTmpNode = FindNode("presentationURL", pRootNode, true);
	if(pTmpNode && pTmpNode->children) {
	  m_sPresentationURL = (char*)pTmpNode->children->content;
	}
				
	// manufacturer
	pTmpNode = FindNode("manufacturer", pRootNode, true);
	if(pTmpNode && pTmpNode->children) {
	  m_sManufacturer = (char*)pTmpNode->children->content;
	}			
				
	// manufacturerURL
	pTmpNode = FindNode("manufacturerURL", pRootNode, true);
	if(pTmpNode && pTmpNode->children) {
	  m_sManufacturerURL = (char*)pTmpNode->children->content;
	}

  // iconList
  pTmpNode = FindNode("iconList", pRootNode, true);
	if(pTmpNode) {

    xmlNode* childNode = pTmpNode->children;
    while(childNode) {

      UPnPDeviceIcon icon;

      if((pTmpNode = FindNode("mimetype", childNode, false)) != NULL)
        icon.mimeType = (char*)pTmpNode->children->content;

      if((pTmpNode = FindNode("width", childNode, false)) != NULL)
        icon.width = atoi((char*)pTmpNode->children->content);

      if((pTmpNode = FindNode("height", childNode, false)) != NULL)
        icon.height = atoi((char*)pTmpNode->children->content);

      if((pTmpNode = FindNode("depth", childNode, false)) != NULL)
        icon.depth = atoi((char*)pTmpNode->children->content);
      
      if((pTmpNode = FindNode("url", childNode, false)) != NULL)
        icon.url = (char*)pTmpNode->children->content;

      m_iconList.push_back(icon);
      
      childNode = childNode->next;
    }
	}
  

  
  xmlFreeDoc(pDoc);  

  /*#warning todo: uuid
  RegEx rxUUID("<UDN>uuid:(.+)</UDN>", PCRE_CASELESS);
  if(rxUUID.Search(p_sDescription.c_str())) {
    //m_sUUID = rxUUID.Match(1);
  }*/

  return result;
}
