//
// This file is part of Condor2Nav file formats translator.
//
// Copyright (C) 2009-2010 Mateusz Pusz
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
* @file gui\condor2navGUI.cpp
*
* @brief Implements the Condor2Nav graphical user interface class. 
*/

#include "stdafx.h"
#include "condor2navGUI.h"
#include "resource.h"
#include "translator.h"



/**
 * @brief Class constructor.
 *
 * @param type         The logger type. 
 * @param [in,out] log Logging window widget. 
 */
condor2nav::gui::CCondor2NavGUI::CLogger::CLogger(TType type, CWidgetRichEdit &log):
condor2nav::CCondor2Nav::CLogger(type), _log(log)
{

}


/**
 * @brief Dumps the text to the logger window.
 *
 * @param str The string to dump. 
 *
 * @exception EOperationFailed Thrown when operation failed to execute. 
 */
void condor2nav::gui::CCondor2NavGUI::CLogger::Dump(const std::string &str) const
{
  switch(Type()) {
  case TYPE_NORMAL:
    _log.Format(0, CWidgetRichEdit::COLOR_AUTO);
    break;
  case TYPE_WARNING:
    _log.Format(CWidgetRichEdit::EFFECT_BOLD, CWidgetRichEdit::COLOR_BLUE);
    break;
  case TYPE_ERROR:
    _log.Format(CWidgetRichEdit::EFFECT_BOLD, CWidgetRichEdit::COLOR_RED);
    break;
  default:
    throw EOperationFailed("ERROR: Unsupported logger type (" + Convert(Type()) + ")!!!");
  }
  _log.Append(str);
}


/**
 * @brief Class constructor. 
 *
 * @param hInst The instance. 
 * @param hDlg  Handle of the dialog. 
 */
condor2nav::gui::CCondor2NavGUI::CCondor2NavGUI(HINSTANCE hInst, HWND hDlg):
_hDlg(hDlg),
_condorPath(CCondor::InstallPath()),
_configParser(CCondor2NavGUI::CONFIG_FILE_NAME),
_fplDefault(hDlg, IDC_FPL_DEFAULT_RADIO),
_fplLastRace(hDlg, IDC_FPL_LAST_RACE_RADIO),
_fplOther(hDlg, IDC_FPL_OTHER_RADIO),
_fplSelect(hDlg, IDC_FPL_SELECT_BUTTON, true),
_fplPath(hDlg, IDC_FPL_PATH_EDIT),
_aatOff(hDlg, IDC_AAT_OFF_RADIO),
_aatOn(hDlg, IDC_AAT_ON_RADIO),
_aatTime(hDlg, IDC_AAT_TIME_COMBO, true),
_aatMinutes(hDlg, IDC_AAT_STATIC, true),
_translate(hDlg, IDC_TRANSLATE_BUTTON),
_log(hDlg, IDC_LOG_RICHEDIT2),
_normal(CLogger::TYPE_NORMAL, _log),
_warning(CLogger::TYPE_WARNING, _log),
_error(CLogger::TYPE_ERROR, _log)
{
  // Attach icon to main dialog
  SendMessage(hDlg, WM_SETICON, ICON_BIG, LPARAM(LoadIcon(hInst, MAKEINTRESOURCE(IDI_CONDOR2NAV))));
  SendMessage(hDlg, WM_SETICON, ICON_SMALL, LPARAM(LoadIcon(hInst, MAKEINTRESOURCE(IDI_CONDOR2NAV))));

  // set default task
  _fplDefault.Select();
  std::string fplPath;
  CCondor::FPLPath(_configParser, CCondor2NavGUI::TYPE_DEFAULT, _condorPath, fplPath);
  _fplPath.String(fplPath);

  // check if last result is available
  try
  {
    CCondor::FPLPath(_configParser, CCondor2NavGUI::TYPE_RESULT, _condorPath, fplPath);
  }
  catch(const Exception &)
  {
    _fplLastRace.Disable();
  }
  
  _aatOff.Select();
  for(unsigned i=2;i<=20;i++)
    _aatTime.Add(Convert(i * 15));
}


/**
 * @brief Checks if translation is valid to execute.
 *
 * @return true if it succeeds, false if it fails. 
 */
bool condor2nav::gui::CCondor2NavGUI::TranslateValid() const
{
  return (!_fplOther.Selected() || _fplPath.String() != "") && (!_aatOn.Selected() || _aatTime.Selection() != "" || _aatTime.ItemSelected());
}


/**
 * @brief Processes a command send to the application widget.
 *
 * @param hwnd      Handle of the hwnd.
 * @param controlID Identifier for the control. 
 * @param command   The command to process.
 */
void condor2nav::gui::CCondor2NavGUI::Command(HWND hwnd, int controlID, int command)
{
  bool changed = false;

  switch(controlID) {
  case IDC_FPL_DEFAULT_RADIO:
    if(command == BN_CLICKED) {
      _fplSelect.Disable();

      // create Condor FPL file path
      std::string fplPath;
      CCondor::FPLPath(_configParser, CCondor2NavGUI::TYPE_DEFAULT, _condorPath, fplPath);
      _fplPath.String(fplPath);

      changed = true;
    }
    break;

  case IDC_FPL_LAST_RACE_RADIO:
    if(command == BN_CLICKED) {
      _fplSelect.Disable();

      // create Condor FPL file path
      std::string fplPath;
      CCondor::FPLPath(_configParser, CCondor2NavGUI::TYPE_RESULT, _condorPath, fplPath);
      _fplPath.String(fplPath);

      changed = true;
    }
    break;

  case IDC_FPL_OTHER_RADIO:
    if(command == BN_CLICKED) {
      _fplPath.String("");
      _fplSelect.Enable();
      changed = true;
    }
    break;

  case IDC_FPL_SELECT_BUTTON:
    if(command == BN_CLICKED) {
      OPENFILENAME ofn;       // common dialog box structure
      char szFile[1024];       // buffer for file name

      // Initialize OPENFILENAME
      ZeroMemory(&ofn, sizeof(ofn));
      ofn.lStructSize = sizeof(ofn);
      ofn.hwndOwner = _hDlg;
      ofn.lpstrFilter = "Condor FPL Files (*.fpl)\0*.fpl\0All Files (*.*)\0*.*\0";
      ofn.nFilterIndex = 1;
      ofn.lpstrFile = szFile;
      // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
      // use the contents of szFile to initialize itself.
      ofn.lpstrFile[0] = '\0';
      ofn.nMaxFile = sizeof(szFile);
      ofn.lpstrFileTitle = NULL;
      ofn.nMaxFileTitle = 0;
      ofn.lpstrInitialDir = NULL;
      ofn.lpstrTitle = "Open Condor FPL file";
      ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_READONLY;

      // Display the Open dialog box. 
      if(GetOpenFileName(&ofn) == TRUE)
        _fplPath.String(ofn.lpstrFile);

      changed = true;
    }
    break;

  case IDC_AAT_OFF_RADIO:
    if(command == BN_CLICKED) {
      _aatTime.Disable();
      _aatMinutes.Disable();
      changed = true;
    }
    break;

  case IDC_AAT_ON_RADIO:
    if(command == BN_CLICKED) {
      _aatTime.Enable();
      _aatMinutes.Enable();
      changed = true;
    }
    break;

  case IDC_AAT_TIME_COMBO:
    if(command == CBN_CLOSEUP) {
      changed = true;
    }
    break;

  case IDC_TRANSLATE_BUTTON:
    if(command == BN_CLICKED) {
      try
      {
        _log.Clear();
        CTranslator(*this, _configParser, _condorPath, _fplPath.String(), _aatOn.Selected() ? Convert<unsigned>(_aatTime.Selection()) : 0).Run();
      }
      catch(const Exception &ex)
      {
        Error() << ex.what() << std::endl;
      }
    }
    break;
  }

  if(changed) {
    _log.Clear();
    TranslateValid() ? _translate.Enable() : _translate.Disable();
  }
}
