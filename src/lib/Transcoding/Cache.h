/*
 * This file is part of fuppes
 *
 * (c) 2006-2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _TRANSCODING_CACHE_H
#define _TRANSCODING_CACHE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Thread/Thread.h"
#include "../Plugin/Manager.h"

#include "Item.h"
#include "CacheItem.h"

#include <map>
#include <string>

namespace Transcoding
{
    class Cache: private Threading::Thread
    {
        public:
    		Cache(Plugin::Manager& pluginManager);
            ~Cache();

            CacheItem* getItem(Transcoding::Item& item);
            void releaseItem(CacheItem* item);

      private:
            Plugin::Manager&     m_pluginManager;

            Threading::Mutex                            	m_mutex;
            std::map<std::string, CacheItem*>           	m_cachedItems;
			std::map<std::string, CacheItem*>::iterator		m_cachedItemsIterator;

            void run();
    };

}

#endif // _TRANSCODING_CACHE_H
