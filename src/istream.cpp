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
 * @file istream.cpp
 *
 * @brief Implements the IStream wrapper class. 
**/

#include "istream.h"
#include "activeSync.h"
#include <fstream>


/**
 * @brief Class constructor.
 *
 * condor2nav::CIStream class constructor.
 *
 * @param fileName The name of the file to read.
**/
condor2nav::CIStream::CIStream(const std::string &fileName):
CStream(fileName)
{
  switch(Type()) {
    case TYPE_LOCAL:
      {
        std::fstream stream(fileName.c_str(), std::ios_base::in);
        if(!stream) {
          // it is possible that a file is under Vista VirtualStore so try to look for it
          stream.clear();
          stream.open(fileName.c_str(), std::ios_base::in | std::ios_base::out);
          if(!stream)
            throw EOperationFailed("ERROR: Couldn't open file '" + fileName + "' for reading!!!");
        }

        Buffer() << stream.rdbuf();
      }
      break;

    case TYPE_ACTIVE_SYNC:
      {
        CActiveSync &activeSync(CActiveSync::Instance());
        activeSync.Read(FileName(), Buffer());
      }
      break;

    default:
      throw EOperationFailed("ERROR: Unknown stream type!!!");
  }
}


/**
 * @brief Class destructor.
 *
 * condor2nav::CIStream class destructor.
**/
condor2nav::CIStream::~CIStream()
{
}
