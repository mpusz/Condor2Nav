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
  **/
  class CFileParserCSV : public CFileParser {
    typedef std::deque<CStringArray *> CRowsList;	///< @brief The list of string arrays. 
    CRowsList _rowsList;	        ///< @brief The list of file rows.

  public:
    explicit CFileParserCSV(const std::string &filePath);
    ~CFileParserCSV();

    virtual void Dump(const std::string &filePath = "") const;
    
    const CStringArray &Row(const std::string &value, unsigned column = 0) const;
//    void Row(CStringArray &row);
  };

}

#endif /* __FILEPARSERCSV_H__ */