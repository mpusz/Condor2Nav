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


/**
 * @brief Class constructor.
 *
 * condor2nav::CFileParserINI class constructor.
 *
 * @param filePath The path of the INI file to parse.
 */
condor2nav::CFileParserINI::CFileParserINI(boost::filesystem::path filePath) :
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
condor2nav::CFileParserINI::CFileParserINI(const std::string &server, const boost::filesystem::path &url) :
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
      
      auto chapter = std::make_unique<TChapter>();
      chapter->name = line.substr(1, pos2 - 1);
      Trim(chapter->name);
      currentMap = &chapter->valuesMap;
      _chaptersList.emplace_back(std::move(chapter));
      continue;
    }
    
    // parse line as key=value
    auto ret = LineParseKeyValue(line);

    if(currentMap->find(&ret.first) != currentMap->end())
      throw EOperationFailed{"ERROR: Entry '" + ret.first + "' provided more than once in '" + Path().string() + "' INI file!!!"};

    // add new entry
    auto data = std::make_unique<TKeyValue>();
    data->key = std::move(ret.first);
    data->value = std::move(ret.second);
    (*currentMap)[&data->key] = std::move(data);
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
    if(ch->name == chapter)
      return *ch;
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
  const CValuesMap *mapPtr = &_valuesMap;
  if(chapter != "")
    mapPtr = &Chapter(chapter).valuesMap;

  auto it = mapPtr->find(&key);
  if(it == mapPtr->end())
    throw EOperationFailed{"ERROR: Entry '" + key + "' not found in '" + Path().string() + "' INI file!!!"};
  return it->second->value;
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
void condor2nav::CFileParserINI::Value(const std::string &chapter, const std::string &key, const std::string &value)
{
  CValuesMap *mapPtr = &_valuesMap;
  if(chapter != "")
    mapPtr = &Chapter(chapter).valuesMap;

  auto it = mapPtr->find(&key);
  if(it == mapPtr->end()) {
    // add new entry
    auto data = std::make_unique<TKeyValue>();
    data->key = key;
    data->value = value;
    (*mapPtr)[&data->key] = std::move(data);
  }
  else
    // update existing entry
    it->second->value = value;
}


/**
* @brief Dumps class data to the file.
*
* Method dumps class data to the file in the same format as input file has.
*
* @param filePath Path of the file to create (empty means overwrite input file).
*/
void condor2nav::CFileParserINI::Dump(const boost::filesystem::path &filePath /* = "" */) const
{
  COStream ostream{filePath.empty() ? Path() : filePath};
  // dump global scope
  for(const auto &v : _valuesMap)
    ostream << *v.first << "=" << v.second->value << std::endl;

  // dump chapters
  for(auto it=_chaptersList.begin(); it!=_chaptersList.end(); ++it) {
    if(it != _chaptersList.begin() || _valuesMap.size())
      ostream << std::endl;

    ostream << "[" << (*it)->name << "]" << std::endl;
    for(const auto &v : (*it)->valuesMap)
      ostream << *v.first << "=" << v.second->value << std::endl;
  }
}
