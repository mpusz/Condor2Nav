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
 * @file condor2nav.h
 *
 * @brief Declares the condor2nav::CCondor2Nav class. 
**/

#ifndef __CONDOR2NAV_H__
#define __CONDOR2NAV_H__

#include "fileParserINI.h"

/**
 * @brief Condor2Nav project namespace.
**/
namespace condor2nav {

  class CTranslator;

  /**
   * @brief Main project class.
   *
   * condor2nav::CCondor2Nav is a main project class. It is responsible for running
   * the translation and providing project configuration.
  **/
  class CCondor2Nav {
    // inputs
    static const char *CONFIG_FILE_NAME;          ///< @brief The name of the configuration INI file. 
    static const char *SCENERIES_DATA_FILE_NAME;  ///< @brief Sceneries data CSV file name. 
    static const char *GLIDERS_DATA_FILE_NAME;    ///< @brief Gliders data CSV file name.

    const CFileParserINI _configParser;           ///< @brief Configuration INI file parser.
    
    const std::string _taskName;                  ///< @brief The name of the Condor task to parse.

    std::auto_ptr<CTranslator> Translator() const;

  public:
    /**
     * @brief Sceneries data CSV file column names.
    **/
    enum TSceneriesDataColumns {
      SCENERY_NAME,
      SCENERY_XCSOAR_FILE,
      SCENERY_UTC_OFFSET
    };

    /**
     * @brief Gliders data CSV file column names.
    **/
    enum TGlidersDataColumns {
      GLIDER_NAME,
      GLIDER_SPEED_MAX,
      GLIDER_DAEC_INDEX,
      GLIDER_WATER_BALLAST_EMPTY_TIME,
      GLIDER_MASS_DRY_GROSS,
      GLIDER_MAX_WATER_BALLAST,
      GLIDER_SPPED_1,
      GLIDER_SINK_1,
      GLIDER_SPPED_2,
      GLIDER_SINK_2,
      GLIDER_SPPED_3,
      GLIDER_SINK_3
    };

    // inputs
    static const char *DATA_PATH;	                ///< @brief Application data directory path. 

    CCondor2Nav(int argc, const char *argv[]);
    int Run();
  };

}

#endif /* __CONDOR2NAV_H__ */