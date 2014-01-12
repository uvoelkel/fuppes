/*
 * This file is part of fuppes
 *
 * (c) 2005-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

// increment this value if the database structure has changed
#define DB_VERSION 5

#include "Core.h"

#include <iostream>

//#include <fstream>

#include "../Common/Exception.h"
#include "../SharedLog.h"
#include "../Log/Log.h"
//#include "HTTP/HTTPMessage.h"

#include "../ControlInterface/ControlInterface.h"

#include "../Database/TableManager.h"

#include "FuppesHttpSession.h"

using namespace std;
using namespace Fuppes;

Core::Core(fuppes_status_callback_t callback, Fuppes::FuppesConfig& fuppesConfig) :
        m_fuppesConfig(fuppesConfig),

        m_pluginManager(),
        m_databaseConnection(m_pluginManager),
        m_accessControl(fuppesConfig.networkSettings.accessControlConfig),
        m_deviceIdentMgr(fuppesConfig, fuppesConfig.deviceIdentificationSettings, m_accessControl),
        m_fileDetails(m_pluginManager),
        m_virtualContainerMgr(fuppesConfig.virtualContainerSettings),
        m_contentDatabase(m_fuppesConfig.contentDatabaseSettings, m_fileDetails, m_pluginManager, m_virtualContainerMgr),

        m_httpServer(fuppesConfig.networkSettings.ipAddress, fuppesConfig.networkSettings.httpPort, this),

        m_subscriptionMgr(fuppesConfig.networkSettings.ipAddress),
        m_mediaServer(fuppesConfig.globalSettings.uuid, this),
        m_contentDirectory(),
        m_connectionManager(),
        m_xmsMediaReceiverRegistrar(),

        m_transcoder(m_pluginManager, fuppesConfig.transcodingSettings),
        m_presentationHandler(fuppesConfig),
        m_controlInterface(this, fuppesConfig, m_contentDatabase)
{
    m_statusCallback = callback;

    m_status = STATUS_ERROR;

    m_pSSDPCtrl = NULL;
}

Core::~Core()
{
    m_RemoteDeviceIterator = m_RemoteDevices.begin();
    while (m_RemoteDeviceIterator != m_RemoteDevices.end()) {
        delete m_RemoteDeviceIterator->second;
        m_RemoteDeviceIterator++;
    }
    m_RemoteDevices.clear();
    CleanupTimedOutDevices();

    if (m_pSSDPCtrl) {
        delete m_pSSDPCtrl;
    }

    m_httpServer.close();
    m_subscriptionMgr.stop();
    CConnectionManagerCore::uninit();
    m_databaseConnection.close();
}

void Core::onHttpServerReady(Http::HttpServer* server)
{
    m_fuppesConfig.networkSettings.httpPort = server->getPort();
    m_fuppesConfig.networkSettings.baseUrl = server->getUrl();
    this->initHttpDependentServices();
}

void Core::onHttpServerError(Http::HttpServer* server, std::string error)
{
    setStatus(STATUS_ERROR, error);
}

Http::HttpSession* Core::createHttpSessionCallback(fuppes::TCPRemoteSocket* socket, void* userData) // static
{
    Core* fuppes = (Core*)userData;
    return new ::Fuppes::FuppesHttpSession(socket, fuppes->m_fuppesConfig, fuppes->m_accessControl, fuppes->m_deviceIdentMgr, fuppes->m_subscriptionMgr, fuppes->m_mediaServer, fuppes->m_transcoder, fuppes->m_presentationHandler, fuppes->m_controlInterface);
}

fuppes_bool_t Core::init()
{
    if (NULL == m_statusCallback) {
        return FUPPES_FALSE;
    }

    setStatus(STATUS_INITIALIZING);

    // load plugins
    if (!m_pluginManager.load(m_fuppesConfig.globalSettings.pluginDir)) {
        std::string error = "error loading plugins from: " + m_fuppesConfig.globalSettings.pluginDir;
        setStatus(STATUS_ERROR, error);
        return FUPPES_FALSE;
    }

    // create db connection
    m_databaseConnection.setParameters(m_fuppesConfig.databaseSettings);
    if (!m_databaseConnection.connect()) {
        log("core") << Log::Log::error << "error opening database" << m_fuppesConfig.databaseSettings.type.c_str();
        setStatus(STATUS_ERROR);
        return FUPPES_FALSE;
    }
    Database::Connection::setDefaultConnection(&m_databaseConnection);

    // setup db tables
    Database::TableManager tables;
    bool newdb = !tables.exist();
    if (newdb) {
        if (!tables.create(DB_VERSION)) {
            setStatus(STATUS_ERROR, "error creating database tables");
            return FUPPES_FALSE;
        }
    }

    if (!tables.isVersion(DB_VERSION)) {
        setStatus(STATUS_ERROR, "invalid database version");
        return FUPPES_FALSE;
    }

    // init the device identification mgr
    m_deviceIdentMgr.initialize();

    // init file details
    m_fileDetails.setBaseDevice(m_deviceIdentMgr.getBaseDevice());

    // init the content database
    m_contentDatabase.init();
    if (newdb) {
        m_contentDatabase.rebuild();
        //m_contentDatabase.RebuildDB(2000);
    }

    // init virtual containers
    m_virtualContainerMgr.init();

    // start the http server    
    m_httpServer.setCreateSessionCallback(&createHttpSessionCallback, this);
    m_httpServer.start();

    return FUPPES_TRUE;
}

void Core::initHttpDependentServices()
{
    // init SSDP-controller
    //CSharedLog::Log(L_EXT, __FILE__, __LINE__, "init ssdp-controller");
    m_pSSDPCtrl = new CSSDPCtrl(m_fuppesConfig.networkSettings.ipAddress, m_httpServer.getUrl(), this, m_deviceIdentMgr.getBaseDevice()->getHttpServerSignature(), m_fuppesConfig.globalSettings.uuid);
    if (!m_pSSDPCtrl->Start()) {
        return;
    }

    // init SubscriptionMgr
    m_subscriptionMgr.start();

    /** UPnP base device and services */

    // init MediaServer
    m_mediaServer.setHttpServerUrl(m_httpServer.getUrl());

    // init ContentDirectory
    m_contentDirectory.setHttpServerUrl(m_httpServer.getUrl());
    m_mediaServer.AddUPnPService(&m_contentDirectory);

    // init ConnectionManager
    m_connectionManager.setHttpServerUrl(m_httpServer.getUrl());
    m_mediaServer.AddUPnPService(&m_connectionManager);

    // init XMSMediaReceiverRegistrar
    m_xmsMediaReceiverRegistrar.setHttpServerUrl(m_httpServer.getUrl());
    m_mediaServer.AddUPnPService(&m_xmsMediaReceiverRegistrar);

    setStatus(STATUS_INITIALIZED);
}

void Core::start()
{
    setStatus(STATUS_STARTING);

    // start alive timer
    m_mediaServer.GetTimer()->SetInterval(60);
    m_mediaServer.GetTimer()->start();

    // if everything is up and running, multicast alive messages and search for other devices       
    m_pSSDPCtrl->send_alive();
    m_pSSDPCtrl->send_msearch();

    m_fuppesConfig.globalSettings.startTime = fuppes::DateTime::now();

    setStatus(STATUS_STARTED);
}

void Core::stop()
{
    setStatus(STATUS_STOPPING);

    //cout << "delete SSDP" << endl;
    if (m_pSSDPCtrl) {
        /* multicast notify-byebye */
        //CSharedLog::Shared()->Log(L_EXT, __FILE__, __LINE__, "multicasting byebye messages");
        m_pSSDPCtrl->send_byebye();

        /* stop SSDP-controller */
        //CSharedLog::Shared()->Log(L_EXT, __FILE__, __LINE__, "stopping SSDP controller");
        m_pSSDPCtrl->Stop();
    }

    //cout << "stop HTTP Server" << endl;
    /* stop HTTP-server */
    /*if(m_pHTTPServer) {
     CSharedLog::Shared()->Log(L_EXT, __FILE__, __LINE__, "stopping HTTP server");
     m_pHTTPServer->stop(); // stop thread execution
     }*/
    //if(m_httpServer) {
    //CSharedLog::Shared()->Log(L_EXT, __FILE__, __LINE__, "stopping HTTP server");
    m_httpServer.stop();

    setStatus(STATUS_STOPPED);
}

void Core::setStatus(fuppes_status_t status, std::string msg /*= ""*/)
{
    m_status = status;
    m_statusCallback(m_status, msg.empty() ? NULL : msg.c_str());
}

fuppes_status_t Core::getStatus()
{
    return m_status;
}

void Core::CleanupTimedOutDevices()
{
    Threading::MutexLocker locker(&m_RemoteDevicesMutex);

    if (m_TimedOutDevices.size() == 0) {
        return;
    }

    // iterate device list ...
    for (m_TimedOutDevicesIterator = m_TimedOutDevices.begin(); m_TimedOutDevicesIterator != m_TimedOutDevices.end(); m_TimedOutDevicesIterator++) {
        // ... and delete timed out devices
        delete *m_TimedOutDevicesIterator;
    }
    m_TimedOutDevices.clear();
}

void Core::OnTimer(CUPnPDevice* pSender)
{
    Threading::MutexLocker locker(&m_OnTimerMutex);

    CleanupTimedOutDevices();

    // local device must send alive message
    if (pSender->isLocalDevice()) {

        CSharedLog::Log(L_EXT, __FILE__, __LINE__, "device: %s send timed alive", +pSender->GetUUID().c_str());
        m_pSSDPCtrl->send_alive();

        m_mediaServer.GetTimer()->reset();
    }

    // remote device timed out
    else {
        if (!pSender->GetFriendlyName().empty()) {
            CSharedLog::Log(L_EXT, __FILE__, __LINE__, "device: %s timed out", pSender->GetFriendlyName().c_str());
        }

        m_RemoteDevicesMutex.lock();

        m_RemoteDeviceIterator = m_RemoteDevices.find(pSender->GetUUID());
        if (m_RemoteDeviceIterator != m_RemoteDevices.end()) {

            // remove device from list of remote devices
            m_RemoteDevices.erase(pSender->GetUUID());
            // stop the device's timer and HTTP client and ...
            pSender->GetTimer()->stop();
            // ... push it to the list containing timed out devices
            m_TimedOutDevices.push_back(pSender);
        }

        m_RemoteDevicesMutex.unlock();

    }

}

/** Returns URL of the HTTP member
 * @return std::string
 */
std::string Core::GetHTTPServerURL()
{
    //assert(m_httpServer != NULL);
    return m_httpServer.getUrl();
}

/** Returns IP address 
 *  @return std::string
 */
std::string Core::GetIPAddress()
{
    return m_fuppesConfig.networkSettings.ipAddress;
}

std::string Core::GetUUID()
{
    return m_fuppesConfig.globalSettings.uuid;
}

/** Returns a UPnP device
 *  @return std::vector<CUPnPDevice*>
 */
std::vector<CUPnPDevice*> Core::GetRemoteDevices()
{
    std::vector<CUPnPDevice*> vResult;

    /* Iterate devices */
    for (m_RemoteDeviceIterator = m_RemoteDevices.begin(); m_RemoteDeviceIterator != m_RemoteDevices.end(); m_RemoteDeviceIterator++) {
        vResult.push_back((*m_RemoteDeviceIterator).second);
    }

    return vResult;
}

void Core::onSSDPCtrlReady(CSSDPCtrl* ssdpCtrl)
{
    setStatus(STATUS_INITIALIZED);
}

void Core::onSSDPCtrlError(CSSDPCtrl* ssdpCtrl, std::string error)
{
    setStatus(STATUS_ERROR, error);
}

void Core::onSSDPCtrlReceiveMsg(CSSDPMessage* pMessage)
{
//  cout << inet_ntoa(pMessage->GetRemoteEndPoint().sin_addr) << ":" << ntohs(pMessage->GetRemoteEndPoint().sin_port) << endl;  
    CSharedLog::Log(L_DBG, __FILE__, __LINE__, "OnSSDPCtrlReceiveMsg\n%s", pMessage->GetMessage().c_str());

    if ((m_fuppesConfig.networkSettings.ipAddress.compare(inet_ntoa(pMessage->GetRemoteEndPoint().sin_addr)) != 0) || (pMessage->GetUUID().compare(m_fuppesConfig.globalSettings.uuid) != 0)) {
        switch (pMessage->GetMessageType()) {
            case SSDP_MESSAGE_TYPE_NOTIFY_ALIVE:
                //CSharedLog::Shared()->ExtendedLog(LOGNAME, "SSDP_MESSAGE_TYPE_NOTIFY_ALIVE");
                HandleSSDPAlive(pMessage);
                break;
            case SSDP_MESSAGE_TYPE_M_SEARCH_RESPONSE:
                //CSharedLog::Shared()->ExtendedLog(LOGNAME, "SSDP_MESSAGE_TYPE_M_SEARCH_RESPONSE");
                HandleSSDPAlive(pMessage);
                break;
            case SSDP_MESSAGE_TYPE_NOTIFY_BYEBYE:
                //CSharedLog::Shared()->ExtendedLog(LOGNAME, "SSDP_MESSAGE_TYPE_NOTIFY_BYEBYE");
                HandleSSDPByeBye(pMessage);
                break;
            case SSDP_MESSAGE_TYPE_M_SEARCH:
                /* m-search is handled by SSDPCtrl */
                break;
            case SSDP_MESSAGE_TYPE_UNKNOWN:
                /* this should never happen :) */
                break;
        }
    }
}

void Core::HandleSSDPAlive(CSSDPMessage* pMessage)
{
    m_RemoteDevicesMutex.lock();

    m_RemoteDeviceIterator = m_RemoteDevices.find(pMessage->GetUUID());

    // known device
    if (m_RemoteDeviceIterator != m_RemoteDevices.end()) {

        m_RemoteDevices[pMessage->GetUUID()]->GetTimer()->reset();
        /*CSharedLog::Log(L_EXT, __FILE__, __LINE__,
         "received \"Notify-Alive\" from known device id: %s", pMessage->GetUUID().c_str());*/
    }
    // new device
    else {
        CSharedLog::Log(L_EXT, __FILE__, __LINE__, "received \"Notify-Alive\" from unknown device id: %s", pMessage->GetUUID().c_str());

        if ((pMessage->GetLocation().compare("") == 0) || (pMessage->GetUUID().compare("") == 0)) {
            m_RemoteDevicesMutex.unlock();
            return;
        }

        CUPnPDevice* pDevice = new CUPnPDevice(m_fuppesConfig.networkSettings.ipAddress, this, pMessage->GetUUID());
        m_RemoteDevices[pMessage->GetUUID()] = pDevice;
        pDevice->GetTimer()->SetInterval(30); // wait max. 30 sec. for description
        pDevice->GetTimer()->start();
        pDevice->BuildFromDescriptionURL(pMessage->GetLocation());
    }

    m_RemoteDevicesMutex.unlock();
}

void Core::HandleSSDPByeBye(CSSDPMessage* pMessage)
{
    CSharedLog::Shared()->Log(L_EXT, __FILE__, __LINE__, "received \"Notify-ByeBye\" from device: %s", pMessage->GetUUID().c_str());

    m_RemoteDevicesMutex.lock();

    m_RemoteDeviceIterator = m_RemoteDevices.find(pMessage->GetUUID());
    if (m_RemoteDeviceIterator != m_RemoteDevices.end()) {

        CSharedLog::Log(L_EXT, __FILE__, __LINE__, "received byebye from %s :: %s", m_RemoteDevices[pMessage->GetUUID()]->GetFriendlyName().c_str(), m_RemoteDevices[pMessage->GetUUID()]->GetUPnPDeviceTypeAsString().c_str());

        /*CSharedLog::Shared()->UserNotify(m_RemoteDevices[pMessage->GetUUID()]->GetFriendlyName(),
         "UPnP device gone:\n" + m_RemoteDevices[pMessage->GetUUID()]->GetFriendlyName() + " (" + m_RemoteDevices[pMessage->GetUUID()]->GetUPnPDeviceTypeAsString() + ")");*/

        delete m_RemoteDevices[pMessage->GetUUID()];
        m_RemoteDevices.erase(pMessage->GetUUID());
    }

    m_RemoteDevicesMutex.unlock();
}

