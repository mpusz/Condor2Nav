#ifndef __CONDOR_H__
#define __CONDOR_H__

#include "fileParserINI.h"
#include <windows.h> 

namespace condor2nav {

  /**
   * @brief Condor data class. 
   *
   * condor2nav::CCondor is responsible for storing and handling of Condor
   * (The Soaring Competition Simulator) data. It also provides some
   * tools to interpret that data.
  **/
  class CCondor {
  public:
    /**
     * @brief The types of Condor sectors.
    **/
    enum TSectorType {
      SECTOR_CLASSIC,	    ///< @brief Line, FAI Sector or Circle.
      SECTOR_WINDOW	      ///< @brief Window. 
    };

    /**
     * @brief Condor map coordinates converter.
     *
     * condor2nav::CCondor::CCoordConverter is responsible for
     * Condor map coordinates convertions. It uses NaviCon.dll library
     * provided with every Condor release.
    **/
    class CCoordConverter {
      typedef int (WINAPI *FNaviConInit)(const char *trnFile);  ///< @brief NaviCon.dll interface
      typedef float (WINAPI *FXYToLon)(float X, float Y);       ///< @brief NaviCon.dll interface
      typedef float (WINAPI *FXYToLat)(float X, float Y);       ///< @brief NaviCon.dll interface
      typedef float (WINAPI *FGetMaxX)();                       ///< @brief NaviCon.dll interface
      typedef float (WINAPI *FGetMaxY)();                       ///< @brief NaviCon.dll interface

      /**
       * @brief NaviCon.dll interface.
      **/
      struct TDLLIface {
        FNaviConInit naviConInit;
        FGetMaxX getMaxX;
        FGetMaxY getMaxY;
        FXYToLon xyToLon;
        FXYToLat xyToLat;
      };
      HINSTANCE _hInstLib;	  ///< @brief DLL instance. 
      TDLLIface _iface;	      ///< @brief DLL interface.

      std::string DDFFToDDMMFF(float value, bool longitude) const;

    public:
      CCoordConverter(const std::string &condorPath, const std::string &trnName);
      ~CCoordConverter();
      std::string Longitude(const std::string &x, const std::string &y) const;
      std::string Latitude(const std::string &x, const std::string &y) const;
    };

  private:
    static const unsigned CONDOR_VERSION_SUPPORTED = 1120;	  ///< @brief Supported Condor version. 

    const CFileParserINI _taskParser;	             ///< @brief Condor task file parser. 
    const CCoordConverter _coordConverter;	       ///< @brief Condor map coordinates converter. 

  public:
    CCondor(const std::string &condorPath, const std::string &taskName);
    const CFileParserINI &TaskParser() const { return _taskParser; }
    const CCoordConverter &CoordConverter() const { return _coordConverter; }
  };

}

#endif /* __CONDOR_H__ */