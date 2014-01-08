/*
 * This file is part of fuppes
 *
 * (c) 2010-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _CONFIGURATION_SHARED_OBJECTS
#define _CONFIGURATION_SHARED_OBJECTS

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BaseSettings.h"
#include "../ContentDatabase/SharedObject.h"

#include <string>
#include <vector>
#include "../Common/UUID.h"


namespace Configuration
{
    class SharedObject: public ::ContentDatabase::SharedObject
    {
        public:
            SharedObject() {
                //type = SharedObject::unknown;
                uuid = GenerateUUID();
                xmlNode = NULL;
            }

            /*enum SharedObjectType {
              unknown     = 0,
              directory   = 1,
              playlist    = 2,
              other       = 3,
            };*/

            std::string       uuid;
            /*SharedObjectType  type;
            std::string       path;

            std::string       name;
            std::string       otherType;*/

            // itunes settings
            std::string       rewritePathOld;
            std::string       rewritePathNew;

            xmlNodePtr        xmlNode;
    };


    class SharedObjects: public BaseSettings
    {
        public:
            void readConfig(ConfigFile& configFile) throw (Configuration::ReadException);
            void setup(PathFinder& pathFinder) throw (Configuration::SetupException);
            void writeConfig(ConfigFile& configFile) throw (Configuration::WriteException);

            size_t count();
            ::Configuration::SharedObject* getSharedObject(size_t index);
            ::Configuration::SharedObject* getSharedObject(std::string uuid);
            ::Configuration::SharedObject* add(::ContentDatabase::SharedObject obj);

        //private:
            std::vector<SharedObject>  m_sharedObjects;
    };

}


/*
#include "ConfigSettings.h"


class SharedObjects : public ConfigSettings
{
  public:
    virtual ~SharedObjects();
    virtual bool Read(void);

    int sharedObjectCount() { return m_sharedObjects.size(); }
    SharedObject* sharedObject(int idx) { return m_sharedObjects[idx]; }
    SharedObject* sharedObject(std::string uuid) { 
      for(m_sharedObjectsIter = m_sharedObjects.begin();
        m_sharedObjectsIter != m_sharedObjects.end();
        m_sharedObjectsIter++) {
        if((*m_sharedObjectsIter)->uuid() == uuid)
          return *m_sharedObjectsIter;
      }
      return NULL;
    }
    SharedObject* addSharedObject(SharedObject::SharedObjectType type, std::string path, std::string name = "", std::string otherType = "");
    void removeSharedObject(std::string uuid);
    
  private:
    void clear();

    std::vector<SharedObject*>  m_sharedObjects;
    std::vector<SharedObject*>::iterator  m_sharedObjectsIter;
};
*/
#endif // _CONFIGURATION_SHARED_OBJECTS
