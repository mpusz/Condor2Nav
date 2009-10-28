#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include "condor.h"
#include "fileParserCSV.h"

namespace condor2nav {

  class CTranslator {
    const std::string _outputPath;

    CTranslator(const CTranslator &);                     // disallowed
    const CTranslator &operator=(const CTranslator &);    // disallowed

  protected:
    const std::string &OutputPath() const;

  public:
    explicit CTranslator(const std::string &outputPath);
    virtual ~CTranslator();

    virtual void SceneryMap(const CFileParserCSV::CStringArray &sceneryData) = 0;
    virtual void SceneryTime(const CFileParserCSV::CStringArray &sceneryData) = 0;
    virtual void Glider(const CFileParserCSV::CStringArray &gliderData) = 0;
    virtual void Task(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv) = 0;
    virtual void Airspaces(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv) = 0;
    virtual void Weather(const CFileParserINI &taskParser) = 0;
  };

}

#endif /* __TRANSLATOR_H__ */