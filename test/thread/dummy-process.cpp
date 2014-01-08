
#include <iostream>
using namespace std;

#ifndef WIN32
#include <signal.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

#include <string.h>

static bool g_run = true;

void signal_handler(int signal)
{ 
  cout << "dummy process: signal_handler: " << signal << endl;
  g_run = false;
}


int main(int argc, char* argv[])
{
	cout << "dummy process: started" << endl;


  for(int i = 0; i < argc; i++) {
    cout << "dummy process: arg " << i << ": " << argv[i] << endl;
  }
  

  if(argc > 1 && strcmp(argv[1], "exit") == 0) {
    
    cout << "dummy process: will sleep for two seconds and then exit" << endl;
    #ifndef WIN32
    usleep(1000 * 1000 * 2);
    #else
    Sleep(1000);
    #endif
    
  }
  else if(argc > 1 && strcmp(argv[1], "loop") == 0) {

    cout << "dummy process: will loop forever" << endl;
    
    #ifndef WIN32
    signal(SIGINT, signal_handler);
    signal(SIGKILL, signal_handler);
    signal(SIGTERM, signal_handler);
    #endif
    
    while(g_run) {
      #ifndef WIN32
	    usleep(1000000);
      #else
      Sleep(1000);
      #endif
      cout << "dummy process: running" << endl;
    }

    cout << "dummy process: one second delay exiting after exit request" << endl;
    #ifndef WIN32
    usleep(1000000);
    #else
    Sleep(1000);
    #endif

  }
    
	cout << "dummy process: exiting" << endl;
	return 0;
}

