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
 * @file tools.cpp
 *
 * @brief Implements common tools. 
**/

#include "tools.h"
#include <fstream>
#include <windows.h>


/**
 * @brief Converts the speed units.
 *
 * Function converts the speed units from km/h to m/s.
 *
 * @param value The speed value to convert.
 *
 * @return Converted speed.
**/
unsigned condor2nav::KmH2MS(unsigned value)
{
  return static_cast<unsigned>((value * 10.0 / 36) + 0.5);
}


/** 
 * @brief Creates specified directory
 * 
 * Function creates given directory recursively.
 * 
 * @param dirName Directory name to created.
 *
 * @exception std::runtime_error Operation did not succeed
 */
void condor2nav::DirectoryCreate(const std::string &dirName)
{
  if (!dirName.empty()) {
    size_t pos = 0;
    while(pos != std::string::npos) {
      pos = dirName.find_first_of("/\\", pos);
      if(pos == 0 && dirName[1] == '\\') {
        // network path
        pos = dirName.find_first_of("/\\", 2);
        // skip computer name also
        pos = dirName.find_first_of("/\\", pos + 1);
      }
      std::string subDir(dirName, 0, pos);
    
      if (!::CreateDirectory(subDir.c_str(), 0) && GetLastError() != ERROR_ALREADY_EXISTS)
        throw std::runtime_error("Cannot create directory '" + subDir + "'!!!");

      if(pos != std::string::npos)
        pos++;
    }
  }
}


/** 
 * @brief Checks if file exists in current directory
 * 
 * Function checks if specified file exists.
 * 
 * @param fileName File name to search for
 * 
 * @return Check status
 */
bool condor2nav::FileExists(const std::string &fileName) 
{
  std::ifstream file(fileName.c_str());
  return file.is_open();
}
