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
 * @file targetXCSoarCommon.cpp
 *
 * @brief Implements the condor2nav::CTargetXCSoarCommon class. 
**/

#include "targetXCSoarCommon.h"
#include "imports/xcsoarTypes.h"
#include "ostream.h"
#include <iostream>


const char *condor2nav::CTargetXCSoarCommon::OUTPUT_PROFILE_NAME    = "Condor.prf";
const char *condor2nav::CTargetXCSoarCommon::TASK_FILE_NAME         = "Condor.tsk";
const char *condor2nav::CTargetXCSoarCommon::DEFAULT_TASK_FILE_NAME = "Default.tsk";
const char *condor2nav::CTargetXCSoarCommon::POLAR_FILE_NAME        = "Polar_Condor.plr";
const char *condor2nav::CTargetXCSoarCommon::AIRSPACES_FILE_NAME    = "A_Condor.txt";
const char *condor2nav::CTargetXCSoarCommon::WP_FILE_NAME           = "WP_CondorTask.dat";

/**
 * @brief Class constructor.
 *
 * condor2nav::CTargetXCSoarCommon class constructor.
 *
 * @param translator Configuration INI file parser.
**/
condor2nav::CTargetXCSoarCommon::CTargetXCSoarCommon(const CTranslator &translator):
CTranslator::CTarget(translator)
{
}


/**
* @brief Sets scenery map. 
*
* Method sets scenery map XCM data file according to the Condor landscape name. 
*
* @param profileParser XCSoar profile file parser.
* @param sceneryData Information describing the scenery. 
* @param pathPrefix Scenery map subdirectory prefix (in XCSoar format).
**/
void condor2nav::CTargetXCSoarCommon::SceneryMapProcess(CFileParserINI &profileParser, const CFileParserCSV::CStringArray &sceneryData, const std::string &pathPrefix) const
{
  profileParser.Value("", "MapFile", "\"" + pathPrefix + "\\" + sceneryData.at(SCENERY_XCSOAR_FILE) + "\"");
}


/**
* @brief Sets time for scenery time zone. 
*
* Method sets UTC time offset for selected scenery and forces time synchronization to the GPS source.
*
* @param profileParser XCSoar profile file parser.
**/
void condor2nav::CTargetXCSoarCommon::SceneryTimeProcess(CFileParserINI &profileParser) const
{
  profileParser.Value("", "UTCOffset", "0");
}


/**
* @brief Set glider data. 
*
* Method created and sets glider polar file, handicap, safety speed, the time to empty the water
* ballast and glider name for the logger.
*
* @param profileParser XCSoar profile file parser.
* @param gliderData Information describing the glider. 
* @param pathPrefix Polar file subdirectory prefix (in XCSoar format).
* @param outputPathPrefix Polar file subdirectory prefix (in filesystem format).
**/
void condor2nav::CTargetXCSoarCommon::GliderProcess(CFileParserINI &profileParser, const CFileParserCSV::CStringArray &gliderData, const std::string &pathPrefix, const std::string &outputPathPrefix) const
{
  // set WinPilot Polar
  profileParser.Value("", "Polar", "6");
  profileParser.Value("", "PolarFile", "\"" + pathPrefix + std::string("\\") + POLAR_FILE_NAME + std::string("\""));

  profileParser.Value("", "AircraftType", "\"" + gliderData.at(GLIDER_NAME) + "\"");
  profileParser.Value("", "SafteySpeed", Convert(KmH2MS(Convert<unsigned>(gliderData.at(GLIDER_SPEED_MAX)))));
  profileParser.Value("", "Handicap", gliderData.at(GLIDER_DAEC_INDEX));
  const std::string &waterBallastEmptyTime = gliderData.at(GLIDER_WATER_BALLAST_EMPTY_TIME);
  profileParser.Value("", "BallastSecsToEmpty", waterBallastEmptyTime == "0" ? "10" : waterBallastEmptyTime);

  // create polar file
  std::string polarFileName = outputPathPrefix + std::string("\\") + POLAR_FILE_NAME;
  COStream polarFile(polarFileName);

  polarFile << "***************************************************************************************************" << std::endl;
  polarFile << "* " << gliderData.at(GLIDER_NAME) << " WinPilot POLAR file generated with Condor2Nav" << std::endl;
  polarFile << "*" << std::endl;
  polarFile << "* MassDryGross[kg], MaxWaterBallast[liters], Speed1[km/h], Sink1[m/s], Speed2, Sink2, Speed3, Sink3" << std::endl;
  polarFile << "***************************************************************************************************" << std::endl;
  for(unsigned i=GLIDER_MASS_DRY_GROSS; i<=GLIDER_SINK_3; i++) {
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
    std::cerr << "WARNING: Cannot set initial glider ballast in XCSoar automatically. Please open 'Config'->'Setup Basic' and set '" << xcsoarPercent << "%' for the glider ballast." << std::endl;
  }
}


/**
* @brief Sets task information. 
*
* Method sets task information.
*
* @param profileParser XCSoar profile file parser.
* @param taskParser Condor task parser. 
* @param coordConv  Condor coordinates converter.
* @param sceneryData Information describing the scenery. 
* @param outputTaskFilePath The path of output XCSoar task file.
* @param maxTaskPoints The number of waypoints stored in a task file
* @param maxStartPoints The number of alternate startpoints stored in a task file
* @param generateWPFile Flag specifying if WP file should be generated
* @param wpOutputPathPrefix XCSoar WP subdirectory prefix (in filesystem format).
**/
void condor2nav::CTargetXCSoarCommon::TaskProcess(CFileParserINI &profileParser, const CFileParserINI &taskParser,
                                                  const CCondor::CCoordConverter &coordConv,
                                                  const CFileParserCSV::CStringArray &sceneryData,
                                                  const std::string &outputTaskFilePath,
                                                  unsigned maxTaskPoints, unsigned maxStartPoints,
                                                  bool generateWPFile, const std::string &wpOutputPathPrefix) const
{
  using namespace xcsoar;

  std::string wpFileName = wpOutputPathPrefix + std::string("\\") + WP_FILE_NAME;

  std::auto_ptr<COStream> wpFile;
  if(generateWPFile)
    wpFile = std::auto_ptr<COStream>(new COStream(wpFileName));

  // set task settings
  SETTINGS_TASK settingsTask = { 0 };
  settingsTask.AATEnabled       = false;
  settingsTask.AATTaskLength    = 0;
  try {
    settingsTask.AutoAdvance      = static_cast<AutoAdvanceMode_t>(Convert<unsigned>(profileParser.Value("", "AutoAdvance")));
  }
  catch(const std::range_error &) {
    settingsTask.AutoAdvance    = AUTOADVANCE_AUTO;
  }
  settingsTask.EnableMultipleStartPoints = false;

  // reset data
  TASK_POINT *taskPointArray = new TASK_POINT[maxTaskPoints];
  memset(taskPointArray, 0, maxTaskPoints * sizeof(TASK_POINT));
  START_POINT *startPointArray = new START_POINT[maxStartPoints];
  memset(startPointArray, 0, maxStartPoints * sizeof(START_POINT));
  WAYPOINT *taskWaypointArray = new WAYPOINT[maxTaskPoints];
  memset(taskWaypointArray, 0, maxTaskPoints * sizeof(WAYPOINT));
  WAYPOINT *startWaypointArray = new WAYPOINT[maxStartPoints];
  memset(startWaypointArray, 0, maxStartPoints * sizeof(WAYPOINT));

  for(unsigned i=0; i<maxTaskPoints; i++)
    taskPointArray[i].Index = -1;
  for(unsigned i=0; i<maxTaskPoints; i++)
    startPointArray[i].Index = -1;

  bool tpsValid(true);
  unsigned tpNum = condor2nav::Convert<unsigned>(taskParser.Value("Task", "Count"));

  // check if enough waypoints to create a task
  if(tpNum - 1 > maxTaskPoints)
    throw std::range_error("ERROR: Too many waypoints (" + Convert(tpNum - 1) + ") in a task file (only " + Convert(maxTaskPoints) + " supported)!!!");

  std::auto_ptr<CFileParserCSV> waypointsParser;
  if(sceneryData.at(SCENERY_WAYPOINTS_FILE) != "")
    waypointsParser = std::auto_ptr<CFileParserCSV>(new CFileParserCSV("data\\Waypoints\\" + sceneryData.at(SCENERY_WAYPOINTS_FILE)));
  
  // skip takeoff waypoint
  for(unsigned i=1; i<tpNum; i++) {
    // dump WP file line
    std::string tpIdxStr(Convert(i));
    std::string name;
    if(i == 1)
      name = "START";
    else if(i == tpNum - 1)
      name = "FINISH";
    else
      name = "TP" + Convert(i - 1);

    std::string x(taskParser.Value("Task", "TPPosX" + tpIdxStr));
    std::string y(taskParser.Value("Task", "TPPosY" + tpIdxStr));

    double latitude = coordConv.Latitude(x, y);
    double longitude = coordConv.Longitude(x, y);
    std::string latitudeStr = DDFF2DDMMFF(latitude, false);
    std::string longitudeStr = DDFF2DDMMFF(longitude, true);
    unsigned waypointFlags = xcsoar::WAYPOINT_TURNPOINT;
    std::string waypointFlagsStr = "T";
    
    if(sceneryData.at(SCENERY_WAYPOINTS_FILE) != "") {
      // look for waypoint data in a scenery waypoints file
      const CFileParserCSV::CRowsList &waypoints = waypointsParser->Rows();
      for(CFileParserCSV::CRowsList::const_iterator it=waypoints.begin(); it!=waypoints.end(); ++it) {
        if((**it).at(1) == latitudeStr && (**it).at(2) == longitudeStr) {
          if((**it).at(4) == "AT") {
            // mark waypoint as airport
            waypointFlagsStr = "AT";
            waypointFlags |= xcsoar::WAYPOINT_AIRPORT;
          }
          break;
        }
      }
    }
    
    if(generateWPFile)
      *wpFile << i << "," << latitudeStr << "," << longitudeStr << ","
              << taskParser.Value("Task", "TPPosZ" + tpIdxStr) << "M," << waypointFlagsStr << "," << name << ","
              << taskParser.Value("Task", "TPName" + tpIdxStr) << std::endl;

    taskPointArray[i - 1].Index = taskWaypointArray[i - 1].Number = WAYPOINT_INDEX_OFFSET + i;
    taskWaypointArray[i - 1].Latitude = latitude;
    taskWaypointArray[i - 1].Longitude = longitude;
    taskWaypointArray[i - 1].Altitude = Convert<double>(taskParser.Value("Task", "TPPosZ" + tpIdxStr));
    taskWaypointArray[i - 1].Flags = waypointFlags;
    mbstowcs(taskWaypointArray[i - 1].Name, name.c_str(), NAME_SIZE);
    mbstowcs(taskWaypointArray[i - 1].Comment, taskParser.Value("Task", "TPName" + tpIdxStr).c_str(), COMMENT_SIZE);
    taskWaypointArray[i - 1].InTask = 1;

    // dump Task File data
    std::string sectorTypeStr(taskParser.Value("Task", "TPSectorType" + tpIdxStr));
    unsigned sectorType(Convert<unsigned>(sectorTypeStr));
    if(sectorType == CCondor::SECTOR_CLASSIC) {
      unsigned radius(condor2nav::Convert<unsigned>(taskParser.Value("Task", "TPRadius" + tpIdxStr)));
      unsigned angle(condor2nav::Convert<unsigned>(taskParser.Value("Task", "TPAngle" + tpIdxStr)));

      switch(angle) {
      case 90:
        if(i == 1)
          settingsTask.StartType = xcsoar::START_SECTOR;
        else if(i == tpNum - 1)
          settingsTask.FinishType = xcsoar::FINISH_SECTOR;
        else {
          if(i > 2 && settingsTask.SectorType != xcsoar::AST_FAI) {
            tpsValid = false;
            settingsTask.SectorRadius = radius;
          }
          settingsTask.SectorType = xcsoar::AST_FAI;
          if(i > 2 && settingsTask.SectorRadius != radius) {
            std::cerr << "WARNING: " << name << ": XCSoar does not support different TPs types. The smallest radius will be used for all FAI sectors. If you advance a sector in XCSoar you will advance it in Condor." << std::endl;
            settingsTask.SectorRadius = min(settingsTask.SectorRadius, radius);
          }
          else
            settingsTask.SectorRadius = radius;
        }
        break;

      case 180:
        if(i == 1)
          settingsTask.StartType = xcsoar::START_LINE;
        else if(i == tpNum - 1)
          settingsTask.FinishType = xcsoar::FINISH_LINE;
        else {
          if(i > 2 && settingsTask.SectorType == xcsoar::AST_FAI)
            tpsValid = false;
          else {
            std::cerr << "WARNING: " << name << ": XCSoar does not support line TP type. FAI Sector will be used instead. You may need to manualy advance a waypoint after reaching it in Condor." << std::endl;
            settingsTask.SectorType = xcsoar::AST_FAI;
            settingsTask.SectorRadius = radius;
          }
        }
        break;

      case 270:
        std::cerr << "WARNING: " << name << ": XCSoar does not support TP with angle '270'. Circle sector will be used instead. Be carefull to advance a waypoint in Condor after it has been advanced by the XCSoar." << std::endl;
      case 360:
        if(i == 1)
          settingsTask.StartType = xcsoar::START_CIRCLE;
        else if(i == tpNum - 1)
          settingsTask.FinishType = xcsoar::FINISH_CIRCLE;
        else {
          if(i > 2 && settingsTask.SectorType != xcsoar::AST_CIRCLE)
            tpsValid = false;
          else {
            settingsTask.SectorType = xcsoar::AST_CIRCLE;
            if(i > 2 && settingsTask.SectorRadius != radius) {
              std::cerr << "WARNING: " << name << ": XCSoar does not support different TPs types. The smallest radius will be used for all circle sectors. If you advance a sector in XCSoar you will advance it in Condor." << std::endl;
              settingsTask.SectorRadius = min(settingsTask.SectorRadius, radius);
            }
            else
              settingsTask.SectorRadius = radius;
          }
        }
        break;

      default:
        ;
      }
      
      if(i == 1) {
        settingsTask.StartRadius = radius;
        settingsTask.StartMaxHeight = condor2nav::Convert<unsigned>(taskParser.Value("Task", "TPHeight" + tpIdxStr));
      }
      else if(i == tpNum - 1) {
        settingsTask.FinishRadius = radius;
//        settingsTask.FinishMinHeight = condor2nav::Convert<unsigned>(taskParser.Value("Task", "TPWidth" + tpIdxStr));
        // AGL only in XCSoar ;-(
        settingsTask.FinishMinHeight = 0;
      }
    }
    else if(sectorType == CCondor::SECTOR_WINDOW) {
      std::cerr << "WARNING: " << name << ": XCSoar does not support window TP type. Circle TP will be used and you are responsible for reaching it on correct height and with correct heading." << std::endl;
    }
    else
      throw std::range_error("ERROR: Unsupported sector type '" + sectorTypeStr + "' specified for TP '" + name + "'!!!");
  }

  if(!tpsValid)
    std::cerr << "WARNING: XCSoar does not support different TPs types. FAI Sector will be used for all sectors. You may need to manualy advance a waypoint after reaching it in Condor." << std::endl;

  // set profile parameters
  // HomeWaypoint

  profileParser.Value("", "StartLine", Convert(settingsTask.StartType));
  profileParser.Value("", "StartMaxHeight", Convert(settingsTask.StartMaxHeight));
  profileParser.Value("", "StartMaxHeightMargin", "0");
  profileParser.Value("", "StartHeightRef", "1");        // AMSL
  profileParser.Value("", "StartRadius", Convert(settingsTask.StartRadius));
  profileParser.Value("", "StartMaxSpeed", "0");
  profileParser.Value("", "StartMaxSpeedMargin", "0");

  profileParser.Value("", "FAISector", Convert(settingsTask.SectorType));
  profileParser.Value("", "Radius", Convert(settingsTask.SectorRadius));

  profileParser.Value("", "FinishLine", Convert(settingsTask.FinishType));
  profileParser.Value("", "FinishMinHeight", Convert(settingsTask.FinishMinHeight));
  profileParser.Value("", "FinishRadius", Convert(settingsTask.FinishRadius));
  profileParser.Value("", "FAIFinishHeight", Convert(settingsTask.FinishMinHeight));

  // dump Task file
  COStream tskFile(outputTaskFilePath);

  tskFile.Write(reinterpret_cast<const char *>(taskPointArray), maxTaskPoints * sizeof(TASK_POINT));

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

  tskFile.Write(reinterpret_cast<const char *>(startPointArray), maxStartPoints * sizeof(START_POINT));
  tskFile.Write(reinterpret_cast<const char *>(taskWaypointArray), maxTaskPoints * sizeof(WAYPOINT));
  tskFile.Write(reinterpret_cast<const char *>(startWaypointArray), maxStartPoints * sizeof(WAYPOINT));
}

  // set units
  // Speed
//  switch(Speed)
//    {
//    case 0 :
//      SPEEDMODIFY = TOMPH;
//      break;
//    case 1 :
//      SPEEDMODIFY = TOKNOTS;
//      break;
//    case 2 :
//      SPEEDMODIFY = TOKPH;
//      break;
//    }
//  // Distance
//  switch(Distance)
//    {
//    case 0 : DISTANCEMODIFY = TOMILES; break;
//    case 1 : DISTANCEMODIFY = TONAUTICALMILES; break;
//    case 2 : DISTANCEMODIFY = TOKILOMETER; break;
//    }
//
//  // Altitude
//  GetFromRegistryD(szRegistryAltitudeUnitsValue,Altitude);
//  switch(Altitude)
//    {
//    case 0 : ALTITUDEMODIFY = TOFEET; break;
//    case 1 : ALTITUDEMODIFY = TOMETER; break;
//    }
//
//
//    
//}
 

/**
* @brief Sets task penalty zones. 
*
* Method sets penalty zones used in the task.
*
* @param profileParser XCSoar profile file parser.
* @param taskParser Condor task parser. 
* @param coordConv  Condor coordinates converter.
* @param pathPrefix Polar file subdirectory prefix (in XCSoar format).
* @param outputPathPrefix Polar file subdirectory prefix (in filesystem format).
**/
void condor2nav::CTargetXCSoarCommon::PenaltyZonesProcess(CFileParserINI &profileParser, const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv, const std::string &pathPrefix, const std::string &outputPathPrefix) const
{
  unsigned pzNum = condor2nav::Convert<unsigned>(taskParser.Value("Task", "PZCount"));
  if(pzNum == 0) {
    profileParser.Value("", "AirspaceFile", "\"\"");
    return;
  }
  
  profileParser.Value("", "AirspaceFile", "\"" + pathPrefix + std::string("\\") + AIRSPACES_FILE_NAME + std::string("\""));
  std::string airspacesFileName = outputPathPrefix + std::string("\\") + AIRSPACES_FILE_NAME;
  COStream airspacesFile(airspacesFileName);

  airspacesFile << "*******************************************************" << std::endl;
  airspacesFile << "* Condor Task Penalty Zones generated with Condor2Nav *" << std::endl;
  airspacesFile << "*******************************************************" << std::endl;
  for(unsigned i=0; i<pzNum; i++) {
    std::string tpIdxStr(Convert(i));
    airspacesFile << std::endl;
    airspacesFile << "AC P" << std::endl;
    airspacesFile << "AN Penalty Zone " << i + 1 << std::endl;
    airspacesFile << "AH " << taskParser.Value("Task", "PZTop" + tpIdxStr) << "m AMSL" << std::endl;
    unsigned base(Convert<unsigned>(taskParser.Value("Task", "PZBase" + tpIdxStr)));
    if(base == 0)
      airspacesFile << "AL 0" << std::endl;
    else
      airspacesFile << "AL " << base << "m AMSL" << std::endl;
    
    for(unsigned j=0; j<4; j++) {
      std::string tpCornerIdxStr(Convert(j));
      std::string x(taskParser.Value("Task", "PZPos" + tpCornerIdxStr + "X" + tpIdxStr));
      std::string y(taskParser.Value("Task", "PZPos" + tpCornerIdxStr + "Y" + tpIdxStr));
      airspacesFile << "DP " << coordConv.Latitude(x, y, CCondor::CCoordConverter::FORMAT_DDMMSS) <<
        " " << coordConv.Longitude(x, y, CCondor::CCoordConverter::FORMAT_DDMMSS) << std::endl;
    }
  }
}


/**
* @brief Sets weather data. 
*
* Method sets the wind data.
*
* @param profileParser XCSoar profile file parser.
* @param taskParser Condor task parser. 
**/
void condor2nav::CTargetXCSoarCommon::WeatherProcess(CFileParserINI &profileParser, const CFileParserINI &taskParser) const
{
  unsigned dir = static_cast<unsigned>(Convert<float>(taskParser.Value("Weather", "WindDir")) + 0.5);
  unsigned speed = static_cast<unsigned>(Convert<float>(taskParser.Value("Weather", "WindSpeed")) + 0.5);
  profileParser.Value("", "WindBearing", Convert(dir));
  profileParser.Value("", "WindSpeed", Convert(speed));
}


//
//Print info about:
//1. Copy
