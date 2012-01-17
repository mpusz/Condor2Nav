//
// This file is part of Condor2Nav file formats translator.
//
// Copyright (C) 2009-2011 Mateusz Pusz
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

  class CCondor;

  /**
   * @brief Condor2Nav GUI interface namespace.
   */
  namespace gui {

    /**
     * @brief Main GUI project class.
     *
     * condor2nav::CCondor2NavGUI is a main GUI project class. It is responsible for
     * getting needed action from the user and running the translation.
     */
    class CCondor2NavGUI : public CCondor2Nav {
    public:

      /**
       * @brief Logger. 
       *
       * Class is responsible for logging Condor2Nav traces to the logging window
       */
      class CLogger : public CCondor2Nav::CLogger {
        CWidgetRichEdit &_log;	              ///< @brief The logging window widget
        virtual void Dump(const std::string &str) const override;
      public:
        CLogger(TType type, CWidgetRichEdit &log);
      };

    private:
      const HWND _hDlg;	                         ///< @brief The dialog handle
      const boost::filesystem::path _condorPath; ///< @brief Full pathname of the Condor directory
      const CFileParserINI _configParser;	     ///< @brief The INI file configuration parser

      CWidgetRadioButton _fplDefault;	         ///< @brief The default FPL button
      CWidgetRadioButton _fplLastRace;           ///< @brief The last race FPL button
      CWidgetRadioButton _fplOther;              ///< @brief The other FPL path button
      CWidgetButton _fplSelect;                  ///< @brief The FPL path selection button
      CWidgetEdit _fplPath;                      ///< @brief Currently selected FPL path

      CWidgetRadioButton _aatOff;                ///< @brief The AAT task OFF button
      CWidgetRadioButton _aatOn;                 ///< @brief The AAT task ON button
      CWidgetComboBox _aatTime;                  ///< @brief The AAT task minimum time combo box
      CWidget _aatMinutes;                       ///< @brief The AAT task minimum time minutes label

      CWidgetButton _translate;                  ///< @brief The Condor2Nav start translation button

      CWidgetRichEdit _log;                      ///< @brief The Condor2Nav logging window

      CLogger _normal;                           ///< @brief Normal logging level logger
      CLogger _warning;                          ///< @brief Warning logging level logger
      CLogger _error;                            ///< @brief Error logging level logger

      void AATCheck(const CCondor &condor) const;
      bool TranslateValid() const;

    public:
      CCondor2NavGUI(HINSTANCE hInst, HWND hDlg);

      virtual const CLogger &Log() const override     { return _normal; }
      virtual const CLogger &Warning() const override { return _warning; }
      virtual const CLogger &Error() const override   { return _error; }

      void Command(HWND hwnd, int controlID, int command);
    };

  } // namespace gui

} // namespace condor2nav

#endif // __CONDOR2NAV_GUI_H__
