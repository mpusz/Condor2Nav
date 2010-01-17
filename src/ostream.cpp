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
 * @file ostream.cpp
 *
 * @brief Implements the OStream wrapper class. 
**/

#include "ostream.h"
#include "activeSync.h"
#include <fstream>
#include <iostream>


/**
 * @brief Class constructor.
 *
 * condor2nav::COStream class constructor.
 *
 * @param fileName The name of the file to create.
**/
condor2nav::COStream::COStream(const std::string &fileName):
CStream(fileName)
{
}


/**
 * @brief Class destructor.
 *
 * condor2nav::COStream class destructor. Writes local buffer to
 * a file.
**/
condor2nav::COStream::~COStream()
{
  if(Buffer().str().size()) {
    switch(Type()) {
      case TYPE_LOCAL:
        {
          char dirCurr[MAX_PATH];
          std::string file(FileName());
          size_t pos = file.find_last_of('\\');
          if(pos != std::string::npos) {
            GetCurrentDirectory(MAX_PATH, dirCurr);
            std::string dir(file, 0, pos);
            SetCurrentDirectory(dir.c_str());
            file = file.substr(pos + 1);
          }

          std::ofstream stream(file.c_str(), std::ios_base::out | std::ios_base::binary);
          if(!stream)
            std::cerr << "ERROR: Couldn't open file '" << FileName() << "' for writing!!!" << std::endl;
          stream << Buffer().str();

          if(pos != std::string::npos)
            SetCurrentDirectory(dirCurr);
        }
        break;

      case TYPE_ACTIVE_SYNC:
        {
          CActiveSync &activeSync(CActiveSync::Instance());
          activeSync.Write(FileName(), Buffer().str());
        }
        break;

      default:
        std::cerr << "ERROR: Unknown stream type (" << Type() << ")!!!" << std::endl;
    }
  }
}
