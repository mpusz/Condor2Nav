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
 * @file targetXCSoar.h
 *
 * @brief Declares the condor2nav::CTargetXCSoar class. 
**/

#ifndef __TARGET_XCSOAR_H__
#define __TARGET_XCSOAR_H__

#include "targetXCSoarCommon.h"


namespace condor2nav {

  /**
   * @brief Translator to XCSoar data format.
   *
   * condor2nav::CTranslatorXCSoar class is responsible for Condor data translation
   * to XCSoar (http://www.xcsoar.org) format.
  **/
  class CTargetXCSoar : public CTargetXCSoarCommon {
    // inputs
    static const char *XCSOAR_PROFILE_NAME;	     ///< @brief XCSoar profile file name to use for input. 

    std::auto_ptr<CFileParserINI> _profileParser;///< @brief XCSoar profile file parser. 
    const std::string _outputXCSoarDataPath;     ///< @brief The path to the output XCSoarData directory.
    std::string _outputCondor2NavDataPath;       ///< @brief The path to the output Condor2Nav directory.
    std::string _condor2navDataPath;             ///< @brief The Condor2Nav destination data directory path (in XCSoar format) on the target device that runs XCSoar.
    std::string _outputTaskFilePath;             ///< @brief The path where output XCSoar task file should be located
    
  public:
    explicit CTargetXCSoar(const CTranslator &translator);
    virtual ~CTargetXCSoar();

    virtual void Gps();
    virtual void SceneryMap(const CFileParserCSV::CStringArray &sceneryData);
    virtual void SceneryTime();
    virtual void Glider(const CFileParserCSV::CStringArray &gliderData);
    virtual void Task(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv, const CFileParserCSV::CStringArray &sceneryData, unsigned aatTime);
    virtual void PenaltyZones(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv);
    virtual void Weather(const CFileParserINI &taskParser);
  };

}

#endif /* __TARGET_XCSOAR_H__ */
