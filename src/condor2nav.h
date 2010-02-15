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

#include <string>

/**
 * @brief Condor2Nav project namespace.
**/
namespace condor2nav {

  class CFileParserINI;

  /**
   * @brief Main project class.
   *
   * condor2nav::CCondor2Nav is a main project class. It is responsible for
   * command line handling and running the translation.
  **/
  class CCondor2Nav {
    /**
     * @brief Values that represent different types of supported flight plan types. 
    **/
    enum TFPLType {
      TYPE_DEFAULT,	          ///< @brief Default task name (from INI) file will be run. 
      TYPE_RESULT,	          ///< @brief Use last flown FPL from race result directory.
      TYPE_USER	              ///< @brief User provided exact path to FPL file. 
    };

    static const char *CONFIG_FILE_NAME;          ///< @brief The name of the configuration INI file.

    void Usage() const;
    void CLIParse(int argc, const char *argv[], TFPLType &fplType, std::string &fplPath, unsigned &aatTime) const;
    std::string CondorPath() const;
    void FPLPath(const CFileParserINI &configParser, TFPLType fplType, const std::string &condorPath, std::string &fplPath) const;

  public:
    int Run(int argc, const char *argv[]) const;
  };

}

#endif /* __CONDOR2NAV_H__ */