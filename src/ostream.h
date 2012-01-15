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
 * @file ostream.h
 *
 * @brief Declares the OStream wrapper class. 
 */

#ifndef __OSTREAM_H__
#define __OSTREAM_H__

#include "stream.h"

namespace condor2nav {

  /**
   * @brief Output stream wrapper
   *
   * condor2nav::COStream class is a wrapper for different stream types.
   */
  class COStream : public CStream {
  public:
    COStream(const boost::filesystem::path &fileName);
    ~COStream();
    COStream &Write(const char *buffer, std::streamsize num);

    /**
     * @brief Writes new data to a stream. 
     *
     * Function writes new data to a stream.
     *
     * @param stream Stream to use. 
     * @param obj Data to write. 
     *
     * @return Stream instance.
     */
    template<class T>
    friend COStream &operator<<(COStream &stream, const T &obj)
    {
      stream.Buffer() << obj;
      return stream;
    }

    /**
     * @brief Writes functor (e.g. std::endl) to a stream.
     *
     * Method writes functor (e.g. std::endl) to a stream.
     *
     * @param stream Stream to use. 
     * @param f Functor to write. 
     *
     * @return Stream instance.
     */
    friend COStream &operator<<(COStream &stream, std::ostream &(*f)(std::ostream &))
    {
      stream.Buffer() << f;
      return stream;
    }
  };

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
inline condor2nav::COStream &condor2nav::COStream::Write(const char *buffer, std::streamsize num)
{
  Buffer().write(buffer, num);
  return *this;
}


#endif /* __OSTREAM_H__ */