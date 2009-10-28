#ifndef __FILEPARSER_H__
#define __FILEPARSER_H__

#include <vector>

namespace condor2nav {

  /**
   * @brief File parsers abstract base class.
   *
   * condor2nav::CFileParser is an abstract base class for all the file parsers.
   * Each parser is not only responsible for parsing and providing the data but
   * it should also be able to dump all its data to the new file of the same
   * format. That class provides also some common parsing tools that may be reused
   * in child classes.
  **/
  class CFileParser {
  public:
    typedef std::vector<std::string> CStringArray;	///< @brief The array of strings.

  private:
    const std::string _filePath;	                  ///< @brief Input file path.

    CFileParser(const CFileParser &);               ///< @brief Disallowed
    CFileParser &operator=(const CFileParser &);    ///< @brief Disallowed

  protected:
    std::string &WhiteSpacesRemove(std::string &str) const;
    void LineParseKeyValue(const std::string &line, std::string &key, std::string &value) const;
    void LineParseCSV(const std::string &line, CStringArray &values) const;

  public:
    explicit CFileParser(const std::string &filePath);
    virtual ~CFileParser();

    /**
     * @brief Dumps class data to the file.
     *
     * Method dumps class data to the file in the same format as input file has.
     * 
     * @param filePath Path of the file to create (empty means overwrite input file).
    **/
    virtual void Dump(const std::string &filePath = "") const = 0;

    const std::string &Path() const;
  };

}

#endif /* __FILEPARSER_H__ */