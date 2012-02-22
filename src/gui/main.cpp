//
// This file is part of Condor2Nav file formats translator.
//
// Copyright (C) 2009-2012 Mateusz Pusz
//
// Condor2Nav is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Condor2Nav is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Condor2Nav. If not, see <http://www.gnu.org/licenses/>.
//
// Visit the project webpage (http://sf.net/projects/condor2nav) for more info.
//

/**
* @file gui/main.cpp
*
* @brief Defines the entry point for the GUI application. 
*/

#include "stdafx.h"
#include "condor2navGUI.h"
#include "widgets.h"
#include "resource.h"
#include <exception>

static HINSTANCE hInst = nullptr;


namespace condor2nav {

  namespace gui {

    INT_PTR CALLBACK AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    INT_PTR CALLBACK MainDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

  }

}


/**
 * @brief Message handler for about box.
 *
 * @param hDlg    Handle of the dialog. 
 * @param message The message to process. 
 * @param wParam  The wParam field of the message. 
 * @param lParam  The lParam field of the message. 
 *
 * @return true if the message was handled.
 */
INT_PTR CALLBACK condor2nav::gui::AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message) {
  case WM_INITDIALOG:
    return TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      EndDialog(hDlg, LOWORD(wParam));
      return TRUE;
    }
    break;
  }
  return FALSE;
}


/**
 * @brief Message handler for main application window.
 *
 * @param hDlg    Handle of the dialog. 
 * @param message The message to process. 
 * @param wParam  The wParam field of the message. 
 * @param lParam  The lParam field of the message. 
 *
 * @return true if the message was handled.
 */
INT_PTR CALLBACK condor2nav::gui::MainDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  static condor2nav::gui::CCondor2NavGUI *app = nullptr;
  switch(message) {
  case WM_INITDIALOG:
    app = new CCondor2NavGUI(hInst, hDlg);
    app->OnStart([=]{ return app->Abort(); });
    return TRUE;

  case WM_COMMAND:
    {
      int wmId = LOWORD(wParam);
      int wmEvent = HIWORD(wParam);
      switch(wmId) {
      // Parse the menu selections
      case IDM_ABOUT:
        DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hDlg, AboutDialogProc);
        return TRUE;
      case IDM_EXIT:
        DestroyWindow(hDlg);
        return TRUE;
      default:
        // Pass the command to MainDialog widgets
        app->Command(hDlg, wmId, wmEvent);
      }
    }
    return TRUE;

  case WM_LOG:
    app->Log(static_cast<CCondor2NavGUI::CLogger::TType>(wParam), std::unique_ptr<std::string>(reinterpret_cast<std::string *>(lParam)));
    return TRUE;

  case WM_CLOSE:
    delete app;
    DestroyWindow(hDlg);
    return TRUE;

  case WM_DESTROY:
    PostQuitMessage(0);
    return TRUE;
  }
  return FALSE;
}


/**
 * @brief GUI application main entry point.
 *
 * @param hInstance         The instance.
 * @param hPrevInst         The previous instance. 
 * @param [in,out] cmdParam If non-null, the command parameter. 
 * @param cmdShow           Specifies if a command should be shown. 
 *
 * @exception condor2nav::EOperationFailed Thrown when operation failed to execute. 
 *
 * @return Application exit code. 
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, char *cmdParam, int cmdShow)
{
  try {
    hInst = hInstance;

    // init RichEdit controls
    std::unique_ptr<HMODULE, condor2nav::CHModuleDeleter> _richEditLib(::LoadLibrary("RichEd20.dll"));

    // create MainDialog window
    HWND hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_MAIN_DIALOG), nullptr, (DLGPROC)condor2nav::gui::MainDialogProc);
    if(!hDialog)
      throw condor2nav::EOperationFailed("Unable to create main application dialog (error: " + condor2nav::Convert(GetLastError()) + ")!!!");

    // process application messages
    MSG msg;
    int status;
    while((status = GetMessage(&msg, nullptr, 0, 0)) != 0) {
      if(status == -1)
        return EXIT_FAILURE;
      if(!IsDialogMessage(hDialog, &msg)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    
    return msg.wParam;
  }
  catch(const condor2nav::Exception &ex) {
    MessageBox(0, ex.what(), "Condor2Nav Exception", MB_ICONEXCLAMATION | MB_OK);
  }
  catch(const std::exception &ex) {
    MessageBox(0, ex.what(), "Exception", MB_ICONEXCLAMATION | MB_OK);
  }
  catch(...) {
    MessageBox(0, "Unknown", "Exception", MB_ICONEXCLAMATION | MB_OK);
  }
  
  return EXIT_FAILURE;
}
