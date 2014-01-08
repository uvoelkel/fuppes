/***************************************************************************
 *            EventNotification.h
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

#ifndef _EVENTNOTIFICATION_H
#define _EVENTNOTIFICATION_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>

class CEventNotification
{
  public:
    std::string BuildHeader();
    void SetHost(std::string p_sHost) { m_sHost = p_sHost; }
  
    std::string GetContent() { return m_sContent; }
    void SetContent(std::string p_sContent) { m_sContent = p_sContent; }
  
    void SetCallback(std::string p_sCallback);
    
    void SetSID(std::string p_sSID) { m_sSID = p_sSID; }    
    
    std::string  GetSubscriberIP() { return m_sSubscriberIP; }
    unsigned int GetSubscriberPort() { return m_nSubscriberPort; }
    
  private:
    std::string m_sContent;
    std::string m_sHost;
    std::string m_sSID;
  
    std::string  m_sSubscriberIP;
    unsigned int m_nSubscriberPort;
    std::string  m_sSubscriberPath;
};
  
#endif // _EVENTNOTIFICATION_H
