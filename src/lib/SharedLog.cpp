/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            SharedLog.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#include "SharedLog.h"
#include <iostream>
#include <sstream>
#include <time.h>
#include <string.h>
#include "Common/Exception.h"

#ifdef HAVE_LIBNOTIFY
#include <libnotify/notify.h>
#include <libnotify/notification.h>
#include <libnotify/notify-enum-types.h>
#endif

using namespace std;

CSharedLog* CSharedLog::m_Instance = 0;
ofstream*   CSharedLog::m_fsLogFile = NULL;
std::string CSharedLog::m_sLogFileName = "";

CSharedLog* CSharedLog::Shared()
{
	if (m_Instance == 0)
		m_Instance = new CSharedLog();
	return m_Instance;
}


CSharedLog::CSharedLog()
{
  SetLogLevel(1, false);
  
	
  m_log_cb = NULL;
  m_err_cb = NULL;
  m_notify_cb = NULL;
  m_user_input_cb = NULL;
		
	#ifdef HAVE_LIBNOTIFY
  if(!notify_init("fuppes"))
    printf("notify_init() failed\n");
  #endif        
    
  m_fsLogFile = NULL;
}

CSharedLog::~CSharedLog()
{
	#ifdef HAVE_LIBNOTIFY
  notify_uninit();
  #endif		
		
  /*#ifdef HAVE_SYSLOG_H
  if(m_bUseSyslog)
    closelog();
  #endif*/
  
  if(m_fsLogFile) {
    m_fsLogFile->close();
    delete m_fsLogFile;
    m_fsLogFile = NULL;
  }
}

bool CSharedLog::SetLogFileName(std::string p_sLogFileName)
{
  if(!CSharedLog::m_sLogFileName.empty())
    return false;
  
  CSharedLog::m_sLogFileName = p_sLogFileName;
  m_fsLogFile = new ofstream();
  m_fsLogFile->open(CSharedLog::m_sLogFileName.c_str(), ios::out | ios::trunc);
  return true;
}

void CSharedLog::SetLogLevel(int p_nLogLevel, bool p_bPrintLogLevel)
{
  m_bShowLog         = false;
  m_bShowExtendedLog = false;
  m_bShowDebugLog    = false;

  m_nLogLevel = p_nLogLevel;  
  switch(m_nLogLevel)
  {
    case 0:
      if(p_bPrintLogLevel)
        CSharedLog::Print("log-level: 0 (disabled)");
      break;    
    case 1:
      m_bShowLog = true;
      if(p_bPrintLogLevel)
        CSharedLog::Print("log-level: 1 (normal)");
      break;
    case 2:
      m_bShowLog         = true;
      m_bShowExtendedLog = true;
      if(p_bPrintLogLevel)
        CSharedLog::Print("log-level: 2 (extended)");
      break;
    case 3:
      m_bShowLog         = true;
      m_bShowExtendedLog = true;
      m_bShowDebugLog    = true;
      if(p_bPrintLogLevel)
        CSharedLog::Print("log-level: 3 (debug)");
      break;
    default:
      break;
  }

}

std::string CSharedLog::GetLogLevel()
{
  std::string sResult;
  
  switch(m_nLogLevel)
  {
    case 0:      
      sResult = "0 (disabled)";
      break;    
    case 1:
      sResult = "1 (normal)";
      break;
    case 2:
      sResult = "2 (extended)";
      break;
    case 3:
      sResult = "3 (debug)";
      break;
    default:
      break;
  }  
  
  return sResult;
}

void CSharedLog::ToggleLog()
{
  if(m_nLogLevel < 3)
    m_nLogLevel++;
  else
    m_nLogLevel = 0;
  
  SetLogLevel(m_nLogLevel);
}

/*void CSharedLog::Log(std::string p_sSender, std::string p_sMessage)
{
  #ifndef DISABLELOG  
  if(m_bShowLog)
  {*/
    //fuppesThreadLockMutex(&m_Mutex);
    /*stringstream sLog;
    sLog << "[" << p_sSender << "] " << p_sMessage << std::endl;
    cout << sLog.str() << endl;
    fflush(stdout);*/
		/*	
		CSharedLog::Log(L_NORM, "", 0, "[%s] %s", p_sSender.c_str(), p_sMessage.c_str());
    //fuppesThreadUnlockMutex(&m_Mutex);    
  }  
  #endif
}*/



void CSharedLog::UserError(std::string p_sErrMsg)
{
  if(m_err_cb) {    
    m_err_cb(p_sErrMsg.c_str());
  }
  else {
    CSharedLog::Print("[ERROR] %s", p_sErrMsg.c_str());
  }
}

void CSharedLog::UserNotify(std::string p_sTitle, std::string p_sNotifyMsg)
{
  if(m_notify_cb) {
    m_notify_cb(p_sTitle.c_str(), p_sNotifyMsg.c_str());
  }
  else {
		#ifdef HAVE_LIBNOTIFY
		NotifyNotification* pNotification; 
  	pNotification = notify_notification_new(p_sTitle.c_str(), p_sNotifyMsg.c_str(), NULL, NULL);
  
  	//NOTIFY_URGENCY_LOW 	 Low urgency. Used for unimportant notifications.
  	//NOTIFY_URGENCY_NORMAL 	Normal urgency. Used for most standard notifications.
  	//NOTIFY_URGENCY_CRITICAL 	Critical urgency. Used for very important notifications.  
  	//notify_notification_set_urgency(pNotification, NOTIFY_URGENCY_LOW);
  
  	GError* pError = NULL;
  	if(!notify_notification_show(pNotification,  &pError)) {
    	//
  	}			
		#else
    CSharedLog::Print(p_sNotifyMsg.c_str());
		#endif
  }
}

std::string CSharedLog::UserInput(std::string p_sMessage)
{
  string sResult;
  
  if(m_user_input_cb) {
    char szResult[200];
    m_user_input_cb(p_sMessage.c_str(), szResult, 200);
    sResult = szResult;
  }
  else {    
    cout << p_sMessage << endl;
    cin >> sResult;    
  }    
  
  return sResult;
}


/*void CSharedLog::ExtendedLog(std::string p_sSender, std::string p_sMessage)
{
  if(m_bShowExtendedLog)
		CSharedLog::Log(L_EXT, "", 0, "[%s] %s", p_sSender.c_str(), p_sMessage.c_str());
    //this->Log(p_sSender, p_sMessage);
}

void CSharedLog::DebugLog(std::string p_sSender, std::string p_sMessage)
{
  if(m_bShowDebugLog)
		CSharedLog::Log(L_DBG, "", 0, "[%s] %s", p_sSender.c_str(), p_sMessage.c_str());
    //this->Log(p_sSender, p_sMessage);
}*/

/*void CSharedLog::Warning(std::string p_sSender, std::string p_sMessage)
{
  #ifndef DISABLELOG  
  if(m_bShowLog)
  { 
		CSharedLog::Log(L_EXT, "", 0, "[WARNING :: %s] %s", p_sSender.c_str(), p_sMessage.c_str());
  }
  #endif
}

void CSharedLog::Critical(std::string p_sSender, std::string p_sMessage)
{
  #ifndef DISABLELOG  
  if(m_bShowExtendedLog)
  {
		CSharedLog::Log(L_EXT, "", 0, "[CRITICAL :: %s] %s", p_sSender.c_str(), p_sMessage.c_str());
  }
  #endif
}

void CSharedLog::Error(std::string p_sSender, std::string p_sMessage)
{
  #ifndef DISABLELOG  
  if (m_bShowExtendedLog)
  {
		CSharedLog::Log(L_EXT, "", 0, "[ERROR :: %s] %s", p_sSender.c_str(), p_sMessage.c_str());
  }
  #endif
}*/

void CSharedLog::Log(int nLogLevel, std::string p_sMessage, char* p_szFileName, int p_nLineNumber)
{
  #ifdef DISABLELOG
  return;
  #endif
  
  if(m_log_cb) {
    if(m_nLogLevel > 0)
      m_log_cb(p_sMessage.c_str());
    return;
  }
  
  CSharedLog::Log(nLogLevel, p_szFileName, p_nLineNumber, p_sMessage.c_str());  
}

void CSharedLog::Log(int p_nLogLevel, const std::string p_sFileName, int p_nLineNumber, const std::string msg)
{
  if(p_nLogLevel >= CSharedLog::Shared()->m_nLogLevel)
    return;

  if(!m_sLogFileName.empty()) {  
    if(!p_sFileName.empty() && p_nLineNumber > 0) {
      #ifndef WIN32
      time_t now;
      char nowtime[26];
      time(&now);  
      ctime_r(&now, nowtime);
      nowtime[24] = '\0';
      string sNowtime = nowtime;
      #else		
      char timeStr[9];    
      _strtime(timeStr);	
      string sNowtime = timeStr;	
      #endif 
          
      *m_fsLogFile << "== " << p_sFileName << " (" << p_nLineNumber <<  ") :: " <<
        sNowtime << " ==" << endl;
    }
    *m_fsLogFile << msg << endl << endl;
  }
  else {
    if(!p_sFileName.empty() && p_nLineNumber > 0) {
      #ifndef WIN32
      time_t now;
      char nowtime[26];
      time(&now);  
      ctime_r(&now, nowtime);
      nowtime[24] = '\0';
      string sNowtime = nowtime;
      #else
      char timeStr[9];
      _strtime(timeStr);
      string sNowtime = timeStr;
      #endif 
          
      cout << "== " << p_sFileName << " (" << p_nLineNumber <<  ") :: " <<
        sNowtime << " ==" << endl;
    }      
    cout << msg << endl << endl;
  }
}

void CSharedLog::Log(int p_nLogLevel, const std::string p_sFileName, int p_nLineNumber, const char* p_szFormat, ...)
{
	if(p_nLogLevel >= CSharedLog::Shared()->m_nLogLevel)
    return;
	
	va_list args;
  va_start(args, p_szFormat);
	LogArgs(p_nLogLevel, p_sFileName, p_nLineNumber, p_szFormat, args);
	va_end(args);
}

void CSharedLog::LogArgs(int p_nLogLevel, const std::string p_sFileName, int p_nLineNumber, const char* p_szFormat, va_list args)
{
	if(p_nLogLevel >= CSharedLog::Shared()->m_nLogLevel)
    return;
	
	char buffer[8192];	
  memset(buffer, 0, sizeof(buffer));
	vsnprintf(buffer, sizeof(buffer) - 1, p_szFormat, args);
	Log(p_nLogLevel, p_sFileName, p_nLineNumber, string(buffer));	
}

/*void CSharedLog::Log(int p_nLogLevel, fuppes::Exception exception)
{	
	Log(p_nLogLevel, exception.file(), exception.line(), exception.what());	
}*/

void CSharedLog::Print(const char* p_szFormat, ...)
{
  va_list args;
	char buffer[8192];
  memset(buffer, 0, sizeof(buffer));
  va_start(args, p_szFormat);
	vsnprintf(buffer, sizeof(buffer) - 1, p_szFormat, args);
  va_end(args);
		
  if(!m_sLogFileName.empty()) {
    CSharedLog::Log(L_NORM, "", 0, buffer);
  }  
  else {
    cout << buffer << endl;
  }
}
