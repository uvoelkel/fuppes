/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            HTTPClient.cpp
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

#include "HTTPClient.h"
#include "../Common/Common.h"
#include "../Common/Exception.h"
#include "../Common/RegEx.h"
#include "../SharedLog.h"
#include "HTTPParser.h"

#ifndef WIN32
#include <pthread.h>
#endif

#include <string>
#include <sstream>

using namespace std;

CHTTPClient::CHTTPClient(std::string ip, IHTTPClient* pAsyncReceiveHandler, std::string name)
:Thread("HTTP client " + name), m_socket(ip)
{
  m_bIsAsync    = false;
  m_pAsyncReceiveHandler = pAsyncReceiveHandler;
  m_bAsyncDone = false;
}

CHTTPClient::~CHTTPClient()
{
	m_socket.close();
	// stop and close thread
	close();
}

void CHTTPClient::run()
{
	try {
		// connect
		m_socket.connect();

		if(this->stopRequested())
			return;
		
		// send message
		//cout << m_sMessage << endl;
		m_socket.send(m_sMessage);
		
		// receive response
		int loop = 0;
		fuppes_off_t received = 0;
		fuppes_off_t contentLength = 0;
		CHTTPMessage Response;
		bool complete = false;

  	char* szHeader = NULL;
	  size_t	nHeaderPos	= 0;


    Response.SetRemoteEndPoint(m_socket.remoteEndpoint());
    
		while(!this->stopRequested()) {

			received = m_socket.oldreceive(1);			
			if(received == 0) {
				if(loop == 10)
					break;
				
				loop++;
				continue;
			}

			// check if we got the full header
			char*	pos	= NULL;
			if((pos = strstr(m_socket.buffer(), "\r\n\r\n")) == NULL) {
        continue;
      }

      // get the header
      nHeaderPos = (pos - m_socket.buffer()) + strlen("\r\n\r\n");
  	  szHeader = (char*)malloc((nHeaderPos + 1) * sizeof(char*));
		  strncpy(szHeader, m_socket.buffer(), nHeaderPos);
		  szHeader[nHeaderPos] = '\0';

      Response.SetHeader(szHeader);
      
      free(szHeader);
  	  szHeader = NULL;



      if(Response.GetTransferEncoding() == HTTP_TRANSFER_ENCODING_NONE) {


        contentLength = 0;
				if(CHTTPParser::hasContentLength(m_socket.buffer()))
					contentLength = CHTTPParser::getContentLength(m_socket.buffer());

				// full header no content => finished
				if(contentLength == 0) {
					 complete = true;
					 break;
				}


        if(contentLength > 0) {
					fuppes_off_t headerSize = pos - m_socket.buffer();
					headerSize += 4; // strlen("\r\n\r\n")

					// full header and content => finished
					if(m_socket.bufferFill() - headerSize == contentLength) {
						complete = true;
						break;
					} 
				}
        
      }
      else if(Response.GetTransferEncoding() == HTTP_TRANSFER_ENCODING_CHUNKED) {

        // size (hex) CRLF
        // data CRLF
        // 0 (possible whitespace) CRLF
        // CRLF

        int offset = m_socket.bufferFill() - 1;
        if(m_socket.buffer()[offset] == '\n') offset--; else { continue; }
        if(m_socket.buffer()[offset] == '\r') offset--; else { continue; }
        if(m_socket.buffer()[offset] == '\n') offset--; else { continue; }
        if(m_socket.buffer()[offset] == '\r') offset--; else { continue; }
        while(m_socket.buffer()[offset] == ' ' && offset > 0)
          offset--;

        if(m_socket.buffer()[offset] != '0') {
          continue; 
        }

        string msg = &m_socket.buffer()[nHeaderPos];
        memset(&m_socket.buffer()[nHeaderPos], '\0', m_socket.bufferFill() - nHeaderPos);

        size_t pos;
        unsigned int size;
        unsigned int msgOffset = 0;
        while((pos = msg.find("\r\n")) != string::npos) {
          
          size = HexToInt(msg.substr(0, pos));
          msg = msg.substr(pos + 2);

          if(size == 0) {
            continue;
          }
          
          strncpy(&m_socket.buffer()[nHeaderPos + msgOffset], msg.c_str(), size);
          msgOffset += size;        
          msg = msg.substr(size);
        }

        //Response.SetMessage(m_socket.buffer());        
        complete = true;
        break;
        
      } // chunked	
			
		} // while
		
		
		if(complete && this->m_pAsyncReceiveHandler != NULL && !this->stopRequested()) {
			Response.SetMessage(m_socket.buffer());
			this->m_pAsyncReceiveHandler->OnAsyncReceiveMsg(&Response);
		}
	} 
	catch(fuppes::Exception &ex)	{
	    //CSharedLog::Log(L_DBG, ex);
	}

	m_bAsyncDone = true;
}

void CHTTPClient::AsyncNotify(CEventNotification* pNotification)
{  
  // set remote end point	
	m_socket.remoteAddress(pNotification->GetSubscriberIP());
	m_socket.remotePort(pNotification->GetSubscriberPort());
  
  // set the notification's host
  stringstream sHost;
	sHost << m_socket.localAddress() << ":" << m_socket.localPort();  
  pNotification->SetHost(sHost.str());
  
  // start async send
  m_sMessage   = pNotification->BuildHeader() + pNotification->GetContent();
  m_bIsAsync   = true;
  m_bAsyncDone = false;
	this->start();
}


/** HTTP GET implementation
 *  @param  p_sGetURL  the URL to GET
 *
 *  @return returns true on success otherwise false
 */
bool CHTTPClient::AsyncGet(std::string p_sGetURL)
{
  std::string  sGet;
  std::string  sIPAddress;
  unsigned int nPort;

  if(!SplitURL(p_sGetURL, &sIPAddress, &nPort))
    return false;

  RegEx rxGet("[http://]*[0-9|\\.]+:*[0-9]*([/|\\w|\\.]*)", PCRE_CASELESS);
  if(rxGet.Search(p_sGetURL.c_str())) {
		sGet = rxGet.Match(1);
    if(sGet.empty())
      return false;
  }
	else {
	  return false;
	}

  
  // set remote end point								
	m_socket.remoteAddress(sIPAddress);
	m_socket.remotePort(nPort);
  
  // build GET header
  std::string sMsg = BuildGetHeader(sGet, sIPAddress, nPort);  
  CSharedLog::Log(L_DBG, __FILE__, __LINE__, "send GET Header \n%s", sMsg.c_str());
	
  // start async send
  m_sMessage   = sMsg;
  m_bIsAsync   = true;
  m_bAsyncDone = false;
	this->start();
	
  return true;
}

std::string CHTTPClient::BuildGetHeader(std::string p_sGet, std::string p_sTargetIPAddress, unsigned int p_nTargetPort)
{
  std::stringstream sHeader;
  
  // Build header
  sHeader << "GET " << p_sGet << " HTTP/1.1" << "\r\n";
  sHeader << "HOST: " << p_sTargetIPAddress << ":" << p_nTargetPort << "\r\n";
  //sHeader << "CONTENT-LENGTH: 0" << "\r\n";
 /* sHeader << "USER-AGENT: " << CSharedConfig::Shared()->GetOSName() << "/" << CSharedConfig::Shared()->GetOSVersion() << ", " <<
             "UPnP/1.0, " << CSharedConfig::Shared()->GetAppFullname() << "/" << CSharedConfig::Shared()->GetAppVersion() << "\r\n"; */
	
  sHeader << "\r\n";
  
  return sHeader.str();
}
