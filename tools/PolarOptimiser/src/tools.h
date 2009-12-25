//
// This file is part of PolarOptimiser gliders polar files optimisation helper.
//
// Copyright (C) 2009 Mateusz Pusz
//
// PolarOptimiser is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// PolarOptimiser is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with PolarOptimiser. If not, see <http://www.gnu.org/licenses/>.
//
// Visit the project webpage (http://sf.net/projects/condor2nav) for more info.
//

/**
 * @file tools.h
 *
 * @brief Common utilities. 
**/

#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <sstream>

namespace polarOptimiser {

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
  T Convert(const std::string &str)
  {
    T value;
    std::stringstream stream(str);
    stream >> value;
    if(stream.fail() && !stream.eof())
      throw std::runtime_error("Cannot convert '" + str + "' to requested type!!!");
    return value;
  }

} // namespace polarOptimiser

#endif // __TOOLS_H__
