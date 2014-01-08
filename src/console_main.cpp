/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            console_main.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2007-2009 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/fuppes.h"

#include <iostream>
#include <string>
#include <unistd.h>
#include <stdio.h>

#include <fcntl.h>

#ifdef WIN32
#include <shlwapi.h>
#endif

#ifndef WIN32
#include <signal.h>
#include <termios.h>
#endif

using namespace std;

static fuppes_status_t fuppesStatus = STATUS_ERROR;
static bool g_bExitApp = false;

class StdIn
{
  public:
    // stolen from http://cc.byexamples.com/2007/04/08/non-blocking-user-input-in-loop-without-ncurses/
    static void setNonblocking(bool nonblocking) {
#ifdef WIN32
#else
      struct termios ttystate;

      //get the terminal state
      tcgetattr(STDIN_FILENO, &ttystate);
      if(nonblocking==true) {
        //turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        //minimum of number input read.
        ttystate.c_cc[VMIN] = 1;
      }
      else if (nonblocking==false) {
        //turn on canonical mode
        ttystate.c_lflag |= ICANON;
      }
      
      //set the terminal attributes.
      tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
#endif
    }

    static bool getline(int timeout, std::string &line) {
#ifdef WIN32
      std::string input;
      ::getline(cin, input);
      line = input;
      return true;
#else
      struct timeval tv;
      fd_set fds;
      tv.tv_sec = timeout;
      tv.tv_usec = 0;
      FD_ZERO(&fds);
      FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
      select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
      if(!FD_ISSET(STDIN_FILENO, &fds)) {
        return false;
      }

      if(g_bExitApp)
        return false;

      char c = fgetc(stdin);
      if(c == 10 || c == 13) {
        line = m_line;
        m_line = "";
        return true;
      }
      m_line += c;

      return false;
#endif
    }

  private:
    static std::string m_line;
    
};

std::string StdIn::m_line;

void PrintHelp()
{
    cout << endl;
    /*cout << "l = change log-level" << endl;
    cout << "    (disabled, normal, extended, debug) default is \"normal\"" << endl;
    cout << "i = print system info" << endl;
    cout << "s = print device settings" << endl;*/
    cout << "r = rebuild database" << endl;
    cout << "v = rebuild virtual container layout" << endl;
    cout << "u = update database" << endl;
    cout << "h = print help" << endl;
    cout << endl;
    /*cout << "m = send m-search" << endl;
    cout << "a = send notify-alive" << endl;
    cout << "b = send notify-byebye" << endl;
    cout << endl;*/

    #ifdef WIN32
    cout << "q = quit" << endl;
    #else
    cout << "ctrl-c or q = quit" << endl;
    #endif

    cout << endl;
}


void SignalHandler(int /*signal*/)
{  
    g_bExitApp = true;
}

void init_callback(fuppes_status_t status, const char* msg)
{
    if(NULL != msg) {
        cout << msg << endl;
    }

    fuppesStatus = status;
    if(STATUS_ERROR == status) {
        g_bExitApp = true; 
    }
}

int main(int argc, char* argv[])
{
    #ifndef WIN32
    if(!geteuid()) {
        cout << "WARNING: Do not run fuppes as the root user." << endl;
        cout << "future versions of fuppes will not allow this" << endl;
        //return -2;
    }
    #endif

    // install signal handler
    #ifndef WIN32
    signal(SIGINT, SignalHandler);  // ctrl-c
    signal(SIGTERM, SignalHandler); // start-stop-daemon -v --stop -nfuppes
    #endif

    // initialize libfuppes
    if(FUPPES_FALSE == fuppes_init(argc, argv, &init_callback, NULL)) {
        return 1;
    }

    // wait for initialization to finish
    while(STATUS_INITIALIZED != fuppesStatus && STATUS_ERROR != fuppesStatus) {
        #ifndef WIN32
        usleep(100);
        #else
        Sleep(1);
        #endif
    }

    // start fuppes
    if(STATUS_INITIALIZED == fuppesStatus) {
        fuppes_start();
    }

    // wait for startup to finish
    while(STATUS_STARTED != fuppesStatus && STATUS_ERROR != fuppesStatus) {
        #ifndef WIN32
        usleep(100);
        #else
        Sleep(1);
        #endif
    }

    // fuppes could not be started
    if(STATUS_ERROR == fuppesStatus) {
        return 1;
    }


  
    char szHTTP[100];
    fuppes_get_http_server_address(szHTTP, 100);
    cout << "webinterface: " << szHTTP << endl;

    cout << endl << "h = help, ";
    #ifdef WIN32
    cout << "q = quit" << endl;
    #else
    cout << "ctrl-c or q = quit" << endl << endl;
    #endif

    string input;
    string cmd;
    string param;

    StdIn::setNonblocking(true);
  
    #ifdef WIN32
    while(input != "q")
    #else
    while(!g_bExitApp && (input != "q"))
    #endif
    {
        input = "";
        if(!StdIn::getline(2, input))
            continue;
    
        cmd = "";
        param = "";

        size_t pos;
        if((pos = input.find(" ")) != string::npos) {
            cmd = input.substr(0, pos);
            param = input.substr(pos + 1);
        }
    
        if (input == "m") {
            fuppes_send_msearch();
        }
        else if (input == "a") {
            fuppes_send_alive();
        }
        else if (input == "b") {
            fuppes_send_byebye();
        }
        else if (input == "l") {
            fuppes_inc_loglevel();
        }
        else if (cmd == "la") {
            fuppes_activate_log_sender(param.c_str());
        }
        else if (cmd == "ld") {
            fuppes_deactivate_log_sender(param.c_str());
        }
        else if(input == "r") {
            fuppes_rebuild_db();
        }
        else if(input == "u") {
            fuppes_update_db();
        }
        else if(input == "v") {
            fuppes_rebuild_vcontainers();
        }
        else if (input == "h") {
            PrintHelp();
        }
        else if (input == "i") {
            fuppes_print_info();
        }
        else if (input == "s") {
            fuppes_print_device_settings();
        }
        /*else if (input == "restart") {
            fuppes_stop();
            fuppes_start();
        }*/
        else {
            fuppes_cli_command(input.c_str());
        }
    }

    StdIn::setNonblocking(false);
  
    cout << "[FUPPES] shutting down" << endl;
    fuppes_stop();
    while(STATUS_STOPPED != fuppesStatus && STATUS_ERROR != fuppesStatus) {
        #ifndef WIN32
        usleep(1000);
        #else
        Sleep(1);
        #endif
    }
    fuppes_cleanup();
    cout << "[FUPPES] exit" << endl;
	return 0;
}
