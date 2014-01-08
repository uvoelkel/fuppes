/***************************************************************************
 *            EventNotification.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2007 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
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

#include "EventNotification.h"

#include "../Common/Common.h"
#include "../Common/RegEx.h"
#include <sstream>
#include <iostream>
using namespace std;

std::string CEventNotification::BuildHeader()
{
  stringstream sHeader;
  
  sHeader << 
    "NOTIFY " << m_sSubscriberPath << " HTTP/1.1\r\n" <<  
    "HOST: " << m_sHost << "\r\n" <<
    "CONTENT-TYPE: text/xml\r\n" <<
    "CONTENT-LENGTH: " << m_sContent.length() << "\r\n" <<
    "NT: upnp:event\r\n" <<
    "NTS: upnp:propchange\r\n" <<
    "SID: uuid:" << m_sSID << "\r\n" <<
    "SEQ: 0\r\n" <<
    "User-Agent: SSDP CD Events\r\n" << 
    "Cache-Control: no-cache\r\n\r\n";
  
  return sHeader.str();
}

void CEventNotification::SetCallback(std::string p_sCallback)
{ 
  SplitURL(p_sCallback, &m_sSubscriberIP, &m_nSubscriberPort);  
  RegEx rxCallback("[http://]*([0-9|\\.]+):*([0-9]*)(/.*)", PCRE_CASELESS);
  if(rxCallback.Search(p_sCallback.c_str())) {
    m_sSubscriberPath = rxCallback.Match(3);
  }
}
