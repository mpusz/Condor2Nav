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
 * @file activeSync.cpp
 *
 * @brief Implements the ActiveSync wrapper class. 
 */

#include "activeSync.h"
#include <fstream>
#include <memory>
#include <algorithm>


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
condor2nav::CActiveSync::CActiveSync():
_hInstLib(::LoadLibrary("rapi.dll"))
{
  if(!_hInstLib.get())
    throw EOperationFailed("ERROR: Couldn't open 'rapi.dll' library!!! Please check that ActiveSync is installed correctly.");
  
  _iface.ceRapiInitEx = (FCeRapiInitEx)GetProcAddress(_hInstLib.get(), "CeRapiInitEx");
  if(!_iface.ceRapiInitEx)
    throw EOperationFailed("ERROR: Couldn't map CeRapiInitEx() from 'rapi.dll'!!!");

  _iface.ceRapiUninit = (FCeRapiUninit)GetProcAddress(_hInstLib.get(), "CeRapiUninit");
  if(!_iface.ceRapiUninit)
    throw EOperationFailed("ERROR: Couldn't map CeRapiUninit() from 'rapi.dll'!!!");

  _iface.ceGetLastError = (FCeGetLastError)GetProcAddress(_hInstLib.get(), "CeGetLastError");
  if(!_iface.ceGetLastError)
    throw EOperationFailed("ERROR: Couldn't map CeGetLastError() from 'rapi.dll'!!!");

  _iface.ceCreateFile = (FCeCreateFile)GetProcAddress(_hInstLib.get(), "CeCreateFile");
  if(!_iface.ceCreateFile)
    throw EOperationFailed("ERROR: Couldn't map CeCreateFile() from 'rapi.dll'!!!");

  _iface.ceGetFileSize = (FCeGetFileSize)GetProcAddress(_hInstLib.get(), "CeGetFileSize");
  if(!_iface.ceGetFileSize)
    throw EOperationFailed("ERROR: Couldn't map CeGetFileSize() from 'rapi.dll'!!!");

  _iface.ceReadFile = (FCeReadFile)GetProcAddress(_hInstLib.get(), "CeReadFile");
  if(!_iface.ceReadFile)
    throw EOperationFailed("ERROR: Couldn't map CeReadFile() from 'rapi.dll'!!!");

  _iface.ceWriteFile = (FCeWriteFile)GetProcAddress(_hInstLib.get(), "CeWriteFile");
  if(!_iface.ceWriteFile)
    throw EOperationFailed("ERROR: Couldn't map CeWriteFile() from 'rapi.dll'!!!");

  _iface.ceCloseHandle = (FCeCloseHandle)GetProcAddress(_hInstLib.get(), "CeCloseHandle");
  if(!_iface.ceCloseHandle)
    throw EOperationFailed("ERROR: Couldn't map CeCloseHandle() from 'rapi.dll'!!!");

  _iface.ceCreateDirectory = (FCeCreateDirectory)GetProcAddress(_hInstLib.get(), "CeCreateDirectory");
  if(!_iface.ceCreateDirectory)
    throw EOperationFailed("ERROR: Couldn't map CeCreateDirectory() from 'rapi.dll'!!!");

  // init RAPI
  RAPIINIT initData = { 0 };
  initData.cbSize = sizeof(initData);

  HRESULT hr = _iface.ceRapiInitEx(&initData);
  if(FAILED(hr))
    throw EOperationFailed("Cannot initialize ActiveSync connection!!!");

  DWORD status = WaitForSingleObject(initData.heRapiInit, TIMEOUT);
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

    _iface.ceRapiUninit();
    throw EOperationFailed("ERROR: Cannot initialize ActiveSync interface!!!");
  }
}


/**
 * @brief Class destructor.
 *
 * condor2nav::CActiveSync class destructor.
 */
condor2nav::CActiveSync::~CActiveSync()
{
  _iface.ceRapiUninit();
}


/**
 * @brief Reads whole file to a string.
 *
 * Method reads whole file to a string.
 *
 * @param src Target file path. 
 * @param stream Stream to store file content. 
 */
std::string condor2nav::CActiveSync::Read(const std::string &src) const
{
  std::wstring fileName(src.begin(), src.end());
  std::shared_ptr<void> hSrc(_iface.ceCreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL),
    [this](HANDLE h){ if(h) _iface.ceCloseHandle(h); });
  if(hSrc.get() == INVALID_HANDLE_VALUE)
    throw EOperationFailed("ERROR: Unable to open ActiveSync file '" + src + "'!!!");

  DWORD numBytes = _iface.ceGetFileSize(hSrc.get(), NULL);
  std::unique_ptr<char> buff(new char[numBytes]);

  if(!_iface.ceReadFile(hSrc.get(), buff.get(), numBytes, &numBytes, NULL))
    throw EOperationFailed("ERROR: Reading ActiveSync file '" + src + "'!!!");

  // remove all returns from a file
  std::string buffer(buff.get(), numBytes);
  std::remove_if(buffer.begin(), buffer.end(), [](char c){ return c == '\r'; }); 

  return buffer;
}


/**
 * @brief Writes buffer to a file on the target device.
 *
 * Method writes buffer to a file on the target device.
 *
 * @param dest Target file path. 
 * @param buffer Buffer with file content. 
 */
void condor2nav::CActiveSync::Write(const std::string &dest, const std::string &buffer) const
{
  std::wstring destName(dest.begin(), dest.end());
  std::shared_ptr<void> hDest(_iface.ceCreateFile(destName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL),
    [this](HANDLE h){ if(h) _iface.ceCloseHandle(h); });
  if(hDest.get() == INVALID_HANDLE_VALUE)
    throw EOperationFailed("ERROR: Unable to open ActiveSync file '" + dest + "'!!!");

  DWORD numBytes;
  if(!_iface.ceWriteFile(hDest.get(), buffer.c_str(), buffer.size(), &numBytes, NULL))
    throw EOperationFailed("ERROR: Writing ActiveSync file '" + dest + "'!!!");
}


/**
 * @brief Creates directory on the target device.
 *
 * Method creates directory on the target device.
 *
 * @param path Target directory path. 
 */
void condor2nav::CActiveSync::DirectoryCreate(const std::string &path) const
{
  std::wstring dirName(path.begin(), path.end());
  if(!_iface.ceCreateDirectory(dirName.c_str(), NULL) && _iface.ceGetLastError() != ERROR_ALREADY_EXISTS)
    throw EOperationFailed("ERROR: Creating ActiveSync directory '" + path + "'!!!");
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
bool condor2nav::CActiveSync::FileExists(const std::string &path) const
{
  std::wstring fileName(path.begin(), path.end());
  std::shared_ptr<void> hDest(_iface.ceCreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL),
    [this](HANDLE h){ if(h) _iface.ceCloseHandle(h); });
  if(hDest.get() == INVALID_HANDLE_VALUE) {
    if(_iface.ceGetLastError() == ERROR_FILE_NOT_FOUND)
      return false;
    throw EOperationFailed("ERROR: Unable to check if file '" + path + "' exists!!!");
  }
  else
    return true;
}
