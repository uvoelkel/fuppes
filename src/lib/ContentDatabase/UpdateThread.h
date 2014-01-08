/*
 * This file is part of fuppes
 *
 * (c) 2010-2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _CONTENTDATABASE_UPDATETHREAD_H
#define _CONTENTDATABASE_UPDATETHREAD_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Configuration.h"
#include "FileDetails.h"
#include "../Thread/Thread.h"
#include "../Plugin/Manager.h"
#include "../Database/Connection.h"
#include "../Database/ObjectManager.h"
#include "../VirtualContainer/VirtualContainerMgr.h"

namespace ContentDatabase
{

    class ContentDatabase;

    class UpdateThread: public Threading::Thread
    {
        public:
            UpdateThread(ContentDatabase& contentDatabase, Configuration& configuration, CFileDetails& fileDetails, Plugin::Manager& pluginManager, VirtualContainer::Manager& virtualContainerMgr);
            ~UpdateThread();

        private:
            //inline bool famEventOccurred();
            void run();

            int updateItemsMetadata(Database::ObjectManager& objMgr);

            void updateAudioFile(Database::ObjectManager& objMgr, Database::Item& item);
            void updateVideoFile(Database::ObjectManager& objMgr, Database::Item& item);
            void updateImageFile(Database::ObjectManager& objMgr, Database::Item& item);
            //void updateBroadcastUrl(DbObject* obj, SQLQuery* qry);

            int updateAlbumArt(Database::ObjectManager& objMgr); //SQLQuery* qry, SQLQuery* get, SQLQuery* ins);

            int createVideoThumbnails(Database::ObjectManager& objMgr);

            //void parsePlaylists(SQLQuery* qry, SQLQuery* get, SQLQuery* ins);

            //CContentDatabase* m_contentDb;
            //Fuppes::FuppesConfig& m_fuppesConfig;

            ContentDatabase& m_contentDatabase;
            Configuration& m_configuration;

            CFileDetails& m_fileDetails;
            Plugin::Manager& m_pluginManager;
            VirtualContainer::Manager& m_virtualContainerMgr;
            //FileAlterationHandler& m_famHandler;

            Database::Connection* m_connection;
    };

}

#endif // _CONTENTDATABASE_UPDATETHREAD_H
