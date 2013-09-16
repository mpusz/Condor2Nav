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
 * @file fileParserINI.h
 *
 * @brief Declares the condor2nav::CFileParserINI class. 
 */

#ifndef __FILEPARSERINI_H__
#define __FILEPARSERINI_H__

#include "nonCopyable.h"
#include "tools.h"
#include <memory>
#include <deque>
#include <map>
#include <boost/filesystem.hpp>

namespace condor2nav {

  class CIStream;

  /**
   * @brief INI type files parser.
   *
   * condor2nav::CFileParserINI is the INI type files parser. Any file that
   * provides key=value pairs can be processed with that class. Input file
   * may have those pairs grouped into chapters or provide one plain set
   * of pairs (set "" for chapter name in that case).
   */
  class CFileParserINI : CNonCopyable {
    using CValuesMap = std::map<std::string, std::string>;	///< @brief The map of key=value pairs. 

    /**
     * @brief INI file chapter data.
     */
    struct TChapter {
      std::string name;
      CValuesMap valuesMap;
    };
    using CChaptersList = std::deque<TChapter>;	      ///< @brief The list of INI file chapters.

    const bfs::path _filePath;                        ///< @brief Input file path.
    CValuesMap _valuesMap;	                          ///< @brief The map of plain key=value pairs. 
    CChaptersList _chaptersList;                      ///< @brief The list of chapters and their data found in the file.

    void Parse(CIStream &inputStream);
    TChapter &Chapter(const std::string &chapter);
    const TChapter &Chapter(const std::string &chapter) const;

  public:
    explicit CFileParserINI(bfs::path filePath);
    CFileParserINI(const std::string &server, const bfs::path &url);
    const bfs::path &Path() const { return _filePath; }
    const std::string &Value(const std::string &chapter, const std::string &key) const;
    void Value(const std::string &chapter, const std::string &key, std::string value);
    void Dump(const bfs::path &filePath = "") const;
  };

}

#endif /* __FILEPARSERINI_H__ */