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
 * @file targetXCSoarCommon.h
 *
 * @brief Declares the condor2nav::CTargetXCSoarCommon class. 
**/

#ifndef __TARGET_XCSOAR_COMMON_H__
#define __TARGET_XCSOAR_COMMON_H__

#include "translator.h"


namespace condor2nav {

  /**
   * @brief Common tools for XCSoar family translations.
   *
   * condor2nav::CTranslatorXCSoarCommon class is responsible for Condor data translation
   * to XCSoar (http://www.xcsoar.org) format.
  **/
  class CTargetXCSoarCommon : public CTranslator::CTarget {
  protected:
    // outputs
    static const char *OUTPUT_PROFILE_NAME;      ///< @brief The name of XCSoar profile file to generate. 
    static const char *TASK_FILE_NAME;           ///< @brief The name of XCSoar task file to generate. 
    static const char *DEFAULT_TASK_FILE_NAME;   ///< @brief The name of the default XCSoar task file. 
    static const char *POLAR_FILE_NAME;          ///< @brief The name of XCSoar glider polar file to generate.
    static const char *AIRSPACES_FILE_NAME;      ///< @brief The name of XCSoar airspaces file to generate. 
    static const unsigned WAYPOINT_INDEX_OFFSET = 100000; ///< @brief A big value that should point behind all the waypoints

    void SceneryMapProcess(CFileParserINI &profileParser, const CFileParserCSV::CStringArray &sceneryData, const std::string &pathPrefix) const;
    void SceneryTimeProcess(CFileParserINI &profileParser, const CFileParserCSV::CStringArray &sceneryData) const;
    void GliderProcess(CFileParserINI &profileParser, const CFileParserCSV::CStringArray &gliderData, const std::string &pathPrefix, const std::string &outputPathPrefix) const;
    void TaskProcess(CFileParserINI &profileParser, const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv, const std::string &outputTaskFilePath) const;
    void PenaltyZonesProcess(CFileParserINI &profileParser, const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv, const std::string &pathPrefix, const std::string &outputPathPrefix) const;
    void WeatherProcess(CFileParserINI &profileParser, const CFileParserINI &taskParser) const;

  public:
    explicit CTargetXCSoarCommon(const CTranslator &translator);
  };

}

#endif /* __TARGET_XCSOAR_COMMON_H__ */
