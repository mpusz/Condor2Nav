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
 * @file ostream.cpp
 *
 * @brief Implements the OStream wrapper class. 
 */

#include "ostream.h"
#include "activeSync.h"
#include <algorithm>
#include <boost/filesystem/fstream.hpp>


/**
 * @brief Class constructor.
 *
 * condor2nav::COStream class constructor.
 *
 * @param fileName The name of the file to create.
 */
condor2nav::COStream::COStream(boost::filesystem::path fileName) :
  _pathList{{std::move(fileName)}}
{
}


/**
 * @brief Class constructor.
 *
 * condor2nav::COStream class constructor.
 *
 * @param pathList The list of files to create.
 */
condor2nav::COStream::COStream(CPathList pathList) :
  _pathList{std::move(pathList)}
{
}


/**
 * @brief Class destructor.
 *
 * condor2nav::COStream class destructor. Writes local buffer to
 * a file.
 */
condor2nav::COStream::~COStream()
{
  if(Buffer().str().size()) {
    for(auto &path : _pathList) {
      switch(Type(path)) {
      case TType::LOCAL:
        {
          boost::filesystem::ofstream stream{path, std::ios_base::out | std::ios_base::binary};
          if(!stream)
            throw EOperationFailed{"ERROR: Couldn't open file '" + path.string() + "' for writing!!!"};
          stream << Buffer().str();
        }
        break;

      case TType::ACTIVE_SYNC:
        CActiveSync::Instance().Write(path, Buffer().str());
        break;
      }
    }
  }
}


/**
* @brief Writes binary buffer to a stream.
*
* Method writes binary buffer to a stream.
*
* @param buffer Buffer data.
* @param num Buffer size.
*
* @return Stream instance.
*/
condor2nav::COStream &condor2nav::COStream::Write(const char *buffer, std::streamsize num)
{
  Buffer().write(buffer, num);
  return *this;
}
