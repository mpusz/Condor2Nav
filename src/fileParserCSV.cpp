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
condor2nav::CFileParserCSV::CFileParserCSV(const std::string &filePath):
CFileParser(filePath)
{
  // open CSV file
  CIStream inputStream(filePath);

  // parse all lines
  std::string line;
  while(inputStream.GetLine(line)) {
    if(line.empty())
      continue;

    std::auto_ptr<CStringArray> row(new CStringArray);
    LineParseCSV(line, *row.get());
    _rowsList.push_back(row.release());
  }
}


/**
 * @brief Class destructor. 
 *
 * condor2nav::CFileParserCSV class destructor.
 */
condor2nav::CFileParserCSV::~CFileParserCSV()
{
  Purge(_rowsList);
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
const condor2nav::CFileParser::CStringArray &condor2nav::CFileParserCSV::Row(const std::string &value, unsigned column /* = 0 */, bool nocase /* = false */) const
{
  for(CRowsList::const_iterator it=_rowsList.begin(); it!=_rowsList.end(); ++it)
    if((**it).at(column) == value || (nocase && (**it).at(column).c_str() == CStringNoCase(value.c_str())))
      return **it;

  throw EOperationFailed("ERROR: Couldn't find value '" + value + "' in column '" + Convert(column) + "' of CSV file '" + Path() + "'!!!");
}


/**
 * @brief Returns all rows.
 *
 * Method returns all rows parsed from a CSV file.
 *
 * @return Rows.
 */
const condor2nav::CFileParserCSV::CRowsList &condor2nav::CFileParserCSV::Rows() const
{
  return _rowsList;
}


//void condor2nav::CFileParserCSV::Row(CStringArray &row)
//{
//}


/**
 * @brief Dumps class data to the file.
 *
 * Method dumps class data to the file in the same format as input file has.
 * 
 * @param filePath Path of the file to create (empty means overwrite input file).
 */
void condor2nav::CFileParserCSV::Dump(const std::string &filePath /* = "" */) const
{
  std::string path = filePath != "" ? filePath.c_str() : Path().c_str();
  COStream outputStream(path);

  for(CRowsList::const_iterator it=_rowsList.begin(); it!=_rowsList.end(); ++it) {
    for(unsigned i=0; i<(*it)->size(); i++) {
      if(i)
        outputStream << ",";
      outputStream << (**it)[i];
    }
    outputStream << std::endl;
  }
}
