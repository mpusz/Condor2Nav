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
 * @file targetXCSoarCommon.h
 *
 * @brief Declares the condor2nav::CTargetXCSoarCommon class. 
 */

#ifndef __TARGET_XCSOAR_COMMON_H__
#define __TARGET_XCSOAR_COMMON_H__

#include "translator.h"
#include "imports/xcsoarTypes.h"


namespace condor2nav {

  class COStream;

  /**
   * @brief Common tools for XCSoar family translations.
   *
   * condor2nav::CTranslatorXCSoarCommon class is responsible for Condor data translation
   * to XCSoar (http://www.xcsoar.org) format.
   */
  class CTargetXCSoarCommon : public CTranslator::CTarget {
    unsigned WaypointBearing(double lon1, double lat1, double lon2, double lat2) const;
    
  protected:
    /**
     * @brief Waypoint data.
     */
    struct TWaypoint {
      int number;
      double latitude;
      double longitude;
      double altitude;
      int flags;
      std::string name;
      std::string comment;
      bool inTask;
    };
    typedef std::vector<TWaypoint> CWaypointArray;

    typedef void (CTargetXCSoarCommon::*FTaskDump)(CFileParserINI &, const std::string &, const xcsoar::SETTINGS_TASK &, const xcsoar::TASK_POINT *, const xcsoar::START_POINT *, const CWaypointArray &) const;
    void TaskDumpXCSoar(CFileParserINI &profileParser, const std::string &outputTaskFilePath, const xcsoar::SETTINGS_TASK &settingsTask, const xcsoar::TASK_POINT *taskPointArray, const xcsoar::START_POINT *startPointArray, const CWaypointArray &waypointArray) const;
    void TaskDumpLK8000(CFileParserINI &profileParser, const std::string &outputTaskFilePath, const xcsoar::SETTINGS_TASK &settingsTask, const xcsoar::TASK_POINT *taskPointArray, const xcsoar::START_POINT *startPointArray, const CWaypointArray &waypointArray) const;

    // outputs
    static const char *OUTPUT_PROFILE_NAME;      ///< @brief The name of XCSoar profile file to generate. 
    static const char *TASK_FILE_NAME;           ///< @brief The name of XCSoar task file to generate. 
    static const char *DEFAULT_TASK_FILE_NAME;   ///< @brief The name of the default XCSoar task file. 
    static const char *POLAR_FILE_NAME;          ///< @brief The name of XCSoar glider polar file to generate.
    static const char *AIRSPACES_FILE_NAME;      ///< @brief The name of XCSoar airspaces file to generate. 
    static const char *WP_FILE_NAME;             ///< @brief The name of XCSoar WP file with task waypoints.
    static const unsigned WAYPOINT_INDEX_OFFSET = 100000; ///< @brief A big value that should point behind all the waypoints

    void SceneryMapProcess(CFileParserINI &profileParser, const CFileParserCSV::CStringArray &sceneryData, const std::string &pathPrefix) const;
    void SceneryTimeProcess(CFileParserINI &profileParser) const;
    void GliderProcess(CFileParserINI &profileParser, const CFileParserCSV::CStringArray &gliderData, bool wingArea, const std::string &pathPrefix, const std::string &outputPathPrefix) const;
    void TaskProcess(CFileParserINI &profileParser, const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv,
                     const CFileParserCSV::CStringArray &sceneryData, const std::string &outputTaskFilePath,
                     unsigned aatTime, unsigned maxTaskPoints, unsigned maxStartPoints, FTaskDump taskDumpFunc,
                     bool generateWPFile, const std::string &wpOutputPathPrefix) const;
    void PenaltyZonesProcess(CFileParserINI &profileParser, const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv, const std::string &pathPrefix, const std::string &outputPathPrefix) const;
    void WeatherProcess(CFileParserINI &profileParser, const CFileParserINI &taskParser) const;

  public:
    explicit CTargetXCSoarCommon(const CTranslator &translator);
  };

}

#endif /* __TARGET_XCSOAR_COMMON_H__ */
