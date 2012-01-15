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
 * @file fileParser.h
 *
 * @brief Declares the condor2nav::CFileParser class. 
 */

#ifndef __FILEPARSER_H__
#define __FILEPARSER_H__

#include <vector>
#include <boost/filesystem.hpp>

namespace condor2nav {

  /**
   * @brief File parsers abstract base class.
   *
   * condor2nav::CFileParser is an abstract base class for all the file parsers.
   * Each parser is not only responsible for parsing and providing the data but
   * it should also be able to dump all its data to the new file of the same
   * format. That class provides also some common parsing tools that may be reused
   * in child classes.
   */
  class CFileParser {
  public:
    typedef std::vector<std::string> CStringArray;	///< @brief The array of strings.

  private:
    const boost::filesystem::path _filePath;	    ///< @brief Input file path.

    CFileParser(const CFileParser &);               ///< @brief Disallowed
    CFileParser &operator=(const CFileParser &);    ///< @brief Disallowed

  protected:
    std::string &WhiteSpacesRemove(std::string &str) const;
    void LineParseKeyValue(const std::string &line, std::string &key, std::string &value) const;
    void LineParseCSV(const std::string &line, CStringArray &values) const;

  public:
    explicit CFileParser(const boost::filesystem::path &filePath);
    virtual ~CFileParser();

    /**
     * @brief Dumps class data to the file.
     *
     * Method dumps class data to the file in the same format as input file has.
     * 
     * @param filePath Path of the file to create (empty means overwrite input file).
     */
    virtual void Dump(const boost::filesystem::path &filePath = "") const = 0;

    const boost::filesystem::path &Path() const;
  };

}

#endif /* __FILEPARSER_H__ */