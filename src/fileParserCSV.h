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
 * @file fileParserCSV.h
 *
 * @brief Declares the condor2nav::CFileParserCSV class. 
 */

#ifndef __FILEPARSERCSV_H__
#define __FILEPARSERCSV_H__

#include "fileParser.h"
#include <deque>

namespace condor2nav {

  /**
   * @brief CSV type files parser.
   *
   * condor2nav::CFileParserCSV is the CSV (Comma Separated Values) type
   * files parser. Any file that provides values separated with commas can
   * be processed with that class.
   */
  class CFileParserCSV : public CFileParser {
  public:
    typedef std::deque<CStringArray> CRowsList;	 ///< @brief The list of string arrays. 

  private:
    CRowsList _rowsList;	                     ///< @brief The list of file rows.

  public:
    explicit CFileParserCSV(const boost::filesystem::path &filePath);

    virtual void Dump(const boost::filesystem::path &filePath = "") const override;
    
    const CStringArray &Row(const std::string &value, unsigned column = 0, bool nocase = false) const;
    CStringArray &Row(const std::string &value, unsigned column = 0, bool nocase = false);
    const CRowsList &Rows() const;
//    void Row(CStringArray &row);
  };

}

#endif /* __FILEPARSERCSV_H__ */
