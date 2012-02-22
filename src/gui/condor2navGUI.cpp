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
* @file condor2navGUI.cpp
*
* @brief Implements the Condor2Nav graphical user interface class. 
*/

#include "stdafx.h"
#include "condor2navGUI.h"
#include "resource.h"
#include "translator.h"
#include "condor.h"
#include <array>


/**
 * @brief Class constructor.
 *
 * @param type   The logger type.
 * @param hDlg   Main dialog window handle.
 */
condor2nav::gui::CCondor2NavGUI::CLogger::CLogger(TType type, HWND hDlg):
condor2nav::CCondor2Nav::CLogger(type), _hDlg(hDlg)
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
  std::unique_ptr<std::string> dup(new std::string(str));
  PostMessage(_hDlg, WM_LOG, Type(), reinterpret_cast<WPARAM>(dup.release()));
}


/**
 * @brief Class constructor. 
 *
 * @param hInst The instance. 
 * @param hDlg  Handle of the dialog. 
 */
condor2nav::gui::CCondor2NavGUI::CCondor2NavGUI(HINSTANCE hInst, HWND hDlg):
_running(false),
_abort(false),
_normal(CLogger::TYPE_LOG_NORMAL, hDlg),
_high(CLogger::TYPE_LOG_HIGH, hDlg),
_warning(CLogger::TYPE_WARNING, hDlg),
_error(CLogger::TYPE_ERROR, hDlg),
_hDlg(hDlg),
_condorPath(CCondor::InstallPath()),
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
_log(hDlg, IDC_LOG_RICHEDIT2)
{
  // Attach icon to main dialog
  SendMessage(hDlg, WM_SETICON, ICON_BIG, LPARAM(LoadIcon(hInst, MAKEINTRESOURCE(IDI_CONDOR2NAV))));
  SendMessage(hDlg, WM_SETICON, ICON_SMALL, LPARAM(LoadIcon(hInst, MAKEINTRESOURCE(IDI_CONDOR2NAV))));

  // set AAT data
  _aatOff.Select();
  for(unsigned i=2; i<=20; i++)
    _aatTime.Add(Convert(i * 15));

  // set default task
  auto fplPath = CCondor::FPLPath(ConfigParser(), CCondor2NavGUI::TYPE_DEFAULT, _condorPath);

  try {
    CCondor condor(_condorPath, fplPath);
    _fplPath.String(fplPath.string());
    AATCheck(condor);
    _fplDefault.Select();
  }
  catch(const Exception &) {
    _fplDefault.Disable();
    _fplOther.Select();
    _fplSelect.Enable();
  }

  // check if last result is available
  try {
    fplPath = CCondor::FPLPath(ConfigParser(), CCondor2NavGUI::TYPE_RESULT, _condorPath);
  }
  catch(const Exception &) {
    _fplLastRace.Disable();
  }
}


condor2nav::gui::CCondor2NavGUI::~CCondor2NavGUI()
{
  _abort = true;
}


/**
 * @brief Checks if condor-club AAT task file.
 *
 * Method Checks if condor-club AAT task file is provided. It looks for certain entries
 * that are added by http://condor-club.eu server to the file.
 *
 * @param condor Condor wrapper
 */
void condor2nav::gui::CCondor2NavGUI::AATCheck(const CCondor &condor) const
{
  try {
    const CFileParserINI &taskParser = condor.TaskParser();
    if(taskParser.Value("Task", "AAT") == "Distance")
      Error() << "ERROR: AAT/D tasks are not supported!!!" << std::endl;
    else if(taskParser.Value("Task", "AAT") == "Speed") {
      _aatOn.Click();
      try {
        _aatTime.String(taskParser.Value("Task", "DesignatedTime"));
      }
      catch(EOperationFailed &) {
        Error() << "ERROR: Corrupted condor-club task file!!!" << std::endl;
      }
    }
  }
  catch(EOperationFailed &) {
  }
}


/**
 * @brief Checks if translation is valid to execute.
 *
 * @return true if it succeeds, false if it fails. 
 */
bool condor2nav::gui::CCondor2NavGUI::TranslateValid() const
{
  return !_running && (!_fplOther.Selected() || _fplPath.String() != "") && (!_aatOn.Selected() || _aatTime.Selection() != "" || _aatTime.ItemSelected());
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
  bool fplChanged = false;
  bool changed = false;

  switch(controlID) {
  case IDC_FPL_DEFAULT_RADIO:
    if(command == BN_CLICKED) {
      _fplSelect.Disable();

      // create Condor FPL file path
      auto fplPath = CCondor::FPLPath(ConfigParser(), CCondor2NavGUI::TYPE_DEFAULT, _condorPath);
      _fplPath.String(fplPath.string());

      fplChanged = true;
    }
    break;

  case IDC_FPL_LAST_RACE_RADIO:
    if(command == BN_CLICKED) {
      _fplSelect.Disable();

      // create Condor FPL file path
      auto fplPath = CCondor::FPLPath(ConfigParser(), CCondor2NavGUI::TYPE_RESULT, _condorPath);
      _fplPath.String(fplPath.string());

      fplChanged = true;
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
      OPENFILENAME ofn;                // common dialog box structure
      std::array<char, 1024> szFile;   // buffer for file name

      // Initialize OPENFILENAME
      ZeroMemory(&ofn, sizeof(ofn));
      ofn.lStructSize = sizeof(ofn);
      ofn.hwndOwner = _hDlg;
      ofn.lpstrFilter = "Condor FPL Files (*.fpl)\0*.fpl\0All Files (*.*)\0*.*\0";
      ofn.nFilterIndex = 1;
      ofn.lpstrFile = szFile.data();
      // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
      // use the contents of szFile to initialize itself.
      ofn.lpstrFile[0] = '\0';
      ofn.nMaxFile = sizeof(szFile);
      ofn.lpstrFileTitle = nullptr;
      ofn.nMaxFileTitle = 0;
      ofn.lpstrInitialDir = nullptr;
      ofn.lpstrTitle = "Open Condor FPL file";
      ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_READONLY;

      // Display the Open dialog box. 
      if(GetOpenFileName(&ofn) == TRUE) {
        _fplPath.String(ofn.lpstrFile);
        fplChanged = true;
      }

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
      _log.Clear();
      _activeObject.Send([this]{
        try {
          _running = true;
          _translate.Disable();

          CCondor condor(_condorPath, _fplPath.String());
          CTranslator(*this, ConfigParser(), condor, _aatOn.Selected() ? Convert<unsigned>(_aatTime.Selection()) : 0).Run();

          _running = false;
          if(TranslateValid())
            _translate.Enable();
        }
        catch(const std::exception &ex) {
          Error() << ex.what() << std::endl;
        }
      });
    }
    break;
  }

  if(fplChanged) {
    CCondor condor(_condorPath, _fplPath.String());
    AATCheck(condor);
  }
  if(changed || fplChanged) {
    if(TranslateValid())
      _translate.Enable();
    else
      _translate.Disable();
  }
}


void condor2nav::gui::CCondor2NavGUI::OnStart(std::function<bool()> abort)
{
  _activeObject.Send([this, abort]{
    try {
      _running = true;
      _translate.Disable();
      this->CCondor2Nav::OnStart(abort);
      _running = false;
      if(TranslateValid())
        _translate.Enable();
    }
    catch(const std::exception &ex) {
      Error() << ex.what() << std::endl;
    }
  });
}


void condor2nav::gui::CCondor2NavGUI::Log(CLogger::TType type, std::unique_ptr<std::string> &&str)
{
  switch(type) {
  case CLogger::TYPE_LOG_NORMAL:
    _log.Format(0, CWidgetRichEdit::COLOR_AUTO);
    break;
  case CLogger::TYPE_LOG_HIGH:
    _log.Format(0, CWidgetRichEdit::COLOR_BLUE);
    break;
  case CLogger::TYPE_WARNING:
    _log.Format(CWidgetRichEdit::EFFECT_BOLD, CWidgetRichEdit::COLOR_GREEN);
    break;
  case CLogger::TYPE_ERROR:
    _log.Format(CWidgetRichEdit::EFFECT_BOLD, CWidgetRichEdit::COLOR_RED);
    break;
  default:
    throw EOperationFailed("ERROR: Unsupported logger type (" + Convert(type) + ")!!!");
  }
  _log.Append(*str.release());
}
