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

#include <iostream>
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


CacheItem* Cache::getItem(Transcoding::Item& item)
{
	Threading::MutexLocker locker(&m_mutex);

	std::string key = item.getCacheIdentifier();

	CacheItem* ci = m_cachedItems[key];
	if (NULL == ci) {
		ci = new CacheItem();
		ci->m_item = &item;
		m_cachedItems[key] = ci;
	}
	ci->m_refCount++;

	log("transcoding cache") << Log::Log::debug << "get item: " << ci->m_refCount << "::" << key;
	return ci;
}

void Cache::releaseItem(CacheItem* item)
{
	Threading::MutexLocker locker(&m_mutex);

	log("transcoding cache") << Log::Log::debug << "release item: " << item->m_refCount << "::" << item->m_item->getCacheIdentifier();

	item->m_refCount--;
	if (0 == item->m_refCount) {

		item->m_relCount = item->m_item->getDeviceSettings()->ReleaseDelay(item->m_item->originalExtension);
		if(!this->running()) {
			this->close();
			this->start();
		}
	}

}

/*
CTranscodingCacheObject* Cache::GetCacheObject(std::string p_sFileName, CDeviceSettings* deviceSettings)
{
	Threading::MutexLocker locker(&m_mutex);

	CTranscodingCacheObject* obj = m_CachedObjects[p_sFileName + deviceSettings->name()];
	if (NULL == obj) {
		obj = new CTranscodingCacheObject(m_pluginManager, deviceSettings);
		m_CachedObjects[p_sFileName + deviceSettings->name()] = obj;
		obj->m_sInFileName = p_sFileName;
	}

	obj->m_nRefCount++;
	obj->ResetReleaseCount();

	return obj;
}

void Cache::ReleaseCacheObject(CTranscodingCacheObject* pCacheObj)
{
    Threading::MutexLocker locker(&m_mutex);

	if(!this->running()) {
		this->start();
	}

	stringstream sLog;
	sLog << "release object \"" << pCacheObj->m_sInFileName << "\"" << endl <<
				"ref count: " << pCacheObj->m_nRefCount << endl <<
				"delay: " << pCacheObj->ReleaseCountOrig();
	log("transcoding", Log::Log::extended) << sLog.str();

	pCacheObj->m_nRefCount--;
  
  // todo: pause transcoding if ref count == 0
  
}
*/

void Cache::run()
{
    CacheItem* item;
    std::map<std::string, CacheItem*>::iterator iter;
  
	while(0 < m_cachedItems.size()) {
	    msleep(1000);
    
	    m_mutex.lock();
   
	    for(m_cachedItemsIterator = m_cachedItems.begin(); m_cachedItemsIterator != m_cachedItems.end(); ) {
	    	item = (*m_cachedItemsIterator).second;
      
            if(0 == item->m_refCount && 0 == item->m_relCount) {
            	iter = m_cachedItemsIterator;
            	iter++;

                if(item->m_item->isThreaded() && !item->finished()) {
                	item->stop();
                    m_cachedItemsIterator++;
                    continue;
                }

                log("transcoding cache") << Log::Log::debug << "delete item:" << (*m_cachedItemsIterator).first;

				/*stringstream sLog;
				sLog << "delete object \"" << pCacheObj->m_sInFileName << "\"" << endl <<
				"delay: " << pCacheObj->ReleaseCountOrig();
		
				CSharedLog::Log(L_EXT, __FILE__, __LINE__, sLog.str().c_str());*/
						 
				m_cachedItems.erase(m_cachedItemsIterator);
				item->stop();
				item->close();
				delete item;
				m_cachedItemsIterator = iter;
            }
            else if(0 == item->m_refCount) {
            	item->m_relCount--;
                m_cachedItemsIterator++;
            }
            else {
            	m_cachedItemsIterator++;
            }
	    }
    
	    m_mutex.unlock();
	}

}
