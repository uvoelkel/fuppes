/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            HTTPMessage.cpp
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

#include "HTTPMessage.h"
#include "../Common/Common.h"
#include "../SharedLog.h"
#include "HTTPParser.h"

#include <iostream>
#include <sstream>
#include <time.h>

#include "../Common/RegEx.h"
//#include "../UPnPActions/UPnPActionFactory.h"

using namespace std;

const string LOGNAME = "HTTPMessage";

CHTTPMessage::CHTTPMessage()
{
  // Init
  m_nHTTPVersion			  = HTTP_VERSION_UNKNOWN;
  m_nHTTPMessageType    = HTTP_MESSAGE_TYPE_UNKNOWN;
	m_sHTTPContentType    = "";
  m_nBinContentLength   = 0; 
  m_nBinContentPosition = 0;
  m_nBufferSize         = 0;
  m_nContentLength      = 0;  
  m_sBuffer             = NULL; 
  m_nBytesConsumed      = 0;
  m_bIsBinary           = false;
  m_nRangeStart         = 0;
  m_nRangeEnd           = 0;
	m_hasRange						= false;
  m_nHTTPConnection     = HTTP_CONNECTION_UNKNOWN;
  //m_pUPnPAction         = NULL;
	//m_pDeviceSettings			= NULL;
  /*#ifndef DISABLE_TRANSCODING
  m_pTranscodingSessionInfo = NULL;  
  m_pTranscodingCacheObj = NULL;
  #endif*/
  m_nTransferEncoding    = HTTP_TRANSFER_ENCODING_NONE;

  m_dlnaGetContentFeatures = false;

  m_secGetCaptionInfo = false;
  
	memset(&m_LocalEp, 0, sizeof(struct sockaddr_in));
	memset(&m_RemoteEp, 0, sizeof(struct sockaddr_in));
}

CHTTPMessage::~CHTTPMessage()
{
  // Cleanup
  /*if(m_pUPnPAction)
    delete m_pUPnPAction;*/
  
  if(m_sBuffer)
    free(m_sBuffer);

  /*
  #ifndef DISABLE_TRANSCODING
  if(m_pTranscodingCacheObj) {
    //CTranscodingCache::Shared()->ReleaseCacheObject(m_pTranscodingCacheObj);
  }    

  if(m_pTranscodingSessionInfo) {    
    delete m_pTranscodingSessionInfo;
  }  
  #endif
*/

  /*if(m_fsFile.is_open())
    m_fsFile.close();*/
	if(m_file.isOpen())
		m_file.close();
}

void CHTTPMessage::SetMessage(HTTP_MESSAGE_TYPE nMsgType, string p_sContentType)
{
	m_sMessage = "";  
  m_nHTTPMessageType  = nMsgType;
	m_sHTTPContentType  = p_sContentType;
  m_nBinContentLength = 0;
}

bool CHTTPMessage::SetMessage(string p_sMessage)
{
	m_sMessage = p_sMessage;
	string::size_type nPos = m_sMessage.find("\r\n\r\n");  
	if(nPos != string::npos) {	
		m_sHeader  = m_sMessage.substr(0, nPos + strlen("\r\n"));
		m_sContent = m_sMessage.substr(nPos + 4, m_sMessage.length() - nPos - 4);
	}
	else {
		//return false;
	}  
	
  CHTTPParser::parseHeader(m_sHeader, this);  
  return BuildFromString(p_sMessage);
}

bool CHTTPMessage::SetHeader(string p_sHeader)
{
  // header is already set
  if(m_sHeader.length() > 0)
    return true;

	m_sHeader = p_sHeader;
  return CHTTPParser::parseHeader(m_sHeader, this);
}

void CHTTPMessage::SetBinContent(char* p_szBinContent, fuppes_off_t p_nBinContenLength)
{ 
  m_bIsBinary = true;

  m_nBinContentLength = p_nBinContenLength;
  m_nBufferSize       = m_nBinContentLength;
  m_sBuffer           = (char*)malloc(sizeof(char) * m_nBufferSize);    
  memcpy(m_sBuffer, p_szBinContent, m_nBufferSize); 
  m_nBytesConsumed    = m_nBufferSize;
}

/*
CUPnPAction* CHTTPMessage::GetAction()
{
  if(!m_pUPnPAction) {
    m_pUPnPAction = CUPnPActionFactory::buildActionFromString(m_sContent, m_pDeviceSettings, m_virtualFolderLayout);
  }
  return m_pUPnPAction;
}
*/

string CHTTPMessage::GetHeaderAsString()
{
	stringstream sResult;
	string sVersion;
	string sType;
	string sContentType;
	  
  /* Version */
	switch(m_nHTTPVersion)
	{
		case HTTP_VERSION_1_0: sVersion = "HTTP/1.0"; break;
		case HTTP_VERSION_1_1: sVersion = "HTTP/1.1"; break;
    default:               ASSERT(0);             break;
  }
	
  /* Message Type */
	switch(m_nHTTPMessageType)
	{
		case HTTP_MESSAGE_TYPE_GET:	          /* todo */			                            break;
    case HTTP_MESSAGE_TYPE_HEAD:	        /* todo */			                            break;
		case HTTP_MESSAGE_TYPE_POST:          /* todo */		                              break;
		case HTTP_MESSAGE_TYPE_200_OK:
      sResult << sVersion << " 200 OK\r\n";
      break;
    case HTTP_MESSAGE_TYPE_206_PARTIAL_CONTENT:
      sResult << sVersion << " 206 Partial Content\r\n";
      break;
    case HTTP_MESSAGE_TYPE_400_BAD_REQUEST:
      sResult << sVersion << " 400 Bad Request\r\n";
      break;
    case HTTP_MESSAGE_TYPE_403_FORBIDDEN:
      sResult << sVersion << " 403 Forbidden\r\n";
      break;
	  case HTTP_MESSAGE_TYPE_404_NOT_FOUND:
      sResult << sVersion << " 404 Not Found\r\n";
      break;
	  case HTTP_MESSAGE_TYPE_500_INTERNAL_SERVER_ERROR:
      sResult << sVersion << " " << "500 Internal Server Error\r\n";
      break;
    
    /* GENA */
    case HTTP_MESSAGE_TYPE_GENA_OK:
      sResult << sVersion << " 200 OK\r\n";
      break;    
      
    default:
      CSharedLog::Log(L_DBG, __FILE__, __LINE__, "GetHeaderAsString() :: unhandled message type");
      assert(0);
      break;
	}		  
  
  

	if(m_nHTTPMessageType != HTTP_MESSAGE_TYPE_GENA_OK)
  {
    /* Content Type */
    sResult << "Content-Type: " << m_sHTTPContentType << "\r\n";
    
    /* Content length */
    
    // if it's a non binary file give the length of m_sContent
    if(!m_bIsBinary) {
      sResult << "Content-Length: " << (int)strlen(m_sContent.c_str()) << "\r\n";
    }
    // otherwise calc length
    else
    { 
      // transcoding responses don't contain content length
      if(//!this->IsTranscoding() &&
         (m_nBinContentLength > 0) &&
         (m_nTransferEncoding != HTTP_TRANSFER_ENCODING_CHUNKED))
      {      
        // ranges
        if(m_nHTTPMessageType == HTTP_MESSAGE_TYPE_206_PARTIAL_CONTENT) // (m_nRangeStart > 0) || (m_nRangeEnd > 0))
        {
          if(m_nRangeEnd < m_nBinContentLength) {
            sResult << "Content-Length: " << m_nRangeEnd - m_nRangeStart + 1 << "\r\n";
            sResult << "Content-Range: bytes " << m_nRangeStart << "-" << m_nRangeEnd << "/" << m_nBinContentLength << "\r\n";
          }
          else {
            sResult << "Content-Length: " << m_nBinContentLength - m_nRangeStart << "\r\n";
            sResult << "Content-Range: bytes " << m_nRangeStart << "-" << m_nBinContentLength - 1 << "/" << m_nBinContentLength << "\r\n";
          }
        }
        // complete
        else {
          sResult << "Content-Length: " << m_nBinContentLength << "\r\n";
        }
      }
      /*
      #ifndef DISABLE_TRANSCODING
      // transcoding
      else if(this->IsTranscoding() && (m_nTransferEncoding != HTTP_TRANSFER_ENCODING_CHUNKED)) {
        
        if((m_nRangeStart > 0) || (m_nRangeEnd > 0)) {
          
          // id3 v1 request
          if((m_nRangeEnd - m_nRangeStart) == 127) {            
            sResult << "Content-Length: " << m_pTranscodingSessionInfo->m_nGuessContentLength - m_nRangeStart << "\r\n";
            sResult << "Content-Range: bytes " << m_nRangeStart << "-" << m_pTranscodingSessionInfo->m_nGuessContentLength - 1 << "/" << m_pTranscodingSessionInfo->m_nGuessContentLength << "\r\n";
          }
          else if(m_nRangeEnd < m_pTranscodingSessionInfo->m_nGuessContentLength) {
            sResult << "Content-Length: " << m_nRangeEnd - m_nRangeStart + 1 << "\r\n";
            sResult << "Content-Range: bytes " << m_nRangeStart << "-" << m_nRangeEnd << "/" << m_pTranscodingSessionInfo->m_nGuessContentLength << "\r\n";
          }
          else {
            sResult << "Content-Length: " << m_pTranscodingSessionInfo->m_nGuessContentLength - m_nRangeStart << "\r\n";
            sResult << "Content-Range: bytes " << m_nRangeStart << "-" << m_pTranscodingSessionInfo->m_nGuessContentLength - 1 << "/" << m_pTranscodingSessionInfo->m_nGuessContentLength << "\r\n";
          }  
          
          
        }
        else {
                    
          if(m_pTranscodingSessionInfo->m_nGuessContentLength > 0) {
            sResult << "Content-Length: " << m_pTranscodingSessionInfo->m_nGuessContentLength << "\r\n";
          }
          //else {
           // unsigned int nSize = -1;
           // sResult << "Content-Length: " << nSize << "\r\n";
          //}
          
        }
        

      }
      #endif // DISABLE_TRANSCODING
      */
      
    } // if(m_bIsBinary)
    /* end Content length */        
    
    
    switch(m_nTransferEncoding) {
      case HTTP_TRANSFER_ENCODING_NONE:
        break;
      case HTTP_TRANSFER_ENCODING_CHUNKED:
        sResult << "Transfer-Encoding: chunked\r\n";
        break;      
    }
    
    // Accept-Ranges
   if( /*!this->IsTranscoding() && */ (m_nTransferEncoding != HTTP_TRANSFER_ENCODING_CHUNKED)) {
      sResult << "Accept-Ranges: bytes\r\n";
    }
    else {
      sResult << "Accept-Ranges: none\r\n";
    }

		// cache
    sResult << "Pragma: no-cache\r\n";
    sResult << "Cache-control: no-cache\r\n";
		
    // connection
    sResult << "Connection: close\r\n";
	
    // date
    char   szTime[30];
    time_t tTime = time(NULL);
    strftime(szTime, 30,"%a, %d %b %Y %H:%M:%S GMT" , gmtime(&tTime));   
  	sResult << "DATE: " << szTime << "\r\n";	
	
	  // dlna
    if(m_dlnaGetContentFeatures)
      sResult << "getcontentFeatures.dlna.org: 1\r\n";
    if(!m_dlnaContentFeatures.empty())
      sResult << "contentFeatures.dlna.org: " << m_dlnaContentFeatures << "\r\n";
    if(!m_dlnaTransferMode.empty())
      sResult << "transferMode.dlna.org: " << m_dlnaTransferMode << "\r\n";

    // ext
    sResult << "EXT:\r\n";    
  }
  
  
  /* GENA header information */
  else if(m_nHTTPMessageType == HTTP_MESSAGE_TYPE_GENA_OK)
  {
    // subscription or renew    
    if(m_sGENASubscriptionID.length() > 0) { 
      sResult << "SID: uuid:" << m_sGENASubscriptionID << "\r\n";
      sResult << "TIMEOUT: Second-" << 180 << "\r\n";
    }
	  sResult << "Content-Length: 0\r\n";
  }
  
 	// server signature
  /*sResult <<
    "Server: " << CSharedConfig::Shared()->GetOSName() << "/" << CSharedConfig::Shared()->GetOSVersion() << ", " <<
    "UPnP/1.0, " << CSharedConfig::Shared()->GetAppName() << "/" << CSharedConfig::Shared()->GetAppVersion() << "\r\n";*/
	
	sResult << "\r\n";
  
	return sResult.str();
}

string CHTTPMessage::GetMessageAsString()
{
  stringstream sResult;
  sResult << GetHeaderAsString();
  sResult << m_sContent;
  return sResult.str();
}

fuppes_off_t CHTTPMessage::GetBinContentLength()
{
    /*
  #ifndef DISABLE_TRANSCODING
  if(m_pTranscodingSessionInfo)
    return m_pTranscodingSessionInfo->m_nGuessContentLength;  
  else 
  #endif
  */
    return m_nBinContentLength;
  
}

unsigned int CHTTPMessage::GetBinContentChunk(char* p_sContentChunk, unsigned int p_nSize, fuppes_off_t p_nOffset)
{
    /*
}
  // read from file
  #ifndef DISABLE_TRANSCODING
  //if(m_pTranscodingSessionInfo == NULL && m_fsFile.is_open()) {
	if(m_pTranscodingSessionInfo == NULL && m_file.isOpen()) {        
  #else
  */
  //if(m_fsFile.is_open()) {
	if(m_file.isOpen()) {
  //#endif
    
    if((p_nOffset > 0) && (p_nOffset != m_nBinContentPosition)) {
      //m_fsFile.seekg(p_nOffset, ios::beg);
			m_file.seek(p_nOffset);
      m_nBinContentPosition = p_nOffset;
    }
      
    fuppes_off_t nRead = 0;
    if((m_nBinContentLength - p_nOffset) < p_nSize)
      nRead = m_nBinContentLength - p_nOffset;
    else
      nRead = p_nSize;

    //m_fsFile.read(p_sContentChunk, nRead);   
		m_file.read(p_sContentChunk, nRead);
      
    m_nBinContentPosition += nRead;
    m_nBytesConsumed  = nRead;
    return nRead;
  }
  
  // read (transcoded) data from memory
  else
  { 
    /*#ifndef DISABLE_TRANSCODING
    bool bTranscode = (m_pTranscodingSessionInfo != NULL);
    #else*/
    //bool bTranscode = false;
    //#endif
    
    /*
    #ifndef DISABLE_TRANSCODING
    // id3v1 request
    if(bTranscode && m_pTranscodingCacheObj->IsMp3Encoding() &&
       (m_pTranscodingSessionInfo->m_nGuessContentLength > 0) && 
       (m_pTranscodingSessionInfo->m_nGuessContentLength - p_nOffset) == 128) {
             
      m_pTranscodingCacheObj->GetId3v1(p_sContentChunk);
      return 128;      
    }
    #endif
    */
    
    unsigned int nRest = 0;
    

   /* #ifndef DISABLE_TRANSCODING
     unsigned int nDelayCount = 0;
    if(bTranscode) {
      
      if(p_nOffset > 0 && p_nOffset != m_nBinContentPosition) {

				if(m_pTranscodingCacheObj->m_bIsComplete) {
					// offset groesser als verfuegbare daten
					if(p_nOffset > m_pTranscodingCacheObj->GetValidBytes()) {
						return 0;
					}
				}
				else {
					// offset groesser als geschaetzte laenge
					if(p_nOffset >= m_pTranscodingSessionInfo->m_nGuessContentLength) {
						return 0;
					}
				}

				m_nBinContentPosition = p_nOffset;
      }
     
      if(m_pTranscodingCacheObj->GetValidBytes() > m_nBinContentPosition) {
        nRest = m_pTranscodingCacheObj->GetValidBytes() - m_nBinContentPosition; 
      }
      else {
        nRest = 0;
      }
       
    }
    else
      nRest = m_nBinContentLength - m_nBinContentPosition;
    #else */
      nRest = m_nBinContentLength - m_nBinContentPosition;
  //  #endif
    
   /*
    #ifndef DISABLE_TRANSCODING
    while(bTranscode && 
          !m_pTranscodingCacheObj->m_bIsComplete && 
          (nRest < p_nSize) && 
          !m_pTranscodingSessionInfo->m_bBreakTranscoding)
    { 
      if(m_pTranscodingCacheObj->GetValidBytes() > m_nBinContentPosition) {
        nRest = m_pTranscodingCacheObj->GetValidBytes() - m_nBinContentPosition; 
      }
      else {
        nRest = 0;
      }
      
      stringstream sLog;
      sLog << "we are sending faster then we can transcode!" << endl;
      sLog << "  try     : " << (nDelayCount + 1) << "/20" << endl;
      sLog << "  length  : " << m_pTranscodingCacheObj->GetValidBytes() << endl;
      sLog << "  position: " << m_nBinContentPosition << endl;
      sLog << "  size    : " << p_nSize << endl;
      sLog << "  rest    : " << nRest << endl;
      sLog << "delaying send-process!";
      
      CSharedLog::Shared()->Log(L_DBG, __FILE__, __LINE__,  sLog.str().c_str());
                                   
      fuppesSleep(500);
                              
      nDelayCount++;
      
      // if bufer is still empty after 10 seconds
      //   the machine seems to be too slow. so
      //   we give up
      if (nDelayCount == 20) { // 20 * 500ms = 10sec
        BreakTranscoding();                                  
        return 0;
      }
    }

    
    if(bTranscode) {
      nRest = m_pTranscodingCacheObj->GetValidBytes() - m_nBinContentPosition;
    }
    else {
      nRest = m_nBinContentLength - m_nBinContentPosition;         
    }
    #else */
    nRest = m_nBinContentLength - m_nBinContentPosition;         
    //#endif
      
   
    if(nRest > p_nSize) {
      nRest = p_nSize;
    }
      
    /*#ifndef DISABLE_TRANSCODING
    if(bTranscode && m_pTranscodingCacheObj->TranscodeToFile()) {   
    
      fstream fsTmp;        
      fsTmp.open(m_pTranscodingCacheObj->m_sOutFileName.c_str(), ios::binary|ios::in);
      if(fsTmp.fail() != 1) { 
        fsTmp.seekg(m_nBinContentPosition, ios::beg);   
          
        fsTmp.read(p_sContentChunk, nRest);
        fsTmp.close();

        m_nBinContentPosition += nRest;
        return nRest;
      }
      else {
          return 0;
      }
    }
    else {   
    #endif    */
      
    /*
      #ifndef DISABLE_TRANSCODING
      if(bTranscode) {        
        m_pTranscodingCacheObj->Lock();        
        memcpy(p_sContentChunk, &m_pTranscodingCacheObj->m_sBuffer[m_nBinContentPosition], nRest);
        m_pTranscodingCacheObj->Unlock();
      }
      else
      #endif
      */
        memcpy(p_sContentChunk, &m_sBuffer[m_nBinContentPosition], nRest);
      
      
      m_nBinContentPosition += nRest; 
      return nRest;
   /* #ifndef DISABLE_TRANSCODING
    }
    #endif */

  }
  return 0;
}


bool CHTTPMessage::PostVarExists(string p_sPostVarName)
{
  if (m_nHTTPMessageType != HTTP_MESSAGE_TYPE_POST)
   return false;
  
  stringstream sExpr;
  sExpr << p_sPostVarName << "=";
  
  RegEx rxPost(sExpr.str().c_str(), PCRE_CASELESS);
  if (rxPost.Search(m_sContent.c_str()))  
    return true;
  else
    return false;    
}

string CHTTPMessage::GetPostVar(string p_sPostVarName)
{
  if (m_nHTTPMessageType != HTTP_MESSAGE_TYPE_POST)
   return "";
  
  stringstream sExpr;
  sExpr << p_sPostVarName << "=(.*)";
  
  string sResult = "";
  RegEx rxPost(sExpr.str().c_str(), PCRE_CASELESS);
  if (rxPost.Search(m_sContent.c_str()))
  { 
    if(rxPost.SubStrings() == 2)
      sResult = rxPost.Match(1);
    
    /* remove trailing carriage return */
    if((sResult.length() > 0) && (sResult[sResult.length() - 1] == '\r'))
      sResult = sResult.substr(0, sResult.length() - 1);    
  }
  
  return sResult;
}


bool CHTTPMessage::GetVarExists(string key)
{
  return (getGetVar(key).length() > 0);
}

string CHTTPMessage::getGetVar(string key)
{
  RegEx rxGet("[?|&]" + key + "=(\\w+)");
  if(rxGet.search(m_sRequest))
    return rxGet.match(1);
  return "";
}


string	CHTTPMessage::getVarAsStr(string key)
{
	m_getVarsIter = m_getVars.find(key);
	if(m_getVarsIter == m_getVars.end()) {
		return "";
	}
	
	return m_getVars[key];
}

int CHTTPMessage::getVarAsInt(string key)
{
	string val;
	val = getVarAsStr(key);
	if(val.length() == 0) {
		return 0;
	}
	
	int ret = atoi(val.c_str());
	return ret;
}

bool CHTTPMessage::BuildFromString(string p_sMessage)
{
  m_nBinContentLength = 0;
  m_sMessage = p_sMessage;  
  
  bool bResult = false;

  /* Message GET */
  RegEx rxGET("GET +(.+) +HTTP/1\\.([1|0])", PCRE_CASELESS);
  if(rxGET.Search(p_sMessage.c_str()))
  {
    m_nHTTPMessageType = HTTP_MESSAGE_TYPE_GET;

    string sVersion = rxGET.Match(2);
    if(sVersion.compare("0") == 0)		
      m_nHTTPVersion = HTTP_VERSION_1_0;		
    else if(sVersion.compare("1") == 0)		
      m_nHTTPVersion = HTTP_VERSION_1_1;

    m_sRequest = rxGET.Match(1);
    bResult = true;		
  }

  /* Message HEAD */
  RegEx rxHEAD("HEAD +(.+) +HTTP/1\\.([1|0])", PCRE_CASELESS);
  if(rxHEAD.Search(p_sMessage.c_str()))
  {
    m_nHTTPMessageType = HTTP_MESSAGE_TYPE_HEAD;

    string sVersion = rxHEAD.Match(2);
    if(sVersion.compare("0") == 0)		
      m_nHTTPVersion = HTTP_VERSION_1_0;		
    else if(sVersion.compare("1") == 0)		
      m_nHTTPVersion = HTTP_VERSION_1_1;

    m_sRequest = rxHEAD.Match(1);			   
    bResult = true;  
  }
   
  
  /* Message POST */
  RegEx rxPOST("POST +(.+) +HTTP/1\\.([1|0])", PCRE_CASELESS);
  if(rxPOST.Search(p_sMessage.c_str()))
  {
    string sVersion = rxPOST.Match(2);
    if(sVersion.compare("0") == 0)		
      m_nHTTPVersion = HTTP_VERSION_1_0;		
    else if(sVersion.compare("1") == 0)
      m_nHTTPVersion = HTTP_VERSION_1_1;

    m_sRequest = rxPOST.Match(1);			

    bResult = ParsePOSTMessage(p_sMessage);
  }
  
  /* Message SUBSCRIBE */
  RegEx rxSUBSCRIBE("[SUBSCRIBE|UNSUBSCRIBE]+ +(.+) +HTTP/1\\.([1|0])", PCRE_CASELESS);
  if(rxSUBSCRIBE.Search(p_sMessage.c_str()))
  {
    m_nHTTPMessageType = HTTP_MESSAGE_TYPE_SUBSCRIBE;     
    
    string sVersion = rxSUBSCRIBE.Match(2);
    if(sVersion.compare("0") == 0)		
      m_nHTTPVersion = HTTP_VERSION_1_0;		
    else if(sVersion.compare("1") == 0)
      m_nHTTPVersion = HTTP_VERSION_1_1;

    m_sRequest = rxSUBSCRIBE.Match(1);
                                 
    bResult = ParseSUBSCRIBEMessage(p_sMessage);       
  }
  
  /* Range */
  RegEx rxRANGE("RANGE: +BYTES=(\\d*)(-\\d*)", PCRE_CASELESS);
  if(rxRANGE.Search(p_sMessage.c_str()))
  {
		m_hasRange = true;
    string sMatch1 = rxRANGE.Match(1);    
    string sMatch2;
    if(rxRANGE.SubStrings() > 2)
     sMatch2 = rxRANGE.Match(2);

    if(sMatch1.substr(0,1) != "-")
      m_nRangeStart = strToOffT(rxRANGE.Match(1));
    else
      m_nRangeStart = 0;
    
    m_nRangeEnd = 0;
    
    string sSub;
    if(sMatch1.substr(0, 1) == "-") {
      sSub = sMatch1.substr(1, sMatch1.length());
      m_nRangeEnd   = strToOffT(sSub.c_str());
    }
    else if(rxRANGE.SubStrings() > 2)
    {
      sSub = sMatch2.substr(1, sMatch2.length());
      m_nRangeEnd   = strToOffT(sSub.c_str());
    }
  }
  
  /* CONNECTION */
  RegEx rxCONNECTION("CONNECTION: +(close|keep-alive)", PCRE_CASELESS);
  if(rxCONNECTION.Search(p_sMessage.c_str())) {   
    string sConnection = ToLower(rxCONNECTION.Match(1));
    if(sConnection.compare("close") == 0)
      m_nHTTPConnection = HTTP_CONNECTION_CLOSE;
  }
  
  return bResult;
}

bool CHTTPMessage::LoadContentFromFile(string p_sFileName)
{
  m_bIsBinary = true;  

	m_file.setFileName(p_sFileName);
	if(m_file.open(fuppes::File::Read)) {
		m_nBinContentLength = m_file.size();
    return (m_nBinContentLength >= 0);
	}
	else {
    return false;
	}


	/*
  m_fsFile.open(p_sFileName.c_str(), ios::binary|ios::in);
  //if(m_fsFile.fail() != 1) {
	if(!m_fsFile.is_open()) {  
    m_fsFile.seekg(0, ios::end); 
    m_nBinContentLength = streamoff(m_fsFile.tellg()); 
    m_fsFile.seekg(0, ios::beg);
    return (m_nBinContentLength >= 0);
  } 
  else {
    return false;
  }	*/
}

/*
bool CHTTPMessage::TranscodeContentFromFile(string p_sFileName, CSQLResult* sqlResult)
{ 
  #ifdef DISABLE_TRANSCODING
  CSharedLog::Log(L_EXT, __FILE__, __LINE__, "TranscodeContentFromFile :: %s - %s", p_sFileName.c_str(), "ERROR: transcoding disabled");
  return false;
  #else
  
  CSharedLog::Log(L_EXT, __FILE__, __LINE__, "TranscodeContentFromFile :: %s", p_sFileName.c_str());
    
  if(m_pTranscodingSessionInfo) {
    delete m_pTranscodingSessionInfo;

    CTranscodingCache::Shared()->ReleaseCacheObject(m_pTranscodingCacheObj);
    m_pTranscodingCacheObj = NULL;
  }  
	
  m_bIsBinary  = true;  
  m_pTranscodingSessionInfo = new CTranscodeSessionInfo();
  m_pTranscodingSessionInfo->m_bBreakTranscoding   = false;
  m_pTranscodingSessionInfo->m_bIsTranscoding      = true;
  m_pTranscodingSessionInfo->m_sInFileName         = p_sFileName;  
  m_pTranscodingSessionInfo->m_nGuessContentLength = 0;
  
  m_pTranscodingSessionInfo->m_sTitle   = sqlResult->asString("TITLE"); //object->title(); //.sTitle;
  m_pTranscodingSessionInfo->m_sArtist  = sqlResult->asString("AV_ARTIST"); //object->details()->artist(); //sArtist;
  m_pTranscodingSessionInfo->m_sAlbum   = sqlResult->asString("AV_ALBUM"); //object->details()->album(); //sAlbum;
  m_pTranscodingSessionInfo->m_sGenre   = sqlResult->asString("AV_GENRE"); //object->details()->genre(); //sGenre;
  m_pTranscodingSessionInfo->m_sOriginalTrackNumber = sqlResult->asString("A_TRACK_NUMBER"); //object->details()->trackNumber(); // sOriginalTrackNumber;
  m_pTranscodingSessionInfo->sACodec    = sqlResult->asString("AUDIO_CODEC"); //object->details()->audioCodec(); //.sACodec;
  m_pTranscodingSessionInfo->sVCodec    = sqlResult->asString("VIDEO_CODEC"); //object->details()->videoCodec(); //.sVCodec;
  
  m_pTranscodingCacheObj = CTranscodingCache::Shared()->GetCacheObject(m_pTranscodingSessionInfo->m_sInFileName, DeviceSettings());
  if(!m_pTranscodingCacheObj->Init(m_pTranscodingSessionInfo)) {
		CSharedLog::Log(L_EXT, __FILE__, __LINE__, "init transcoding failed :: %s", p_sFileName.c_str());
		return false;
	}
	
  m_pTranscodingCacheObj->Transcode(DeviceSettings()); 
  
  if(DeviceSettings()->TranscodingHTTPResponse(ExtractFileExt(p_sFileName)) == RESPONSE_CHUNKED) {
    m_nTransferEncoding = HTTP_TRANSFER_ENCODING_CHUNKED;
  }
  else if(DeviceSettings()->TranscodingHTTPResponse(ExtractFileExt(p_sFileName)) == RESPONSE_STREAM) {
    m_nTransferEncoding = HTTP_TRANSFER_ENCODING_NONE;
  }
  
  if(!m_pTranscodingCacheObj->Threaded()) {
    LoadContentFromFile(m_pTranscodingCacheObj->m_sOutFileName);
  }
  
  return true;
  #endif
}
*/

/*
bool CHTTPMessage::IsTranscoding()
{ 
  #ifndef DISABLE_TRANSCODING
  if(m_pTranscodingCacheObj)
    return m_pTranscodingCacheObj->m_bIsTranscoding;
  else
  #endif
    return false;
}

void CHTTPMessage::BreakTranscoding()
{
  #ifndef DISABLE_TRANSCODING
  if(m_pTranscodingSessionInfo) {                            
    m_pTranscodingSessionInfo->m_bBreakTranscoding = true;   
  }
  #endif
}
*/



bool CHTTPMessage::ParsePOSTMessage(string p_sMessage)
{
  /*POST /UPnPServices/ContentDirectory/control HTTP/1.1
    Host: 192.168.0.3:32771
    SOAPACTION: "urn:schemas-upnp-org:service:ContentDirectory:1#Browse"
    CONTENT-TYPE: text/xml ; charset="utf-8"
    Content-Length: 467*/
	
  RegEx rxSOAP("SOAPACTION: *\"(.*)#(.+)\"", PCRE_CASELESS);
	if(rxSOAP.Search(p_sMessage.c_str()))	{
		
		m_soapTarget = rxSOAP.Match(1);
		m_soapAction = rxSOAP.Match(2);

    m_nHTTPMessageType = HTTP_MESSAGE_TYPE_POST_SOAP_ACTION;
	}
  else
  {
    m_nHTTPMessageType = HTTP_MESSAGE_TYPE_POST;
  }
      
  /* Content length */
  RegEx rxContentLength("CONTENT-LENGTH: *(\\d+)", PCRE_CASELESS);
  if(rxContentLength.Search(p_sMessage.c_str()))
  {
    string sContentLength = rxContentLength.Match(1);    
    m_nContentLength = atoll(sContentLength.c_str());
  }
  
  if((unsigned int)m_nContentLength >= p_sMessage.length())                      
    return false;
  
  //m_sContent = p_sMessage.substr(p_sMessage.length() - m_nContentLength, m_nContentLength);
  
  return true;
}

bool CHTTPMessage::ParseSUBSCRIBEMessage(string p_sMessage)
{
/*
SUBSCRIBE /UPnPServices/ConnectionManager/event/ HTTP/1.1
HOST: 192.168.0.3:58444
CALLBACK: <http://192.168.0.3:49152/>
NT: upnp:event
TIMEOUT: Second-1801


SUBSCRIBE /UPnPServices/ConnectionManager/event/ HTTP/1.1
TIMEOUT: Second-15
HOST: 192.168.0.3:1050
CALLBACK: <http://192.168.0.3:42577>
NT: upnp:event
Content-Length: 0

*/     

  //cout << "PARSE SUBSCRIBE" << endl;

  RegEx rxCallBack("CALLBACK: *(.+)", PCRE_CASELESS);
  if(rxCallBack.Search(p_sMessage.c_str()))
  {
    m_sGENACallBack = rxCallBack.Match(1);
  }

  RegEx rxNT("NT: *(.+)", PCRE_CASELESS);
  if(rxNT.Search(p_sMessage.c_str()))
  {
    m_sGENANT = rxNT.Match(1);
  }

  // Header information: Timeout (GENA - Request)
  //string        m_sGENATimeout;  
  // Header information: Subscription-ID (GENA - Request & Response)
  //string        m_sGENASubscriptionID;    
  
  
  return true;     
}

void CHTTPMessage::SetLocalEndPoint(sockaddr_in p_EndPoint)
{	
	m_LocalEp.sin_family	= p_EndPoint.sin_family;
	m_LocalEp.sin_addr 		= p_EndPoint.sin_addr;
	m_LocalEp.sin_port		= p_EndPoint.sin_port;
	memset(&m_LocalEp.sin_zero, 0, sizeof(m_LocalEp.sin_zero));	
}

void CHTTPMessage::SetRemoteEndPoint(sockaddr_in p_EndPoint)
{
	m_RemoteEp.sin_family	= p_EndPoint.sin_family;
	m_RemoteEp.sin_addr		= p_EndPoint.sin_addr;
	m_RemoteEp.sin_port		= p_EndPoint.sin_port;
	memset(&m_RemoteEp.sin_zero, 0, sizeof(m_RemoteEp.sin_zero));	
}

string CHTTPMessage::GetRemoteIPAddress() 
{ 
	string ip = inet_ntoa(m_RemoteEp.sin_addr);
	return ip;
}
