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
 * @file translator.h
 *
 * @brief Declares the translators hierarchy base class (condor2nav::CTranslator). 
 */

#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include "condor.h"
#include "fileParserINI.h"
#include "fileParserCSV.h"


namespace condor2nav {

  class CCondor2Nav;

  /**
   * @brief Translator class.
   *
   * condor2nav::CTranslator is a class responsible for translation management.
   */
  class CTranslator : CNonCopyable {
  public:

    /**
     * @brief Translation targets hierarchy base class.
     *
     * condor2nav::CTranslator::CTarget is a base abstract class for all translation targets.
     */
    class CTarget : CNonCopyable {
      const CTranslator &_translator;               ///< @brief Translator class
      const boost::filesystem::path _outputPath;    ///< @brief Translation output directory

    protected:
      /**
       * @brief Sceneries data CSV file column names.
       */
      enum TSceneriesDataColumns {
        SCENERY_NAME,
        SCENERY_XCM_FILE
      };

      /**
       * @brief Gliders data CSV file column names.
       */
      enum TGlidersDataColumns {
        GLIDER_NAME,
        GLIDER_SPEED_MAX,
        GLIDER_DAEC_INDEX,
        GLIDER_WATER_BALLAST_EMPTY_TIME,
        GLIDER_WING_AREA,
        GLIDER_MASS_DRY_GROSS,
        GLIDER_MAX_WATER_BALLAST,
        GLIDER_SPPED_1,
        GLIDER_SINK_1,
        GLIDER_SPPED_2,
        GLIDER_SINK_2,
        GLIDER_SPPED_3,
        GLIDER_SINK_3,
      };

      const CTranslator &Translator() const;
      const CFileParserINI &ConfigParser() const;
      const CCondor &Condor() const;
      const boost::filesystem::path &OutputPath() const;

    public:
      explicit CTarget(const CTranslator &translator);
      virtual ~CTarget();

      /**
       * @brief Returns target name.
       *
       * Method returns target name.
       */
      virtual const char *Name() const = 0;

      /**
       * @brief Sets Condor GPS data.
       *
       * Method sets Condor GPS data. 
       */
      virtual void Gps() = 0;

      /**
       * @brief Sets scenery map. 
       *
       * Method sets scenery map data. 
       *
       * @param sceneryData Information describing the scenery. 
       */
      virtual void SceneryMap(const CFileParserCSV::CStringArray &sceneryData) = 0;

      /**
       * @brief Sets time for scenery time zone. 
       *
       * Method sets time for scenery time zone.
       */
      virtual void SceneryTime() = 0;

      /**
       * @brief Set glider data. 
       *
       * Method sets all the data related to the glider.
       *
       * @param gliderData Information describing the glider. 
       */
      virtual void Glider(const CFileParserCSV::CStringArray &gliderData) = 0;

      /**
       * @brief Sets task information. 
       *
       * Method sets task information.
       *
       * @param taskParser  Condor task parser. 
       * @param coordConv   Condor coordinates converter.
       * @param sceneryData Information describing the scenery.
       * @param aatTime     Minimum time for AAT task
       */
      virtual void Task(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv, const CFileParserCSV::CStringArray &sceneryData, unsigned aatTime) = 0;

      /**
       * @brief Sets task penalty zones. 
       *
       * Method sets penalty zones used in the task.
       *
       * @param taskParser Condor task parser. 
       * @param coordConv  Condor coordinates converter.
       */
      virtual void PenaltyZones(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv) = 0;

      /**
       * @brief Sets weather data. 
       *
       * Method sets task weather data (e.g wind).
       *
       * @param taskParser Condor task parser. 
       */
      virtual void Weather(const CFileParserINI &taskParser) = 0;
    };

  private:
    const CCondor2Nav &_app;
    CTranslator(const CTranslator &);                     ///< @brief Disallowed
    const CTranslator &operator=(const CTranslator &);    ///< @brief Disallowed
    const CCondor &_condor;                               ///< @brief Condor data.
    const unsigned _aatTime;                              ///< @brief Minimum time for AAT task

    std::unique_ptr<CTarget> Target() const;

  public:
    // inputs
    static const boost::filesystem::path DATA_PATH;                 ///< @brief Application data directory path. 
    static const boost::filesystem::path SCENERIES_DATA_FILE_NAME;  ///< @brief Sceneries data CSV file name. 
    static const boost::filesystem::path GLIDERS_DATA_FILE_NAME;    ///< @brief Gliders data CSV file name.

    CTranslator(const CCondor2Nav &app, const CFileParserINI &configParser, const CCondor &condor, unsigned aatTime);
    void Run();
    const CCondor2Nav &App() const { return _app; }
  };

}

#endif /* __TRANSLATOR_H__ */
