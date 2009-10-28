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
 * @file tools.h
 *
 * @brief Declares common tools. 
**/

#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <sstream>

namespace condor2nav {

  /**
   * @brief Pointer version of std::less
   *
   * Pointer version of std::less.
   */
  struct CPtrCmp {
    template <class T>
    bool operator()(T v1, T v2) const
    {
      return *v1 < *v2;
    }
  };

  template<class Seq> void Purge(Seq &container);
  template<class Map> void PurgeMap(Map &container);

  // conversions
  template<class T> T Convert(const std::string &str);
  template<class T> std::string Convert(const T &val);

  unsigned KmH2MS(unsigned value);

  // disk operations
  void DirectoryCreate(const std::string &fileName);
  bool FileExists(const std::string &dirName);
}


/**
* @brief Clears STL sequence container
* 
* Deletes all pointers and clears STL sequence container.
* 
* @param container STL sequence container to clear
*/
template<class Seq> void condor2nav::Purge(Seq &container)
{
  typename Seq::iterator it;
  for(it = container.begin(); it != container.end(); ++it)
    delete *it;
  container.clear();
}


/**
* @brief Clears STL map container
* 
* Deletes all pointers and clears STL map container.
* 
* @param container STL map container to clear
*/
template<class Map> void condor2nav::PurgeMap(Map &container)
{
  typename Map::iterator it;
  for(it = container.begin(); it != container.end(); ++it)
    delete it->second;
  container.clear();
}


/**
 * @brief Converts string to specific type.
 *
 * Function converts provided string to specified type. The convertion
 * is being done using STL streams so the output type have to provide
 * the means to initialize itself from the stream.
 *
 * @param str The string to convert.
 *
 * @exception std Thrown when operation failed.
 *
 * @return The data of specified type.
**/
template<class T>
T condor2nav::Convert(const std::string &str)
{
  T value;
  std::stringstream stream(str);
  stream >> value;
  if(stream.fail() && !stream.eof())
    throw std::runtime_error("Cannot convert '" + str + "' to requested type!!!");
  return value;
}


/**
 * @brief Converts any type to a string.
 *
 * Function converts provided data to a string. The convertion
 * is being done using STL streams so the input type have to provide
 * the means to convert itself into the stream.
 *
 * @param val The value to convert.
 *
 * @exception std Thrown when operation failed.
 *
 * @return The string describing provided data.
**/
template<class T>
std::string condor2nav::Convert(const T &val)
{
  std::stringstream stream;
  stream << val;
  return stream.str();
}


#endif /* __TOOLS_H__ */