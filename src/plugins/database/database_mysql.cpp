/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            core_mysql.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2009-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */




/*
if THREAD_SAFE_CLIENT is defined then fuppes uses the same connection for all queries
otherwise it uses a single connection per query 
*/
#define THREAD_SAFE_CLIENT 1
//#undef THREAD_SAFE_CLIENT

#include "../../include/fuppes_plugin.h"
#include "../../include/fuppes_db_connection_plugin.h"

#include "database_mysql_sql.h"

#include <string>
#include <sstream>
#include <iostream>

using namespace std;

#if defined(WIN32)
#include <shlwapi.h>
#endif

#include <assert.h>
#include <mysql.h>
#include <time.h>

#if !defined(WIN32) && defined(THREAD_SAFE_CLIENT)
#include <pthread.h>
#endif

// http://dev.mysql.com/doc/refman/5.1/en/c.html

// http://dev.mysql.com/doc/refman/5.1/en/threaded-clients.html

class CMySQLConnection: public CDatabaseConnection
{
	public:
		CMySQLConnection();
		~CMySQLConnection();

		bool startTransaction();
		bool commit();
		void rollback();

		const char* getStatement(fuppes_sql_no number) {
      //cout << "MYSQL GET STATEMENT: " << mysql_sql[number].sql << endl; 
			return mysql_sql[number].sql;
		}

		bool setup();

#ifdef THREAD_SAFE_CLIENT
    void lock() { 
    	#ifdef WIN32
      EnterCriticalSection(&m_mutex);
      #else
      pthread_mutex_lock(&m_mutex);
      #endif
    }
    void unlock() { 
      #ifdef WIN32
      LeaveCriticalSection(&m_mutex);
      #else
      pthread_mutex_unlock(&m_mutex);
      #endif
    }
#endif



    void check();

    void vacuum() { }
	private:
		bool				connect(const CConnectionParams params);
		ISQLQuery*	query();

		MYSQL				m_handle;
    //CConnectionParams m_params;

#ifdef THREAD_SAFE_CLIENT
		#ifdef WIN32
		CRITICAL_SECTION  m_mutex;
		#else
		pthread_mutex_t   m_mutex;
		#endif
#endif

    time_t            m_lastQueryTime;

    int getLastQuerySeconds() {
      
      int sec = 0;


      // calc seconds since last query
      time_t now = time(NULL);
      sec = now - m_lastQueryTime;

      // set last query time to now
      m_lastQueryTime = now;

      //cout << "last mysql query: " << sec << " seconds ago" << endl;
      return sec;
    }
      
    unsigned long     m_mysqlThreadId;
    
};

class CMySQLQuery: public ISQLQuery
{
	friend class CMySQLConnection;
	
	public:
		~CMySQLQuery() {
			clear();
#ifndef THREAD_SAFE_CLIENT
      delete m_connection;
#endif
		}

		virtual bool select(const std::string sql);
		virtual bool exec(const std::string sql);
		virtual fuppes_off_t insert(const std::string sql);
		
		bool eof() { return (m_ResultListIterator == m_ResultList.end()); }
		void next() {
		  if(m_ResultListIterator != m_ResultList.end()) {
		    m_ResultListIterator++;
		  }
		}
		CSQLResult* result() { return *m_ResultListIterator; }	
		
		fuppes_off_t lastInsertId() { return m_lastInsertId; }
		
		void clear() {
      while (!m_ResultList.empty()) {
        delete m_ResultList.front();
        m_ResultList.pop_front();
      }
		
			m_rowsReturned = 0;
			m_ResultListIterator = m_ResultList.end();
		}
		
		CDatabaseConnection* connection() { return m_connection; }

    unsigned int size() {
      return m_ResultList.size();
    }

    void  prepare(const std::string sql) {
      m_sqlPrepare = sql;
    }

    void setLogSubsystem(int logSubsystem) {
      //m_logSubsystem = logSubsystem;
    }
    
	private:
		CMySQLQuery(CDatabaseConnection* connection, MYSQL* handle);		
		MYSQL* m_handle;
		CMySQLConnection* m_connection;		
		
		fuppes_off_t		m_lastInsertId;
		
		std::list<CSQLResult*> m_ResultList;
    std::list<CSQLResult*>::iterator m_ResultListIterator;
		fuppes_off_t m_rowsReturned;

    std::string   m_sqlPrepare;
};


class CMySQLResult: public CSQLResult
{	
	friend class CMySQLQuery;
	
  public:
    bool isNull(std::string fieldName){			
			std::string sValue = asString(fieldName);
			if((sValue.length() == 0) || (sValue.compare("NULL") == 0))
				return true;
			else
				return false;
		}

    std::string	asString(std::string fieldName) {
		  return m_FieldValues[fieldName];
		}

		unsigned int asUInt(std::string fieldName) {	
			if(!isNull(fieldName)) {
				return strtoul(asString(fieldName).c_str(), NULL, 0);
			}
			return 0;
		}
			
		int asInt(std::string fieldName) {
			if(!isNull(fieldName)) {
  			return atoi(asString(fieldName).c_str());
		  }
			return 0;
		}
		
		CSQLResult* clone() {
			
			CMySQLResult* result = new CMySQLResult();			
			for(m_FieldValuesIterator = m_FieldValues.begin();
					m_FieldValuesIterator != m_FieldValues.end();
					m_FieldValuesIterator++) {

				result->m_FieldValues[m_FieldValuesIterator->first] = m_FieldValuesIterator->second;
			}			
			return result;
		}

    std::vector<std::string> fieldNames() {

      std::vector<std::string> result;
      for(m_FieldValuesIterator = m_FieldValues.begin();
					m_FieldValuesIterator != m_FieldValues.end();
					m_FieldValuesIterator++) {
				result.push_back(m_FieldValuesIterator->first);
			}
      return result;      
    }
    
  private:
    std::map<std::string, std::string> m_FieldValues;
    std::map<std::string, std::string>::iterator m_FieldValuesIterator;  
};




CMySQLConnection::CMySQLConnection()
{

//cout << "CMySQLConnection" << endl;
  
#ifdef THREAD_SAFE_CLIENT
  #ifdef WIN32
  InitializeCriticalSection(&m_mutex);
  #else
  pthread_mutex_init(&m_mutex, NULL);
  #endif
#endif
  
	mysql_init(&m_handle);


  // int mysql_options(MYSQL *mysql, enum mysql_option option, const void *arg)

  // http://dev.mysql.com/doc/refman/5.1/en/auto-reconnect.html
  
  // enable auto reconnect
  my_bool reconnect = 1;
  mysql_options(&m_handle, MYSQL_OPT_RECONNECT, &reconnect);

  m_lastQueryTime = 0;  
  m_mysqlThreadId = 0;
}

CMySQLConnection::~CMySQLConnection()
{
//cout << "~CMySQLConnection" << endl;
  
	mysql_close(&m_handle);

#ifdef THREAD_SAFE_CLIENT
	#ifdef WIN32
  DeleteCriticalSection(&m_mutex);
  #else
  pthread_mutex_destroy(&m_mutex);
  #endif
#endif
}

bool CMySQLConnection::connect(const CConnectionParams params)
{
	if(!mysql_real_connect(&m_handle, 
												 params.hostname.c_str(),
												 params.username.c_str(),
												 params.password.c_str(),
												 params.dbname.c_str(), 0, NULL, 0)) {
    fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&m_handle));
		return false;
	}

	m_connectionParams = params;

  check();  

  return true;
}


void CMySQLConnection::check()
{


  int last = getLastQuerySeconds();

  //cout << "CMySQLConnection::check(): thread id: " << m_mysqlThreadId << " last: " << last <<  endl;
  
  // if the last query was 5 seconds or more ago we check if the connection
  // was rebuild and reset our connection settings
  if(last < 5) {
    //cout << "RETURN 1" << endl;
    return;
  }

  // if the last query was more than 5 seconds ago we
  // ping the server to reconnect if necessary
  mysql_ping(&m_handle);

  // check if the thread id has changed
  // if so we got a reconnect
  unsigned long id = mysql_thread_id(&m_handle);
  if(id == m_mysqlThreadId) {
    //cout << "RETURN 2" << endl;
    return;
  }


  m_mysqlThreadId = id;

  mysql_query(&m_handle, "SET NAMES 'utf8'");

  //cout << "RETURN 3 new thread id: " << m_mysqlThreadId << endl;
}
    

bool CMySQLConnection::setup()
{
	return true;
}

ISQLQuery* CMySQLConnection::query()
{	

  CMySQLConnection* connection;
#ifdef THREAD_SAFE_CLIENT
  connection = this;
#else
  connection = new CMySQLConnection();
  connection->connect(m_params);
#endif

	return new CMySQLQuery(connection, &m_handle);
}

bool CMySQLConnection::startTransaction()
{
	return false;
}

bool CMySQLConnection::commit()
{
	return false;
}

void CMySQLConnection::rollback()
{
}


CMySQLQuery::CMySQLQuery(CDatabaseConnection* connection, MYSQL* handle)
{
	m_handle = handle;
	m_connection = (CMySQLConnection*)connection;
}

bool CMySQLQuery::select(const std::string sql)
{
	clear();

	MYSQL_RES* res;
	MYSQL_ROW row;
	MYSQL_FIELD *fields;
	unsigned int num_fields;
	unsigned int i;
  
//cout << "MYSQLQUERY: " << sql << endl;

  m_connection->check();
  
#ifdef THREAD_SAFE_CLIENT
  // when using a single connection in a multithreaded application the
  // connection must be locked between mysql_query and mysql_store_result
  m_connection->lock();
#endif
  
	if(mysql_query(m_handle, sql.c_str()) != 0) {
		std::cout << "mysql: query error " << sql << std::endl << mysql_error(m_handle) << std::endl;
    assert(true == false);
#ifdef THREAD_SAFE_CLIENT
    m_connection->unlock();
#endif
		return false;
	}
	
	res = mysql_store_result(m_handle);	

#ifdef THREAD_SAFE_CLIENT
  m_connection->unlock();
#endif

  num_fields = mysql_num_fields(res);
	fields = mysql_fetch_fields(res);
	
	CMySQLResult* pResult;
	while((row = mysql_fetch_row(res))) {
		
		pResult = new CMySQLResult();		
		for(i = 0; i < num_fields; i++)	{

			if(!row[i]) {			
				pResult->m_FieldValues[std::string(fields[i].name)] = "NULL";	
				continue;
			}
			
			std::string value;
			if(IS_NUM(fields[i].type)) {
				std::stringstream str;
				str << row[i];
				value = str.str();
				str.str("");
			}
			else {
				value = row[i];
			}
			pResult->m_FieldValues[std::string(fields[i].name)] = value;

		}
		m_ResultList.push_back(pResult);
	}
  m_ResultListIterator = m_ResultList.begin();
	mysql_free_result(res);
	
	return true;
}

bool CMySQLQuery::exec(const std::string sql)
{
  m_connection->check();
  
	if(mysql_query(m_handle, sql.c_str()) != 0) {
		std::cout << "mysql: exec error : " << sql << std::endl;
		return false;
	}
		
	return true;
}

fuppes_off_t CMySQLQuery::insert(const std::string sql)
{
  m_connection->check();
  
	if(mysql_query(m_handle, sql.c_str()) != 0) {
		std::cout << "mysql: insert error : " << sql << std::endl;
		return 0;
  }

  m_lastInsertId = mysql_insert_id(m_handle);
	return m_lastInsertId;
}



#ifdef __cplusplus
extern "C" {
#endif

void register_fuppes_plugin(plugin_info* plugin)
{
	strcpy(plugin->plugin_name, "mysql");
	strcpy(plugin->plugin_author, "Ulrich Voelkel");
	plugin->plugin_type = PT_DATABASE_CONNECTION;
	
	if(mysql_library_init(0, NULL, NULL) != 0) {
    fprintf(stderr, "could not initialize MySQL library\n");
  }
}

CDatabaseConnection* fuppes_plugin_create_db_connection(plugin_info* plugin __attribute__((unused)))
{
	return new CMySQLConnection();
}

void unregister_fuppes_plugin(plugin_info* plugin __attribute__((unused)))
{
  mysql_library_end();
}
	
#ifdef __cplusplus
}
#endif
