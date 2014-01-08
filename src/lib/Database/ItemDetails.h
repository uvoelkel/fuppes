/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _DATABASE_ITEMDETAILS_H
#define _DATABASE_ITEMDETAILS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_database_objects.h>

namespace Database
{
    class ItemDetails: public Database::AbstractItemDetails
    {
        public:
            ItemDetails();

            void clear();
    };
}

#endif // _DATABASE_ITEMDETAILS_H
