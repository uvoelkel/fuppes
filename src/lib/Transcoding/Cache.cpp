/*
 * This file is part of fuppes
 *
 * (c) 2006-2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Cache.h"
#include "Item.h"

#include "../Log/Log.h"

#include <fstream>
#include <string.h>

using namespace std;

using namespace Transcoding;

Cache::Cache(Plugin::Manager& pluginManager):
        Threading::Thread("TranscodingCache"),
        m_pluginManager(pluginManager)
{
}

Cache::~Cache()
{
	// stop transcoding for all cached items
    for(m_cachedItemsIterator = m_cachedItems.begin(); m_cachedItemsIterator != m_cachedItems.end(); m_cachedItemsIterator++) {
        (*m_cachedItemsIterator).second->stop();
    }

	// stop cleanup
	if (this->running()) {
		this->stop();
	}
    this->close();

	// close and remove all cached items
    for(m_cachedItemsIterator = m_cachedItems.begin(); m_cachedItemsIterator != m_cachedItems.end(); m_cachedItemsIterator++) {
        (*m_cachedItemsIterator).second->close();
        delete (*m_cachedItemsIterator).second;
    }
    m_cachedItems.clear();

}


CacheItem* Cache::getItem(const std::string identifier, const unsigned int releaseDelay /*= 5*/)
{
	Threading::MutexLocker locker(&m_mutex);

	CacheItem* ci = m_cachedItems[identifier];
	if (NULL == ci) {
		ci = new CacheItem(identifier);
		ci->m_releaseDelay = releaseDelay;
		m_cachedItems[identifier] = ci;
	}
	ci->m_referenceCount++;

	if (ci->isPaused()) {
		ci->resume();
	}

	log("transcoding cache") << Log::Log::debug << "get item: " << ci->m_referenceCount << "::" << identifier;
	return ci;
}

void Cache::releaseItem(CacheItem* item)
{
	Threading::MutexLocker locker(&m_mutex);

	log("transcoding cache") << Log::Log::debug << "release item: " << item->m_referenceCount << "::" << item->m_identifier;

	item->m_referenceCount--;
	if (0 == item->m_referenceCount) {

		if (item->canPause()) {
			item->pause();
		}

		item->m_releaseCount = item->m_releaseDelay;
		if(!this->running()) {
			this->close();
			this->start();
		}
	}

}



void Cache::run()
{
    CacheItem* item;
    std::map<std::string, CacheItem*>::iterator iter;
  
	while(0 < m_cachedItems.size()) {
	    msleep(1000);
    
	    m_mutex.lock();
   
	    for(m_cachedItemsIterator = m_cachedItems.begin(); m_cachedItemsIterator != m_cachedItems.end(); ) {
	    	item = (*m_cachedItemsIterator).second;
      
            if(0 == item->m_referenceCount && 0 == item->m_releaseCount) {
            	iter = m_cachedItemsIterator;
            	iter++;

                if(item->m_threaded && !item->finished()) {
                	item->stop();
                    m_cachedItemsIterator++;
                    continue;
                }

                log("transcoding cache") << Log::Log::debug << "delete item:" << (*m_cachedItemsIterator).first;

				m_cachedItems.erase(m_cachedItemsIterator);
				item->stop();
				item->close();
				delete item;
				m_cachedItemsIterator = iter;
            }
            else if(0 == item->m_referenceCount) {
            	item->m_releaseCount--;
                m_cachedItemsIterator++;
            }
            else {
            	m_cachedItemsIterator++;
            }
	    }
    
	    m_mutex.unlock();
	}

}
