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
 * @file condor.cpp
 *
 * @brief Implements the condor2nav::CCondor class. 
 */

#include "condor.h"
#include "traitsNoCase.h"
#include "tools.h"
#include <iomanip>
#include <algorithm>
#include <cmath>

const bfs::path condor2nav::CCondor::FLIGHT_PLANS_PATH = "FlightPlans\\User";
const bfs::path condor2nav::CCondor::RACE_RESULTS_PATH = "RaceResults";

namespace {

  // NaviCon.dll interface
  using FNaviConInit = int(WINAPI*)(const char *trnFile);
  using FXYToLon = float(WINAPI*)(float X, float Y);
  using FXYToLat = float(WINAPI*)(float X, float Y);
  using FGetMaxX = float(WINAPI*)();
  using FGetMaxY = float(WINAPI*)();


  template<typename SYMBOL_TYPE>
  inline void Symbol(const HMODULE &module, const std::string &name, SYMBOL_TYPE &out)
  {
    SYMBOL_TYPE sym = reinterpret_cast<SYMBOL_TYPE>(GetProcAddress(module, name.c_str()));
    if(!sym)
      throw condor2nav::EOperationFailed{"ERROR: Couldn't map " + name + "() from 'NaviCon.dll'!!!"};
    out = sym;
  }

}

namespace condor2nav {

  /**
  * @brief NaviCon.dll interface.
  */
  struct CCondor::CCoordConverter::TDLLIface {
    FNaviConInit naviConInit;
    FGetMaxX     getMaxX;
    FGetMaxY     getMaxY;
    FXYToLon     xyToLon;
    FXYToLat     xyToLat;
  };

}

/* ******************** C O N D O R   -   C O O R D   C O N V E R T E R ********************* */

/**
 * @brief Class constructor
 *
 * condor2nav::CCondor::CCoordConverter class constructor that connects to 
 * NaviCon.dll library interface and initializes it with current terrain file.
 *
 * @param condorPath The path to Condor directory
 * @param trnName The name of the terrain used in task
 */
condor2nav::CCondor::CCoordConverter::CCoordConverter(const bfs::path &condorPath, const std::string &trnName) :
  _iface{std::make_unique<TDLLIface>()}, _lib{::LoadLibrary((condorPath / "NaviCon.dll").string().c_str())}
{
  if(!_lib.get())
    throw EOperationFailed{"ERROR: Couldn't open 'NaviCon.dll' from Condor directory '" + condorPath.string() + "'!!!"};
  
  Symbol(_lib.get(), "NaviConInit", _iface->naviConInit);
  Symbol(_lib.get(), "GetMaxX",     _iface->getMaxX);
  Symbol(_lib.get(), "GetMaxY",     _iface->getMaxY);
  Symbol(_lib.get(), "XYToLon",     _iface->xyToLon);
  Symbol(_lib.get(), "XYToLat",     _iface->xyToLat);

  // init coordinates
  auto trnPath = condorPath / "Landscapes" / trnName / (trnName + ".trn");
  _iface->naviConInit(trnPath.string().c_str());
}


/**
* @brief Class constructor
*
* NOTE: Destructor definition is needed here to make sure that TDLLIface is defined.
*/
condor2nav::CCondor::CCoordConverter::~CCoordConverter()
{
}


/**
 * @brief Converts Condor coordinates to longitude.
 *
 * Method converts Condor coordinates to a double longitude value.
 * 
 * @param x The x coordinate.
 * @param y The y coordinate. 
 *
 * @return Converted double longitude value.
 */
condor2nav::TLongitude condor2nav::CCondor::CCoordConverter::Longitude(const std::string &x, const std::string &y) const
{
  auto xVal = Convert<float>(x);
  auto yVal = Convert<float>(y);
  auto lon = _iface->xyToLon(xVal, yVal);
  auto deg = static_cast<int>(lon);
  auto min = static_cast<int>(floor((lon - deg) * 60.0 * 1000 + 0.5)) / static_cast<double>(1000.0);
  return TLongitude{deg + min / 60};
}


/**
 * @brief Converts Condor coordinates to double latitude value.
 *
 * Method converts Condor coordinates to latitude.
 * 
 * @param x The x coordinate.
 * @param y The y coordinate. 
 *
 * @return Converted double latitude value.
 */
condor2nav::TLatitude condor2nav::CCondor::CCoordConverter::Latitude(const std::string &x, const std::string &y) const
{
  auto xVal = Convert<float>(x);
  auto yVal = Convert<float>(y);
  auto lat = _iface->xyToLat(xVal, yVal);
  auto deg = static_cast<int>(lat);
  auto min = static_cast<int>(floor((lat - deg) * 60.0 * 1000 + 0.5)) / static_cast<double>(1000.0);
  return TLatitude{deg + min / 60};
}




/* ************************************* C O N D O R **************************************** */

/**
* @brief Returns a path to Condor: The Competition Soaring Simulator
*
* Method returns a path to Condor: The Competition Soaring Simulator.
*
* @return Path to Condor: The Competition Soaring Simulator
*/
bfs::path condor2nav::CCondor::InstallPath()
{
  HKEY hTestKey;
  if((RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Condor", 0, KEY_READ, &hTestKey)) == ERROR_SUCCESS) {
    DWORD bufferSize = 0;
    RegQueryValueEx(hTestKey, "InstallDir", nullptr, nullptr, nullptr, &bufferSize);
    auto buffer = std::make_unique<char[]>(bufferSize);
    RegQueryValueEx(hTestKey, "InstallDir", nullptr, nullptr, reinterpret_cast<BYTE *>(buffer.get()), &bufferSize);
    bfs::path condorPath{buffer.get()};
    RegCloseKey(hTestKey);
    return condorPath;
  }
  else
    throw EOperationFailed{"ERROR: Condor installation not found!!!"};
}


/**
* @brief Returns FPL file path.
*
* Method returns FPL file path.
*
* @param configParser     The INI file configuration parser. 
* @param fplType          Type of the FPL file. 
* @param condorPath       Full pathname of the Condor: The Competition Soaring Simulator. 
*
* @exception std::runtime_error Thrown when FPL file cannot be found.
*
* @return Full pathname of the FPL file. 
 */
bfs::path condor2nav::CCondor::FPLPath(const CFileParserINI &configParser,
                                       CCondor2Nav::TFPLType fplType,
                                       const bfs::path &condorPath)
{
  bfs::path fplPath;
  if(fplType == CCondor2Nav::TFPLType::DEFAULT) {
    auto fplPathUser = bfs::path{configParser.Value("Condor", "FlightPlansPath")};
    if(fplPathUser.empty())
      fplPath = condorPath / FLIGHT_PLANS_PATH / (configParser.Value("Condor", "DefaultTaskName") + ".fpl");
    else
      fplPath = fplPathUser / (configParser.Value("Condor", "DefaultTaskName") + ".fpl");
  }
  else if(fplType == CCondor2Nav::TFPLType::RESULT) {
    auto resultsPath = bfs::path{configParser.Value("Condor", "RaceResultsPath")};
    if(resultsPath.empty())
      resultsPath = condorPath / RACE_RESULTS_PATH;

    // find the latest race result
    std::vector<bfs::path> results;
    std::copy_if(bfs::directory_iterator(resultsPath), bfs::directory_iterator(), std::back_inserter(results),
                 [](const bfs::path &f){ return CStringNoCase{f.extension().string().c_str()} == ".fpl"; });
    auto result = std::max_element(cbegin(results), cend(results),
                                   [](const bfs::path &f1, const bfs::path &f2)
                                   { return bfs::last_write_time(f1) < bfs::last_write_time(f2); });
    if(result != results.end())
      fplPath = result->string();
    else
      throw EOperationFailed{"ERROR: Cannot find last result FPL file in '" + fplPath.string() + "'(" + Convert(GetLastError()) + ")!!!"};
  }
  return fplPath;
}



/**
 * @brief Class constructor. 
 *
 * condor2nav::CCondor class constructor.
 * 
 * @param condorPath Full pathname of the Condor directory. 
 * @param fplPath    Condor FPL file to convert path
 *
 * @exception std Thrown when not supported Condor version.
 */
condor2nav::CCondor::CCondor(const bfs::path &condorPath, const bfs::path &fplPath):
_taskParser{fplPath},
_coordConverter{condorPath, _taskParser.Value("Task", "Landscape")}
{
  if(Convert<unsigned>(_taskParser.Value("Version", "Condor version")) < CONDOR_VERSION_SUPPORTED)
    throw EOperationFailed{"Condor vesion '" + _taskParser.Value("Version", "Condor version") + "' not supported!!!"};
}
