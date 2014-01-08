/***************************************************************************
 *            WinMainForm.cpp
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

#include "WinMainForm.h"

#include <sstream>
#include "../../include/fuppes.h"
#include "Common.h"

#define WM_TRAYICON WM_APP + 1

#define ID_EDITCHILD 12

#define MAX_MEMO_LENGTH 29996

#define ID_TRAY_POPUP_QUIT 1
#define ID_TRAY_POPUP_SHOW_WEBINF 2

#define ID_TRAY_POPUP_REBUILD_DB 3
#define ID_TRAY_POPUP_UPDATE_DB 4
#define ID_TRAY_POPUP_REBUILD_VFOLDER 5

#define ID_TRAY_POPUP_SEND_ALIVE 6
#define ID_TRAY_POPUP_SEND_BYEBYE 7
#define ID_TRAY_POPUP_SEND_MSEARCH 8

using namespace std;

LRESULT CALLBACK WindowProcedure(HWND p_hWnd, UINT message, WPARAM wParam, LPARAM lParam);

CMainForm* pForm = NULL;

CMainForm::CMainForm(HINSTANCE hInstance)
{
  nLogLength = 0;
  m_bVisible = false;
  hWnd = NULL;
                               
  // The Window structure 
  wincl.hInstance     = hInstance;
  wincl.lpszClassName = "CMainForm";
  wincl.lpfnWndProc   = WindowProcedure;      /* This function is called by windows */
  wincl.style         = CS_DBLCLKS;           /* Catch double-clicks */
  wincl.cbSize        = sizeof (WNDCLASSEX);

  // Use default icon and mouse-pointer 
  wincl.hIcon   = LoadIcon(hInstance, "A");  //LoadIcon (NULL, IDI_APPLICATION);
  wincl.hIconSm = LoadIcon(hInstance, "A");  //LoadIcon (NULL, IDI_APPLICATION);
  wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
  wincl.lpszMenuName = NULL;                 /* No menu */
  wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
  wincl.cbWndExtra = 0;                      /* structure or the window instance */
  // Use Windows's default color as the background of the window 
  wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

  // Register the window class, and if it fails quit the program 
  RegisterClassEx(&wincl);
  /*if(!RegisterClassEx(&wincl))
    throw EException("RegisterClassEx()", __FILE__, __LINE__);                      */
        
  /* The class is registered, let's create the program*/
  hWnd = CreateWindowEx (
         0,                   /* Extended possibilites for variation */
         "CMainForm",         /* Classname */
         GetAppFullName().c_str(),    /* Title Text */
         WS_OVERLAPPEDWINDOW, /* default window */
         CW_USEDEFAULT,       /* Windows decides the position */
         CW_USEDEFAULT,       /* where the window ends up on the screen */
         544,                 /* The programs width */
         375,                 /* and height in pixels */
         HWND_DESKTOP,        /* The window is a child-window to desktop */
         NULL,                /* No menu */
         hInstance,           /* Program Instance handler */
         NULL                 /* No Window Creation data */
         );        
                      
  // create popup menu                
  hPopup = CreatePopupMenu();
  AppendMenu(hPopup, MF_STRING, ID_TRAY_POPUP_SHOW_WEBINF, "Show webinterface");    
  AppendMenu(hPopup, MF_SEPARATOR, 0, "" ); 
  AppendMenu(hPopup, MF_STRING, ID_TRAY_POPUP_REBUILD_DB, "Rebuild database");  
	AppendMenu(hPopup, MF_STRING, ID_TRAY_POPUP_UPDATE_DB, "Update database");
  AppendMenu(hPopup, MF_STRING, ID_TRAY_POPUP_REBUILD_VFOLDER, "Rebuild virtual containers");  

  AppendMenu(hPopup, MF_SEPARATOR, 0, "" ); 
  AppendMenu(hPopup, MF_STRING, ID_TRAY_POPUP_SEND_ALIVE, "Send \"Notify-Alive\"");  
  AppendMenu(hPopup, MF_STRING, ID_TRAY_POPUP_SEND_BYEBYE, "Send \"Notify-ByeBye\""); 
  AppendMenu(hPopup, MF_STRING, ID_TRAY_POPUP_SEND_MSEARCH, "Send \"M-Search\""); 
      
  AppendMenu(hPopup, MF_SEPARATOR, 0, "" );    
  AppendMenu(hPopup, MF_STRING, ID_TRAY_POPUP_QUIT, "Quit" );    
  pForm = this; 
  
  OnCreate();
}

CMainForm::~CMainForm()
{
  DestroyWindow(h_rbLog0);
  DestroyWindow(h_rbLog1);
  DestroyWindow(h_rbLog2);
  DestroyWindow(h_rbLog3);
  DestroyWindow(hWndMemo);
                       
  DestroyMenu(hPopup);
}

LRESULT CALLBACK WindowProcedure(HWND p_hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  return pForm->WindowProc(p_hWnd, message, wParam, lParam);
}

LRESULT CALLBACK CMainForm::WindowProc(HWND p_hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)                  /* handle the messages */
  {
    /*case WM_CREATE:  
      if(p_hWnd == hWnd)
        OnCreate(); 
      break;*/
      
    case WM_SIZE: 
      /*MoveWindow(hWndRadioBox, 
                 8, 8,                  // starting x- and y-coordinates 
                 LOWORD(lParam) - 16,        // width of client area 
                 100 - 8,        // height of client area 
                 TRUE);               // repaint window                   */
         
      // Make the edit control the size of the window's client area. 
      MoveWindow(hWndMemo, 
                 8, 38,                  // starting x- and y-coordinates 
                 LOWORD(lParam) - 16,        // width of client area 
                 HIWORD(lParam) - 16 - 30,        // height of client area 
                 TRUE);                 // repaint window 
      return 0;
      break;
      
    case WM_COMMAND:
    
      if(HIWORD(wParam) == BN_CLICKED) {                        
        
        // disabled    
        if((HWND)lParam == h_rbLog0) {
          fuppes_set_loglevel(0);
          return 0;
        }
        // normal
        else if((HWND)lParam == h_rbLog1) {
          fuppes_set_loglevel(1);
          return 0;
        }
        // extended
        else if((HWND)lParam == h_rbLog2) {
          fuppes_set_loglevel(2);
          return 0;
        }
        // debug
        else if((HWND)lParam == h_rbLog3) {
          fuppes_set_loglevel(3);
          return 0;
        }       
         
      }
      break;
      
         
    case WM_TRAYICON:
      OnWmTrayicon(wParam, lParam);
      break;
         
    case WM_CLOSE:
      Hide();
      break;         
         
    case WM_DESTROY:
      PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
      break;
      
    default:                      /* for messages that we don't deal with */
      return DefWindowProc(p_hWnd, message, wParam, lParam);
  }

  return 0;        
}


void CMainForm::OnCreate()
{
 /* hWndRadioBox = CreateWindow("STATIC", NULL,
       WS_VISIBLE | WS_CHILD | SS_GRAYFRAME,
       0, 0, 0, 0, hWnd, NULL, // set size in WM_SIZE message
       (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL);*/
     
  // radio buttons
  h_rbLog0 = CreateWindow("BUTTON", "disabled", 
    WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
    8, 8, 100, 16, hWnd,  NULL,
    (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
     
  h_rbLog1 = CreateWindow("BUTTON", "normal", 
    WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
    108, 8, 100, 16, hWnd,  NULL,
    (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL);     
  
  h_rbLog2 = CreateWindow("BUTTON", "extended", 
    WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
    216, 8, 100, 16, hWnd,  NULL,
    (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL);  
    
  h_rbLog3 = CreateWindow("BUTTON", "debug", 
    WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
    324, 8, 100, 16, hWnd,  NULL,
    (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL);       
     
     
  // memo
  hWndMemo = CreateWindow("EDIT", NULL,
    WS_CHILD | WS_VISIBLE | WS_VSCROLL | 
    ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
    0, 0, 0, 0, hWnd,
   (HMENU)ID_EDITCHILD, 
   (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

}

void CMainForm::Show()
{
  ShowWindow(hWnd, SW_SHOW);
  m_bVisible = true;
}

void CMainForm::Hide()
{
  ShowWindow(hWnd, SW_HIDE);
  m_bVisible = false;    
}

void CMainForm::ShowTrayIcon()
{
  NOTIFYICONDATA tsym;
  ZeroMemory (&tsym, sizeof (NOTIFYICONDATA));

  tsym.cbSize = sizeof (NOTIFYICONDATA);
  tsym.hWnd   = hWnd;
  tsym.uID    = 1;
  tsym.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
  tsym.uCallbackMessage = WM_TRAYICON;
  tsym.hIcon  = wincl.hIcon;
  strcpy (tsym.szTip, GetAppShortName().c_str());
  Shell_NotifyIcon (NIM_ADD, &tsym);  
}

void CMainForm::HideTrayIcon()
{
  NOTIFYICONDATA tsym;
  ZeroMemory (&tsym, sizeof (NOTIFYICONDATA));

  tsym.cbSize = sizeof (NOTIFYICONDATA);
  tsym.hWnd   = hWnd;
	tsym.uID    = 1;
	tsym.uFlags = 0;
	Shell_NotifyIcon (NIM_DELETE, &tsym);     
}


void CMainForm::Log(std::string p_sLog)
{    
  p_sLog.append("\r\n");
  nLogLength += p_sLog.length();

  // if the memo is full we remove some lines at the start
  if(nLogLength > MAX_MEMO_LENGTH) {
    SendMessage(hWndMemo, EM_SETSEL, 0, p_sLog.length());
    SendMessage(hWndMemo, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)"");
    nLogLength -= p_sLog.length();   
    SendMessage(hWndMemo, EM_SETSEL, nLogLength, nLogLength);
  }
  
  // append message
  SendMessage(hWndMemo, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)p_sLog.c_str());  
}

void CMainForm::Notify(std::string p_sTitle, std::string p_sMsg)
{
  NOTIFYICONDATA tsym;
  ZeroMemory (&tsym, sizeof (NOTIFYICONDATA));
  
  tsym.cbSize = sizeof(NOTIFYICONDATA);
  tsym.hWnd   = hWnd;
  tsym.uID    = 1;
  tsym.uFlags = NIF_INFO;
  tsym.uTimeout = 5000; //uTimeout;
  tsym.dwInfoFlags = NIIF_INFO; //dwInfoFlags;
  strcpy(tsym.szInfo,!p_sMsg.empty() ? p_sMsg.c_str() : "");
  strcpy(tsym.szInfoTitle,p_sTitle.empty() ? p_sTitle.c_str() : "");
  Shell_NotifyIcon(NIM_MODIFY, &tsym);     
}

void CMainForm::Error(std::string p_sError)
{
  MessageBox(hWnd, p_sError.c_str(), GetAppShortName().c_str(), MB_ICONERROR | MB_OK);
}

void CMainForm::OnWmTrayicon(WPARAM wParam, LPARAM lParam)
{
  switch (lParam)
  {
    case WM_LBUTTONDBLCLK:
      !m_bVisible ? Show() : Hide();
      break;
         
    //case WM_LBUTTONUP:
    case WM_RBUTTONUP:
      OnTrayIconLButtonUp();
      break;      
  }
}

void CMainForm::OnTrayIconLButtonUp()
{
  POINT pt;
  unsigned int ret;
  SetForegroundWindow(hWnd);
	GetCursorPos(&pt);
	ret = TrackPopupMenu(hPopup, TPM_RETURNCMD|TPM_LEFTBUTTON, pt.x, pt.y, 0, hWnd, NULL);  
	PostMessage(hWnd, WM_NULL, 0, 0);
	
	stringstream sTmp;
	
	switch(ret)
	{
    case ID_TRAY_POPUP_QUIT:
      if(MessageBox(hWnd, "Shutdown FUPPES?", GetAppShortName().c_str(), MB_ICONQUESTION | MB_YESNO) == IDYES) {
         PostMessage(hWnd, WM_DESTROY, 0, 0);
      }
      break;
    case ID_TRAY_POPUP_REBUILD_DB:
      if(MessageBox(hWnd, "Rebuild database?", GetAppShortName().c_str(), MB_ICONQUESTION | MB_YESNO) == IDYES) {
        fuppes_rebuild_db();
      }
      break;
    case ID_TRAY_POPUP_UPDATE_DB:
      if(MessageBox(hWnd, "Update database?", GetAppShortName().c_str(), MB_ICONQUESTION | MB_YESNO) == IDYES) {
        fuppes_update_db();
      }
      break;			
			
    case ID_TRAY_POPUP_REBUILD_VFOLDER:
      if(MessageBox(hWnd, "Rebuild virtual containers?", GetAppShortName().c_str(), MB_ICONQUESTION | MB_YESNO) == IDYES) {
        fuppes_rebuild_vcontainers();
      }
      break;
			
    case ID_TRAY_POPUP_SHOW_WEBINF:
      char szAddr[128];
      fuppes_get_http_server_address(szAddr, 127);
      ShellExecute(hWnd, "open", szAddr, NULL, NULL, SW_NORMAL);
      sTmp.str("");
      break;
    
    case ID_TRAY_POPUP_SEND_ALIVE:
      fuppes_send_alive();
      break;
      
    case ID_TRAY_POPUP_SEND_BYEBYE:
      fuppes_send_byebye();
      break;
    
    case ID_TRAY_POPUP_SEND_MSEARCH:
      fuppes_send_msearch();
      break;      
  }
}

/*void CMainForm::AddLogMsg(std::string p_sMessage)
{
 // MessageBox(hWnd, p_sMessage.c_str(), "log", MB_ICONQUESTION | MB_OK);
}*/
