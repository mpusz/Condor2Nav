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
  **/
  class CFileParserINI : public CFileParser {
    /**
     * @brief Stores key=value pairs.
    **/
    struct TKeyValue {
      std::string key;
      std::string value;
    };
    typedef std::map<const std::string *, TKeyValue *, CPtrCmp> CValuesMap;	///< @brief The map of key=value pairs. 

    /**
     * @brief INI file chapter data.
    **/
    struct TChapter {
      std::string name;
      CValuesMap valuesMap;
    };
    typedef std::deque<TChapter *> CChaptersList;	  ///< @brief The list of INI file chapters.

    CValuesMap _valuesMap;	                 ///< @brief The map of plain key=value pairs. 
    CChaptersList _chaptersList;             ///< @brief The list of chapters and their data found in the file.

    TChapter &Chapter(const std::string &chapter);
    const TChapter &Chapter(const std::string &chapter) const;
    
  public:
    explicit CFileParserINI(const std::string &filePath);
    ~CFileParserINI();

    virtual void Dump(const std::string &filePath = "") const;

    const std::string &Value(const std::string &chapter, const std::string &key) const;
    void Value(const std::string &chapter, const std::string &key, const std::string &value);
  };

}

#endif /* __FILEPARSERINI_H__ */