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
 * @file fileParser.cpp
 *
 * @brief Implements the condor2nav::CFileParser class. 
 */

#include "fileParser.h"
#include "ostream.h"
#include "tools.h"
#include <string>


/**
 * @brief Class constructor.
 *
 * condor2nav::CFileParser class constructor.
 *
 * @param filePath The path of the file to parse.
 */
condor2nav::CFileParser::CFileParser(const boost::filesystem::path &filePath) :
  _filePath{filePath}
{
}


/**
 * @brief Returns the path to the input file. 
 *
 * Method returns the path to the input file.
 * 
 * @return The path to the input file.
 */
const boost::filesystem::path &condor2nav::CFileParser::Path() const
{
  return _filePath;
}


/**
* @brief Dumps class data to the file.
*
* Method dumps class data to the file in the same format as input file has.
*
* @param filePath Path of the file to create (empty means overwrite input file).
*/
void condor2nav::CFileParser::Dump(const boost::filesystem::path &filePath /* = "" */) const
{
  COStream ostream{filePath.empty() ? Path() : filePath};
  Write(ostream);
}





/**
* @brief Parses the line as key=value pairs.
*
* Method parses the line as key=value pairs.
*
* @param line           The line to parse.
*
* @return Key and value pair.
*
* @exception std Thrown when operation failed.
*/
std::pair<std::string, std::string> condor2nav::LineParseKeyValue(const std::string &line)
{
  auto pos = line.find_first_of("=");
  if(pos == std::string::npos)
    throw EOperationFailed{"ERROR: '=' sign not found in line '" + line + "'!!!"};

  auto ret = std::make_pair(line.substr(0, pos), line.substr(pos + 1));
  Trim(ret.first);
  Trim(ret.second);
  return ret;
}


/**
* @brief Parses the line as CSV (Comma Separated Values).
*
* Method parses the line as CSV (Comma Separated Values).
*
* @param line            The line to parse.
*
* @return Parsed values.
*/
std::vector<std::string> condor2nav::LineParseCSV(const std::string &line)
{
  std::vector<std::string> values;
  bool insideQuote = false;
  size_t pos = 0, newValuePos = 0;
  do {
    auto posOld = pos;
    if(insideQuote) {
      pos = line.find_first_of("\"", posOld);
      insideQuote = false;
    }
    else {
      pos = line.find_first_of(",\"", posOld);
      if(pos != std::string::npos && line[pos] == '\"') {
        insideQuote = true;
      }
      else {
        auto len = (pos != std::string::npos) ? (pos - newValuePos) : pos;
        auto value = line.substr(newValuePos, len);
        Trim(value);
        if(!value.empty() && value[0] == '\"')
          // remove quotes
          value = value.substr(1, value.size() - 2);
        values.emplace_back(std::move(value));
        if(pos != std::string::npos)
          newValuePos = pos + 1;
      }
    }
    if(pos != std::string::npos)
      pos++;
  }
  while(pos != std::string::npos);

  return values;
}
