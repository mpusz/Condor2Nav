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

  class CTranslatorXCSoar : public CTranslator {
    // inputs
    static const char *XCSOAR_PROFILE_NAME;

    // outputs
    static const char *OUTPUT_PROFILE_NAME;
    static const char *WP_FILE_NAME;
    static const char *POLAR_FILE_NAME;

    const std::string _xcsoarDataPath;
    CFileParserINI _profileParser;
    
  public:
    explicit CTranslatorXCSoar(const std::string &outputPath, const std::string &xcsoarDataPath);
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