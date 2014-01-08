/*
 * This file is part of fuppes
 *
 * (c) 2005-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _FUPPES_CORE_H
#define _FUPPES_CORE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <map>
#include <vector>

#include "../Configuration/FuppesConfig.h"

#include "../Plugin/Manager.h"
#include "../Database/Connection.h"

#include "../Security/AccessControl.h"
#include "../DeviceIdentification/IdentificationMgr.h"
#include "../ContentDatabase/FileDetails.h"
#include "../ContentDatabase/ContentDatabase.h"
#include "../VirtualContainer/VirtualContainerMgr.h"

#include "../Http/HttpServer.h"

#include "../SSDP/SSDPCtrl.h"
#include "../GENA/SubscriptionMgr.h"

#include "../MediaServer/MediaServer.h"
#include "../ContentDirectory/ContentDirectory.h"
#include "../ConnectionManager/ConnectionManager.h"
#include "../XMSMediaReceiverRegistrar/XMSMediaReceiverRegistrar.h"

#include "../Transcoding/Transcoder.h"
#include "../Presentation/PresentationHandler.h"
#include "../ControlInterface/ControlInterface.h"

#include "../../../include/fuppes.h"

namespace Fuppes
{

    class Core: public ISSDPCtrl,
            IUPnPDevice,
            public Http::HttpServerEventReceiver
    {
        public:
            Core(fuppes_status_callback_t callback, Fuppes::FuppesConfig& fuppesConfig);
            virtual ~Core();

            fuppes_bool_t init();
            void start();
            void stop();

            fuppes_status_t getStatus();

            void OnTimer(CUPnPDevice* pSender);
            //void onUPnPDeviceDeviceReady(std::string uuid);

            Fuppes::FuppesConfig* getConfig()
            {
                return &m_fuppesConfig;
            }

            ControlInterface* getControlInterface()
            {
                return &m_controlInterface;
            }

            CSSDPCtrl* GetSSDPCtrl()
            {
                return m_pSSDPCtrl;
            }

            CContentDirectory* GetContentDirectory()
            {
                return &m_contentDirectory;
            }

            std::string GetHTTPServerURL();
            std::string GetIPAddress();
            std::vector<CUPnPDevice*> GetRemoteDevices();
            std::string GetUUID();

            ContentDatabase::ContentDatabase* getContentDatabase()
            {
                return &m_contentDatabase;
            }

            VirtualContainer::Manager* getVirtualContainerMgr()
            {
                return &m_virtualContainerMgr;
            }

            static Http::HttpSession* createHttpSessionCallback(fuppes::TCPRemoteSocket* socket, void* userData);

        private:
            fuppes_status_t m_status;
            fuppes_status_callback_t m_statusCallback;
            void setStatus(fuppes_status_t status, std::string msg = "");

            void onHttpServerReady(Http::HttpServer* server);
            void onHttpServerError(Http::HttpServer* server, std::string error);

            void initHttpDependentServices();

            Threading::Mutex m_OnTimerMutex;

            // config
            //CSharedConfig& m_config;
            Fuppes::FuppesConfig& m_fuppesConfig;

            /** dependencies in the order of usage */

            // load the configuration
            // init plugin mgr
            Plugin::Manager m_pluginManager;

            // get database plugin and establish connection
            Database::Connection m_databaseConnection;

            // start access control
            Security::AccessControl m_accessControl;

            // device ident mgr
            DeviceIdentification::IdentificationMgr m_deviceIdentMgr;

            // file details
            CFileDetails m_fileDetails;

            // setup the virtual container mgr
            VirtualContainer::Manager m_virtualContainerMgr;

            // init the content db
            ContentDatabase::ContentDatabase m_contentDatabase;

            // start the http server (depends on AccessControl and DeviceIdentificationMgr)
            Http::HttpServer m_httpServer;

            // everything below this line is initialized after the http server is started
            // because it depends on it somehow (usually it needs the base url)

            // start the ssdp service
            CSSDPCtrl *m_pSSDPCtrl;

            // start the subscription manager
            CSubscriptionMgr m_subscriptionMgr;

            // setup the media server
            MediaServer m_mediaServer;

            // setup the content directory
            CContentDirectory m_contentDirectory;

            // setup connection manager
            CConnectionManager m_connectionManager;

            // setup ms media receiver registrar
            XMSMediaReceiverRegistrar m_xmsMediaReceiverRegistrar;

            // transcoder
            Transcoding::Transcoder m_transcoder;

            // presentation
            Presentation::PresentationHandler m_presentationHandler;

            // setup the control interface
            ControlInterface m_controlInterface;

            std::map<std::string, CUPnPDevice*> m_RemoteDevices;
            std::map<std::string, CUPnPDevice*>::iterator m_RemoteDeviceIterator;
            Threading::Mutex m_RemoteDevicesMutex;

            std::vector<CUPnPDevice*> m_TimedOutDevices;
            std::vector<CUPnPDevice*>::iterator m_TimedOutDevicesIterator;

            void CleanupTimedOutDevices();

            void onSSDPCtrlReady(CSSDPCtrl* ssdpCtrl);
            void onSSDPCtrlError(CSSDPCtrl* ssdpCtrl, std::string error);

            void onSSDPCtrlReceiveMsg(CSSDPMessage* ssdpMessage);
            void HandleSSDPAlive(CSSDPMessage* pMessage);
            void HandleSSDPByeBye(CSSDPMessage* pMessage);

    };

}

#endif // _FUPPES_CORE_H
