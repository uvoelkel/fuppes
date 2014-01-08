/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _DATABASE_ITEM_H
#define _DATABASE_ITEM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_database_objects.h>

namespace Database
{
    class Item: public Database::AbstractItem
    {
        public:
            Item();
            ~Item();

            Database::Item& operator=(Database::Item& item);
            void clear();

            std::string getObjectIdAsHex() const;
            std::string getParentIdAsHex() const;
            std::string getRefIdAsHex() const;

            bool isAudio() const;
            bool isVideo() const;
            bool isImage() const;
            //int getChildCount(std::string objectTypes) const;
    };
}

#endif // _DATABASE_ITEM_H
