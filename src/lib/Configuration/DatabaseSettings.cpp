/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "DatabaseSettings.h"
//#include "../SharedConfig.h"

#include "../Common/Common.h"
#include "../Common/Directory.h"
#include "../Common/File.h"

using namespace std;
//using namespace fuppes;

using namespace Configuration;

#define DBFILE_NAME "fuppes.db"

DatabaseSettings::DatabaseSettings()
{
    readonly = false;
    type = "sqlite3";
}

void DatabaseSettings::readConfig(ConfigFile& configFile) throw (Configuration::ReadException)
{
    type = configFile.getStringValue("/fuppes_config/database/type/text()");

    // for sqlite
    filename = configFile.getStringValue("/fuppes_config/database/file/text()");

    // for mysql
    hostname = configFile.getStringValue("/fuppes_config/database/hostname/text()");
    username = configFile.getStringValue("/fuppes_config/database/username/text()");
    password = configFile.getStringValue("/fuppes_config/database/password/text()");
    dbname = configFile.getStringValue("/fuppes_config/database/dbname/text()");

    // common
    readonly = configFile.getBoolValue("/fuppes_config/database/dbname/text()");
}

void DatabaseSettings::setup(PathFinder& pathFinder) throw (Configuration::SetupException)
{

    if(0 == type.compare("sqlite3")) {

        if(filename.empty()) {

            // check if there is an existing fuppes.db
            int flags = fuppes::File::Readable;
            if(!readonly)
                flags |= fuppes::File::Writable;

            string path = pathFinder.findInDataPath(DBFILE_NAME, flags);
            if(!path.empty()) {
                filename = path;
            }
            else {

                // if none exists find the first writable data dir
                path = pathFinder.findDataPath(fuppes::Directory::Readable | fuppes::Directory::Writable);
                if(path.empty()) {
                    throw Configuration::SetupException("no writable data directory found");
                }

                path += DBFILE_NAME;
                filename = path;

            }

        }

    }
    else if(0 == type.compare("mysql")) {

    }
    else {
        throw Configuration::SetupException("no valid database type selected");
    }
}

void DatabaseSettings::writeConfig(ConfigFile& configFile) throw (Configuration::WriteException)
{

}

/*


 DatabaseSettings::DatabaseSettings()
 :ConfigSettings()
 {
 m_dbConnectionParams.readonly = false;
 m_dbConnectionParams.filename = "";
 m_dbConnectionParams.type = "sqlite3";
 }



 bool DatabaseSettings::InitPostRead() {

 // set dbfilename if necessary
 if(dbConnectionParams().type == "sqlite3" && dbConnectionParams().filename.empty()) {

 // check if there is an existing fuppes.db
 int flags = File::Readable;
 if(!m_dbConnectionParams.readonly)
 flags |= File::Writable;

 string path = ""; //PathFinder::instance()->findInDataPath(DBFILE_NAME, flags);
 if(!path.empty()) {
 setDbFilename(path);
 return true;
 }

 // if none exists find the first writable data dir
 path = m_fuppesConfig->pathFinder.findDataPath(Directory::Readable | Directory::Writable);
 if(path.empty()) {
 return false;
 }

 path += DBFILE_NAME;
 setDbFilename(path);

 */

/*
 #ifdef WIN32
 setDbFilename(string(getenv("APPDATA")) + "\\fuppes\\fuppes.db");
 #else
 if(Directory::writable(Directory::appendTrailingSlash(FUPPES_LOCALSTATEDIR)))
 setDbFilename(Directory::appendTrailingSlash(FUPPES_LOCALSTATEDIR) + "fuppes.db");
 else
 setDbFilename(string(getenv("HOME")) + "/.fuppes/fuppes.db");
 #endif
 */

/*
 }

 return true;
 }

 bool DatabaseSettings::UseDefaultSettings(void) {
 return InitPostRead();
 }

 bool DatabaseSettings::Read(void)
 {
 m_dbConnectionParams.type = ToLower(m_config->getAttribute("database", "type"));

 // for sqlite
 m_dbConnectionParams.filename = m_config->getValue("database", "file");

 // for mysql
 m_dbConnectionParams.hostname = m_config->getValue("database", "hostname");
 m_dbConnectionParams.username = m_config->getValue("database", "username");
 m_dbConnectionParams.password = m_config->getValue("database", "password");
 m_dbConnectionParams.dbname   = m_config->getValue("database", "dbname");

 // common
 m_dbConnectionParams.readonly = (m_config->getValue("database", "readonly").compare("true") == 0);

 return true;
 */

/*
 assert(pStart != NULL);
 m_dbConnectionParams.type = ToLower(pStart->Attribute("type"));

 CXMLNode* pTmp;
 for(int i = 0; i < pStart->ChildCount(); i++) {
 pTmp = pStart->ChildNode(i);

 if(pTmp->Name().compare("filename") == 0) {
 m_dbConnectionParams.filename = pTmp->Value();
 }

 else if(pTmp->Name().compare("hostname") == 0) {
 m_dbConnectionParams.hostname = pTmp->Value();
 }
 else if(pTmp->Name().compare("username") == 0) {
 m_dbConnectionParams.username = pTmp->Value();
 }
 else if(pTmp->Name().compare("password") == 0) {
 m_dbConnectionParams.password = pTmp->Value();
 }
 else if(pTmp->Name().compare("dbname") == 0) {
 m_dbConnectionParams.dbname = pTmp->Value();
 }
 else if(pTmp->Name().compare("readonly") == 0) {
 m_dbConnectionParams.readonly = (pTmp->Value() == "true");
 }
 }
 */
/*
 }

 */
