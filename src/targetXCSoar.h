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
 * @file targetXCSoar.h
 *
 * @brief Declares the condor2nav::CTargetXCSoar class. 
 */

#ifndef __TARGET_XCSOAR_H__
#define __TARGET_XCSOAR_H__

#include "targetXCSoarCommon.h"


namespace condor2nav {

  /**
   * @brief Translator to XCSoar data format.
   *
   * condor2nav::CTranslatorXCSoar class is responsible for Condor data translation
   * to XCSoar (http://www.xcsoar.org) format.
   */
  class CTargetXCSoar : public CTargetXCSoarCommon {
    // inputs
    static const boost::filesystem::path XCSOAR_PROFILE_NAME; ///< @brief XCSoar profile file name to use for input. 

    std::unique_ptr<CFileParserINI> _profileParser;           ///< @brief XCSoar profile file parser. 
    const boost::filesystem::path _outputXCSoarDataPath;      ///< @brief The path to the output XCSoarData directory.
    boost::filesystem::path _outputCondor2NavDataPath;        ///< @brief The path to the output Condor2Nav directory.
    boost::filesystem::path _outputTaskFilePath;              ///< @brief The path where output XCSoar task file should be located
    std::string _condor2navDataPathString;                    ///< @brief The Condor2Nav destination data directory path (in XCSoar format) on the target device that runs XCSoar.
    
    virtual void TaskDump(CFileParserINI &profileParser,
                          const CFileParserINI &taskParser, 
                          const boost::filesystem::path &outputTaskFilePath,
                          const xcsoar::SETTINGS_TASK &settingsTask, 
                          const xcsoar::TASK_POINT taskPointArray[],
                          const xcsoar::START_POINT startPointArray[],
                          const CWaypointArray &waypointArray) const override;

  public:
    explicit CTargetXCSoar(const CTranslator &translator);
    virtual ~CTargetXCSoar();

    virtual const char *Name() const override { return "XCSoar 5"; }
    virtual void Gps() override;
    virtual void SceneryMap(const CFileParserCSV::CStringArray &sceneryData) override;
    virtual void SceneryTime() override;
    virtual void Glider(const CFileParserCSV::CStringArray &gliderData) override;
    virtual void Task(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv, const CFileParserCSV::CStringArray &sceneryData, unsigned aatTime) override;
    virtual void PenaltyZones(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv) override;
    virtual void Weather(const CFileParserINI &taskParser) override;
  };

}

#endif /* __TARGET_XCSOAR_H__ */
