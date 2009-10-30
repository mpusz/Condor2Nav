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
 * @file condor.cpp
 *
 * @brief Implements the condor2nav::CCondor class. 
**/

#include "condor.h"
#include "tools.h"
#include <fstream>
#include <iomanip>


/**
 * @brief Class constructor
 *
 * condor2nav::CCondor::CCoordConverter class constructor that connects to 
 * NaviCon.dll library interface and initializes it with current terrain file.
 *
 * @param condorPath The path to Condor directory
 * @param trnName The name of the terrain used in task
**/
condor2nav::CCondor::CCoordConverter::CCoordConverter(const std::string &condorPath, const std::string &trnName)
{
  std::string dllPath(condorPath + "\\NaviCon.dll");
  _hInstLib = LoadLibrary(dllPath.c_str()); 
  if(!_hInstLib)
    throw std::invalid_argument("ERROR: Couldn't open 'NaviCon.dll' from Condor directory '" + condorPath + "'!!!");
  
  _iface.naviConInit = (FNaviConInit)GetProcAddress(_hInstLib, "NaviConInit");
  if(!_iface.naviConInit)
    throw std::runtime_error("ERROR: Couldn't map NaviConInit() from 'NaviCon.dll'!!!");

  _iface.getMaxX = (FGetMaxX)GetProcAddress(_hInstLib, "GetMaxX");
  if(!_iface.getMaxX)
    throw std::runtime_error("ERROR: Couldn't map GetMaxX() from 'NaviCon.dll'!!!");

  _iface.getMaxY = (FGetMaxY)GetProcAddress(_hInstLib, "GetMaxY");
  if(!_iface.getMaxY)
    throw std::runtime_error("ERROR: Couldn't map GetMaxY() from 'NaviCon.dll'!!!");

  _iface.xyToLon = (FXYToLon)GetProcAddress(_hInstLib, "XYToLon");
  if(!_iface.xyToLon)
    throw std::runtime_error("ERROR: Couldn't map XYToLon() from 'NaviCon.dll'!!!");

  _iface.xyToLat = (FXYToLon)GetProcAddress(_hInstLib, "XYToLat");
  if(!_iface.xyToLat)
    throw std::runtime_error("ERROR: Couldn't map XYToLat() from 'NaviCon.dll'!!!");

  // init coordinates
  std::string trnPath(condorPath + "\\Landscapes\\" + trnName + "\\" + trnName + ".trn");
  _iface.naviConInit(trnPath.c_str());
}


/**
 * @brief Class destructor.
 *
 * condor2nav::CCondor::CCoordConverter class destructor.
**/
condor2nav::CCondor::CCoordConverter::~CCoordConverter()
{
  FreeLibrary(_hInstLib);
}


/**
 * @brief Converts longitude and latitude coordinates.
 *
 * Method converts longitude and latitude coordinates from DD.FF
 * to DD:MM.FF format.
 *
 * @param value     The coordinate value to convert. 
 * @param longitude @c true - longitude; @c false - latitude. 
 *
 * @return Converted coordinate string.
**/
std::string condor2nav::CCondor::CCoordConverter::DDFFToDDMMFF(float value, bool longitude) const
{
  int deg = static_cast<int>(value);
  if(deg < 0)
    deg = -deg;
  float min = (value - deg) * 60;

  std::stringstream stream;
  stream.setf(std::ios::fixed, std::ios::floatfield);
  stream.setf(std::ios::showpoint);
  stream.precision(3);
  stream << deg << ":" << min << (longitude ? (value > 0 ? "E" : "W") : (value > 0 ? "N" : "S"));
  return stream.str();
}


/**
 * @brief Converts longitude and latitude coordinates.
 *
 * Method converts longitude and latitude coordinates from DD.FF
 * to DD:MM::SS format.
 *
 * @param value     The coordinate value to convert. 
 * @param longitude @c true - longitude; @c false - latitude. 
 *
 * @return Converted coordinate string.
**/
std::string condor2nav::CCondor::CCoordConverter::DDFFToDDMMSS(float value, bool longitude) const
{
  int deg = static_cast<int>(value);
  if(deg < 0)
    deg = -deg;
  unsigned min = static_cast<unsigned>((value - deg) * 60);
  unsigned sec = static_cast<unsigned>(((value - deg) * 60 - min) * 60);
  std::stringstream stream;
  stream << std::setfill('0') << std::setw(longitude ? 3 : 2) << deg << ":" << std::setw(2) << min << ":" << std::setw(2) << sec << (longitude ? (value > 0 ? "E" : "W") : (value > 0 ? "N" : "S"));
  return stream.str();
}


/**
 * @brief Converts Condor coordinates to longitude.
 *
 * Method converts Condor coordinates to longitude.
 * 
 * @param x The x coordinate.
 * @param y The y coordinate. 
 * @param format Coordinate output string format.
 *
 * @return Converted coordinate string.
**/
std::string condor2nav::CCondor::CCoordConverter::Longitude(const std::string &x, const std::string &y, TOutputFormat format) const
{
  float xVal(condor2nav::Convert<float>(x));
  float yVal(condor2nav::Convert<float>(y));
  float longitude(_iface.xyToLon(xVal, yVal));
  switch(format) {
  case FORMAT_DDMMFF:
    return DDFFToDDMMFF(longitude, true);
  case FORMAT_DDMMSS:
    return DDFFToDDMMSS(longitude, true);
  default:
    throw std::out_of_range("Not supported longitude format specified!!!");
  }
}


/**
 * @brief Converts Condor coordinates to latitude.
 *
 * Method converts Condor coordinates to latitude.
 * 
 * @param x The x coordinate.
 * @param y The y coordinate. 
 * @param format Coordinate output string format.
 *
 * @return Converted coordinate string.
**/
std::string condor2nav::CCondor::CCoordConverter::Latitude(const std::string &x, const std::string &y, TOutputFormat format) const
{
  float xVal(condor2nav::Convert<float>(x));
  float yVal(condor2nav::Convert<float>(y));
  float latitude(_iface.xyToLat(xVal, yVal));
  switch(format) {
  case FORMAT_DDMMFF:
    return DDFFToDDMMFF(latitude, false);
  case FORMAT_DDMMSS:
    return DDFFToDDMMSS(latitude, false);
  default:
    throw std::out_of_range("Not supported latitude format specified!!!");
  }
}




/**
 * @brief Class constructor. 
 *
 * condor2nav::CCondor class constructor.
 * 
 * @param condorPath Full pathname of the Condor directory. 
 * @param taskName   Name of the Condor task to translate.
 *
 * @exception std Thrown when not supported Condor version.
**/
condor2nav::CCondor::CCondor(const std::string &condorPath, const std::string &taskName):
_taskParser(condorPath + "\\FlightPlans\\User\\" + taskName + ".fpl"), _coordConverter(condorPath, _taskParser.Value("Task", "Landscape"))
{
  if(Convert<unsigned>(_taskParser.Value("Version", "Condor version")) != CONDOR_VERSION_SUPPORTED)
    throw std::out_of_range("Condor vesion '" + _taskParser.Value("Version", "Condor version") + "' not supported!!!");
}
