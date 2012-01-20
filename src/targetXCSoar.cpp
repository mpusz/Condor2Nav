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
 * @file targetXCSoar.cpp
 *
 * @brief Implements the condor2nav::CTargetXCSoar class. 
 */

#include "targetXCSoar.h"
#include "imports/xcsoarTypes.h"
#include "ostream.h"
#include <array>


const boost::filesystem::path condor2nav::CTargetXCSoar::XCSOAR_PROFILE_NAME = "xcsoar-registry.prf";


/**
 * @brief Class constructor.
 *
 * condor2nav::CTargetXCSoar class constructor.
 *
 * @param translator Configuration INI file parser.
 */
condor2nav::CTargetXCSoar::CTargetXCSoar(const CTranslator &translator):
CTargetXCSoarCommon(translator),
_outputXCSoarDataPath(OutputPath() / "XCSoarData")
{
  boost::filesystem::path subDir = ConfigParser().Value("XCSoar", "Condor2NavDataSubDir");
  _outputCondor2NavDataPath = _outputXCSoarDataPath / subDir;
  _condor2navDataPathString = ConfigParser().Value("XCSoar", "XCSoarDataPath") + "\\" + subDir.string();

  DirectoryCreate(_outputCondor2NavDataPath);

  if(Convert<unsigned>(ConfigParser().Value("XCSoar", "DefaultTaskOverwrite")))
    _outputTaskFilePath = _outputXCSoarDataPath / DEFAULT_TASK_FILE_NAME;
  else
    _outputTaskFilePath = _outputCondor2NavDataPath / TASK_FILE_NAME;

  auto profilePath = _outputCondor2NavDataPath / OUTPUT_PROFILE_NAME;
  if(!FileExists(profilePath)) {
    profilePath = _outputXCSoarDataPath / XCSOAR_PROFILE_NAME;
    if(!FileExists(profilePath)) {
      profilePath = CTranslator::DATA_PATH / XCSOAR_PROFILE_NAME;
      if(!FileExists(profilePath))
        throw EOperationFailed("ERROR: Please copy '" + XCSOAR_PROFILE_NAME.string() + "' file to '" + CTranslator::DATA_PATH.string() + "' directory.");
    }
  }
  _profileParser = std::unique_ptr<CFileParserINI>(new CFileParserINI(profilePath));
}


/**
 * @brief Class destructor.
 *
 * condor2nav::CTargetXCSoar class destructor.
 */
condor2nav::CTargetXCSoar::~CTargetXCSoar()
{
  _profileParser->Dump(_outputCondor2NavDataPath / OUTPUT_PROFILE_NAME);
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
void condor2nav::CTargetXCSoar::TaskDump(CFileParserINI &profileParser,
                                         const CFileParserINI &taskParser,
                                         const boost::filesystem::path &outputTaskFilePath,
                                         const xcsoar::SETTINGS_TASK &settingsTask,
                                         const xcsoar::TASK_POINT taskPointArray[],
                                         const xcsoar::START_POINT startPointArray[],
                                         const CWaypointArray &waypointArray) const
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

  std::array<WAYPOINT, MAXTASKPOINTS> taskWaypointArray;
  std::array<WAYPOINT, MAXSTARTPOINTS> startWaypointArray;
  memset(taskWaypointArray.data(), 0, taskWaypointArray.size() * sizeof(taskWaypointArray[0]));
  memset(startWaypointArray.data(), 0, startWaypointArray.size() * sizeof(startWaypointArray[0]));

  for(size_t i=0; i<waypointArray.size(); i++) {
    taskWaypointArray[i].Number = waypointArray[i].number;
    taskWaypointArray[i].Latitude = waypointArray[i].latitude;
    taskWaypointArray[i].Longitude = waypointArray[i].longitude;
    taskWaypointArray[i].Altitude = waypointArray[i].altitude;
    taskWaypointArray[i].Flags = waypointArray[i].flags;
    mbstowcs(taskWaypointArray[i].Name, waypointArray[i].name.c_str(), NAME_SIZE);
    mbstowcs(taskWaypointArray[i].Comment, waypointArray[i].comment.c_str(), COMMENT_SIZE);
    taskWaypointArray[i].InTask = true;
  }
  
  tskFile.Write(reinterpret_cast<const char *>(taskWaypointArray.data()), taskWaypointArray.size() * sizeof(taskWaypointArray[0]));
  tskFile.Write(reinterpret_cast<const char *>(startWaypointArray.data()), startWaypointArray.size() * sizeof(startWaypointArray[0]));
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
  try {
    _profileParser->Value("", "Port2Index", _profileParser->Value("", "PortIndex"));
    _profileParser->Value("", "Speed2Index", _profileParser->Value("", "SpeedIndex"));
  }
  catch(const Exception &) {
    Translator().App().Warning() << "WARNING: COM port for Condor communication probably not set. Please verify that in " << Name() << " System Setup." << std::endl;
  }
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
  SceneryMapProcess(*_profileParser, sceneryData, _condor2navDataPathString);
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
  // set WinPilot Polar
  _profileParser->Value("", "Polar", "6");
  _profileParser->Value("", "PolarFile", "\"" + _condor2navDataPathString + "\\" + POLAR_FILE_NAME.string() + "\"");

  _profileParser->Value("", "AircraftType", "\"" + gliderData.at(GLIDER_NAME) + "\"");
  _profileParser->Value("", "SafteySpeed", Convert(KmH2MS(Convert<unsigned>(gliderData.at(GLIDER_SPEED_MAX)))));
  _profileParser->Value("", "Handicap", gliderData.at(GLIDER_DAEC_INDEX));
  const std::string &waterBallastEmptyTime = gliderData.at(GLIDER_WATER_BALLAST_EMPTY_TIME);
  _profileParser->Value("", "BallastSecsToEmpty", waterBallastEmptyTime == "0" ? "10" : waterBallastEmptyTime);

  // create polar file
  auto polarFileName = _outputCondor2NavDataPath / POLAR_FILE_NAME;
  COStream polarFile(polarFileName);

  polarFile << "***************************************************************************************************" << std::endl;
  polarFile << "* " << gliderData.at(GLIDER_NAME) << " WinPilot POLAR file generated with Condor2Nav" << std::endl;
  polarFile << "*" << std::endl;
  polarFile << "* MassDryGross[kg], MaxWaterBallast[liters], Speed1[km/h], Sink1[m/s], Speed2, Sink2, Speed3, Sink3" << std::endl;
  polarFile << "***************************************************************************************************" << std::endl;
  for(size_t i=GLIDER_MASS_DRY_GROSS; i<=GLIDER_SINK_3; i++) {
    if(i > GLIDER_MASS_DRY_GROSS)
      polarFile << ",";
    polarFile << gliderData.at(i);
  }
  polarFile << std::endl;

  unsigned ballast(Convert<unsigned>(Condor().TaskParser().Value("Plane", "Water")));
  unsigned maxBallast(Convert<unsigned>(gliderData.at(GLIDER_MAX_WATER_BALLAST)));
  if(maxBallast > 0 && ballast > 0) {
    unsigned xcsoarPercent = ballast * 100 / maxBallast;
    // round it to 5% increment steps
    xcsoarPercent = static_cast<unsigned>((static_cast<float>(xcsoarPercent) + 2.5) / 5) * 5;
    Translator().App().Warning() << "WARNING: Cannot set initial glider ballast in " << Name() << " automatically. Please open 'Config'->'Setup Basic' and set '" << xcsoarPercent << "%' for the glider ballast." << std::endl;
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
  PenaltyZonesProcess(*_profileParser, taskParser, coordConv, _condor2navDataPathString, _outputCondor2NavDataPath);
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
  unsigned dir = static_cast<unsigned>(Convert<float>(taskParser.Value("Weather", "WindDir")) + 0.5);
  unsigned speed = static_cast<unsigned>(Convert<float>(taskParser.Value("Weather", "WindSpeed")) + 0.5);
  _profileParser->Value("", "WindBearing", Convert(dir));
  _profileParser->Value("", "WindSpeed", Convert(speed));
}
