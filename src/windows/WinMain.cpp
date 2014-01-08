/***************************************************************************
 *            WinMain.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2007 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
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

#include <windows.h>
#include <iostream>

#include "../../include/fuppes.h"
#include "WinMainForm.h"
#include "Common.h"

using namespace std;

CMainForm* pMainForm = NULL;
static fuppes_status_t fuppesStatus = STATUS_ERROR;

void LogCallback(const char* sz_log)
{
    pMainForm->Log(sz_log);
}

void NotifyCallback(const char* sz_title, const char* sz_msg)
{
    pMainForm->Notify(sz_title, sz_msg);
}

void ErrorCallback(const char* sz_err)
{
    pMainForm->Error(sz_err);
}

std::string wstrtostr(const std::wstring &wstr)
{
    // Convert a Unicode string to an ASCII string
    std::string strTo;
    char *szTo = new char[wstr.length() + 1];
    szTo[wstr.size()] = '\0';
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
    strTo = szTo;
    delete[] szTo;
    return strTo;
}

std::wstring strtowstr(const std::string &str)
{
    // Convert an ASCII string to a Unicode String
    std::wstring wstrTo;
    wchar_t *wszTo = new wchar_t[str.length() + 1];
    wszTo[str.size()] = L'\0';
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wszTo, (int)str.length());
    wstrTo = wszTo;
    delete[] wszTo;
    return wstrTo;
}

void init_callback(fuppes_status_t status, const char* msg)
{
    fuppesStatus = status;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nWindowStyle)
{

    MSG messages;
    pMainForm = new CMainForm(hInstance);
    pMainForm->ShowTrayIcon();

    fuppes_set_notify_callback(NotifyCallback);
    fuppes_set_error_callback(ErrorCallback);

    int argc;
    LPWSTR *szArglist;
    szArglist = CommandLineToArgvW(GetCommandLineW(), &argc);

    char* argv[100];
    for(int i = 0; i < argc; i++) {
        argv[i] = (char*)malloc(wstrtostr(szArglist[i]).length());
        strcpy(argv[i], wstrtostr(szArglist[i]).c_str());
    }
    LocalFree(szArglist);

    // fuppes_init()
    if(fuppes_init(argc, argv, init_callback, LogCallback) == FUPPES_FALSE) {
        pMainForm->HideTrayIcon();
        delete pMainForm;
        return 1;
    }

    for(int i = 0; i < argc; i++) {
        free(argv[i]);
    }

    // wait for initialization to finish
    while(STATUS_INITIALIZED != fuppesStatus && STATUS_ERROR != fuppesStatus) {
        Sleep(1);
    }

    // start fuppes
    if(STATUS_INITIALIZED == fuppesStatus) {
        fuppes_start();
    }

    // wait for startup to finish
    while(STATUS_STARTED != fuppesStatus && STATUS_ERROR != fuppesStatus) {
        Sleep(1);
    }

    fuppes_set_loglevel(0);

    // fuppes could not be started
    if(STATUS_ERROR == fuppesStatus) {
        pMainForm->HideTrayIcon();
        delete pMainForm;
        return 1;
    }

    // Run the message loop. It will run until GetMessage() returns 0
    while (GetMessage (&messages, NULL, 0, 0)) {
        // Translate virtual-key messages into character messages
        TranslateMessage(&messages);
        // Send message to WindowProcedure
        DispatchMessage(&messages);
    }

    // cleanup
    fuppes_stop();
    while(STATUS_STOPPED != fuppesStatus && STATUS_ERROR != fuppesStatus) {
        Sleep(1);
    }

    fuppes_cleanup();

    pMainForm->HideTrayIcon();
    delete pMainForm;

    return messages.wParam;
}
