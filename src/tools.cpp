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
 * @file tools.cpp
 *
 * @brief Implements common tools. 
 */

#include "tools.h"
#include "activeSync.h"
#include <fstream>
#include <iomanip>
#include <windows.h>



namespace condor2nav {

  const double PI = 3.1415923865;

}


/**
 * @brief Removes leading and trailing white spaces from string.
 *
 * Method removes leading and trailing white spaces from given string.
 *
 * @param [in,out] str The string to cut.
 *
 * @return Cut string.
 */
void condor2nav::Trim(std::string &str)
{
  if(str != "") {
    size_t pos1 = str.find_first_not_of(" ");
    size_t pos2 = str.find_last_not_of(" ");
    str = str.substr(pos1, pos2 - pos1 + 1);
  }
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
 */
std::string condor2nav::DDFF2DDMMFF(double value, bool longitude)
{
  double absValue = value;
  if(value < 0)
    absValue = -absValue;
  unsigned deg = static_cast<unsigned>(absValue);
  double min = (absValue - deg) * 60;
  std::stringstream stream;
  stream.setf(std::ios::fixed, std::ios::floatfield);
  stream.setf(std::ios::showpoint);
  stream.precision(3);
  stream << deg << ":" << std::setfill('0') << std::setw(6) << min << (longitude ? (value > 0 ? "E" : "W") : (value > 0 ? "N" : "S"));
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
 */
std::string condor2nav::DDFF2DDMMSS(double value, bool longitude)
{
  double absValue = value;
  if(value < 0)
    absValue = -absValue;
  unsigned deg = static_cast<unsigned>(absValue);
  unsigned min = static_cast<unsigned>((absValue - deg) * 60);
  unsigned sec = static_cast<unsigned>(((absValue - deg) * 60 - min) * 60);
  std::stringstream stream;
  stream << std::setfill('0') << std::setw(longitude ? 3 : 2) << deg << ":" << std::setw(2) << min << ":" << std::setw(2) << sec << (longitude ? (value > 0 ? "E" : "W") : (value > 0 ? "N" : "S"));
  return stream.str();
}


/**
 * @brief Converts the speed units.
 *
 * Function converts the speed units from km/h to m/s.
 *
 * @param value The speed value to convert.
 *
 * @return Converted speed.
 */
unsigned condor2nav::KmH2MS(unsigned value)
{
  return static_cast<unsigned>((value * 10.0 / 36) + 0.5);
}


/**
 * @brief Converts angle described in degrees to radians
 *
 * Method converts angle described in degrees to radians.
 *
 * @param angle     Angle to convert. 
 *
 * @return Converted angle.
 */
double condor2nav::Deg2Rad(double angle)
{
  return angle * PI / 180 ;
}


/**
 * @brief Converts angle described in radians to degrees
 *
 * Method converts angle described in radians to degrees.
 *
 * @param angle     Angle to convert. 
 *
 * @return Converted angle.
 */
double condor2nav::Rad2Deg(double angle)
{
  return angle * 180 / PI;
}


/** 
 * @brief Creates specified directory
 * 
 * Function creates given directory recursively.
 * 
 * @param dirName Directory name to created.
 *
 * @exception std::runtime_error Operation did not succeed
 */
void condor2nav::DirectoryCreate(const std::string &dirName)
{
  bool activeSync = false;
  if(dirName.size() > 2 && dirName[0] == '\\' && dirName[1] != '\\')
    activeSync = true;

  if (!dirName.empty()) {
    size_t pos = 0;
    while(pos != std::string::npos) {
      pos = dirName.find_first_of("/\\", pos);
      if(dirName[1] == ':' && (pos == 2 || dirName.size() == 2)) {
        // disk drive prefix
        if(dirName.size() <= 3)
          break;
        pos = dirName.find_first_of("/\\", 3);
      }
      else if(pos == 0) {
        if(dirName.size() > 1 && dirName[1] == '\\') {
          // network path
          pos = dirName.find_first_of("/\\", 2);
          // skip computer name also
          pos = dirName.find_first_of("/\\", pos + 1);
        }
        else {
          // ActiveSync path
          pos = dirName.find_first_of("/\\", 1);
        }
      }
      std::string subDir(dirName, 0, pos);

      if(activeSync) {
        CActiveSync &activeSync(CActiveSync::Instance());
        activeSync.DirectoryCreate(subDir);
      }
      else {
        DWORD error = ERROR_SUCCESS;
        if (!::CreateDirectory(subDir.c_str(), 0) && (error = GetLastError()) != ERROR_ALREADY_EXISTS)
          throw EOperationFailed("Cannot create directory '" + subDir + "' (" + Convert(error) + ")!!!");
      }

      if(pos != std::string::npos)
        pos++;
    }
  }
}


/** 
 * @brief Checks if file exists in current directory
 * 
 * Function checks if specified file exists.
 * 
 * @param fileName File name to search for
 * 
 * @return Check status
 */
bool condor2nav::FileExists(const std::string &fileName) 
{
  bool activeSync = false;
  if(fileName.size() > 2 && fileName[0] == '\\' && fileName[1] != '\\')
    activeSync = true;

  if(activeSync) {
    CActiveSync &activeSync(CActiveSync::Instance());
    return activeSync.FileExists(fileName);
  }
  else {
    std::ifstream file(fileName.c_str());
    return file.is_open();
  }
}


/**
 * @brief Class constructor. 
 *
 * @param error The error string. 
 */
condor2nav::Exception::Exception(const std::string &error) throw():
_error(error)
{

}


/**
 * @brief Class copy-constructor. 
 *
 * @param org The original copy of the class. 
 */
condor2nav::Exception::Exception(const Exception &org) throw():
_error(org._error)
{

}


/**
 * @brief Class destructor. 
 */
condor2nav::Exception::~Exception()
{
}


/**
 * @brief Returns exception description.
 *
 * @return Exception description. 
 */
const char *condor2nav::Exception::what() const throw()
{
  return _error.c_str();
}


condor2nav::EOperationFailed::EOperationFailed(const std::string &error) throw():
Exception(error)
{

}
