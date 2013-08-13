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
 * @file tools.cpp
 *
 * @brief Implements common tools. 
 */

#include "tools.h"
#include "istream.h"
#include "activeSync.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <windows.h>



namespace {

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
    const size_t pos1 = str.find_first_not_of(" \t\r");
    const size_t pos2 = str.find_last_not_of(" \t\r");
    str = str.substr(pos1, pos2 - pos1 + 1);
  }
}


/**
* @brief Parses the line as key=value pairs.
*
* Method parses the line as key=value pairs.
*
* @param line           The line to parse.
*
* @return Key and value pair.
*
* @exception std Thrown when operation failed.
*/
std::pair<std::string, std::string> condor2nav::LineParseKeyValue(const std::string &line)
{
  auto pos = line.find_first_of("=");
  if(pos == std::string::npos)
    throw EOperationFailed{"ERROR: '=' sign not found in line '" + line + "'!!!"};

  auto ret = std::make_pair(line.substr(0, pos), line.substr(pos + 1));
  Trim(ret.first);
  Trim(ret.second);
  return ret;
}


/**
* @brief Parses the line as CSV (Comma Separated Values).
*
* Method parses the line as CSV (Comma Separated Values).
*
* @param line            The line to parse.
*
* @return Parsed values.
*/
std::vector<std::string> condor2nav::LineParseCSV(const std::string &line)
{
  std::vector<std::string> values;
  bool insideQuote = false;
  size_t pos = 0, newValuePos = 0;
  do {
    auto posOld = pos;
    if(insideQuote) {
      pos = line.find_first_of("\"", posOld);
      insideQuote = false;
    }
    else {
      pos = line.find_first_of(",\"", posOld);
      if(pos != std::string::npos && line[pos] == '\"') {
        insideQuote = true;
      }
      else {
        auto len = (pos != std::string::npos) ? (pos - newValuePos) : pos;
        auto value = line.substr(newValuePos, len);
        Trim(value);
        if(!value.empty() && value[0] == '\"')
          // remove quotes
          value = value.substr(1, value.size() - 2);
        values.emplace_back(std::move(value));
        if(pos != std::string::npos)
          newValuePos = pos + 1;
      }
    }
    if(pos != std::string::npos)
      pos++;
  }
  while(pos != std::string::npos);

  return values;
}



namespace {

  template<typename T>
  std::string Coord2DDMMFFImpl(T coord)
  {
    double absValue = coord.value;
    if(coord.value < 0)
      absValue = -absValue;
    const unsigned deg = static_cast<unsigned>(absValue);
    const double min = (absValue - deg) * 60;
    std::stringstream stream;
    stream.setf(std::ios::fixed, std::ios::floatfield);
    stream.setf(std::ios::showpoint);
    stream.precision(3);
    stream << deg << ":" << std::setfill('0') << std::setw(6) << min << coord.Sign();
    return stream.str();
  }

}

/**
 * @brief Converts longitude coordinate.
 *
 * Method converts longitude coordinate from DD.FF to DD:MM.FF format.
 *
 * @param coord     The coordinate value to convert. 
 *
 * @return Converted coordinate string.
 */
std::string condor2nav::Coord2DDMMFF(TLongitude coord)
{
  return Coord2DDMMFFImpl(coord);
}

/**
* @brief Converts latitude coordinate to string.
*
* Method converts latitude coordinate from DD.FF to DD:MM.FF format.
*
* @param coord     The coordinate value to convert.
*
* @return Converted coordinate string.
*/
std::string condor2nav::Coord2DDMMFF(TLatitude coord)
{
  return Coord2DDMMFFImpl(coord);
}


namespace {

  template<typename T>
  std::string Coord2DDMMSSImpl(T coord)
  {
    double absValue = coord.value;
    if(coord.value < 0)
      absValue = -absValue;
    const unsigned deg = static_cast<unsigned>(absValue);
    const unsigned min = static_cast<unsigned>((absValue - deg) * 60);
    const unsigned sec = static_cast<unsigned>(((absValue - deg) * 60 - min) * 60);
    std::stringstream stream;
    stream << std::setfill('0') << std::setw(T::degStrLength) << deg << ":" << std::setw(2) << min << ":" << std::setw(2) << sec << coord.Sign();
    return stream.str();
  }

}

/**
 * @brief Converts longitude coordinate to string.
 *
 * Method converts longitude coordinate from DD.FF to DD:MM::SS format.
 *
 * @param coord     The coordinate value to convert. 
 *
 * @return Converted coordinate string.
 */
std::string condor2nav::Coord2DDMMSS(TLongitude coord)
{
  return Coord2DDMMSSImpl(coord);
}

/**
* @brief Converts latitude coordinate to string.
*
* Method converts latitude coordinate from DD.FF to DD:MM::SS format.
*
* @param coord     The coordinate value to convert.
*
* @return Converted coordinate string.
*/
std::string condor2nav::Coord2DDMMSS(TLatitude coord)
{
  return Coord2DDMMSSImpl(coord);
}


/**
* @brief Checks inner rectangle is spaced inside outer area.
*
* Method checks inner rectangle is spaced inside outer area.
*
* @return True if rectangle is inside area.
*/
bool condor2nav::InsideArea(TLongitude outerLonMin, TLongitude outerLonMax, TLatitude outerLatMin, TLatitude outerLatMax,
                            TLongitude innerLonMin, TLongitude innerLonMax, TLatitude innerLatMin, TLatitude innerLatMax)
{
  return innerLonMin.value >= outerLonMin.value && innerLonMax.value <= outerLonMax.value &&
    innerLatMin.value >= outerLatMin.value && innerLatMax.value <= outerLatMax.value;
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
void condor2nav::DirectoryCreate(const boost::filesystem::path &dirName)
{
  bool activeSync = false;
  const std::string str = dirName.string();
  if(str.size() > 2 && str[0] == '\\' && str[1] != '\\')
    activeSync = true;

  if(!dirName.empty()) {
    if(!activeSync) {
      boost::filesystem::create_directories(dirName);
    }
    else {
      auto path = dirName;
      std::vector<boost::filesystem::path> dirs;
      while(path.parent_path() != "\\") {
        dirs.emplace_back(path);
        path = path.parent_path();
      }
      auto &activeSync = CActiveSync::Instance();
      std::for_each(dirs.crbegin(), dirs.crend(),
        [&](const boost::filesystem::path &d){ activeSync.DirectoryCreate(d); });
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
bool condor2nav::FileExists(const boost::filesystem::path &fileName) 
{
  bool activeSync = false;
  const auto str = fileName.string();
  if(str.size() > 2 && str[0] == '\\' && str[1] != '\\')
    activeSync = true;

  if(activeSync)
    return CActiveSync::Instance().FileExists(fileName);
  else
    return boost::filesystem::exists(fileName);
}


void condor2nav::Download(const std::string &server, const boost::filesystem::path &url, const boost::filesystem::path &fileName, unsigned timeout /* = 30 */)
{
  DirectoryCreate(fileName.parent_path());
  CIStream in{server, url.generic_string(), timeout};
  boost::filesystem::ofstream out{fileName, std::ios_base::out | std::ios_base::binary};
  out << in.Buffer().rdbuf();
}
