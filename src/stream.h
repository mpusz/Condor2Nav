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
 * @file stream.h
 *
 * @brief Declares the Stream wrapper class. 
 */

#ifndef __STREAM_H__
#define __STREAM_H__

#include "nonCopyable.h"
#include "boostfwd.h"
#include <sstream>

namespace condor2nav {

  /**
   * @brief Stream wrapper
   *
   * condor2nav::CStream class is a wrapper for different stream types.
   * NOTE: This class is not polymorphic.
   */
  class CStream : CNonCopyable {
    std::stringstream _buffer;            ///< @brief Buffer with file data. 

  protected:
    /*
     * @brief Stream types
     */
    enum class TType {
      LOCAL,                              ///< @brief Local path. 
      ACTIVE_SYNC                         ///< @brief ActiveSync (remote device) path. 
    };

    TType Type(const bfs::path &fileName) const;
    CStream() {}
  public:
    const std::stringstream &Buffer() const { return _buffer; }
    std::stringstream &Buffer() { return _buffer; }
  };

}

#endif /* __STREAM_H__ */