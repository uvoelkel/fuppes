/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */

/*
http://msdn.microsoft.com/en-us/library/ms685969(VS.85).aspx
http://www.devx.com/cplus/Article/9857
http://www.liberidu.com/blog/?p=304
 
HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services
*/

#include <windows.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;


#include "../include/fuppes.h"
#define SERVICE_NAME "fuppes"



SERVICE_STATUS          svcStatus; 
SERVICE_STATUS_HANDLE   svcStatusHandle; 
HANDLE                  ghSvcStopEvent = NULL;

void SvcMain(DWORD argc, LPTSTR *argv);
void ReportSvcStatus( DWORD dwCurrentState,
                      DWORD dwWin32ExitCode,
                      DWORD dwWaitHint);
void SvcCtrlHandler( DWORD dwCtrl );

bool installService(LPCTSTR appName);
bool uninstallService();


#define LOGFILE "C:\\fuppes-svc-log.txt"

static char* applicationExeName;

int log(const char* str)
{
	FILE* log;
	log = fopen(LOGFILE, "a+");
	if (log == NULL)
    return -1;
	fprintf(log, "%s\n", str);
	fclose(log);
	return 0;
}


int main(int argc, TCHAR *argv[]) 
{ 
  
  if(argc > 1) {

    if(strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "install") == 0) {
      if(installService(argv[0])) {
        cout << "service installed" << endl;
      }
      else {
        cout << "error installing service" << endl;
      }
      return 0;
    }
    else if(strcmp(argv[1], "-u") == 0) {
      if(uninstallService()) {
        cout << "service uninstalled" << endl;
      }
      else {
        cout << "error uninstalling service" << endl;
      }
      return 0;
    }

  }
  

  log("main() [start]");

  applicationExeName = (char*)malloc(strlen(argv[0]) + 1);
  strcpy(applicationExeName, argv[0]);

  
  SERVICE_TABLE_ENTRY DispatchTable[] =  { 
    { SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) SvcMain }, 
    { NULL, NULL } 
  };

  StartServiceCtrlDispatcher(DispatchTable);

  free(applicationExeName);
  log("main() [exit]");
  return 0;
} 

void SvcMain(DWORD argc, LPTSTR *argv)
{
  // Register the handler function for the service
  svcStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, (LPHANDLER_FUNCTION)SvcCtrlHandler);

  if(!svcStatusHandle) {
    //SvcReportEvent(TEXT("RegisterServiceCtrlHandler")); 
    return; 
  }

  // These SERVICE_STATUS members remain as set here
  svcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS; 
  svcStatus.dwServiceSpecificExitCode = 0;

  // Report initial status to the SCM
  ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);


  
  // Create an event. The control handler function, SvcCtrlHandler,
  // signals this event when it receives the stop control code.
  ghSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  if(ghSvcStopEvent == NULL) {
    ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
    return;
  }

  if(fuppes_init(1, &applicationExeName, NULL) != FUPPES_TRUE) {
    log("error init fuppes");
    ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
    return;
	}

  // Report running status when initialization is complete.
  ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

  
  if(fuppes_start() != FUPPES_TRUE) {
    log("error start fuppes");
    ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
    return;
  }
  

  char szHTTP[100];
  fuppes_get_http_server_address(szHTTP, 100);
  log(szHTTP);
  

  // wait for service to stop
  WaitForSingleObject(ghSvcStopEvent, INFINITE);

  
  fuppes_stop();
  //fuppes_cleanup();
  
  ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
}

//
// Purpose: 
//   Sets the current service status and reports it to the SCM.
//
// Parameters:
//   dwCurrentState - The current state (see SERVICE_STATUS)
//   dwWin32ExitCode - The system error code
//   dwWaitHint - Estimated time for pending operation, 
//     in milliseconds
// 
// Return value:
//   None
//
void ReportSvcStatus( DWORD dwCurrentState,
                      DWORD dwWin32ExitCode,
                      DWORD dwWaitHint)
{
  //log("ReportSvcStatus()");
    static DWORD dwCheckPoint = 1;

    // Fill in the SERVICE_STATUS structure.

    svcStatus.dwCurrentState = dwCurrentState;
    svcStatus.dwWin32ExitCode = dwWin32ExitCode;
    svcStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        svcStatus.dwControlsAccepted = 0;
    else svcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if ( (dwCurrentState == SERVICE_RUNNING) ||
           (dwCurrentState == SERVICE_STOPPED) )
        svcStatus.dwCheckPoint = 0;
    else svcStatus.dwCheckPoint = dwCheckPoint++;

    // Report the status of the service to the SCM.
    SetServiceStatus( svcStatusHandle, &svcStatus );
}


void SvcCtrlHandler( DWORD dwCtrl )
{
   // Handle the requested control code.  
  switch(dwCtrl) {
    
    case SERVICE_CONTROL_STOP: 
      log("SvcCtrlHandler() :: SERVICE_CONTROL_STOP");
      ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
      // Signal the service to stop.
      SetEvent(ghSvcStopEvent);
      ReportSvcStatus(svcStatus.dwCurrentState, NO_ERROR, 0);       
      return;

    case SERVICE_CONTROL_INTERROGATE:
      log("SvcCtrlHandler() :: SERVICE_CONTROL_INTERROGATE");
      break; 

    default:
      log("SvcCtrlHandler() :: unknown");
      break;
  } 
   
}

bool installService(LPCTSTR appName)
{
  SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
  if(scm == NULL) {
    cout << "error OpenSCManager()" << endl;
    return false;
  }
  
// http://msdn.microsoft.com/en-us/library/ms682450(VS.85).aspx

  LPCTSTR user = NULL;
  LPCTSTR password = NULL;
  
  SC_HANDLE svc = CreateService(
    scm,
    SERVICE_NAME,
    "Fuppes",
    SERVICE_ALL_ACCESS,
    SERVICE_WIN32_OWN_PROCESS,
    SERVICE_DEMAND_START,
    SERVICE_ERROR_NORMAL,
    appName, 
    NULL, NULL,
    NULL, 
    user,
    password
    );

  CloseServiceHandle(scm);
  
  if(svc == NULL) {
    cout << "error CreateService()" << endl;
    return false;
  }    

  CloseServiceHandle(svc); 
  return true;
}

bool uninstallService()
{
  SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if(scm == NULL) {
    cout << "error OpenSCManager()" << endl;
    return false;
  }
  
  SC_HANDLE svc = OpenService(scm, SERVICE_NAME, SERVICE_ALL_ACCESS);
  
  if(svc == NULL) {
    cout << "error OpenService()" << endl;
    CloseServiceHandle(scm);
    return false;
  } 

  bool result = DeleteService(svc);
  if(!result) {
    cout << "error DeleteService()" << endl;
  }
  
  CloseServiceHandle(svc);
  CloseServiceHandle(scm);
  return result;
}
