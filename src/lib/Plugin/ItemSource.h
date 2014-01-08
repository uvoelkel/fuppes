/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PLUGIN_ITEMSOURCE_H
#define _PLUGIN_ITEMSOURCE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_database_objects.h>

#include "Plugin.h"

namespace Plugin
{
    typedef int (*sourceOpen_t)(const char* source, void** data);
    typedef int (*sourceNext_t)(::Database::AbstractItem& item, void** data);
    typedef void (*sourceClose_t)(void** data);

    class ItemSource: public Plugin
    {
        public:
            ItemSource();
            ItemSource(ItemSource* plugin);
            bool init(LibraryHandle handle);

            bool open(const std::string source);
            bool getNext(::Database::AbstractItem& item);
            void close();


        private:
            sourceOpen_t     m_sourceOpen;
            sourceNext_t     m_sourceNext;
            sourceClose_t    m_sourceClose;
    };
}

#endif // _PLUGIN_ITEMSOURCE_H
