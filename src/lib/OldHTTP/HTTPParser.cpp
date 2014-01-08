/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            HTTPParser.cpp
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

#include "HTTPParser.h"
#include "HTTPMessage.h"
#include "../Common/RegEx.h"
#include "../DeviceIdentification/IdentificationMgr.h"

using namespace std;

bool CHTTPParser::parseHeader(string header, CHTTPMessage* message)
{
	// header is already parsed
	if(message->m_nHTTPMessageType != HTTP_MESSAGE_TYPE_UNKNOWN) {
		return true;
	}

  // detect message type and HTTP version
  string sType;
  int nVersion;
  
  RegEx rxRequest("([GET|HEAD|POST|SUBSCRIBE|UNSUBSCRIBE|NOTIFY]+) +(.+) +HTTP/1\\.([1|0])", PCRE_CASELESS);
  RegEx rxResponse("HTTP/1\\.([1|0]) +(\\d+) +(.+)", PCRE_CASELESS);
 	
	// it's a request
  if(rxRequest.Search(header)) { 
    sType    = rxRequest.Match(1);
    nVersion = atoi(rxRequest.Match(3).c_str());
		message->m_sRequest = rxRequest.Match(2);
  }
	// it's a response
  else if(rxResponse.Search(header)) {
		sType    = rxResponse.Match(2);
		nVersion = atoi(rxResponse.Match(1).c_str());
		message->m_sRequest = rxResponse.Match(3);
	}
	else {    
		return false;
	}
  
	// set version
	if(nVersion == 0)
	  message->SetVersion(HTTP_VERSION_1_0);
	else if(nVersion == 1)
	  message->SetVersion(HTTP_VERSION_1_1);
	else {    
	  return false;
  }
    
  // set message type
  sType = ToUpper(sType);
	
  // GET
  if(sType.compare("GET") == 0) {
	  message->SetMessageType(HTTP_MESSAGE_TYPE_GET);
	}
  
	// HEAD
  else if(sType.compare("HEAD") == 0) {
	  message->SetMessageType(HTTP_MESSAGE_TYPE_HEAD);
  }
	
  // POST
	else if(sType.compare("POST") == 0) {
	  message->SetMessageType(HTTP_MESSAGE_TYPE_POST);
  }

  /* SUBSCRIBE|UNSUBSCRIBE */
  
  /* NOTIFY */
  
  // 200 OK
	else if(sType.compare("200") == 0) {
	  message->SetMessageType(HTTP_MESSAGE_TYPE_200_OK);
	}
	
	// 403 FORBIDDEN
	else if(sType.compare("403") == 0) {
	  message->SetMessageType(HTTP_MESSAGE_TYPE_403_FORBIDDEN);
	}
	
	// 404 NOT FOUND
	else if(sType.compare("404") == 0) {
	  message->SetMessageType(HTTP_MESSAGE_TYPE_404_NOT_FOUND);
	}

	
	/*RegEx rxContentLength("CONTENT-LENGTH: *(\\d+)", PCRE_CASELESS);
  if(rxContentLength.Search(header.c_str())) {
    string sContentLength = rxContentLength.Match(1);    
    int nContentLength = ::atoi(sContentLength.c_str());
  }
	else {
		return false;
	}*/
	
	parseCommonValues(header, message);
	parseGetVars(header, message);
  parseDlnaHeader(header, message);

  //CDeviceIdentificationMgr::Shared()->IdentifyDevice(message); 
  return true;
}

bool CHTTPParser::hasContentLength(char* buffer) 
{
	RegEx rxContentLength("CONTENT-LENGTH: *(\\d+)", PCRE_CASELESS);
  if(rxContentLength.Search(buffer)) {
    return true;
  }
	else {
		return false;
	}
}

fuppes_off_t CHTTPParser::getContentLength(char* buffer)
{
	RegEx rxContentLength("CONTENT-LENGTH: *(\\d+)", PCRE_CASELESS);
  if(rxContentLength.Search(buffer)) {
    fuppes_off_t contentLength = strToOffT(rxContentLength.Match(1));
		return contentLength;
  }
	else {
		return 0;
	}
}

void CHTTPParser::ConvertURLEncodeContentToPlain(CHTTPMessage* message)
{
  string sContentType;
	string sBoundary;
	
	RegEx rxContentType("CONTENT-TYPE: *([text/plain|application/x\\-www\\-form\\-urlencoded]+)(.*)", PCRE_CASELESS);
	if(rxContentType.Search(message->GetHeader().c_str())) {
	  sContentType = rxContentType.Match(1);
		if(rxContentType.SubStrings() == 3)
  		sBoundary = rxContentType.Match(2);
		
		if((ToLower(sContentType).compare("text/plain") == 0) && (ToLower(sBoundary).find("boundary") == string::npos))
		  return;
	}
	else {
	  return;
	}
	
	string sPost = message->GetContent();
	string sPart;
	stringstream sVars;

  RegEx rxValue("([\\w|_|-|\\d]+)=(.*)");
	
	while(sPost.length() > 0)
	{
	  if(sPost.find("&") != string::npos) {
		  sPart = sPost.substr(0, sPost.find("&"));
		  sPost = sPost.substr(sPost.find("&") + 1, sPost.length());
		}
		else {
		  sPart = sPost;
	    sPost = "";
		}
			
		if(rxValue.Search(sPart.c_str())) {
			sVars << rxValue.Match(1) << "=" << URLEncodeValueToPlain(rxValue.Match(2)) << "\r\n";
		}
		else {
			sVars << sPart << "\r\n";
		}	
	}
	
  message->SetContent(sVars.str());
}

void CHTTPParser::parseCommonValues(string header, CHTTPMessage* message)
{
  RegEx rxUserAgent("USER-AGENT: *(.*)\r\n", PCRE_CASELESS);
	if(rxUserAgent.Search(header.c_str())) {
		message->m_sUserAgent = rxUserAgent.Match(1);
	}

  RegEx rxTransferEncoding("Transfer-Encoding: *(\\w+)\r\n", PCRE_CASELESS);
	if(rxTransferEncoding.Search(header)) {
    string encoding = ToLower(rxTransferEncoding.Match(1));
    if(encoding == "chunked")
      message->m_nTransferEncoding = HTTP_TRANSFER_ENCODING_CHUNKED;
	}
  
}

void CHTTPParser::parseGetVars(string /*header*/, CHTTPMessage* message)
{
	// get vars already parsed
	if(!message->m_getVars.empty()) {
		return;
	}
	
	// check if there are get vars available
	size_t pos;
	if((pos = message->m_sRequest.find_first_of("?")) == string::npos) {
		return;
	}
	
	// parse the stuff
	string get;
	string key;
	string val;
	size_t amp;
	
	get = message->m_sRequest.substr(pos + 1, message->m_sRequest.length());
	get += "&";
	
	while(get.length() > 0) {
		
		key = get.substr(0, (pos = get.find_first_of("=")));
		val = get.substr(pos + 1, (amp = get.find_first_of("&")) - pos - 1);
	
		message->m_getVars[key] = val;
		
		get = get.substr(amp + 1, get.length());
	}
}

void CHTTPParser::parseDlnaHeader(string header, CHTTPMessage* message) // static
{

  RegEx rxGetCF("getcontentFeatures\\.dlna\\.org: *(\\d+) *\r\n", PCRE_CASELESS);
	if(rxGetCF.Search(header.c_str())) {
    if(rxGetCF.match(1).compare("1") == 0)
  		message->dlnaGetContentFeatures(true);
	}
  
  RegEx rxTM("transferMode\\.dlna\\.org: *(\\w+) *\r\n", PCRE_CASELESS);
	if(rxTM.Search(header.c_str())) {
    message->dlnaTransferMode(rxTM.match(1));
	}
  
  RegEx rxCF("contentFeatures\\.dlna\\.org: *(\\w+) *\r\n", PCRE_CASELESS);
	if(rxCF.Search(header.c_str())) {
    message->dlnaContentFeatures(rxCF.match(1));
  }

}

void CHTTPParser::parseSecHeader(string header, CHTTPMessage* message) // static
{
  RegEx rxGetCI("getCaptionInfo\\.sec: *(\\d+) *\r\n", PCRE_CASELESS);
	if(rxGetCI.Search(header.c_str())) {
    if(rxGetCI.match(1).compare("1") == 0)
  		message->m_secGetCaptionInfo = true;
	}
}
