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


condor2nav::CActiveSync::CActiveSync(double timeout)
{
  RAPIINIT initData = { 0 };
  initData.cbSize = sizeof(initData);

  HRESULT hr = CeRapiInitEx(&initData);
  if(FAILED(hr))
    throw std::run_time_error("Cannot initialize ActiveSync connection!!!");

  DWORD status = WaitForSingleObject(initData.heRapiInit, timeout);
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
  }
}

HRESULT TryRapiConnect(DWORD dwTimeOut)
{
    HRESULT            hr = E_FAIL;
    RAPIINIT           riCopy;
    bool          fInitialized = false;

    ZeroMemory(&riCopy, sizeof(riCopy));
    riCopy.cbSize = sizeof(riCopy);

    hr = CeRapiInitEx(&riCopy);
    if (SUCCEEDED(hr))
    {
        DWORD dwRapiInit = 0;
        fInitialized = true;

        dwRapiInit = WaitForSingleObject(
                    riCopy.heRapiInit,
                    dwTimeOut);
        if (WAIT_OBJECT_0 == dwRapiInit)
        {
            //  heRapiInit signaled:
            // set return error code to return value of RAPI Init function
            hr = riCopy.hrRapiInit;  
        }
        else if (WAIT_TIMEOUT == dwRapiInit)
        {
            // timed out: device is probably not connected
            // or not responding
            hr = HRESULT_FROM_WIN32(ERROR_TIMEOUT);
        }
        else
        {
            // WaitForSingleObject failed
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }

   if (fInitialized && FAILED(hr))
   {
        CeRapiUninit();
   }
    return hr;
}