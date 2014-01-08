/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _CONTENTDATABASE_SCANDIRECTORYTHREAD_H
#define _CONTENTDATABASE_SCANDIRECTORYTHREAD_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Thread/Thread.h"
#include "../Database/Connection.h"

namespace ContentDatabase
{
    class ContentDatabase;

    class ScanDirectoryThread: public Threading::Thread
    {
        public:
            ScanDirectoryThread(ContentDatabase& contentDatabase);
            ~ScanDirectoryThread();

            //bool start(Fam::Monitor* fam);

        protected:
            void run();

        private:
            int checkDirectories();

            Database::Connection* m_connection;
            ContentDatabase& m_contentDatabase;
            //Fam::Monitor* m_fam;
    };

}

#endif // _CONTENTDATABASE_SCANDIRECTORYTHREAD_H
