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
 * @file translator.cpp
 *
 * @brief Implements the translators hierarchy base class (condor2nav::CTranslator). 
**/

#include "translator.h"
#include "condor.h"
#include "targetXCSoar.h"
#include "targetLK8000.h"
#include <iostream>


const char *condor2nav::CTranslator::CONFIG_FILE_NAME = "condor2nav.ini";
const char *condor2nav::CTranslator::DATA_PATH = "data";
const char *condor2nav::CTranslator::SCENERIES_DATA_FILE_NAME = "SceneryData.csv";
const char *condor2nav::CTranslator::GLIDERS_DATA_FILE_NAME = "GliderData.csv";



/* ************************* T R A N S L A T O R   -   T A R G E T ************************** */


/**
 * @brief Class constructor.
 *
 * condor2nav::CTranslator::CTarget class constructor.
 *
 * @param translator Translator class.
**/
condor2nav::CTranslator::CTarget::CTarget(const CTranslator &translator):
_translator(translator),
_outputPath(_translator._configParser.Value("Condor2Nav", "OutputPath"))
{
  DirectoryCreate(_outputPath);
}


/**
 * @brief Class destructor.
 *
 * condor2nav::CTranslator::CTarget class destructor.
**/
condor2nav::CTranslator::CTarget::~CTarget()
{
}


/**
 * @brief Returns translator class.
 *
 * Method returns translator class. Should be used by the translation target
 * if directly provided information is not enough for the translation.
 *
 * @return Translator class. 
**/
const condor2nav::CTranslator &condor2nav::CTranslator::CTarget::Translator() const
{
  return _translator;
}


/**
 * @brief Returns configuration INI file parser.
 *
 * Method returns configuration INI file parser. Should be used by the
 * translation targets if directly provided information is not enough
 * for the translation.
 *
 * @return Configuration INI file parser. 
**/
const condor2nav::CFileParserINI &condor2nav::CTranslator::CTarget::ConfigParser() const
{
  return _translator._configParser;
}


/**
 * @brief Returns Condor data.
 *
 * Method returns Condor data. Should be used by the translation targets 
 * if directly provided information is not enough for the translation.
 *
 * @return Condor data. 
**/
const condor2nav::CCondor &condor2nav::CTranslator::CTarget::Condor() const
{
  return _translator._condor;
}


/**
 * @brief Returns translation output directory.
 *
 * Method returns translation output directory.
 *
 * @return Translation output directory. 
**/
const std::string &condor2nav::CTranslator::CTarget::OutputPath() const
{
  return _outputPath;
}





/* ********************************** T R A N S L A T O R *********************************** */

/**
 * @brief Class constructor.
 *
 * condor2nav::CTranslator class constructor.
 *
 * @param condorPath The path to Condor
 * @param cliTaskName Condor task name provided in application Command Line
 *                   ("" if nothing provided - default should be used).
 * @param aatTime    Minimum time for AAT task
**/
condor2nav::CTranslator::CTranslator(const std::string &condorPath, const std::string &cliTaskName, unsigned aatTime):
_configParser(CONFIG_FILE_NAME),
_condor(condorPath, _configParser.Value("Condor", "DefaultTaskName"), cliTaskName),
_aatTime(aatTime)
{
}


/**
 * @brief Creates Condor data translator target. 
 *
 * Method creates Condor data translator target.
 *
 * @return Condor data translator target.
**/
std::auto_ptr<condor2nav::CTranslator::CTarget> condor2nav::CTranslator::Target() const
{
  std::string target = _configParser.Value("Condor2Nav", "Target");
  if(target == "XCSoar")
    return std::auto_ptr<CTarget>(new CTargetXCSoar(*this));
  else if(target == "LK8000")
    return std::auto_ptr<CTarget>(new CTargetLK8000(*this));
  else
    throw std::out_of_range("ERROR: Unknown translation target '" + target + "'!!!");
}


/**
 * @brief Runs translation.
 *
 * Method is responsible for Condor data translation. Several
 * translate actions are configured through configuration INI file.
**/
void condor2nav::CTranslator::Run()
{
  // create translation target
  std::auto_ptr<CTarget> target(Target());
  
  {
    const CFileParserCSV sceneriesParser(DATA_PATH + std::string("\\") + SCENERIES_DATA_FILE_NAME);
    const CFileParserCSV::CStringArray &sceneryData = sceneriesParser.Row(_condor.TaskParser().Value("Task", "Landscape"));

    // translate scenery data
    if(_configParser.Value("Condor2Nav", "SetSceneryMap") == "1") {
      std::cout << "Setting scenery map data..." << std::endl;
      target->SceneryMap(sceneryData);
    }

    if(_configParser.Value("Condor2Nav", "SetSceneryTime") == "1") {
      std::cout << "Setting scenery time..." << std::endl;
      target->SceneryTime();
    }
  
    // translate task
    if(_configParser.Value("Condor2Nav", "SetTask") == "1") {
      std::cout << "Setting task data..." << std::endl;
      target->Task(_condor.TaskParser(), _condor.CoordConverter(), sceneryData, _aatTime);
    }
  }
  
  // translate glider data
  if(_configParser.Value("Condor2Nav", "SetGlider") == "1") {
    std::cout << "Setting glider data..." << std::endl;
    const CFileParserCSV glidersParser(DATA_PATH + std::string("\\") + GLIDERS_DATA_FILE_NAME);
    target->Glider(glidersParser.Row(_condor.TaskParser().Value("Plane", "Name")));
  }

  // translate penalty zones
  if(_configParser.Value("Condor2Nav", "SetPenaltyZones") == "1") {
    std::cout << "Setting penalty zones..." << std::endl;
    target->PenaltyZones(_condor.TaskParser(), _condor.CoordConverter());
  }

  // translate weather
  if(_configParser.Value("Condor2Nav", "SetWeather") == "1") {
    std::cout << "Setting wheater data..." << std::endl;
    target->Weather(_condor.TaskParser());
  }
}
