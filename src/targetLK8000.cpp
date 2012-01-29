//
// This file is part of Condor2Nav file formats translator.
//
// Copyright (C) 2009-2012 Mateusz Pusz
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
 * @file targetLK8000.cpp
 *
 * @brief Implements the condor2nav::CTargetLK8000 class. 
 */

#include "targetLK8000.h"
#include "imports/lk8000Types.h"
#include "ostream.h"
#include <array>


const boost::filesystem::path condor2nav::CTargetLK8000::AIRSPACES_SUBDIR    = "_Airspaces";
const boost::filesystem::path condor2nav::CTargetLK8000::CONFIG_SUBDIR       = "_Configuration";
const boost::filesystem::path condor2nav::CTargetLK8000::MAPS_SUBDIR         = "_Maps";
const boost::filesystem::path condor2nav::CTargetLK8000::POLARS_SUBDIR       = "_Polars";
const boost::filesystem::path condor2nav::CTargetLK8000::TASKS_SUBDIR        = "_Tasks";
const boost::filesystem::path condor2nav::CTargetLK8000::WAYPOINTS_SUBDIR    = "_Waypoints";

const boost::filesystem::path condor2nav::CTargetLK8000::DEFAULT_SYSTEM_PROFILE_NAME   = "DEFAULT_PROFILE.prf";
const boost::filesystem::path condor2nav::CTargetLK8000::DEFAULT_AIRCRAFT_PROFILE_NAME = "DEFAULT_AIRCRAFT.acf";

const boost::filesystem::path condor2nav::CTargetLK8000::OUTPUT_AIRCRAFT_PROFILE_NAME  = "Condor.acf";


/**
 * @brief Class constructor.
 *
 * condor2nav::CTargetLK8000 class constructor.
 *
 * @param translator Configuration INI file parser.
 */
condor2nav::CTargetLK8000::CTargetLK8000(const CTranslator &translator):
CTargetXCSoarCommon(translator),
_outputLK8000DataPath(OutputPath() / "LK8000")
{
  boost::filesystem::path subDir = "condor2nav";
  _condor2navDataPathString = ConfigParser().Value("LK8000", "LK8000Path");

  // prepare directory names
  _outputAirspacesSubDir = AIRSPACES_SUBDIR / subDir;
  _outputMapsSubDir      = MAPS_SUBDIR / subDir;
  _outputPolarsSubDir    = POLARS_SUBDIR / subDir;
  _outputWaypointsSubDir = WAYPOINTS_SUBDIR / subDir;

  // create directories
  DirectoryCreate(_outputLK8000DataPath / _outputAirspacesSubDir);
  DirectoryCreate(_outputLK8000DataPath / _outputMapsSubDir);
  DirectoryCreate(_outputLK8000DataPath / _outputPolarsSubDir);
  DirectoryCreate(_outputLK8000DataPath / _outputWaypointsSubDir);

  {
    auto outputTaskDir = _outputLK8000DataPath / TASKS_SUBDIR / subDir;
    DirectoryCreate(outputTaskDir);
    _outputTaskFilePathList.push_back(outputTaskDir / TASK_FILE_NAME);
  }
  if(Convert<unsigned>(ConfigParser().Value("LK8000", "DefaultTaskOverwrite"))) {
    auto outputTaskDir = _outputLK8000DataPath / TASKS_SUBDIR;
    _outputTaskFilePathList.push_back(outputTaskDir / DEFAULT_TASK_FILE_NAME);
  }

  {
    auto outputConfigDir = _outputLK8000DataPath / CONFIG_SUBDIR / subDir;
    DirectoryCreate(outputConfigDir);
    _outputSystemProfilePathList.push_back(outputConfigDir / OUTPUT_PROFILE_NAME);
    _outputAircraftProfilePathList.push_back(outputConfigDir / OUTPUT_AIRCRAFT_PROFILE_NAME);
  }
  if(Convert<unsigned>(ConfigParser().Value("LK8000", "DefaultProfilesOverwrite"))) {
    auto outputConfigDir = _outputLK8000DataPath / CONFIG_SUBDIR;
    DirectoryCreate(outputConfigDir);
    _outputSystemProfilePathList.push_back(outputConfigDir / DEFAULT_SYSTEM_PROFILE_NAME);
    _outputAircraftProfilePathList.push_back(outputConfigDir / DEFAULT_AIRCRAFT_PROFILE_NAME);
  }

  // init profile files parsers
  auto systemPath = _outputLK8000DataPath / CONFIG_SUBDIR / subDir / OUTPUT_PROFILE_NAME;
  if(!FileExists(systemPath)) {
    systemPath = _outputLK8000DataPath / CONFIG_SUBDIR / DEFAULT_SYSTEM_PROFILE_NAME;
    if(!FileExists(systemPath)) {
      systemPath = CTranslator::DATA_PATH / DEFAULT_SYSTEM_PROFILE_NAME;
      if(!FileExists(systemPath))
        throw EOperationFailed("ERROR: Please copy '" + DEFAULT_SYSTEM_PROFILE_NAME.string() + "' file to '" + CTranslator::DATA_PATH.string() + "' directory.");
    }
  }
  _systemParser.reset(new CFileParserINI(systemPath));

  auto aircraftPath = _outputLK8000DataPath / CONFIG_SUBDIR / subDir / OUTPUT_AIRCRAFT_PROFILE_NAME;
  if(!FileExists(aircraftPath)) {
    aircraftPath = _outputLK8000DataPath / CONFIG_SUBDIR / DEFAULT_AIRCRAFT_PROFILE_NAME;
    if(!FileExists(aircraftPath)) {
      aircraftPath = CTranslator::DATA_PATH / DEFAULT_AIRCRAFT_PROFILE_NAME;
      if(!FileExists(aircraftPath))
        throw EOperationFailed("ERROR: Please copy '" + DEFAULT_AIRCRAFT_PROFILE_NAME.string() + "' file to '" + CTranslator::DATA_PATH.string() + "' directory.");
    }
  }
  _aircraftParser.reset(new CFileParserINI(aircraftPath));
}


/**
 * @brief Class destructor.
 *
 * condor2nav::CTargetLK8000 class destructor.
 */
condor2nav::CTargetLK8000::~CTargetLK8000()
{
  std::for_each(_outputSystemProfilePathList.begin(), _outputSystemProfilePathList.end(),
    [this](const boost::filesystem::path &path){ _systemParser->Dump(path); });
  std::for_each(_outputAircraftProfilePathList.begin(), _outputAircraftProfilePathList.end(),
    [this](const boost::filesystem::path &path){ _aircraftParser->Dump(path); });
}


/**
 * @brief Dumps waypoints in LK8000 format. 
 *
 * Method dumps waypoints in LK8000 format.
 *
 * @param profileParser      LK8000 profile file parser.
 * @param taskParser         Condor task parser. 
 * @param outputTaskFilePath The output task file path
 * @param settingsTask       Task settings
 * @param taskPointArray     Task points array
 * @param startPointArray    Task start points array
 * @param waypointArray      The array of waypoints data.
 */
void condor2nav::CTargetLK8000::TaskDump(CFileParserINI &profileParser,
                                         const CFileParserINI &taskParser,
                                         const xcsoar::SETTINGS_TASK &settingsTask,
                                         const xcsoar::TASK_POINT taskPointArray[],
                                         const xcsoar::START_POINT startPointArray[],
                                         const CWaypointArray &waypointArray) const
{
  using namespace lk8000;
  std::string ver = "LK" + Convert(LK_TASK_VERSION) + Convert(lk8000::MAXTASKPOINTS) + Convert(lk8000::MAXSTARTPOINTS);
  std::array<char, 50> version = { 0 };
  std::copy(ver.begin(), ver.end(), version.begin());

  COStream tskFile(_outputTaskFilePathList);
  tskFile.Write(version.data(), version.size());

  tskFile.Write(reinterpret_cast<const char *>(taskPointArray), lk8000::MAXTASKPOINTS * sizeof(taskPointArray[0]));

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

  tskFile.Write(reinterpret_cast<const char *>(startPointArray), lk8000::MAXSTARTPOINTS * sizeof(startPointArray[0]));

  std::array<lk8000::WAYPOINT, lk8000::MAXTASKPOINTS> taskWaypointArray;
  std::array<lk8000::WAYPOINT, lk8000::MAXSTARTPOINTS> startWaypointArray;
  memset(taskWaypointArray.data(), 0, taskWaypointArray.size() * sizeof(taskWaypointArray[0]));
  memset(startWaypointArray.data(), 0, startWaypointArray.size() * sizeof(startWaypointArray[0]));

  for(size_t i=0; i<waypointArray.size(); i++) {
    taskWaypointArray[i].Number = waypointArray[i].number;
    taskWaypointArray[i].Latitude = waypointArray[i].latitude;
    taskWaypointArray[i].Longitude = waypointArray[i].longitude;
    taskWaypointArray[i].Altitude = waypointArray[i].altitude;
    taskWaypointArray[i].Flags = waypointArray[i].flags;
    mbstowcs(taskWaypointArray[i].Name, waypointArray[i].name.c_str(), lk8000::NAME_SIZE);
    mbstowcs(taskWaypointArray[i].Comment, waypointArray[i].comment.c_str(), lk8000::COMMENT_SIZE);
    taskWaypointArray[i].InTask = true;
    taskWaypointArray[i].Style = 1;
  }
  
  tskFile.Write(reinterpret_cast<const char *>(taskWaypointArray.data()), taskWaypointArray.size() * sizeof(taskWaypointArray[0]));
  tskFile.Write(reinterpret_cast<const char *>(startWaypointArray.data()), startWaypointArray.size() * sizeof(startWaypointArray[0]));
  
  profileParser.Value("", "StartMaxHeight", Convert(settingsTask.StartMaxHeight * 1000));
  profileParser.Value("", "StartMaxHeightMargin", "0");
  profileParser.Value("", "FinishMinHeight", Convert(settingsTask.FinishMinHeight * 1000));
}


/**
 * @brief Sets Condor GPS data.
 *
 * Method sets Condor GPS data. 
 */
void condor2nav::CTargetLK8000::Gps()
{
  _systemParser->Value("", "DeviceA", "\"Condor\"");

  // disable DeviceB
  _systemParser->Value("", "DeviceB", "\"\"");

  _systemParser->Value("", "UseGeoidSeparation", "0");
}


/**
* @brief Sets scenery map. 
*
* Method sets scenery map XCM data file according to the Condor landscape name. 
*
* @param sceneryData Information describing the scenery. 
 */
void condor2nav::CTargetLK8000::SceneryMap(const CFileParserCSV::CStringArray &sceneryData)
{
  _systemParser->Value("", "MapFile",     "\"" + _condor2navDataPathString + "\\" + (_outputMapsSubDir / sceneryData.at(SCENERY_MAP_FILE)).string() + "\"");
  _systemParser->Value("", "TerrainFile", "\"" + _condor2navDataPathString + "\\" + (_outputMapsSubDir / sceneryData.at(SCENERY_TERRAIN_FILE)).string() + "\"");
  _systemParser->Value("", "WPFile",      "\"" + _condor2navDataPathString + "\\" + (_outputWaypointsSubDir / sceneryData.at(SCENERY_WAYPOINTS_FILE)).string() + "\"");

  // reset landscape specific files in case they were set before profile import
  // if need user can still assign additionl data with second entries
  _systemParser->Value("", "AirfieldFile", "\"\"");
}


/**
* @brief Sets time for scenery time zone. 
*
* Method sets UTC time offset for selected scenery and forces time synchronization to the GPS source.
 */
void condor2nav::CTargetLK8000::SceneryTime()
{
  SceneryTimeProcess(*_systemParser);
}


/**
* @brief Set glider data. 
*
* Method created and sets glider polar file, handicap, safety speed, the time to empty the water
* ballast and glider name for the logger.
*
* @param gliderData Information describing the glider. 
 */
void condor2nav::CTargetLK8000::Glider(const CFileParserCSV::CStringArray &gliderData)
{
  _aircraftParser->Value("", "AircraftCategory1", "\"0\"");
  _aircraftParser->Value("", "PolarFile1", "\"" + _condor2navDataPathString + "\\" + (_outputPolarsSubDir / POLAR_FILE_NAME).string() + "\"");
  _aircraftParser->Value("", "SafteySpeed1", Convert(static_cast<unsigned>(Convert<unsigned>(gliderData.at(GLIDER_SPEED_MAX)) * 1000.0 / 3.6 + 0.5)));
  _aircraftParser->Value("", "Handicap1", gliderData.at(GLIDER_DAEC_INDEX));
  const std::string &waterBallastEmptyTime = gliderData.at(GLIDER_WATER_BALLAST_EMPTY_TIME);
  _aircraftParser->Value("", "BallastSecsToEmpty1", waterBallastEmptyTime == "0" ? "10" : waterBallastEmptyTime);
  _aircraftParser->Value("", "AircraftType1", "\"" + gliderData.at(GLIDER_NAME) + "\"");
  _aircraftParser->Value("", "AircraftRego1", "\"\"");
  _aircraftParser->Value("", "CompetitionClass1", "\"" + Condor().TaskParser().Value("Plane", "Class") + "\"");
  _aircraftParser->Value("", "CompetitionID1", "\"\"");

  // create polar file
  auto polarFileName = _outputLK8000DataPath / _outputPolarsSubDir / POLAR_FILE_NAME;
  COStream polarFile(polarFileName);

  polarFile << "*****************************************************************************************************************" << std::endl;
  polarFile << "* " << gliderData.at(GLIDER_NAME) << " LK8000 POLAR file generated with Condor2Nav" << std::endl;
  polarFile << "*" << std::endl;
  polarFile << "* MassDryGross[kg], MaxWaterBallast[liters], Speed1[km/h], Sink1[m/s], Speed2, Sink2, Speed3, Sink3, WingArea[m2]" << std::endl;
  polarFile << "*****************************************************************************************************************" << std::endl;
  for(size_t i=GLIDER_MASS_DRY_GROSS; i<=GLIDER_SINK_3; i++) {
    if(i > GLIDER_MASS_DRY_GROSS)
      polarFile << ",";
    polarFile << gliderData.at(i);
  }
  polarFile << "," << gliderData.at(GLIDER_WING_AREA) << std::endl;

  if(!gliderData.at(GLIDER_FLAPS).empty()) {
    // add flaps line
    polarFile << std::endl;
    polarFile << gliderData.at(GLIDER_FLAPS) << std::endl;
  }

  unsigned ballast(Convert<unsigned>(Condor().TaskParser().Value("Plane", "Water")));
  unsigned maxBallast(Convert<unsigned>(gliderData.at(GLIDER_MAX_WATER_BALLAST)));
  if(maxBallast > 0 && ballast > 0) {
    unsigned percent = ballast * 100 / maxBallast;
    // round it to 5% increment steps
    percent = static_cast<unsigned>((static_cast<float>(percent) + 2.5) / 5) * 5;
    Translator().App().Warning() << "WARNING: Cannot set initial glider ballast in " << Name() << " automatically. Please open 'Config'->'Setup Basic' and set '" << percent << "%' for the glider ballast." << std::endl;
  }
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
void condor2nav::CTargetLK8000::Task(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv, const CFileParserCSV::CStringArray &sceneryData, unsigned aatTime)
{
  unsigned wpFile(Convert<unsigned>(ConfigParser().Value("LK8000", "TaskWPFileGenerate")));
  TaskProcess(*_systemParser, taskParser, coordConv, aatTime,
              lk8000::MAXTASKPOINTS, lk8000::MAXSTARTPOINTS,
              wpFile > 0, _outputLK8000DataPath / _outputWaypointsSubDir);
}
 

/**
* @brief Sets task penalty zones. 
*
* Method sets penalty zones used in the task.
*
* @param taskParser Condor task parser. 
* @param coordConv  Condor coordinates converter.
 */
void condor2nav::CTargetLK8000::PenaltyZones(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv)
{
  PenaltyZonesProcess(*_systemParser, taskParser, coordConv, _condor2navDataPathString + "\\" + _outputAirspacesSubDir.string(), _outputLK8000DataPath / _outputAirspacesSubDir);
}


/**
* @brief Sets weather data. 
*
* Method sets the wind data.
*
* @param taskParser Condor task parser. 
 */
void condor2nav::CTargetLK8000::Weather(const CFileParserINI &taskParser)
{
  // nothing to do here
}
