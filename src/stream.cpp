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
 * @file stream.cpp
 *
 * @brief Implements the Stream wrapper class. 
 */

#include "stream.h"
#include "activeSync.h"
#include <fstream>


/**
 * @brief Class constructor.
 *
 * condor2nav::CStream class constructor.
 *
 * @param fileName The name of the file to create.
 */
condor2nav::CStream::CStream(const boost::filesystem::path &fileName):
_fileName(fileName)
{
  std::string str(fileName.string());
  if(str.size() > 2 && str[0] == '\\' && str[1] != '\\')
    _type = TYPE_ACTIVE_SYNC;
  else
    _type = TYPE_LOCAL;
}


/**
 * @brief Class destructor.
 *
 * condor2nav::CStream class destructor. Writes local buffer to
 * a file.
 */
condor2nav::CStream::~CStream()
{
}
