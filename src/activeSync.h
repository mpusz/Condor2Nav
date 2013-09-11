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
 * @file activeSync.h
 *
 * @brief Declares the ActiveSync wrapper class. 
 */

#ifndef __ACTIVESYNC_H__
#define __ACTIVESYNC_H__

#include "nonCopyable.h"
#include "tools.h"
#include "boostfwd.h"
#include <functional>


namespace condor2nav {

  /**
   * @brief ActiveSync interface wrapper
   *
   * condor2nav::CActiveSync class is a wrapper around ActiveSync interface.
   * It uses RAPI interface to communicate with remote device.
   *
   * @note Singleton design pattern
   */
  class CActiveSync : CNonCopyable {
    struct TDLLIface;
    class CRapiHandleDeleter;

    class CRapiDeleter {
      const TDLLIface &_iface;
    public:
      using pointer = bool;
      CRapiDeleter(const TDLLIface &iface) : _iface{iface} {}
      CRapiDeleter &operator =(const CRapiDeleter &) = delete;
      void operator ()(pointer status) const;
    };
    using CRapiRes = std::unique_ptr<bool, CRapiDeleter>;

    static const unsigned TIMEOUT = 5000;             ///< @brief Timeout in ms for ActiveSync initialization. 

    CLibraryRes _lib;                                 ///< @brief DLL instance. 
    std::unique_ptr<TDLLIface> _iface;	              ///< @brief DLL interface.
    CRapiRes _rapi;                                   ///< @brief RAPI RAII wrapper. 

    CActiveSync();
  public:
    static CActiveSync &Instance();
    std::string Read(const bfs::path &src) const;
    void Write(const bfs::path &dest, const std::string &buffer) const;
    void DirectoryCreate(const bfs::path &path) const;
    bool FileExists(const bfs::path &path) const;
  };

}

#endif /* __ACTIVESYNC_H__ */
