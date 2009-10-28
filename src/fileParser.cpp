//
// This file is a part of Condor2Nav file formats translator.
//
// Copyright (C) 2009 Mateusz Pusz
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Visit the project webpage (http://sf.net/projects/condor2nav) for more info.
//

#include "fileParser.h"
#include <string>


/**
 * @brief Class constructor.
 *
 * condor2nav::CFileParser class constructor.
 *
 * @param filePath The path of the file to parse.
**/
condor2nav::CFileParser::CFileParser(const std::string &filePath):
_filePath(filePath)
{
}


/**
 * @brief Class destructor.
 *
 * condor2nav::CFileParser class destructor.
**/
condor2nav::CFileParser::~CFileParser()
{
}


/**
 * @brief Returns the path to the input file. 
 *
 * Method returns the path to the input file.
 * 
 * @return The path to the input file.
**/
const std::string &condor2nav::CFileParser::Path() const
{
  return _filePath;
}


/**
 * @brief Removes leading and trailing white spaces from string.
 *
 * Method removes leading and trailing white spaces from given string.
 *
 * @param [in,out] str The string to cut.
 *
 * @return Cut string.
**/
std::string &condor2nav::CFileParser::WhiteSpacesRemove(std::string &str) const
{
  if(str != "") {
    size_t pos1 = str.find_first_not_of(" ");
    size_t pos2 = str.find_last_not_of(" ");
    str = str.substr(pos1, pos2 - pos1 + 1);
  }
  return str;
}

 
/**
 * @brief Parses the line as key=value pairs.
 *
 * Method parses the line as key=value pairs.
 *
 * @param line           The line to parse. 
 * @param [in,out] key   Parsed key. 
 * @param [in,out] value Parsed value. 
 *
 * @exception std Thrown when operation failed.
**/
void condor2nav::CFileParser::LineParseKeyValue(const std::string &line, std::string &key, std::string &value) const
{
  size_t pos = line.find_first_of("=");
  if(pos == std::string::npos)
    throw std::runtime_error("ERROR: '=' sign not found in '" + Path() + "' file line '" + line + "'!!!");
  if(line.find_last_of("=") != pos)
    throw std::runtime_error("ERROR: More than one '=' sign found in '" + Path() + "' file line '" + line + "'!!!");
  
  key = line.substr(0, pos);
  WhiteSpacesRemove(key);

  value = line.substr(pos + 1);
  WhiteSpacesRemove(value);
}


/**
 * @brief Parses the line as CSV (Comma Separated Values).
 *
 * Method parses the line as CSV (Comma Separated Values).
 *
 * @param line            The line to parse.
 * @param [in,out] values Parsed values.
**/
void condor2nav::CFileParser::LineParseCSV(const std::string &line, CStringArray &values) const
{
  size_t pos = 0;
  do {
    size_t posOld = pos;
    pos = line.find_first_of(",", posOld);
    size_t len = (pos != std::string::npos) ? (pos - posOld) : pos;
    std::string value = line.substr(posOld, len);
    WhiteSpacesRemove(value);
    values.push_back(value);
    if(pos != std::string::npos)
      pos++;
  }
  while(pos != std::string::npos);
}
