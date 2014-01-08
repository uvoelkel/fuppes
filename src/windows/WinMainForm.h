/***************************************************************************
 *            WinMainForm.h
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

//#define _WIN32_IE 0x0500

#include <windows.h>
#include <string>

class CMainForm
{
  public:
    CMainForm(HINSTANCE hInstance);
    ~CMainForm();    
      
    LRESULT CALLBACK WindowProc(HWND p_hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    
    void OnCreate();
      
    void Show();
    void Hide();
      
    void ShowTrayIcon();
    void HideTrayIcon();
      
    void OnWmTrayicon(WPARAM wParam, LPARAM lParam);
      
    void OnTrayIconLButtonUp();
      
    void Log(std::string p_sLog);
    void Notify(std::string p_sTitle, std::string p_sMsg);    
    void Error(std::string p_sError);    
      
  private:    
    HWND        hWnd;     // This is the handle for our window 
    WNDCLASSEX  wincl;    // Data structure for the windowclass 
    HMENU       hPopup;   // popup menu
    bool        m_bVisible;
    unsigned int nLogLength;
    
    HWND        hWndMemo;    
    
    HWND        h_rbLog0;
    HWND        h_rbLog1;
    HWND        h_rbLog2;
    HWND        h_rbLog3;
    
    std::string m_sTitle;
};
