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
 * @file activeSync.cpp
 *
 * @brief Implements the ActiveSync wrapper class. 
**/

#include "activeSync.h"
#include "rapi.h"
#include <fstream>
#include <memory>

condor2nav::CDelayedPtr<condor2nav::CActiveSync> condor2nav::CActiveSync::_instance;



/**
 * @brief Returns singleton instance.
 *
 * Method returns singleton instance.
 *
 * @return Singleton instance.
**/
condor2nav::CActiveSync &condor2nav::CActiveSync::Instance()
{
  return *_instance;
}


/**
 * @brief Class constructor.
 *
 * condor2nav::CActiveSync class constructor.
**/
condor2nav::CActiveSync::CActiveSync()
{
  RAPIINIT initData = { 0 };
  initData.cbSize = sizeof(initData);

  HRESULT hr = CeRapiInitEx(&initData);
  if(FAILED(hr))
    throw std::runtime_error("Cannot initialize ActiveSync connection!!!");

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

    CeRapiUninit();
    throw std::runtime_error("ERROR: Cannot initialize ActiveSync interface!!!");
  }
}


/**
 * @brief Class destructor.
 *
 * condor2nav::CActiveSync class destructor.
**/
condor2nav::CActiveSync::~CActiveSync()
{
  CeRapiUninit();
}


/**
 * @brief Reads whole file to a string.
 *
 * Method reads whole file to a string.
 *
 * @param src Target file path. 
 * @param stream Stream to store file content. 
**/
void condor2nav::CActiveSync::Read(const std::string &src, std::stringstream &stream) const
{
  std::wstring fileName(src.begin(), src.end());
  HANDLE hSrc = CeCreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if(hSrc == INVALID_HANDLE_VALUE)
    throw std::runtime_error("ERROR: Unable to open ActiveSync file '" + src + "'!!!");

  DWORD numBytes = CeGetFileSize(hSrc, NULL);
  std::auto_ptr<char> buff(new char[numBytes]);

  if(!CeReadFile(hSrc, buff.get(), numBytes, &numBytes, NULL))
    throw std::runtime_error("ERROR: Reading ActiveSync file '" + src + "'!!!");

  // remove all returns from a file
  std::string buffer(buff.get(), numBytes);
  size_t pos = 0;
  do {
    pos = buffer.find_first_of('\r', pos);
    if(pos != std::string::npos)
      buffer.erase(pos, 1);
  }
  while(pos != std::string::npos);

  stream.str(buffer);

  CeCloseHandle(hSrc);
}


/**
 * @brief Writes buffer to a file on the target device.
 *
 * Method writes buffer to a file on the target device.
 *
 * @param dest Target file path. 
 * @param buffer Buffer with file content. 
**/
void condor2nav::CActiveSync::Write(const std::string &dest, const std::string &buffer) const
{
  std::wstring destName(dest.begin(), dest.end());
  HANDLE hDest = CeCreateFile(destName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if(hDest == INVALID_HANDLE_VALUE)
    throw std::runtime_error("ERROR: Unable to open ActiveSync file '" + dest + "'!!!");

  DWORD numBytes;
  if(!CeWriteFile(hDest, buffer.c_str(), buffer.size(), &numBytes, NULL))
    throw std::runtime_error("ERROR: Writing ActiveSync file '" + dest + "'!!!");

  CeCloseHandle(hDest);
}


/**
 * @brief Creates directory on the target device.
 *
 * Method creates directory on the target device.
 *
 * @param path Target directory path. 
**/
void condor2nav::CActiveSync::DirectoryCreate(const std::string &path) const
{
  std::wstring dirName(path.begin(), path.end());
  if(!CeCreateDirectory(dirName.c_str(), NULL) && CeGetLastError() != ERROR_ALREADY_EXISTS)
    throw std::runtime_error("ERROR: Creating ActiveSync directory '" + path + "'!!!");
}


/**
 * @brief Checks if a file exists on the target device.
 *
 * Method checks if a file exists on the target device.
 *
 * @param path Target file path.
 *
 * @return @p true if file exists.
**/
bool condor2nav::CActiveSync::FileExists(const std::string &path) const
{
  std::wstring fileName(path.begin(), path.end());
  HANDLE hDest = CeCreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if(hDest == INVALID_HANDLE_VALUE) {
    if(CeGetLastError() == ERROR_FILE_NOT_FOUND)
      return false;
    throw std::runtime_error("ERROR: Unable to check if file '" + path + "' exists!!!");
  }
  else {
    CeCloseHandle(hDest);
    return true;
  }
}
