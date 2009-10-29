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

  /**
   * @brief Translators hierarchy base class.
   *
   * condor2nav::CTranslator is a base abstract class for all translations.
   */
  class CTranslator {
    const CFileParserINI &_configParser;                  ///< @brief Configuration INI file parser.
    const std::string _outputPath;                        ///< @brief Translation output directory
    CTranslator(const CTranslator &);                     ///< @brief Disallowed
    const CTranslator &operator=(const CTranslator &);    ///< @brief Disallowed

  protected:
    const std::string &OutputPath() const;
    const CFileParserINI &ConfigParser() const;

  public:
    explicit CTranslator(const CFileParserINI &configParser);
    virtual ~CTranslator();

    /**
     * @brief Sets scenery map. 
     *
     * Method sets scenery map data. 
     *
     * @param sceneryData Information describing the scenery. 
    **/
    virtual void SceneryMap(const CFileParserCSV::CStringArray &sceneryData) = 0;

    /**
     * @brief Sets time for scenery time zone. 
     *
     * Method sets time for scenery time zone.
     *
     * @param sceneryData Information describing the scenery. 
    **/
    virtual void SceneryTime(const CFileParserCSV::CStringArray &sceneryData) = 0;

    /**
     * @brief Set glider data. 
     *
     * Method sets all the data related to the glider.
     *
     * @param gliderData Information describing the glider. 
    **/
    virtual void Glider(const CFileParserCSV::CStringArray &gliderData) = 0;

    /**
     * @brief Sets task information. 
     *
     * Method sets task information.
     *
     * @param taskParser Condor task parser. 
     * @param coordConv  Condor coordinates converter.
    **/
    virtual void Task(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv) = 0;

    /**
     * @brief Sets task airspaces. 
     *
     * Method sets airspaces used in the task.
     *
     * @param taskParser Condor task parser. 
     * @param coordConv  Condor coordinates converter.
     **/
    virtual void Airspaces(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv) = 0;

    /**
     * @brief Sets weather data. 
     *
     * Method sets task weather data (e.g wind).
     *
     * @param taskParser Condor task parser. 
    **/
    virtual void Weather(const CFileParserINI &taskParser) = 0;
  };

}

#endif /* __TRANSLATOR_H__ */