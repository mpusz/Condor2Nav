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
 * @file nonCopyable.h
 *
 * @brief Declares the NonCopyable class. 
 */

#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

namespace condor2nav {

  /**
   * @brief Stream wrapper
   *
   * condor2nav::CStream class is a wrapper for different stream types.
   */
  class CNonCopyable {
    CNonCopyable(const CNonCopyable &);                ///< @brief Disallowed. 
    CNonCopyable &operator=(const CNonCopyable &);     ///< @brief Disallowed. 
  public:
    CNonCopyable() {}
  };

}

#endif /* __NONCOPYABLE_H__ */