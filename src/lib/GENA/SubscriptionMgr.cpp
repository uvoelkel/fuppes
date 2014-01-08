/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            SubscriptionMgr.cpp
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
 
#include "../OldHTTP/HTTPClient.h"

#include "SubscriptionMgr.h"
#include "../Common/RegEx.h"
#include "../Common/UUID.h"
#include "../Common/Exception.h"
#include "../ContentDatabase/ContentDatabase.h"
#include "../Thread/ThreadPool.h"
#include "../SharedLog.h"

using namespace std;
using namespace fuppes;

CSubscription::CSubscription()
{ 
  m_bHandled   = false;
  //m_pHTTPClient = NULL;
}

CSubscription::~CSubscription()
{
  /*
  if(m_pHTTPClient)
    delete m_pHTTPClient;
  */
}


void CSubscription::Renew()
{
  m_nTimeLeft = m_nTimeout;
}

void CSubscription::DecTimeLeft(unsigned int ms)
{
  if(m_nTimeLeft > 0) {
    if(m_nTimeLeft >= ms)
      m_nTimeLeft -= ms;
    else
      m_nTimeLeft = 0;
  }
}

/*
CHTTPClient* CSubscription::GetHTTPClient()
{
  if(!m_pHTTPClient)
    m_pHTTPClient = new CHTTPClient(NULL, "CSubscription");
  return m_pHTTPClient;
}
*/



void CSubscription::AsyncReply(string localIpAddress)
{
  CEventNotification* pNotification = new CEventNotification();
  
  switch(m_nSubscriptionTarget)
  {
    case UPNP_SERVICE_CONTENT_DIRECTORY : {

      stringstream content;
      content <<
        "<e:propertyset xmlns:e=\"urn:schemas-upnp-org:event-1-0\">"
        "<e:property>"
        "<SystemUpdateID>" << ContentDatabase::ContentDatabase::getSystemUpdateId() << "</SystemUpdateID>"
        "</e:property>"
			  "<e:property>"
        "<ContainerUpdateIDs></ContainerUpdateIDs>"
        "</e:property>"
        "<e:property>"
        "<TransferIDs></TransferIDs>"
        "</e:property>"
        "</e:propertyset>";  
     pNotification->SetContent(content.str());
      
     pNotification->SetCallback(m_sCallback);
     pNotification->SetSID(m_sSID);
/*
NOTIFY / HTTP/1.1
HOST: 192.168.0.3:49152
CONTENT-TYPE: text/xml
CONTENT-LENGTH: 260
NT: upnp:event
NTS: upnp:propchange
SID: uuid:1
SEQ: 0
User-Agent: SSDP CD Events
Cache-Control: no-cache

<e:propertyset xmlns:e="urn:schemas-upnp-org:event-1-0">
<e:property>
<SystemUpdateID>1</SystemUpdateID>
</e:property>
<e:property>
<ContainerUpdateIDs></ContainerUpdateIDs>
</e:property>
</e:propertyset>

HTTP/1.1 200 OK
SERVER: Linux/2.6.16.16, UPnP/1.0, Intel SDK for UPnP devices /1.2
CONNECTION: close
CONTENT-LENGTH: 41
CONTENT-TYPE: text/html

<html><body><h1>200 OK</h1></body></html>
*/
      } break;
    case UPNP_SERVICE_CONNECTION_MANAGER :
      
      pNotification->SetContent(
        "<e:propertyset xmlns:e=\"urn:schemas-upnp-org:event-1-0\">"
        "<e:property>"
        "<SourceProtocolInfo>http-get:*:audio/mpeg:*,http-get:*:audio/mpegurl:*,http-get:*:image/jpeg:*</SourceProtocolInfo>"
        "</e:property>"
        "<e:property>"
        "<SinkProtocolInfo></SinkProtocolInfo>"
        "</e:property>"
        "<e:property>"
        "<CurrentConnectionIDs>0</CurrentConnectionIDs>"
        "</e:property>"
        "</e:propertyset>");
    
     pNotification->SetCallback(m_sCallback);
     pNotification->SetSID(m_sSID);      
      
/*
NOTIFY / HTTP/1.1
HOST: 192.168.0.3:49152
CONTENT-TYPE: text/xml
CONTENT-LENGTH: 356
NT: upnp:event
NTS: upnp:propchange
SID: uuid:0
SEQ: 0
User-Agent: SSDP CD Events
Cache-Control: no-cache

<e:propertyset xmlns:e="urn:schemas-upnp-org:event-1-0">
<e:property>
<SourceProtocolInfo>http-get:*:audio/mpeg:*,http-get:*:audio/mpegurl:*,http-get:*:image/jpeg:*</SourceProtocolInfo>
</e:property>
<e:property>
<SinkProtocolInfo></SinkProtocolInfo>
</e:property>
<e:property>
<CurrentConnectionIDs>0</CurrentConnectionIDs>
</e:property>
</e:propertyset>

HTTP/1.1 200 OK
SERVER: Linux/2.6.16.16, UPnP/1.0, Intel SDK for UPnP devices /1.2
CONNECTION: close
CONTENT-LENGTH: 41
CONTENT-TYPE: text/html

<html><body><h1>200 OK</h1></body></html>
*/    
    
      break;

		case UPNP_SERVICE_X_MS_MEDIA_RECEIVER_REGISTRAR :      
  		pNotification->SetContent(
  		        "<e:propertyset xmlns:e=\"urn:schemas-upnp-org:event-1-0\" xmlns:s=\"urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1\">"
  		        "<e:property>"
  		        "<s:AuthorizationGrantedUpdateID xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui4\">1</s:AuthorizationGrantedUpdateID>"
  		        "</e:property>"
  		        "<e:property>"
  		        "<s:AuthorizationDeniedUpdateID xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui4\">1</s:AuthorizationDeniedUpdateID>"
  		        "</e:property>"
  		        "<e:property>"
  		        "<s:ValidationSucceededUpdateID xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui4\">1</s:ValidationSucceededUpdateID>"
  		        "</e:property>"
  		        "<e:property>"
  		        "<s:ValidationRevokedUpdateID xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui4\">1</s:ValidationRevokedUpdateID>"
  		        "</e:property>"
  		        "</e:propertyset>");
   		pNotification->SetCallback(m_sCallback);
   		pNotification->SetSID(m_sSID);    

/*
			 <e:propertyset xmlns:e="urn:schemas-upnp-org:event-1-0" xmlns:s="urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1">
<e:property>
 <s:AuthorizationGrantedUpdateID xmlns:dt="urn:schemas-microsoft-com:datatypes" dt:dt="ui4">1</s:AuthorizationGrantedUpdateID>
</e:property>
<e:property>
 <s:AuthorizationDeniedUpdateID xmlns:dt="urn:schemas-microsoft-com:datatypes" dt:dt="ui4">1</s:AuthorizationDeniedUpdateID>
</e:property>
<e:property>
 <s:ValidationSucceededUpdateID xmlns:dt="urn:schemas-microsoft-com:datatypes" dt:dt="ui4">1</s:ValidationSucceededUpdateID>
</e:property>
<e:property>
<s:ValidationRevokedUpdateID xmlns:dt="urn:schemas-microsoft-com:datatypes" dt:dt="ui4">1</s:ValidationRevokedUpdateID>
</e:property>
</e:propertyset>
			 */
			
   		break;
			
		default:
      break;
  }

  //log(Log::gena, Log::debug) << "send notification for target:" << m_nSubscriptionTarget << "cb:" << m_sCallback;
  
  //this->GetHTTPClient()->AsyncNotify(pNotification);
  CHTTPClient* httpClient = new CHTTPClient(localIpAddress, NULL, "CSubscription");
  Threading::ThreadPool::deleteLater(httpClient);
  httpClient->AsyncNotify(pNotification);

  delete pNotification;
}


CSubscriptionCache::CSubscriptionCache()
{
  //fuppesThreadInitMutex(&m_Mutex);
}

CSubscriptionCache::~CSubscriptionCache()
{
  for(m_SubscriptionsIterator = m_Subscriptions.begin();
      m_SubscriptionsIterator != m_Subscriptions.end();
      m_SubscriptionsIterator++) {
    delete m_SubscriptionsIterator->second;
  }
  m_Subscriptions.clear();
  
  //fuppesThreadDestroyMutex(&m_Mutex);
}

void CSubscriptionCache::Lock()
{
  m_Mutex.lock();
}

void CSubscriptionCache::Unlock()
{
  m_Mutex.unlock();
}

void CSubscriptionCache::AddSubscription(CSubscription* pSubscription)
{
  string sSID = GenerateUUID();
  pSubscription->SetSID(sSID);

  //log(Log::gena, Log::debug) << "add subscription" << sSID;
  
  m_Mutex.lock();
  m_Subscriptions[sSID] = pSubscription;
  m_Mutex.unlock();
}

bool CSubscriptionCache::RenewSubscription(string pSID)
{
  //log(Log::gena, Log::debug) << "renew subscription" << pSID;
  
  bool bResult = false;
  
  m_Mutex.lock();
  m_SubscriptionsIterator = m_Subscriptions.find(pSID);
  if(m_SubscriptionsIterator != m_Subscriptions.end()) {    
    CSharedLog::Log(L_EXT, __FILE__, __LINE__, "renew subscription \"%s\" done", pSID.c_str());
    m_Subscriptions[pSID]->Renew();
    bResult = true;
  }
  else {
    CSharedLog::Log(L_EXT, __FILE__, __LINE__, "renew subscription \"%s\" failed", pSID.c_str());
    bResult = false;
  }
  
  m_Mutex.unlock();
  return bResult;  
}

bool CSubscriptionCache::DeleteSubscription(string pSID)
{
  //log(Log::gena, Log::debug) << "delete subscription" << pSID;
    
  bool bResult = false;
  CSubscription* pSubscription;
  
  m_Mutex.lock();  
  m_SubscriptionsIterator = m_Subscriptions.find(pSID);  
  if(m_SubscriptionsIterator != m_Subscriptions.end()) { 
    CSharedLog::Log(L_EXT, __FILE__, __LINE__, "delete subscription \"%s\" done", pSID.c_str());
    pSubscription = (*m_SubscriptionsIterator).second;
    m_Subscriptions.erase(pSID);
    delete pSubscription;
    bResult = true;
  }
  else {
    CSharedLog::Log(L_EXT, __FILE__, __LINE__, "delete subscription \"%s\" failed", pSID.c_str());
    bResult = false;
  }
  
  m_Mutex.unlock();
  return bResult;  
}


//fuppesThreadCallback MainLoop(void *arg);
CSubscriptionMgr::CSubscriptionMgr(string localIpAddress)
:Threading::Thread("SubscriptionMgr")
{
    m_localIpAddress = localIpAddress;
    m_lastSystemUpdateId = 0;
}

CSubscriptionMgr::~CSubscriptionMgr()
{
  // stop and close thread
  stop();
  close();
}

/*
bool CSubscriptionMgr::HandleSubscription(CHTTPMessage* pRequest, CHTTPMessage* pResponse)
{
	log(Log::gena, Log::debug) << "REQUEST:\n" << pRequest->GetMessage();
  
  CSubscription* pSubscription = new CSubscription();
  try {    
    CSubscriptionMgr::ParseSubscription(pRequest->GetHeader(), pSubscription);
  } 
  catch (fuppes::Exception ex) {
    log(Log::gena, Log::debug) << ex;
    
    delete pSubscription;
    return false;
  }
    
  switch(pSubscription->GetType())
  {
    case ST_SUBSCRIBE:
      CSubscriptionCache::Shared()->AddSubscription(pSubscription);
      pResponse->SetGENASubscriptionID(pSubscription->GetSID());
      break;
    case ST_RENEW:
      CSubscriptionCache::Shared()->RenewSubscription(pSubscription->GetSID());      
      pResponse->SetGENASubscriptionID(pSubscription->GetSID());
      delete pSubscription;
      break;
    case ST_UNSUBSCRIBE:
      CSubscriptionCache::Shared()->DeleteSubscription(pSubscription->GetSID());
      delete pSubscription;
      break;
    
    default:
      delete pSubscription;
      return false;    
  }

  return true;
}
*/


Fuppes::HttpResponse* CSubscriptionMgr::handleSubscription(Fuppes::HttpRequest* request)
{  
    Fuppes::HttpResponse* response = NULL;
  
    CSubscription* pSubscription = new CSubscription();
    try {    
        this->ParseSubscription(request->getRawHeader(), pSubscription);
    } 
    catch (fuppes::Exception &ex) {
        delete pSubscription;
        return response;
    }


    switch(pSubscription->GetType()) {
      
        case ST_SUBSCRIBE:
            m_cache.AddSubscription(pSubscription);
            response = new Fuppes::HttpResponse(Http::OK);
            response->getHeader()->setValue("SID", "uuid:" + pSubscription->GetSID());
            response->getHeader()->setValue("TIMEOUT", "Second-180");
            break;
        case ST_RENEW:
            m_cache.RenewSubscription(pSubscription->GetSID());
            response = new Fuppes::HttpResponse(Http::OK);
            response->getHeader()->setValue("SID", "uuid:" + pSubscription->GetSID());
            response->getHeader()->setValue("TIMEOUT", "Second-180");
            delete pSubscription;
            break;
        case ST_UNSUBSCRIBE:
            m_cache.DeleteSubscription(pSubscription->GetSID());
            response = new Fuppes::HttpResponse(Http::OK);
            delete pSubscription;
            break;
        default:
            delete pSubscription;
            break;
        
    }

    return response;
}


void CSubscriptionMgr::ParseSubscription(string header, CSubscription* pSubscription)
{
	
	// subscription target
  RegEx rxTarget("([SUBSCRIBE|UNSUBSCRIBE]+) +(http://[0-9|\\.]+:\\d+)*(/.+) +HTTP/1\\.[1|0]", PCRE_CASELESS);
  if(rxTarget.Search(header)) {
    
    if (ToLower(rxTarget.Match(3)).compare("/upnpservices/contentdirectory/event/") == 0) {
      pSubscription->SetSubscriptionTarget(UPNP_SERVICE_CONTENT_DIRECTORY);
    }
    else if (ToLower(rxTarget.Match(3)).compare("/upnpservices/connectionmanager/event/") == 0) {
      pSubscription->SetSubscriptionTarget(UPNP_SERVICE_CONNECTION_MANAGER);
    }
		else if (ToLower(rxTarget.Match(3)).compare("/upnpservices/xmsmediareceiverregistrar/event/") == 0) {
      pSubscription->SetSubscriptionTarget(UPNP_SERVICE_X_MS_MEDIA_RECEIVER_REGISTRAR);
    }
    else {
      throw fuppes::Exception(__FILE__, __LINE__, "unknown subscription target :: %s", rxTarget.Match(3).c_str());
    }    
  }
  else {
    throw fuppes::Exception(__FILE__, __LINE__, "parsing subscription");
  }
  
  // SID
  string sSID = "";
  RegEx rxSID("SID: *uuid:([A-Z|0-9|-]+)", PCRE_CASELESS);
  if(rxSID.Search(header.c_str()))
    sSID = rxSID.Match(1);  
  
  string sSubscribe = ToLower(rxTarget.Match(1));
  if(sSubscribe.compare("subscribe") == 0) {
		
    if(sSID.length() == 0) {
      pSubscription->SetType(ST_SUBSCRIBE);
      
      RegEx rxCallback("CALLBACK: *<*(http://[A-Z|0-9|\\-|_|/|\\.|:]+)>*", PCRE_CASELESS);
      if(rxCallback.Search(header.c_str())) {
        pSubscription->SetCallback(rxCallback.Match(1));
      }
      else {
        throw fuppes::Exception(__FILE__, __LINE__, "parsing subscription callback");
      }      
    }
    else {
      pSubscription->SetType(ST_RENEW);
      pSubscription->SetSID(sSID);
    }
    
    pSubscription->SetTimeout(180);    
  } // subscribe
  else if(sSubscribe.compare("unsubscribe") == 0) {
    pSubscription->SetType(ST_UNSUBSCRIBE);
    pSubscription->SetSID(sSID);
  }  
}


/**
 * the CSubscriptionMgr's MainLoop constantly
 * walks the subscriptions and handles them
 */
//fuppesThreadCallback MainLoop(void *arg)
#define SLEEP_TIME_MS 200
void CSubscriptionMgr::run()
{  
  CSubscription* pSubscr = NULL;
  map<string, CSubscription*>::iterator tmpIt;    

  uint32_t systemUpdateId = ContentDatabase::ContentDatabase::getSystemUpdateId();
  
  while(!this->stopRequested()) {

    systemUpdateId = ContentDatabase::ContentDatabase::getSystemUpdateId();
    
    m_cache.Lock();
  
    // walk subscriptions and decrement timeout
    for(m_cache.m_SubscriptionsIterator  = m_cache.m_Subscriptions.begin();
        m_cache.m_SubscriptionsIterator != m_cache.m_Subscriptions.end();)
    {
      if(m_cache.m_Subscriptions.size() == 0) {
        break;
      }      
        
      pSubscr = (*m_cache.m_SubscriptionsIterator).second;
      pSubscr->DecTimeLeft(SLEEP_TIME_MS);      
      
      if(!pSubscr->m_bHandled) {        
        // todo: max send freq 5Hz for "SystemUpdateID" and "ContainerUpdateIDs"        
        pSubscr->m_bHandled = true;        
        pSubscr->AsyncReply(m_localIpAddress);
      }
      else {
        if(pSubscr->GetSubscriptionTarget() == UPNP_SERVICE_CONTENT_DIRECTORY && m_lastSystemUpdateId < systemUpdateId) {
          pSubscr->AsyncReply(m_localIpAddress);
        }
      }
      
      if(pSubscr->GetTimeLeft() == 0) {        
        tmpIt = m_cache.m_SubscriptionsIterator;
        ++tmpIt;
        m_cache.m_Subscriptions.erase(pSubscr->GetSID());        
        m_cache.m_SubscriptionsIterator = tmpIt;
        delete pSubscr;
      }
      else {
        m_cache.m_SubscriptionsIterator++;
      }
    } // for
    
    if(m_lastSystemUpdateId < systemUpdateId) {
      m_lastSystemUpdateId = systemUpdateId;
    }

    m_cache.Unlock();
    msleep(SLEEP_TIME_MS);
  } // while

}

/* SUBSCRIBE
  
SUBSCRIBE /UPnPServices/ContentDirectory/event/ HTTP/1.1
HOST: 192.168.0.3:48756
NT: upnp:event
TIMEOUT: Second-180
User-Agent: POSIX, UPnP/1.0, Intel MicroStack/1.0.1423
CALLBACK: http://192.168.0.23:54877/UPnPServices/ContentDirectory/event/
*/
  
/* RENEW

SUBSCRIBE /UPnPServices/ConnectionManager/event/ HTTP/1.1
HOST: 192.168.0.3:48756
SID: uuid:b7e3b9b4-7059-472e-95ba-5401708fa2de
TIMEOUT: Second-180
User-Agent: POSIX, UPnP/1.0, Intel MicroStack/1.0.1423
*/

/* UNSUBSCRIBE

UNSUBSCRIBE /UPnPServices/ContentDirectory/event/ HTTP/1.1
HOST: 192.168.0.3:58642
SID: uuid:b5117436-a4ef-4944-9fb0-30190a83e2aa
USER-AGENT: Linux/2.6.16.16, UPnP/1.0, Intel SDK for UPnP devices /1.2
*/


/*
NOTIFY / HTTP/1.1
HOST: 192.168.0.3:49152
CONTENT-TYPE: text/xml
CONTENT-LENGTH: 356
NT: upnp:event
NTS: upnp:propchange
SID: uuid:0
SEQ: 0
User-Agent: SSDP CD Events
Cache-Control: no-cache

<e:propertyset xmlns:e="urn:schemas-upnp-org:event-1-0">
<e:property>
<SourceProtocolInfo>http-get:*:audio/mpeg:*,http-get:*:audio/mpegurl:*,http-get:*:image/jpeg:*</SourceProtocolInfo>
</e:property>
<e:property>
<SinkProtocolInfo></SinkProtocolInfo>
</e:property>
<e:property>
<CurrentConnectionIDs>0</CurrentConnectionIDs>
</e:property>
</e:propertyset>

HTTP/1.1 200 OK
SERVER: Linux/2.6.16.16, UPnP/1.0, Intel SDK for UPnP devices /1.2
CONNECTION: close
CONTENT-LENGTH: 41
CONTENT-TYPE: text/html

<html><body><h1>200 OK</h1></body></html>
*/


/*
NOTIFY / HTTP/1.1
HOST: 192.168.0.3:49152
CONTENT-TYPE: text/xml
CONTENT-LENGTH: 260
NT: upnp:event
NTS: upnp:propchange
SID: uuid:1
SEQ: 0
User-Agent: SSDP CD Events
Cache-Control: no-cache

<e:propertyset xmlns:e="urn:schemas-upnp-org:event-1-0">
<e:property>
<SystemUpdateID>1</SystemUpdateID>
</e:property>
<e:property>
<ContainerUpdateIDs></ContainerUpdateIDs>
</e:property>
<e:property>
<TransferIDs></TransferIDs>
</e:property>
</e:propertyset>

HTTP/1.1 200 OK
SERVER: Linux/2.6.16.16, UPnP/1.0, Intel SDK for UPnP devices /1.2
CONNECTION: close
CONTENT-LENGTH: 41
CONTENT-TYPE: text/html

<html><body><h1>200 OK</h1></body></html>
*/
