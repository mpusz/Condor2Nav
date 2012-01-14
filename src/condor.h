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
 * @file condor.h
 *
 * @brief Declares the condor2nav::CCondor class. 
 */

#ifndef __CONDOR_H__
#define __CONDOR_H__

#include "condor2nav.h"
#include "nonCopyable.h"
#include "fileParserINI.h"
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
      typedef int   (WINAPI *FNaviConInit)(const char *trnFile);  ///< @brief NaviCon.dll interface
      typedef float (WINAPI *FXYToLon)(float X, float Y);         ///< @brief NaviCon.dll interface
      typedef float (WINAPI *FXYToLat)(float X, float Y);         ///< @brief NaviCon.dll interface
      typedef float (WINAPI *FGetMaxX)();                         ///< @brief NaviCon.dll interface
      typedef float (WINAPI *FGetMaxY)();                         ///< @brief NaviCon.dll interface

      /**
       * @brief NaviCon.dll interface.
       */
      struct TDLLIface {
        FNaviConInit naviConInit;
        FGetMaxX     getMaxX;
        FGetMaxY     getMaxY;
        FXYToLon     xyToLon;
        FXYToLat     xyToLat;
      };
      std::unique_ptr<HMODULE, HModuleDeleter> _hInstLib;  ///< @brief DLL instance. 
      TDLLIface _iface;	                                   ///< @brief DLL interface.

    public:
      /**
       * @brief Coordinate output string format.
       */
      enum TOutputFormat {
//        FORMAT_DDFF,          ///< @brief Format in form 23.3545645N
        FORMAT_DDMMFF,        ///< @brief Format in form 23:35.454N
        FORMAT_DDMMSS         ///< @brief Format in form 23:35:23N
      };
      CCoordConverter(const std::string &condorPath, const std::string &trnName);
      double Longitude(const std::string &x, const std::string &y) const;
      double Latitude(const std::string &x, const std::string &y) const;
      std::string Longitude(const std::string &x, const std::string &y, TOutputFormat format) const;
      std::string Latitude(const std::string &x, const std::string &y, TOutputFormat format) const;
    };

  private:
    static const unsigned CONDOR_VERSION_SUPPORTED = 1120;	  ///< @brief Supported Condor version.
    static const char *FLIGHT_PLANS_PATH;
    static const char *RACE_RESULTS_PATH;
    const CFileParserINI _taskParser;	           ///< @brief Condor task file parser. 
    const CCoordConverter _coordConverter;	       ///< @brief Condor map coordinates converter. 

  public:
    static std::string InstallPath();
    static void FPLPath(const CFileParserINI &configParser, CCondor2Nav::TFPLType fplType, const std::string &condorPath, std::string &fplPath);

    CCondor(const std::string &condorPath, const std::string &fplPath);
    const CFileParserINI &TaskParser() const      { return _taskParser; }
    const CCoordConverter &CoordConverter() const { return _coordConverter; }
  };

}

#endif /* __CONDOR_H__ */
