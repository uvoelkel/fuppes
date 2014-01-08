/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "ContentDatabase.h"

void Configuration::ContentDatabase::readConfig(::Configuration::ConfigFile& configFile) throw (::Configuration::ReadException)
{

}
void Configuration::ContentDatabase::setup(PathFinder& pathFinder) throw (::Configuration::SetupException)
{
    throw SetupException("please use ContentDatabase::setup(PathFinder& pathFinder, DatabaseSettings& databaseSettings, GlobalSettings& globalSettings, SharedObjects& sharedObjects)");
}

void Configuration::ContentDatabase::setup(PathFinder& pathFinder, DatabaseSettings& databaseSettings, GlobalSettings& globalSettings, SharedObjects& sharedObjects) throw (::Configuration::SetupException)
{
    m_pathFinder = &pathFinder;
    m_databaseSettings = &databaseSettings;
    m_globalSettings = &globalSettings;
    m_sharedObjects = &sharedObjects;
}

void Configuration::ContentDatabase::writeConfig(ConfigFile& configFile) throw (::Configuration::WriteException)
{

}


bool Configuration::ContentDatabase::isReadonly()
{
    return m_databaseSettings->readonly;
}

const std::string Configuration::ContentDatabase::getLocalCharset()
{
    return m_globalSettings->localCharset;
}
const std::string Configuration::ContentDatabase::getThumbnailDirectory()
{
    return m_pathFinder->findThumbnailsDir();
}

::ContentDatabase::SharedObjects_t Configuration::ContentDatabase::getSharedObjects()
{
    ::ContentDatabase::SharedObjects_t result;

    for(size_t i = 0; i < m_sharedObjects->m_sharedObjects.size(); i++) {
       result.push_back(m_sharedObjects->m_sharedObjects.at(i));
    }
    return result;
}
