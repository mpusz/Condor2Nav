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
 * @file translator.h
 *
 * @brief Declares the translators hierarchy base class (condor2nav::CTranslator). 
**/

#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include "condor.h"
#include "fileParserCSV.h"

namespace condor2nav {

  class CTranslator {
    const std::string _outputPath;

    CTranslator(const CTranslator &);                     // disallowed
    const CTranslator &operator=(const CTranslator &);    // disallowed

  protected:
    const std::string &OutputPath() const;

  public:
    explicit CTranslator(const std::string &outputPath);
    virtual ~CTranslator();

    virtual void SceneryMap(const CFileParserCSV::CStringArray &sceneryData) = 0;
    virtual void SceneryTime(const CFileParserCSV::CStringArray &sceneryData) = 0;
    virtual void Glider(const CFileParserCSV::CStringArray &gliderData) = 0;
    virtual void Task(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv) = 0;
    virtual void Airspaces(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv) = 0;
    virtual void Weather(const CFileParserINI &taskParser) = 0;
  };

}

#endif /* __TRANSLATOR_H__ */