/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _DATABASE_TABLEMANAGER_H
#define _DATABASE_TABLEMANAGER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_database_plugin.h>

namespace Database
{
    class TableManager: public AbstractTableManager
    {
        public:
            TableManager();
            ~TableManager();

            bool exist();
            bool isVersion(int version);
            bool create(int version);
            bool empty();

        private:
            AbstractTableManager* m_tableManager;
    };
}

#endif // _DATABASE_OBJECTMANAGER_H
