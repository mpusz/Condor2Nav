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
 * @file condor.cpp
 *
 * @brief Implements the condor2nav::CCondor class. 
 */

#include "condor.h"
#include "tools.h"
#include <fstream>
#include <iomanip>
#include <cmath>

const char *condor2nav::CCondor::FLIGHT_PLANS_PATH = "FlightPlans\\User";
const char *condor2nav::CCondor::RACE_RESULTS_PATH = "RaceResults";



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
condor2nav::CCondor::CCoordConverter::CCoordConverter(const std::string &condorPath, const std::string &trnName)
{
  std::string dllPath(condorPath + "\\NaviCon.dll");
  _hInstLib = LoadLibrary(dllPath.c_str()); 
  if(!_hInstLib)
    throw EOperationFailed("ERROR: Couldn't open 'NaviCon.dll' from Condor directory '" + condorPath + "'!!!");
  
  _iface.naviConInit = (FNaviConInit)GetProcAddress(_hInstLib, "NaviConInit");
  if(!_iface.naviConInit)
    throw EOperationFailed("ERROR: Couldn't map NaviConInit() from 'NaviCon.dll'!!!");

  _iface.getMaxX = (FGetMaxX)GetProcAddress(_hInstLib, "GetMaxX");
  if(!_iface.getMaxX)
    throw EOperationFailed("ERROR: Couldn't map GetMaxX() from 'NaviCon.dll'!!!");

  _iface.getMaxY = (FGetMaxY)GetProcAddress(_hInstLib, "GetMaxY");
  if(!_iface.getMaxY)
    throw EOperationFailed("ERROR: Couldn't map GetMaxY() from 'NaviCon.dll'!!!");

  _iface.xyToLon = (FXYToLon)GetProcAddress(_hInstLib, "XYToLon");
  if(!_iface.xyToLon)
    throw EOperationFailed("ERROR: Couldn't map XYToLon() from 'NaviCon.dll'!!!");

  _iface.xyToLat = (FXYToLon)GetProcAddress(_hInstLib, "XYToLat");
  if(!_iface.xyToLat)
    throw EOperationFailed("ERROR: Couldn't map XYToLat() from 'NaviCon.dll'!!!");

  // init coordinates
  std::string trnPath(condorPath + "\\Landscapes\\" + trnName + "\\" + trnName + ".trn");
  _iface.naviConInit(trnPath.c_str());
}


/**
 * @brief Class destructor.
 *
 * condor2nav::CCondor::CCoordConverter class destructor.
 */
condor2nav::CCondor::CCoordConverter::~CCoordConverter()
{
  FreeLibrary(_hInstLib);
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
double condor2nav::CCondor::CCoordConverter::Longitude(const std::string &x, const std::string &y) const
{
  float xVal(condor2nav::Convert<float>(x));
  float yVal(condor2nav::Convert<float>(y));
  double lon = _iface.xyToLon(xVal, yVal);
  int deg = static_cast<int>(lon);
  double min = static_cast<int>(floor((lon - deg) * 60.0 * 1000 + 0.5)) / static_cast<double>(1000.0);
  return deg + min / 60;
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
double condor2nav::CCondor::CCoordConverter::Latitude(const std::string &x, const std::string &y) const
{
  float xVal(condor2nav::Convert<float>(x));
  float yVal(condor2nav::Convert<float>(y));
  double lat = _iface.xyToLat(xVal, yVal);
  int deg = static_cast<int>(lat);
  double min = static_cast<int>(floor((lat - deg) * 60.0 * 1000 + 0.5)) / static_cast<double>(1000.0);
  return deg + min / 60;
}


/**
 * @brief Converts Condor coordinates to longitude.
 *        
 * Method converts Condor coordinates to longitude. 
 *
 * @param x      The x coordinate. 
 * @param y      The y coordinate. 
 * @param format Coordinate output string format. 
 *
 * @exception std::out_of_range Thrown when unsupported output format is provided. 
 *
 * @return Converted coordinate string. 
 */
std::string condor2nav::CCondor::CCoordConverter::Longitude(const std::string &x, const std::string &y, TOutputFormat format) const
{
  double longitude(Longitude(x, y));
  switch(format) {
  case FORMAT_DDMMFF:
    return DDFF2DDMMFF(longitude, true);
  case FORMAT_DDMMSS:
    return DDFF2DDMMSS(longitude, true);
  default:
    throw EOperationFailed("Not supported longitude format specified!!!");
  }
}


/**
 * @brief Converts Condor coordinates to latitude.
 *        
 * Method converts Condor coordinates to latitude. 
 *
 * @param x      The x coordinate. 
 * @param y      The y coordinate. 
 * @param format Coordinate output string format. 
 *
 * @exception std::out_of_range Thrown when unsupported output format is provided. 
 *
 * @return Converted coordinate string. 
 */
std::string condor2nav::CCondor::CCoordConverter::Latitude(const std::string &x, const std::string &y, TOutputFormat format) const
{
  double latitude(Latitude(x, y));
  switch(format) {
  case FORMAT_DDMMFF:
    return DDFF2DDMMFF(latitude, false);
  case FORMAT_DDMMSS:
    return DDFF2DDMMSS(latitude, false);
  default:
    throw EOperationFailed("Not supported latitude format specified!!!");
  }
}




/* ************************************* C O N D O R **************************************** */


/**
* @brief Returns a path to Condor: The Competition Soaring Simulator
*
* Method returns a path to Condor: The Competition Soaring Simulator.
*
* @return Path to Condor: The Competition Soaring Simulator
*/
std::string condor2nav::CCondor::InstallPath()
{
  std::string condorPath;
  HKEY hTestKey;

  if((RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Condor", 0, KEY_READ, &hTestKey)) == ERROR_SUCCESS) {
    DWORD bufferSize = 0;
    RegQueryValueEx(hTestKey, "InstallDir", NULL, NULL, NULL, &bufferSize);
    std::auto_ptr<char> buffer = std::auto_ptr<char>(new char[bufferSize]);
    RegQueryValueEx(hTestKey, "InstallDir", NULL, NULL, reinterpret_cast<BYTE *>(buffer.get()), &bufferSize);
    condorPath = buffer.get();
    RegCloseKey(hTestKey);
  }
  else if((RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Condor: The Competition Soaring Simulator", 0, KEY_READ, &hTestKey)) == ERROR_SUCCESS) {
    // check for Condor 1.1.2
    DWORD bufferSize = 0;
    RegQueryValueEx(hTestKey, "DisplayIcon", NULL, NULL, NULL, &bufferSize);
    std::auto_ptr<char> buffer = std::auto_ptr<char>(new char[bufferSize]);
    RegQueryValueEx(hTestKey, "DisplayIcon", NULL, NULL, reinterpret_cast<BYTE *>(buffer.get()), &bufferSize);
    condorPath = buffer.get();
    RegCloseKey(hTestKey);
    size_t pos = condorPath.find("Condor.exe");
    condorPath = condorPath.substr(0, pos - 1);
  }
  else if((RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Condor", 0, KEY_READ, &hTestKey)) == ERROR_SUCCESS) {
    // check for Condor 1.1.0
    DWORD bufferSize = 0;
    RegQueryValueEx(hTestKey, "InstallDir", NULL, NULL, NULL, &bufferSize);
    std::auto_ptr<char> buffer = std::auto_ptr<char>(new char[bufferSize]);
    RegQueryValueEx(hTestKey, "InstallDir", NULL, NULL, reinterpret_cast<BYTE *>(buffer.get()), &bufferSize);
    condorPath = buffer.get();
    RegCloseKey(hTestKey);
  }
  else
    throw EOperationFailed("ERROR: Condor installation not found!!!");

  return condorPath;
}


/**
* @brief Returns FPL file path.
*
* Method returns FPL file path.
*
* @param configParser     The INI file configuration parser. 
* @param fplType          Type of the FPL file. 
* @param condorPath       Full pathname of the Condor: The Competition Soaring Simulator. 
* @param [in,out] fplPath Full pathname of the FPL file. 
*
* @exception std::runtime_error Thrown when FPL file cannot be found.
 */
void condor2nav::CCondor::FPLPath(const CFileParserINI &configParser, CCondor2Nav::TFPLType fplType, const std::string &condorPath, std::string &fplPath)
{
  if(fplType == CCondor2Nav::TYPE_DEFAULT) {
    std::string fplPathUser = configParser.Value("Condor", "FlightPlansPath");
    if(fplPathUser.empty())
      fplPath = condorPath + "\\" + FLIGHT_PLANS_PATH + "\\" + configParser.Value("Condor", "DefaultTaskName") + ".fpl";
    else
      fplPath = fplPathUser + "\\" + configParser.Value("Condor", "DefaultTaskName") + ".fpl";
  }
  else if(fplType == CCondor2Nav::TYPE_RESULT) {
    std::string resultsPathUser = configParser.Value("Condor", "RaceResultsPath");
    if(resultsPathUser.empty())
      fplPath = condorPath + "\\" + RACE_RESULTS_PATH + "\\";
    else
      fplPath = resultsPathUser + "\\";

    // find the latest race result
    std::string fileName;
    FILETIME fileTime;
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    hFind = FindFirstFile((fplPath + "*.fpl").c_str(), &findFileData);
    if(hFind == INVALID_HANDLE_VALUE)
      throw EOperationFailed("ERROR: Cannot find last result FPL file in '" + fplPath + "'(" + Convert(GetLastError()) + ")!!!");
    else {
      fileName = findFileData.cFileName;
      fileTime = findFileData.ftLastWriteTime;

      while(FindNextFile(hFind, &findFileData)) {
        if(CompareFileTime(&findFileData.ftLastWriteTime, &fileTime) > 0) {
          fileName = findFileData.cFileName;
          fileTime = findFileData.ftLastWriteTime;
        }
      }
      if(GetLastError() != ERROR_NO_MORE_FILES)
        throw EOperationFailed("ERROR: Cannot find last result FPL file in '" + fplPath + "'(" + Convert(GetLastError()) + ")!!!");
      FindClose(hFind);
    }
    fplPath += fileName;
  }
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
condor2nav::CCondor::CCondor(const std::string &condorPath, const std::string &fplPath):
_taskParser(fplPath),
_coordConverter(condorPath, _taskParser.Value("Task", "Landscape"))
{
  if(Convert<unsigned>(_taskParser.Value("Version", "Condor version")) < CONDOR_VERSION_SUPPORTED)
    throw EOperationFailed("Condor vesion '" + _taskParser.Value("Version", "Condor version") + "' not supported!!!");
}
