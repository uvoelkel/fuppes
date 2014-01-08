/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            daemon_main.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2007-2008 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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
#include <signal.h>
#include <termios.h>
#include <unistd.h>

static bool g_bExitApp = false;
static fuppes_status_t fuppesStatus = STATUS_ERROR;

void SignalHandler(int /*signal*/)
{
    g_bExitApp = true;
}

void init_callback(fuppes_status_t status, const char* msg)
{
    if(NULL != msg) {
        std::cout << msg << std::endl;
    }

    fuppesStatus = status;
    if(STATUS_ERROR == status) {
        g_bExitApp = true;
    }
}

int main(int argc, char* argv[])
{
    pid_t pid;
    pid = fork();

    // error
    if(pid < 0) {
        std::cout << "could not create child process" << std::endl;
        return 1;
    }

    // parent process
    else if(pid > 0) {
        std::cout << "[started]" << std::endl;
        return 0;
    }

    // child process
    else if(pid == 0) {
        //cout << "child process" << endl;
        //close(STDIN_FILENO);
        //close(STDOUT_FILENO);
        //close(STDERR_FILENO);
    }

    // install signal handler
    signal(SIGINT, SignalHandler);  // ctrl-c
    signal(SIGTERM, SignalHandler); // start-stop-daemon -v --stop -nfuppes

    // init fuppes
    if(FUPPES_FALSE == fuppes_init(argc, argv, &init_callback, NULL)) {
        return 1;
    }

    // wait for initialization to finish
    while (STATUS_INITIALIZED != fuppesStatus && STATUS_ERROR != fuppesStatus) {
        usleep(100);
    }

    // start fuppes
    if(STATUS_INITIALIZED == fuppesStatus) {
        fuppes_start();
    }

    // wait for startup to finish
    while (STATUS_STARTED != fuppesStatus && STATUS_ERROR != fuppesStatus) {
        usleep(100);
    }

    // fuppes could not be started
    if(STATUS_ERROR == fuppesStatus) {
        return 1;
    }

    // fuppes is running
    while (!g_bExitApp) {
        usleep(100 * 1000);
    }

    // stop and cleanup fuppes
    fuppes_stop();
    while (STATUS_STOPPED != fuppesStatus && STATUS_ERROR != fuppesStatus) {
        usleep(1000);
    }
    fuppes_cleanup();

    return 0;
}
