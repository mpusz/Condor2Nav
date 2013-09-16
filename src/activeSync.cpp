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
 * @file activeSync.cpp
 *
 * @brief Implements the ActiveSync wrapper class. 
 */

#include "activeSync.h"
#include <memory>
#include <algorithm>
#include <rapi.h>
#include <boost/filesystem.hpp>

namespace {

  // rapi.dll interface
  using FCeRapiInitEx = HRESULT(WINAPI*)(RAPIINIT *pRapiInit);
  using FCeRapiUninit = HRESULT(WINAPI*)();
  using FCeGetLastError = DWORD(WINAPI*)();

  using FCeCreateFile = HANDLE(WINAPI*)(LPCWSTR lpFileName,
                                        DWORD dwDesiredAccess,
                                        DWORD dwShareMode,
                                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                        DWORD dwCreationDisposition,
                                        DWORD dwFlagsAndAttributes,
                                        HANDLE hTemplateFile);
  using FCeGetFileSize = DWORD(WINAPI*)(HANDLE hFile,
                                        LPDWORD lpFileSizeHigh);
  using FCeReadFile = BOOL(WINAPI*)(HANDLE hFile,
                                    LPVOID lpBuffer,
                                    DWORD nNumberOfBytesToRead,
                                    LPDWORD lpNumberOfBytesRead,
                                    LPOVERLAPPED lpOverlapped);
  using FCeWriteFile = BOOL(WINAPI*)(HANDLE hFile,
                                     LPCVOID lpBuffer,
                                     DWORD nNumberOfBytesToWrite,
                                     LPDWORD lpNumberOfBytesWritten,
                                     LPOVERLAPPED lpOverlapped);
  using FCeCloseHandle = BOOL(WINAPI*)(HANDLE hObject);
  using FCeCreateDirectory = BOOL(WINAPI*)(LPCWSTR lpPathName,
                                           LPSECURITY_ATTRIBUTES lpSecurityAttributes);

  template<typename SYMBOL_TYPE>
  inline void Symbol(const HMODULE &module, const std::string &name, SYMBOL_TYPE &out)
  {
    auto sym = reinterpret_cast<SYMBOL_TYPE>(GetProcAddress(module, name.c_str()));
    if(!sym)
      throw condor2nav::EOperationFailed{"ERROR: Couldn't map " + name + "() from 'rapi.dll'!!!"};
    out = sym;
  }

}


namespace condor2nav {

  /**
   * @brief rapi.dll interface.
   */
  struct CActiveSync::TDLLIface {
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

  class CActiveSync::CRapiHandleDeleter {
    const TDLLIface &_iface;
  public:
    using pointer = HANDLE;
    CRapiHandleDeleter(const TDLLIface &iface) : _iface{iface} {}
    CRapiHandleDeleter &operator =(const CRapiHandleDeleter &) = delete;
    void operator ()(pointer handle) const { _iface.ceCloseHandle(handle); }
  };

  void CActiveSync::CRapiDeleter::operator()(pointer status) const { _iface.ceRapiUninit(); }

}



/**
 * @brief Returns singleton instance.
 *
 * Method returns singleton instance.
 *
 * @return Singleton instance.
 */
condor2nav::CActiveSync &condor2nav::CActiveSync::Instance()
{
  static CActiveSync instance;
  return instance;
}


/**
 * @brief Class constructor.
 *
 * condor2nav::CActiveSync class constructor.
 */
condor2nav::CActiveSync::CActiveSync() :
  _lib{::LoadLibrary("rapi.dll")}, _iface{std::make_unique<TDLLIface>()}, _rapi{false, CRapiDeleter(*_iface)}
{
  if(!_lib.get())
    throw EOperationFailed{"ERROR: Couldn't open 'rapi.dll' library!!! Please check that ActiveSync is installed correctly."};
  Symbol(_lib.get(), "CeRapiInitEx",      _iface->ceRapiInitEx);
  Symbol(_lib.get(), "CeRapiUninit",      _iface->ceRapiUninit);
  Symbol(_lib.get(), "CeGetLastError",    _iface->ceGetLastError);
  Symbol(_lib.get(), "CeCreateFile",      _iface->ceCreateFile);
  Symbol(_lib.get(), "CeGetFileSize",     _iface->ceGetFileSize);
  Symbol(_lib.get(), "CeReadFile",        _iface->ceReadFile);
  Symbol(_lib.get(), "CeWriteFile",       _iface->ceWriteFile);
  Symbol(_lib.get(), "CeCloseHandle",     _iface->ceCloseHandle);
  Symbol(_lib.get(), "CeCreateDirectory", _iface->ceCreateDirectory);

  // init RAPI
  RAPIINIT initData{};
  initData.cbSize = sizeof(initData);

  _rapi.reset(!FAILED(_iface->ceRapiInitEx(&initData)));
  if(!_rapi.get())
    throw EOperationFailed{"Cannot initialize ActiveSync connection!!!"};

  HRESULT hr;
  auto status = WaitForSingleObject(initData.heRapiInit, TIMEOUT);
  if(status == WAIT_OBJECT_0) {
    // heRapiInit signaled:
    // set return error code to return value of RAPI Init function
    hr = initData.hrRapiInit;  
  }
  else {
    if(status == WAIT_TIMEOUT) {
      // timed out: device is probably not connected
      // or not responding
      hr = HRESULT_FROM_WIN32(ERROR_TIMEOUT);
    }
    else {
      // WaitForSingleObject failed
      hr = HRESULT_FROM_WIN32(GetLastError());
    }

    throw EOperationFailed{"ERROR: Cannot initialize ActiveSync interface!!!"};
  }
}


/**
 * @brief Reads whole file to a string.
 *
 * Method reads whole file to a string.
 *
 * @param src Target file path. 
 * 
 * @return String with file content. 
 */
std::string condor2nav::CActiveSync::Read(const bfs::path &src) const
{
  std::unique_ptr<HANDLE, CRapiHandleDeleter> hSrc{_iface->ceCreateFile(src.wstring().c_str(),
                                                                        GENERIC_READ,
                                                                        FILE_SHARE_READ,
                                                                        nullptr,
                                                                        OPEN_EXISTING,
                                                                        FILE_ATTRIBUTE_NORMAL,
                                                                        nullptr),
                                                   CRapiHandleDeleter{*_iface}};
  if(hSrc.get() == INVALID_HANDLE_VALUE)
    throw EOperationFailed{"ERROR: Unable to open ActiveSync file '" + src.string() + "'!!!"};

  auto numBytes = _iface->ceGetFileSize(hSrc.get(), nullptr);
  std::vector<char> buffer;
  buffer.resize(numBytes);

  if(!_iface->ceReadFile(hSrc.get(), buffer.data(), numBytes, &numBytes, nullptr))
    throw EOperationFailed{"ERROR: Reading ActiveSync file '" + src.string() + "'!!!"};

  // remove all returns from a file
  buffer.erase(std::remove_if(begin(buffer), end(buffer), [](char c){ return c == '\r'; }), end(buffer));

  return buffer.data();
}


/**
 * @brief Writes buffer to a file on the target device.
 *
 * Method writes buffer to a file on the target device.
 *
 * @param dest Target file path. 
 * @param buffer Buffer with file content. 
 */
void condor2nav::CActiveSync::Write(const bfs::path &dest, const std::string &buffer) const
{
  std::unique_ptr<HANDLE, CRapiHandleDeleter> hDest{_iface->ceCreateFile(dest.wstring().c_str(),
                                                                         GENERIC_WRITE,
                                                                         FILE_SHARE_READ,
                                                                         nullptr,
                                                                         CREATE_ALWAYS,
                                                                         FILE_ATTRIBUTE_NORMAL,
                                                                         nullptr),
                                                    CRapiHandleDeleter{*_iface}};
  if(hDest.get() == INVALID_HANDLE_VALUE)
    throw EOperationFailed{"ERROR: Unable to open ActiveSync file '" + dest.string() + "'!!!"};

  DWORD numBytes;
  if(!_iface->ceWriteFile(hDest.get(), buffer.c_str(), buffer.size(), &numBytes, nullptr))
    throw EOperationFailed{"ERROR: Writing ActiveSync file '" + dest.string() + "'!!!"};
}


/**
 * @brief Creates directory on the target device.
 *
 * Method creates directory on the target device.
 *
 * @param path Target directory path. 
 */
void condor2nav::CActiveSync::DirectoryCreate(const bfs::path &path) const
{
  if(!_iface->ceCreateDirectory(path.wstring().c_str(), nullptr) && _iface->ceGetLastError() != ERROR_ALREADY_EXISTS)
    throw EOperationFailed{"ERROR: Creating ActiveSync directory '" + path.string() + "'!!!"};
}


/**
 * @brief Checks if a file exists on the target device.
 *
 * Method checks if a file exists on the target device.
 *
 * @param path Target file path.
 *
 * @return @p true if file exists.
 */
bool condor2nav::CActiveSync::FileExists(const bfs::path &path) const
{
  std::unique_ptr<HANDLE, CRapiHandleDeleter> hDest{_iface->ceCreateFile(path.wstring().c_str(),
                                                                         GENERIC_READ,
                                                                         FILE_SHARE_READ,
                                                                         nullptr,
                                                                         OPEN_EXISTING,
                                                                         FILE_ATTRIBUTE_NORMAL,
                                                                         nullptr),
                                                    CRapiHandleDeleter{*_iface}};
  if(hDest.get() == INVALID_HANDLE_VALUE) {
    if(_iface->ceGetLastError() == ERROR_FILE_NOT_FOUND)
      return false;
    throw EOperationFailed{"ERROR: Unable to check if file '" + path.string() + "' exists!!!"};
  }
  else
    return true;
}
