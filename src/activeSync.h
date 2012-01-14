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
 * @file activeSync.h
 *
 * @brief Declares the ActiveSync wrapper class. 
 */

#ifndef __ACTIVESYNC_H__
#define __ACTIVESYNC_H__

#include "nonCopyable.h"
#include "tools.h"
#include <functional>
#include <memory>
#include <rapi.h>

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
    typedef HRESULT (WINAPI *FCeRapiInitEx)(RAPIINIT *pRapiInit);    ///< @brief rapi.dll interface
    typedef HRESULT (WINAPI *FCeRapiUninit)();                       ///< @brief rapi.dll interface
    typedef DWORD (WINAPI *FCeGetLastError)();                       ///< @brief rapi.dll interface

    typedef HANDLE (WINAPI *FCeCreateFile)(LPCWSTR lpFileName,
                                           DWORD dwDesiredAccess, 
                                           DWORD dwShareMode, 
                                           LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
                                           DWORD dwCreationDisposition, 
                                           DWORD dwFlagsAndAttributes, 
                                           HANDLE hTemplateFile);    ///< @brief rapi.dll interface
    typedef DWORD (WINAPI *FCeGetFileSize)(HANDLE hFile, 
                                           LPDWORD lpFileSizeHigh);  ///< @brief rapi.dll interface
    typedef BOOL (WINAPI *FCeReadFile)(HANDLE hFile, 
                                       LPVOID lpBuffer, 
                                       DWORD nNumberOfBytesToRead, 
                                       LPDWORD lpNumberOfBytesRead, 
                                       LPOVERLAPPED lpOverlapped);   ///< @brief rapi.dll interface
    typedef BOOL (WINAPI *FCeWriteFile)(HANDLE hFile, 
                                        LPCVOID lpBuffer, 
                                        DWORD nNumberOfBytesToWrite, 
                                        LPDWORD lpNumberOfBytesWritten, 
                                        LPOVERLAPPED lpOverlapped);  ///< @brief rapi.dll interface
    typedef BOOL (WINAPI *FCeCloseHandle)(HANDLE hObject);           ///< @brief rapi.dll interface

    typedef BOOL (WINAPI *FCeCreateDirectory)(LPCWSTR lpPathName,
                                              LPSECURITY_ATTRIBUTES lpSecurityAttributes);  ///< @brief rapi.dll interface

    /**
     * @brief rapi.dll interface.
     */
    struct TDLLIface {
      FCeRapiInitEx      ceRapiInitEx;
      FCeRapiUninit      ceRapiUninit;
      FCeGetLastError    ceGetLastError;
      FCeCreateFile      ceCreateFile;
      FCeGetFileSize     ceGetFileSize;
      FCeReadFile        ceReadFile;
      FCeWriteFile       ceWriteFile;
      FCeCloseHandle     ceCloseHandle;
      FCeCreateDirectory ceCreateDirectory;
    };

    static const unsigned TIMEOUT = 5000;                ///< @brief Timeout in ms for ActiveSync initialization. 

    std::unique_ptr<HMODULE, HModuleDeleter> _hInstLib;  ///< @brief DLL instance. 
    TDLLIface _iface;	                                 ///< @brief DLL interface.
    CActiveSync();
  public:
    static CActiveSync &Instance();
    ~CActiveSync();
    std::string Read(const std::string &src) const;
    void Write(const std::string &dest, const std::string &buffer) const;
    void DirectoryCreate(const std::string &path) const;
    bool FileExists(const std::string &path) const;
  };

}

#endif /* __ACTIVESYNC_H__ */
