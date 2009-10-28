//
// This file is part of Condor2Nav file formats translator.
//
// Copyright (C) 2009 Mateusz Pusz
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
 * @file condor2nav.cpp
 *
 * @brief Implements the condor2nav::CCondor2Nav class. 
**/

#include "condor2nav.h"
#include "condor.h"
#include "xcsoar.h"


const char *condor2nav::CCondor2Nav::CONFIG_FILE_NAME = "condor2nav.ini";
const char *condor2nav::CCondor2Nav::DATA_PATH = "data";
const char *condor2nav::CCondor2Nav::SCENERIES_DATA_FILE_NAME = "SceneryData.csv";
const char *condor2nav::CCondor2Nav::GLIDERS_DATA_FILE_NAME = "GliderData.csv";



/**
 * @brief Class constructor. 
 *
 * condor2nav::CCondor2Nav class constructor that parses application configuration
 * INI file.
 *
 * @param argc   Number of command-line arguments. 
 * @param argv   Array of command-line argument strings. 
**/
condor2nav::CCondor2Nav::CCondor2Nav(int argc, const char *argv[]):
_configParser(CONFIG_FILE_NAME),
_taskName((argc > 1) ? argv[1] : _configParser.Value("Condor", "DefaultTaskName"))
{
}


/**
 * @brief Creates Condor data translator. 
 *
 * Method creates Condor data translator.
 *
 * @note For now only XCSoar is supported but later on that method will return
 *       data translator required by configuration INI file.
 *
 * @param outputPath The path of the translation output directory.
 *
 * @return Condor data translator.
**/
std::auto_ptr<condor2nav::CTranslator> condor2nav::CCondor2Nav::Translator(const std::string &outputPath) const
{
  return std::auto_ptr<condor2nav::CTranslator>(new CTranslatorXCSoar(outputPath, _configParser.Value("XCSoar", "DataPath")));
}


/**
 * @brief Runs translation.
 *
 * Method is responsible for Condor data translation. Several
 * translate actions are configured through configuration INI file.
 * 
 * @return Application execution result.
**/
int condor2nav::CCondor2Nav::Run()
{
  // create Condor data
  condor2nav::CCondor condor(_configParser.Value("Condor", "Path"), _taskName);

  // create translator
  std::auto_ptr<condor2nav::CTranslator> translator(Translator(_configParser.Value("Condor2Nav", "OutputPath")));

  // translate scenery data
  {
    const CFileParserCSV sceneriesParser(DATA_PATH + std::string("\\") + SCENERIES_DATA_FILE_NAME);
    const CFileParserCSV::CStringArray &sceneryData = sceneriesParser.Row(condor.TaskParser().Value("Task", "Landscape"));

    if(_configParser.Value("Condor2Nav", "SetSceneryMap") == "1")
      translator->SceneryMap(sceneryData);
    if(_configParser.Value("Condor2Nav", "SetSceneryTime") == "1")
      translator->SceneryTime(sceneryData);
  }
  
  // translate glider data
  if(_configParser.Value("Condor2Nav", "SetGlider") == "1")
  {
    const CFileParserCSV glidersParser(DATA_PATH + std::string("\\") + GLIDERS_DATA_FILE_NAME);
    translator->Glider(glidersParser.Row(condor.TaskParser().Value("Plane", "Name")));
  }

  // translate task
  if(_configParser.Value("Condor2Nav", "SetTask") == "1")
    translator->Task(condor.TaskParser(), condor.CoordConverter());

  // translate airspaces
  if(_configParser.Value("Condor2Nav", "SetAirspaces") == "1")
    translator->Airspaces(condor.TaskParser(), condor.CoordConverter());

  // translate weather
  if(_configParser.Value("Condor2Nav", "SetWeather") == "1")
    translator->Weather(condor.TaskParser());
  
  return EXIT_SUCCESS;
}
