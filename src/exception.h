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
 * @file exception.h
 *
 * @brief Exceptions hierarchy.
 */

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <exception>
#include <string>

namespace condor2nav {

  /**
   * @brief Condor2Nav exception hierarchy base class
   */
  class Exception : public std::exception {
    const std::string _error;	///< @brief The error string
    Exception &operator=(const Exception &);
  public:
    explicit Exception(const std::string &error);
    virtual ~Exception() = 0 {}
    const char *what() const override;
  };


  /**
   * @brief Operation failed exception. 
   */
  struct EOperationFailed : Exception {
    explicit EOperationFailed(const std::string &error) : Exception(error) {}
  };

}

#endif /* __EXCEPTION_H__ */