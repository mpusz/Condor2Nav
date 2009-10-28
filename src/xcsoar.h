#ifndef __XCSOAR_H__
#define __XCSOAR_H__

#include "translator.h"
#include "fileParserCSV.h"
#include "fileParserINI.h"
#include <string>


namespace condor2nav {

  class CTranslatorXCSoar : public CTranslator {
    // inputs
    static const char *XCSOAR_PROFILE_NAME;

    // outputs
    static const char *OUTPUT_PROFILE_NAME;
    static const char *WP_FILE_NAME;
    static const char *POLAR_FILE_NAME;

    const std::string _xcsoarDataPath;
    CFileParserINI _profileParser;
    
  public:
    explicit CTranslatorXCSoar(const std::string &outputPath, const std::string &xcsoarDataPath);
    virtual ~CTranslatorXCSoar();

    virtual void SceneryMap(const CFileParserCSV::CStringArray &sceneryData);
    virtual void SceneryTime(const CFileParserCSV::CStringArray &sceneryData);
    virtual void Glider(const CFileParserCSV::CStringArray &gliderData);
    virtual void Task(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv);
    virtual void Airspaces(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv);
    virtual void Weather(const CFileParserINI &taskParser);
  };

}

#endif /* __XCSOAR_H__ */