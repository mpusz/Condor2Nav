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


/**
 * @brief Class constructor.
 *
 * condor2nav::COStream class constructor.
 *
 * @param fileName The name of the file to create.
 * @param mode File open mode
**/
condor2nav::COStream::COStream(const std::string &fileName, std::ios_base::openmode mode /* = std::ios::out */):
_fileName(fileName), _buffer(mode)
{
  if(fileName.size() > 2 && fileName[0] == '\\' && fileName[1] != '\\')
    _type = TYPE_ACTIVE_SYNC;
  else
    _type = TYPE_LOCAL;
}


/**
 * @brief Class destructor.
 *
 * condor2nav::COStream class destructor. Writes local buffer to
 * a file.
**/
condor2nav::COStream::~COStream()
{
  if(_buffer.str().size()) {
    switch(_type) {
      case TYPE_LOCAL:
        {
          std::ofstream stream(_fileName.c_str());
          if(!stream)
            throw std::runtime_error("ERROR: Couldn't open file '" + _fileName + "' for writing!!!");
          stream << _buffer.str();
        }
        break;

      case TYPE_ACTIVE_SYNC:
        {
          CActiveSync &activeSync(CActiveSync::Instance());
          activeSync.Write(_fileName, _buffer.str());
        }
        break;

      default:
        throw std::out_of_range("ERROR: Unknown stream type!!!");
    }
  }
}
