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
 * @file fileParserINI.cpp
 *
 * @brief Implements the condor2nav::CFileParserINI class. 
 */

#include "fileParserINI.h"
#include "istream.h"
#include "ostream.h"


namespace {

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
  std::pair<const std::string, std::string> LineParseKeyValue(const std::string &line)
  {
    using namespace condor2nav;
    auto pos = line.find_first_of("=");
    if(pos == std::string::npos)
      throw EOperationFailed{"ERROR: '=' sign not found in line '" + line + "'!!!"};

    auto ret = std::make_pair(line.substr(0, pos), line.substr(pos + 1));
    Trim(ret.first);
    Trim(ret.second);
    return ret;
  }

}

/**
 * @brief Class constructor.
 *
 * condor2nav::CFileParserINI class constructor.
 *
 * @param filePath The path of the INI file to parse.
 */
condor2nav::CFileParserINI::CFileParserINI(bfs::path filePath) :
  _filePath{std::move(filePath)}
{
  // open input INI file
  CIStream inputStream{filePath};
  Parse(inputStream);
}


/**
 * @brief Class constructor.
 *
 * condor2nav::CFileParserINI class constructor.
 *
 * @param server The server from which to download the INI file.
 * @param url The path on the server to the INI file.
 */
condor2nav::CFileParserINI::CFileParserINI(const std::string &server, const bfs::path &url) :
  _filePath{server + url.generic_string()}
{
  CIStream inputStream{server, url.generic_string()};
  Parse(inputStream);
}


/**
 * @brief INI file parser.
 *
 * Parses INI file.
 *
 * @param inputStream Input stream to use for reading.
 */
void condor2nav::CFileParserINI::Parse(CIStream &inputStream)
{
  // parse all lines
  std::string line;
  CValuesMap *currentMap = &_valuesMap;
  while(inputStream.GetLine(line)) {
    if(line.empty())
      continue;

    auto pos = line.find_first_not_of(" ");
    if(pos == std::string::npos)
      continue;

    if(line[pos] == ';' || line[pos] == '#')
      // skip comment
      continue;

    if(line[pos] == '[') {
      // new chapter
      auto pos2 = line.find_first_of("]");
      if(pos2 == std::string::npos)
        throw EOperationFailed{"ERROR: ']' not found in file line '" + line + "' in '" + Path().string() + "' INI !!!"};
      
      TChapter chapter;
      chapter.name = line.substr(1, pos2 - 1);
      Trim(chapter.name);
      _chaptersList.emplace_back(std::move(chapter));
      currentMap = &_chaptersList.back().valuesMap;
      continue;
    }
    
    // add new entry
    auto ret = currentMap->insert(LineParseKeyValue(line));
    if(!ret.second)
       throw EOperationFailed{"ERROR: Entry '" + ret.first->first + "' provided more than once in '" + Path().string() + "' INI file!!!"};
  }
}


/**
 * @brief Returns requested chapter.
 *
 * Method returns chapter specified by the @p chapter parameter.
 * 
 * @param chapter The chapter name to find.
 *
 * @exception std Thrown when chapter not found.
 *
 * @return Requested chapter.
 */
auto condor2nav::CFileParserINI::Chapter(const std::string &chapter) -> TChapter &
{
  for(auto &ch : _chaptersList)
    if(ch.name == chapter)
      return ch;
  throw EOperationFailed{"ERROR: Chapter '" + chapter + "' not found in '" + Path().string() + "' INI file!!!"};
}


/**
 * @brief Returns requested chapter.
 *
 * Method returns chapter specified by the @p chapter parameter.
 * 
 * @param chapter The chapter name to find.
 *
 * @exception std Thrown when chapter not found.
 *
 * @return Requested chapter.
 */
auto condor2nav::CFileParserINI::Chapter(const std::string &chapter) const -> const TChapter &
{
  auto nonConst = const_cast<CFileParserINI *>(this);
  return nonConst->Chapter(chapter);
}


/**
 * @brief Returns requested value. 
 *
 * Method returns the value specified by the chapter and key name. To search in
 * global scope (no chapters) "" should be provided for @p chapter.
 * 
 * @param chapter The chapter name to find ("" means to look in global scope).
 * @param key     The key name o find.
 *
 * @exception std Thrown when value not found.
 *
 * @return Requested value.
 */
const std::string &condor2nav::CFileParserINI::Value(const std::string &chapter, const std::string &key) const
{
  const CValuesMap &map = (chapter != "") ? Chapter(chapter).valuesMap : _valuesMap;
  auto it = map.find(key);
  if(it == map.end())
    throw EOperationFailed{"ERROR: Entry '" + key + "' not found in '" + Path().string() + "' INI file!!!"};
  return it->second;
}


/**
 * @brief Sets specified value.
 *
 * Method sets the value for the provided chapter and its key.
 *
 * @param chapter The chapter name of the value.
 * @param key     The key name. 
 * @param value   The value to set.
 */
void condor2nav::CFileParserINI::Value(const std::string &chapter, const std::string &key, std::string value)
{
  CValuesMap &map = (chapter != "") ? Chapter(chapter).valuesMap : _valuesMap;
  map[key] = std::move(value);
}


/**
* @brief Dumps class data to the file.
*
* Method dumps class data to the file in the same format as input file has.
*
* @param filePath Path of the file to create (empty means overwrite input file).
*/
void condor2nav::CFileParserINI::Dump(const bfs::path &filePath /* = "" */) const
{
  COStream ostream{filePath.empty() ? Path() : filePath};
  // dump global scope
  for(const auto &v : _valuesMap)
    ostream << v.first << "=" << v.second << std::endl;

  // dump chapters
  for(auto it=_chaptersList.begin(); it!=_chaptersList.end(); ++it) {
    if(it != _chaptersList.begin() || _valuesMap.size())
      ostream << std::endl;

    ostream << "[" << it->name << "]" << std::endl;
    for(const auto &v : it->valuesMap)
      ostream << v.first << "=" << v.second << std::endl;
  }
}
