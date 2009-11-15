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
 * @brief Creates direcotry on the target device.
 *
 * Method creates direcotry on the target device.
 *
 * @param name Target directory path. 
**/
void condor2nav::CActiveSync::DirectoryCreate(const std::string &name) const
{
  std::wstring dirName(name.begin(), name.end());
  if(!CeCreateDirectory(dirName.c_str(), NULL) && CeGetLastError() != ERROR_ALREADY_EXISTS)
    throw std::runtime_error("ERROR: Creating ActiveSync directory '" + name + "'!!!");
}
