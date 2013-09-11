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
 * @file condor.h
 *
 * @brief Declares the condor2nav::CCondor class. 
 */

#ifndef __CONDOR_H__
#define __CONDOR_H__

#include "condor2nav.h"
#include "nonCopyable.h"
#include "fileParserINI.h"
#include "boostfwd.h"
#include <windows.h>

namespace condor2nav {

  /**
   * @brief Condor data class. 
   *
   * condor2nav::CCondor is responsible for storing and handling of Condor
   * (The Soaring Competition Simulator) data. It also provides some
   * tools to interpret that data.
   */
  class CCondor : CNonCopyable {
  public:
    /**
     * @brief The types of Condor sectors.
     */
    enum TSectorType {
      SECTOR_CLASSIC,	    ///< @brief Line, FAI Sector or Circle.
      SECTOR_WINDOW	        ///< @brief Window. 
    };

    /**
     * @brief Condor map coordinates converter.
     *
     * condor2nav::CCondor::CCoordConverter is responsible for
     * Condor map coordinates convertions. It uses NaviCon.dll library
     * provided with every Condor release.
     */
    class CCoordConverter : CNonCopyable {
      struct TDLLIface;
      std::unique_ptr<TDLLIface> _iface;	       ///< @brief DLL interface.
      CLibraryRes _lib;                            ///< @brief DLL instance. 
    public:
      CCoordConverter(const bfs::path &condorPath, const std::string &trnName);
      ~CCoordConverter();
      TLongitude Longitude(const std::string &x, const std::string &y) const;
      TLatitude Latitude(const std::string &x, const std::string &y) const;
    };

  private:
    static const unsigned CONDOR_VERSION_SUPPORTED = 1120;	  ///< @brief Supported Condor version.
    static const bfs::path FLIGHT_PLANS_PATH;
    static const bfs::path RACE_RESULTS_PATH;
    const CFileParserINI _taskParser;	           ///< @brief Condor task file parser. 
    const CCoordConverter _coordConverter;	       ///< @brief Condor map coordinates converter. 

  public:
    static bfs::path InstallPath();
    static bfs::path FPLPath(const CFileParserINI &configParser,
                             CCondor2Nav::TFPLType fplType,
                             const bfs::path &condorPath);

    CCondor(const bfs::path &condorPath, const bfs::path &fplPath);
    const CFileParserINI &TaskParser() const      { return _taskParser; }
    const CCoordConverter &CoordConverter() const { return _coordConverter; }
  };

}

#endif /* __CONDOR_H__ */
