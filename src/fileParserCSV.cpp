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
 * @file fileParserCSV.cpp
 *
 * @brief Implements the condor2nav::CFileParserCSV class. 
 */

#include "fileParserCSV.h"
#include "istream.h"
#include "ostream.h"
#include "tools.h"
#include "traitsNoCase.h"
#include <string>


/**
 * @brief Class constructor.
 *
 * condor2nav::CFileParserCSV class constructor.
 *
 * @param filePath The path of the CSV file to parse.
 */
condor2nav::CFileParserCSV::CFileParserCSV(boost::filesystem::path filePath) :
  _filePath{std::move(filePath)}
{
  // open CSV file
  CIStream inputStream{filePath};

  // parse all lines
  std::string line;
  while(inputStream.GetLine(line)) {
    if(line.empty())
      continue;
    _rowsList.emplace_back(LineParseCSV(line));
  }
}


/**
 * @brief Returns requested row.
 *
 * Method returns the values from one row specified by the value in specified column.
 *
 * @param value  The value to use for searching.
 * @param column The column index to be used for value comparison.
 * @param nocase Specifies if a search should be case sensitive.
 *
 * @exception std Thrown when requested row is not found.
 *
 * @return Requested row.
 */
auto condor2nav::CFileParserCSV::Row(const std::string &value, unsigned column /* = 0 */, bool nocase /* = false */) const -> const CStringArray &
{
  auto nonConst = const_cast<CFileParserCSV *>(this);
  return nonConst->Row(value, column, nocase);
}


/**
 * @brief Returns requested row.
 *
 * Method returns the values from one row specified by the value in specified column.
 *
 * @param value  The value to use for searching.
 * @param column The column index to be used for value comparison.
 * @param nocase Specifies if a search should be case sensitive.
 *
 * @exception std Thrown when requested row is not found.
 *
 * @return Requested row.
 */
auto condor2nav::CFileParserCSV::Row(const std::string &value, unsigned column /* = 0 */, bool nocase /* = false */) -> CStringArray &
{
  for(auto &row : _rowsList)
    if(row.at(column) == value || (nocase && row[column].c_str() == CStringNoCase{value.c_str()}))
      return row;
  throw EOperationFailed{"ERROR: Couldn't find value '" + value + "' in column '" + Convert(column) + "' of CSV file '" + Path().string() + "'!!!"};
}


/**
 * @brief Returns all rows.
 *
 * Method returns all rows parsed from a CSV file.
 *
 * @return Rows.
 */
auto condor2nav::CFileParserCSV::Rows() const -> const CRowsList &
{
  return _rowsList;
}


/**
 * @brief Returns all rows.
 *
 * Method returns all rows parsed from a CSV file.
 *
 * @return Rows.
 */
auto condor2nav::CFileParserCSV::Rows() -> CRowsList &
{
  return _rowsList;
}


/**
* @brief Dumps class data to the file.
*
* Method dumps class data to the file in the same format as input file has.
*
* @param filePath Path of the file to create (empty means overwrite input file).
*/
void condor2nav::CFileParserCSV::Dump(const boost::filesystem::path &filePath /* = "" */) const
{
  COStream ostream{filePath.empty() ? Path() : filePath};
  for(const auto &row : _rowsList) {
    for(size_t i = 0; i < row.size(); ++i) {
      if(i)
        ostream << ",";
      ostream << row[i];
    }
    ostream << std::endl;
  }
}
