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
 * @file xcsoar.h
 *
 * @brief Declares the condor2nav::CTranslatorXCSoar class. 
**/

#ifndef __XCSOAR_H__
#define __XCSOAR_H__

#include "translator.h"
#include "fileParserCSV.h"
#include "fileParserINI.h"
#include <string>


namespace condor2nav {

  /**
   * @brief Translator to XCSoar data format.
   *
   * condor2nav::CTranslatorXCSoar class is responsible for Condor data translation
   * to XCSoar (http://www.xcsoar.org) format.
  **/
  class CTranslatorXCSoar : public CTranslator {
    // inputs
    static const char *XCSOAR_PROFILE_NAME;	     ///< @brief XCSoar profile file name to use for input. 

    // outputs
    static const char *OUTPUT_PROFILE_NAME;      ///< @brief The name of XCSoar profile file to generate. 
    static const char *WP_FILE_NAME;           	 ///< @brief The name of XCSoar waypoints file to generate. 
    static const char *POLAR_FILE_NAME;          ///< @brief The name of XCSoar glider polar file to generate.

    CFileParserINI _profileParser;               ///< @brief XCSoar profile file parser. 
    const std::string _outputXCSoarDataPath;     ///< @brief The path to the output XCSoarData directory.
    std::string _outputCondor2NavDataPath;       ///< @brief The path to the output Condor2Nav directory.
    std::string _condor2navDataPath;             ///< @brief The Condor2Nav destination data directory path (in XCSoar format) on the target device that runs XCSoar. 
    
  public:
    explicit CTranslatorXCSoar(const CFileParserINI &configParser);
    virtual ~CTranslatorXCSoar();

    virtual void SceneryMap(const CFileParserCSV::CStringArray &sceneryData);
    virtual void SceneryTime(const CFileParserCSV::CStringArray &sceneryData);
    virtual void Glider(const CFileParserCSV::CStringArray &gliderData);
    virtual void Task(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv);
    virtual void Airspaces(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv);
    virtual void Weather(const CFileParserINI &taskParser);
  };

}

#endif /* __XCSOAR_H__ */