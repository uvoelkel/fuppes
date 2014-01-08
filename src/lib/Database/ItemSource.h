/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _DATABASE_ITEMSOURCE_H
#define _DATABASE_ITEMSOURCE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_database_objects.h>

namespace Database
{
    class ItemSource: public Database::AbstractItemSource
    {
        public:
            ItemSource();

            void clear();
    };
}

#endif // _DATABASE_ITEMSOURCE_H
