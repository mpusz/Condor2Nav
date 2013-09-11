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
 * @file targetLK8000.h
 *
 * @brief Declares the condor2nav::CTargetLK8000 class. 
 */

#ifndef __TARGET_LK8000_H__
#define __TARGET_LK8000_H__

#include "targetXCSoarCommon.h"
#include "ostream.h"


namespace condor2nav {

  /**
   * @brief Translator to LK8000 data format.
   *
   * condor2nav::CTranslatorLK8000 class is responsible for Condor data translation
   * to LK8000 (http://www.bware.it/xcsoar) format.
   */
  class CTargetLK8000 : public CTargetXCSoarCommon {
    // dirs
    static const bfs::path AIRSPACES_SUBDIR;              ///< @brief LK8000 airspaces subdirectory. 
    static const bfs::path CONFIG_SUBDIR;                 ///< @brief LK8000 configuration subdirectory. 
    static const bfs::path MAPS_SUBDIR;                   ///< @brief LK8000 maps subdirectory. 
    static const bfs::path POLARS_SUBDIR;                 ///< @brief LK8000 polars subdirectory. 
    static const bfs::path TASKS_SUBDIR;                  ///< @brief LK8000 tasks subdirectory.
    static const bfs::path WAYPOINTS_SUBDIR;              ///< @brief LK8000 waypoints subdirectory.

    // inputs
    static const bfs::path DEFAULT_SYSTEM_PROFILE_NAME;   ///< @brief LK8000 system profile file name. 
    static const bfs::path DEFAULT_AIRCRAFT_PROFILE_NAME; ///< @brief LK8000 aircraft profile file name.

    // outputs
    static const bfs::path OUTPUT_AIRCRAFT_PROFILE_NAME;  ///< @brief The name of LK8000 aircraft profile file to generate. 

    std::unique_ptr<CFileParserINI> _systemParser;        ///< @brief LK8000 system profile file parser. 
    std::unique_ptr<CFileParserINI> _aircraftParser;      ///< @brief LK8000 aircraft profile file parser. 
    const bfs::path _outputLK8000DataPath;                ///< @brief The path to the output LK8000 directory.
    std::string _condor2navDataPathString;                ///< @brief The Condor2Nav destination data directory path (in LK8000 format) on the target device that runs LK8000.

    bfs::path _outputAirspacesSubDir;                     ///< @brief The subdirectory where output LK8000 airspaces file should be located
    bfs::path _outputMapsSubDir;                          ///< @brief The subdirectory where output LK8000 airspaces file should be located
    bfs::path _outputPolarsSubDir;                        ///< @brief The subdirectory where output LK8000 polars file should be located
    bfs::path _outputWaypointsSubDir;                     ///< @brief The subdirectory where output LK8000 waypoints file should be located

    COStream::CPathList _outputTaskFilePathList;          ///< @brief The path where output LK8000 task file should be located
    COStream::CPathList _outputSystemProfilePathList;     ///< @brief The path where output configuration paths should be located
    COStream::CPathList _outputAircraftProfilePathList;   ///< @brief The path where output configuration paths should be located

    void TaskDump(CFileParserINI &profileParser,
                  const CFileParserINI &taskParser,
                  const xcsoar::SETTINGS_TASK &settingsTask,
                  const xcsoar::TASK_POINT taskPointArray[],
                  const xcsoar::START_POINT startPointArray[],
                  const CWaypointArray &waypointArray) const override;

  public:
    explicit CTargetLK8000(const CTranslator &translator);
    virtual ~CTargetLK8000();

    const char *Name() const override { return "LK8000"; }
    void Gps() override;
    void SceneryMap(const CFileParserCSV::CStringArray &sceneryData) override;
    void SceneryTime() override;
    void Glider(const CFileParserCSV::CStringArray &gliderData) override;
    void Task(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv, const CFileParserCSV::CStringArray &sceneryData, unsigned aatTime) override;
    void PenaltyZones(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv) override;
    void Weather(const CFileParserINI &taskParser) override;
  };

}

#endif /* __TARGET_LK8000_H__ */
