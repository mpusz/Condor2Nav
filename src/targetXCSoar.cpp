//
// This file is part of Condor2Nav file formats translator.
//
// Copyright (C) 2009-2010 Mateusz Pusz
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
 * @file targetXCSoar.cpp
 *
 * @brief Implements the condor2nav::CTargetXCSoar class. 
 */

#include "targetXCSoar.h"
#include "imports/xcsoarTypes.h"
#include "ostream.h"


const char *condor2nav::CTargetXCSoar::XCSOAR_PROFILE_NAME    = "xcsoar-registry.prf";


/**
 * @brief Class constructor.
 *
 * condor2nav::CTargetXCSoar class constructor.
 *
 * @param translator Configuration INI file parser.
 */
condor2nav::CTargetXCSoar::CTargetXCSoar(const CTranslator &translator):
CTargetXCSoarCommon(translator),
_outputXCSoarDataPath(OutputPath() + "\\XCSoarData")
{
  std::string subDir = ConfigParser().Value("XCSoar", "Condor2NavDataSubDir");
  if(subDir != "")
    subDir = "\\" + subDir;
  _outputCondor2NavDataPath = _outputXCSoarDataPath + subDir;
  _condor2navDataPath = ConfigParser().Value("XCSoar", "XCSoarDataPath") + subDir;

  DirectoryCreate(_outputCondor2NavDataPath);

  if(Convert<unsigned>(ConfigParser().Value("XCSoar", "DefaultTaskOverwrite")))
    _outputTaskFilePath = _outputXCSoarDataPath + std::string("\\") + DEFAULT_TASK_FILE_NAME;
  else
    _outputTaskFilePath = _outputCondor2NavDataPath + std::string("\\") + TASK_FILE_NAME;

  std::string profilePath = _outputCondor2NavDataPath + std::string("\\") + OUTPUT_PROFILE_NAME;
  if(!FileExists(profilePath)) {
    profilePath = _outputXCSoarDataPath + std::string("\\") + XCSOAR_PROFILE_NAME;
    if(!FileExists(profilePath)) {
      profilePath = CTranslator::DATA_PATH + std::string("\\") + XCSOAR_PROFILE_NAME;
      if(!FileExists(profilePath))
        throw EOperationFailed("ERROR: Please copy '" + std::string(XCSOAR_PROFILE_NAME) + "' file to '" + std::string(CTranslator::DATA_PATH) + "' directory.");
    }
  }
  _profileParser = std::auto_ptr<CFileParserINI>(new CFileParserINI(profilePath));
}


/**
 * @brief Class destructor.
 *
 * condor2nav::CTargetXCSoar class destructor.
 */
condor2nav::CTargetXCSoar::~CTargetXCSoar()
{
  _profileParser->Dump(_outputCondor2NavDataPath + std::string("\\") + OUTPUT_PROFILE_NAME);
}


/**
 * @brief Dumps waypoints in XCSoar format. 
 *
 * Method dumps waypoints in XCSoar format.
 *
 * @param profileParser      XCSoar profile file parser.
 * @param taskParser         Condor task parser. 
 * @param outputTaskFilePath The output task file path
 * @param settingsTask       Task settings
 * @param taskPointArray     Task points array
 * @param startPointArray    Task start points array
 * @param waypointArray      The array of waypoints data.
 */
void condor2nav::CTargetXCSoar::TaskDump(CFileParserINI &profileParser, const CFileParserINI &taskParser, const std::string &outputTaskFilePath, const xcsoar::SETTINGS_TASK &settingsTask, const xcsoar::TASK_POINT *taskPointArray, const xcsoar::START_POINT *startPointArray, const CWaypointArray &waypointArray) const
{
  using namespace xcsoar;

  COStream tskFile(outputTaskFilePath);

  tskFile.Write(reinterpret_cast<const char *>(taskPointArray), MAXTASKPOINTS * sizeof(TASK_POINT));

  tskFile.Write(reinterpret_cast<const char *>(&settingsTask.AATEnabled), sizeof(settingsTask.AATEnabled));
  tskFile.Write(reinterpret_cast<const char *>(&settingsTask.AATTaskLength), sizeof(settingsTask.AATTaskLength));
  tskFile.Write(reinterpret_cast<const char *>(&settingsTask.FinishRadius), sizeof(settingsTask.FinishRadius));
  tskFile.Write(reinterpret_cast<const char *>(&settingsTask.FinishType), sizeof(settingsTask.FinishType));
  tskFile.Write(reinterpret_cast<const char *>(&settingsTask.StartRadius), sizeof(settingsTask.StartRadius));
  tskFile.Write(reinterpret_cast<const char *>(&settingsTask.StartType), sizeof(settingsTask.StartType));
  tskFile.Write(reinterpret_cast<const char *>(&settingsTask.SectorType), sizeof(settingsTask.SectorType));
  tskFile.Write(reinterpret_cast<const char *>(&settingsTask.SectorRadius), sizeof(settingsTask.SectorRadius));
  tskFile.Write(reinterpret_cast<const char *>(&settingsTask.AutoAdvance), sizeof(settingsTask.AutoAdvance));
  tskFile.Write(reinterpret_cast<const char *>(&settingsTask.EnableMultipleStartPoints), sizeof(settingsTask.EnableMultipleStartPoints));

  tskFile.Write(reinterpret_cast<const char *>(startPointArray), MAXSTARTPOINTS * sizeof(START_POINT));

  WAYPOINT *taskWaypointArray = new WAYPOINT[MAXTASKPOINTS];
  memset(taskWaypointArray, 0, MAXTASKPOINTS * sizeof(WAYPOINT));
  WAYPOINT *startWaypointArray = new WAYPOINT[MAXSTARTPOINTS];
  memset(startWaypointArray, 0, MAXSTARTPOINTS * sizeof(WAYPOINT));

  for(unsigned i=0; i<waypointArray.size(); i++) {
    taskWaypointArray[i].Number = waypointArray[i].number;
    taskWaypointArray[i].Latitude = waypointArray[i].latitude;
    taskWaypointArray[i].Longitude = waypointArray[i].longitude;
    taskWaypointArray[i].Altitude = waypointArray[i].altitude;
    taskWaypointArray[i].Flags = waypointArray[i].flags;
    mbstowcs(taskWaypointArray[i].Name, waypointArray[i].name.c_str(), NAME_SIZE);
    mbstowcs(taskWaypointArray[i].Comment, waypointArray[i].comment.c_str(), COMMENT_SIZE);
    taskWaypointArray[i].InTask = true;
  }
  
  tskFile.Write(reinterpret_cast<const char *>(taskWaypointArray), MAXTASKPOINTS * sizeof(WAYPOINT));
  tskFile.Write(reinterpret_cast<const char *>(startWaypointArray), MAXSTARTPOINTS * sizeof(WAYPOINT));
}



/**
 * @brief Sets Condor GPS data.
 *
 * Method sets Condor GPS data. 
 */
void condor2nav::CTargetXCSoar::Gps()
{
  _profileParser->Value("", "DeviceA", "\"Condor\"");
  
  // copy deviceA to deviceB
  _profileParser->Value("", "DeviceB", _profileParser->Value("", "DeviceA"));
  _profileParser->Value("", "Port2Index", _profileParser->Value("", "PortIndex"));
  _profileParser->Value("", "Speed2Index", _profileParser->Value("", "SpeedIndex"));
}


/**
* @brief Sets scenery map. 
*
* Method sets scenery map XCM data file according to the Condor landscape name. 
*
* @param sceneryData Information describing the scenery. 
 */
void condor2nav::CTargetXCSoar::SceneryMap(const CFileParserCSV::CStringArray &sceneryData)
{
  SceneryMapProcess(*_profileParser, sceneryData, _condor2navDataPath);
}


/**
* @brief Sets time for scenery time zone. 
*
* Method sets UTC time offset for selected scenery and forces time synchronization to the GPS source.
 */
void condor2nav::CTargetXCSoar::SceneryTime()
{
  SceneryTimeProcess(*_profileParser);
}


/**
* @brief Set glider data. 
*
* Method created and sets glider polar file, handicap, safety speed, the time to empty the water
* ballast and glider name for the logger.
*
* @param gliderData Information describing the glider. 
 */
void condor2nav::CTargetXCSoar::Glider(const CFileParserCSV::CStringArray &gliderData)
{
  GliderProcess(*_profileParser, gliderData, false, _condor2navDataPath, _outputCondor2NavDataPath);
}


/**
* @brief Sets task information. 
*
* Method sets task information.
*
* @param taskParser Condor task parser. 
* @param coordConv  Condor coordinates converter.
* @param sceneryData Information describing the scenery. 
* @param aatTime     Minimum time for AAT task
 */
void condor2nav::CTargetXCSoar::Task(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv, const CFileParserCSV::CStringArray &sceneryData, unsigned aatTime)
{
  unsigned wpFile(Convert<unsigned>(ConfigParser().Value("XCSoar", "TaskWPFileGenerate")));
  TaskProcess(*_profileParser, taskParser, coordConv, sceneryData, _outputTaskFilePath, aatTime,
              xcsoar::MAXTASKPOINTS, xcsoar::MAXSTARTPOINTS,
              wpFile > 0, _outputCondor2NavDataPath);
}
 

/**
* @brief Sets task penalty zones. 
*
* Method sets penalty zones used in the task.
*
* @param taskParser Condor task parser. 
* @param coordConv  Condor coordinates converter.
 */
void condor2nav::CTargetXCSoar::PenaltyZones(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv)
{
  PenaltyZonesProcess(*_profileParser, taskParser, coordConv, _condor2navDataPath, _outputCondor2NavDataPath);
}


/**
* @brief Sets weather data. 
*
* Method sets the wind data.
*
* @param taskParser Condor task parser. 
 */
void condor2nav::CTargetXCSoar::Weather(const CFileParserINI &taskParser)
{
  WeatherProcess(*_profileParser, taskParser);
}
