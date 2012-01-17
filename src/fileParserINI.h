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
 * @file fileParserINI.h
 *
 * @brief Declares the condor2nav::CFileParserINI class. 
 */

#ifndef __FILEPARSERINI_H__
#define __FILEPARSERINI_H__

#include "fileParser.h"
#include "tools.h"
#include <deque>
#include <map>

namespace condor2nav {

  /**
   * @brief INI type files parser.
   *
   * condor2nav::CFileParserINI is the INI type files parser. Any file that
   * provides key=value pairs can be processed with that class. Input file
   * may have those pairs grouped into chapters or provide one plain set
   * of pairs (set "" for chapter name in that case).
   */
  class CFileParserINI : public CFileParser {
    /**
     * @brief Stores key=value pairs.
     */
    struct TKeyValue {
      std::string key;
      std::string value;
    };
    typedef std::map<const std::string *, std::unique_ptr<TKeyValue>, CPtrCmp> CValuesMap;	///< @brief The map of key=value pairs. 

    /**
     * @brief INI file chapter data.
     */
    struct TChapter {
      std::string name;
      CValuesMap valuesMap;
    };
    typedef std::deque<std::unique_ptr<TChapter>> CChaptersList;	  ///< @brief The list of INI file chapters.

    CValuesMap _valuesMap;	                          ///< @brief The map of plain key=value pairs. 
    CChaptersList _chaptersList;                      ///< @brief The list of chapters and their data found in the file.

    TChapter &Chapter(const std::string &chapter);
    const TChapter &Chapter(const std::string &chapter) const;
    
  public:
    explicit CFileParserINI(const boost::filesystem::path &filePath);

    virtual void Dump(const boost::filesystem::path &filePath = "") const override;

    const std::string &Value(const std::string &chapter, const std::string &key) const;
    void Value(const std::string &chapter, const std::string &key, const std::string &value);
  };

}

#endif /* __FILEPARSERINI_H__ */