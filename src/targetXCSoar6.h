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
 * @file targetXCSoar6.h
 *
 * @brief Declares the condor2nav::CTargetXCSoar6 class. 
 */

#ifndef __TARGET_XCSOAR_6_H__
#define __TARGET_XCSOAR_6_H__

#include "targetXCSoar.h"


namespace condor2nav {

  /**
   * @brief Translator to XCSoar v6 data format.
   *
   * condor2nav::CTranslatorXCSoar class is responsible for Condor data translation
   * to XCSoar v6 (http://www.xcsoar.org) format.
   */
  class CTargetXCSoar6 : public CTargetXCSoar {
    virtual void TaskDump(CFileParserINI &profileParser, const CFileParserINI &taskParser, const boost::filesystem::path &outputTaskFilePath,
                          const xcsoar::SETTINGS_TASK &settingsTask, const xcsoar::TASK_POINT *taskPointArray,
                          const xcsoar::START_POINT *startPointArray, const CWaypointArray &waypointArray) const override;
  public:
    explicit CTargetXCSoar6(const CTranslator &translator);
    virtual const char *Name() const override { return "XCSoar 6"; }
  };

}

#endif /* __TARGET_XCSOAR_6_H__ */
