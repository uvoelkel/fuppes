/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _CONTENTDATABASE_REMOVEMISSINGTHREAD_H
#define _CONTENTDATABASE_REMOVEMISSINGTHREAD_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Thread/Thread.h"
#include "../Database/Connection.h"

namespace ContentDatabase
{

    class ContentDatabase;

    class RemoveMissingThread: public Threading::Thread
    {
        public:
            RemoveMissingThread(ContentDatabase& contentDatabase);
            ~RemoveMissingThread();

        protected:
            void run();

        private:
            ContentDatabase& m_contentDatabase;
            Database::Connection* m_connection;
    };

}

#endif // _CONTENTDATABASE_REMOVEMISSINGTHREAD_H
