//
// This file is part of Condor2Nav file formats translator.
//
// Copyright (C) 2009 Mateusz Pusz
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
* @file gui\condor2navGUI.h
*
* @brief Declares the Condor2Nav graphical user interface class. 
*/

#ifndef __CONDOR2NAV_GUI_H__
#define __CONDOR2NAV_GUI_H__

#include "condor2nav.h"
#include "fileParserINI.h"
#include "widgets.h"

namespace condor2nav {

  namespace gui {

    class CCondor2NavGUI : public CCondor2Nav {
    public:
      class CLogger : public CCondor2Nav::CLogger {
        CWidgetRichEdit &_log;
        virtual void Dump(const std::string &str) const;
      public:
        CLogger(TType type, CWidgetRichEdit &log):condor2nav::CCondor2Nav::CLogger(type), _log(log) {}
      };

    private:
      const HWND _hDlg;
      const std::string _condorPath;
      const CFileParserINI _configParser;

      CWidgetRadioButton _fplDefault;
      CWidgetRadioButton _fplLastRace;
      CWidgetRadioButton _fplOther;
      CWidgetButton _fplSelect;
      CWidgetEdit _fplPath;

      CWidgetRadioButton _aatOff;
      CWidgetRadioButton _aatOn;
      CWidgetComboBox _aatTime;
      CWidget _aatMinutes;

      CWidgetButton _translate;

      CWidgetRichEdit _log;

      CLogger _normal;
      CLogger _warning;
      CLogger _error;

      bool TranslateValid() const;

    public:
      CCondor2NavGUI(HINSTANCE hInst, HWND hDlg);

      virtual const condor2nav::CCondor2Nav::CLogger &Log() const { return _normal; }
      virtual const condor2nav::CCondor2Nav::CLogger &Warning() const { return _warning; }
      virtual const condor2nav::CCondor2Nav::CLogger &Error() const { return _error; }

      void Command(HWND hwnd, int controlID, int command);
    };

  } // namespace gui

} // namespace condor2nav

#endif // __CONDOR2NAV_GUI_H__