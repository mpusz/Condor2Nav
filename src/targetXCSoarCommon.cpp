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
 * @file targetXCSoarCommon.cpp
 *
 * @brief Implements the condor2nav::CTargetXCSoarCommon class. 
 */

#include "targetXCSoarCommon.h"
#include "condor2nav.h"
#include "imports/xcsoarTypes.h"
#include "imports/lk8000Types.h"
#include "ostream.h"
#include <cmath>


const boost::filesystem::path condor2nav::CTargetXCSoarCommon::OUTPUT_PROFILE_NAME    = "Condor.prf";
const boost::filesystem::path condor2nav::CTargetXCSoarCommon::TASK_FILE_NAME         = "Condor.tsk";
const boost::filesystem::path condor2nav::CTargetXCSoarCommon::DEFAULT_TASK_FILE_NAME = "Default.tsk";
const boost::filesystem::path condor2nav::CTargetXCSoarCommon::POLAR_FILE_NAME        = "Condor.plr";
const boost::filesystem::path condor2nav::CTargetXCSoarCommon::AIRSPACES_FILE_NAME    = "Condor.txt";
const boost::filesystem::path condor2nav::CTargetXCSoarCommon::WP_FILE_NAME           = "Condor.dat";

/**
 * @brief Class constructor.
 *
 * condor2nav::CTargetXCSoarCommon class constructor.
 *
 * @param translator Configuration INI file parser.
 */
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
 */
void condor2nav::CTargetXCSoarCommon::SceneryMapProcess(CFileParserINI &profileParser,
                                                        const CFileParserCSV::CStringArray &sceneryData,
                                                        const boost::filesystem::path &pathPrefix) const
{
  profileParser.Value("", "MapFile", "\"" + (pathPrefix / sceneryData.at(SCENERY_XCM_FILE)).string() + "\"");

  // reset landscape specific files in case they were set before profile import
  // if need user can still assign additionl data with second entries
  profileParser.Value("", "TerrainFile", "\"\"");
  profileParser.Value("", "WPFile", "\"\"");
  profileParser.Value("", "AirfieldFile", "\"\"");
}


/**
 * @brief Sets time for scenery time zone. 
 *
 * Method sets UTC time offset for selected scenery and forces time synchronization to the GPS source.
 *
 * @param profileParser XCSoar profile file parser.
 */
void condor2nav::CTargetXCSoarCommon::SceneryTimeProcess(CFileParserINI &profileParser) const
{
  profileParser.Value("", "UTCOffset", "0");
}


/**
* @brief Calculates the bearing between 2 locations.
*
* Method calculates the bearing between 2 locations.
*
* @param lon1 First point longitude.
* @param lat1 First point latitude.
* @param lon2 Second point longitude.
* @param lat2 Second point latitude.
*
* @return The bearing between 2 locations.
 */
unsigned condor2nav::CTargetXCSoarCommon::WaypointBearing(double lon1, double lat1, double lon2, double lat2) const
{
  lon1 = Deg2Rad(lon1);
  lat1 = Deg2Rad(lat1);
  lon2 = Deg2Rad(lon2);
  lat2 = Deg2Rad(lat2);
  
  double clat1 = cos(lat1);
  double clat2 = cos(lat2);
  double dlon = lon2 - lon1;

  double y = sin(dlon) * clat2;
  double x = clat1 * sin(lat2) - sin(lat1) * clat2 * cos(dlon);
  return (x==0 && y==0) ? 0 : (static_cast<unsigned>(360 + Rad2Deg(atan2(y, x)) + 0.5) % 360);
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
* @param aatTime     Minimum time for AAT task
* @param maxTaskPoints The number of waypoints stored in a task file.
* @param maxStartPoints The number of alternate startpoints stored in a task file.
* @param generateWPFile Flag specifying if WP file should be generated.
* @param wpOutputPathPrefix XCSoar WP subdirectory prefix (in filesystem format).
 */
void condor2nav::CTargetXCSoarCommon::TaskProcess(CFileParserINI &profileParser, const CFileParserINI &taskParser,
                                                  const CCondor::CCoordConverter &coordConv,
                                                  const CFileParserCSV::CStringArray &sceneryData,
                                                  const boost::filesystem::path &outputTaskFilePath,
                                                  unsigned aatTime,
                                                  unsigned maxTaskPoints, unsigned maxStartPoints,
                                                  bool generateWPFile, const boost::filesystem::path &wpOutputPathPrefix) const
{
  using namespace xcsoar;

  auto wpFileName = wpOutputPathPrefix / WP_FILE_NAME;

  std::unique_ptr<COStream> wpFile;
  if(generateWPFile)
    wpFile = std::unique_ptr<COStream>(new COStream(wpFileName));

  // set task settings
  SETTINGS_TASK settingsTask = { 0 };
  settingsTask.AATEnabled       = aatTime > 0;
  settingsTask.AATTaskLength    = aatTime;
  try {
    settingsTask.AutoAdvance      = static_cast<AutoAdvanceMode_t>(Convert<unsigned>(profileParser.Value("", "AutoAdvance")));
  }
  catch(const Exception &) {
    settingsTask.AutoAdvance    = AUTOADVANCE_ARMSTART;
  }
  settingsTask.EnableMultipleStartPoints = false;

  // reset data
  TASK_POINT *taskPointArray = new TASK_POINT[maxTaskPoints];
  memset(taskPointArray, 0, maxTaskPoints * sizeof(TASK_POINT));
  START_POINT *startPointArray = new START_POINT[maxStartPoints];
  memset(startPointArray, 0, maxStartPoints * sizeof(START_POINT));
  CWaypointArray waypointArray;
  waypointArray.reserve(maxTaskPoints);
  for(size_t i=0; i<maxTaskPoints; i++) {
    taskPointArray[i].Index = -1;
    taskPointArray[i].AATStartRadial = 0;
    taskPointArray[i].AATFinishRadial = 360;
  }
  for(size_t i=0; i<maxStartPoints; i++)
    startPointArray[i].Index = -1;

  bool tpsValid(true);
  unsigned tpNum = Convert<unsigned>(taskParser.Value("Task", "Count"));

  // check if enough waypoints to create a task
  if(tpNum - 1 > maxTaskPoints)
    throw EOperationFailed("ERROR: Too many waypoints (" + Convert(tpNum - 1) + ") in a task file (only " + Convert(maxTaskPoints) + " supported)!!!");

  // skip takeoff waypoint
  for(size_t i=1; i<tpNum; i++) {
    // dump WP file line
    std::string tpIdxStr(Convert(i));
    std::string name;
    std::string tpName = taskParser.Value("Task", "TPName" + tpIdxStr);
    if(i == 1)
      name = "S:" + tpName;
    else if(i == tpNum - 1)
      name = "F:" + tpName;
    else
      name = Convert(i - 1) + ":" + tpName;

    std::string x(taskParser.Value("Task", "TPPosX" + tpIdxStr));
    std::string y(taskParser.Value("Task", "TPPosY" + tpIdxStr));
    double latitude = coordConv.Latitude(x, y);
    double longitude = coordConv.Longitude(x, y);
    std::string latitudeStr = DDFF2DDMMFF(latitude, false);
    std::string longitudeStr = DDFF2DDMMFF(longitude, true);
    double minAlt = Convert<unsigned>(taskParser.Value("Task", "TPWidth" + tpIdxStr));
    double altitude = minAlt ? minAlt : Convert<double>(taskParser.Value("Task", "TPPosZ" + tpIdxStr));
    
    if(generateWPFile)
      *wpFile << i << "," << latitudeStr << "," << longitudeStr << ","
              << altitude << "M,T," << name << "," << tpName << std::endl;

    // fill waypoint data
    TWaypoint waypoint;
    taskPointArray[i - 1].Index = waypoint.number = WAYPOINT_INDEX_OFFSET + i;
    waypoint.latitude = latitude;
    waypoint.longitude = longitude;
    waypoint.altitude = altitude;
    waypoint.flags = xcsoar::WAYPOINT_TURNPOINT;
    waypoint.name = name;
    waypoint.comment = tpName;
    waypoint.inTask = true;
    waypointArray.push_back(waypoint);

    // dump Task File data
    std::string sectorTypeStr(taskParser.Value("Task", "TPSectorType" + tpIdxStr));
    unsigned sectorType(Convert<unsigned>(sectorTypeStr));
    if(sectorType == CCondor::SECTOR_CLASSIC) {
      unsigned radius(condor2nav::Convert<unsigned>(taskParser.Value("Task", "TPRadius" + tpIdxStr)));
      unsigned angle(condor2nav::Convert<unsigned>(taskParser.Value("Task", "TPAngle" + tpIdxStr)));

      if(settingsTask.AATEnabled && i > 1 && i < tpNum - 1) {
        // AAT waypoints
        if(angle == 360) {
          taskPointArray[i - 1].AATType = WAYPOINT_AAT_CIRCLE;
          taskPointArray[i - 1].AATCircleRadius = radius;
        }
        else {
          taskPointArray[i - 1].AATType = WAYPOINT_AAT_SECTOR;
          taskPointArray[i - 1].AATSectorRadius = radius;

          std::string x1 = taskParser.Value("Task", "TPPosX" + Convert(i - 1));
          std::string y1 = taskParser.Value("Task", "TPPosY" + Convert(i - 1));
          double lon1 = coordConv.Longitude(x1, y1);
          double lat1 = coordConv.Latitude(x1, y1);
          unsigned angle1 = WaypointBearing(lon1, lat1, longitude, latitude);

          std::string x2 = taskParser.Value("Task", "TPPosX" + Convert(i + 1));
          std::string y2 = taskParser.Value("Task", "TPPosY" + Convert(i + 1));
          double lon2 = coordConv.Longitude(x2, y2);
          double lat2 = coordConv.Latitude(x2, y2);
          unsigned angle2 = WaypointBearing(lon2, lat2, longitude, latitude);

          unsigned halfAngle;
          if(angle1 == angle2)
            halfAngle = angle1;
          else {
            halfAngle = static_cast<unsigned>((angle1 + angle2) / 2.0);
            if((angle1 > angle2 && angle1 - angle2 > 180) || (angle1 < angle2 && angle2 - angle1 > 180))
              halfAngle = (halfAngle + 180) % 360;
          }
          taskPointArray[i - 1].AATStartRadial = static_cast<unsigned>(360 + halfAngle - angle / 2.0) % 360;
          taskPointArray[i - 1].AATFinishRadial = static_cast<unsigned>(360 + halfAngle + angle / 2.0) % 360;
        }
      }
      else {
        // START, END or regular (not AAT) waypoint
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
              Translator().App().Warning() << "WARNING: " << name << ": " << Name() << " does not support different TPs types. The smallest radius will be used for all FAI sectors. If you advance a sector in " << Name() << " you will advance it in Condor." << std::endl;
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
              Translator().App().Warning() << "WARNING: " << name << ": " << Name() << " does not support line TP type. FAI Sector will be used instead. You may need to manualy advance a waypoint after reaching it in Condor." << std::endl;
              settingsTask.SectorType = xcsoar::AST_FAI;
              settingsTask.SectorRadius = radius;
            }
          }
          break;

        case 270:
          Translator().App().Warning() << "WARNING: " << name << ": " << Name() << " does not support TP with angle '270'. Circle sector will be used instead. Be carefull to advance a waypoint in Condor after it has been advanced by the " << Name() << "." << std::endl;
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
                Translator().App().Warning() << "WARNING: " << name << ": " << Name() << " does not support different TPs types. The smallest radius will be used for all circle sectors. If you advance a sector in " << Name() << " you will advance it in Condor." << std::endl;
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
          settingsTask.StartMaxHeight = Convert<unsigned>(taskParser.Value("Task", "TPHeight" + tpIdxStr));
        }
        else if(i == tpNum - 1) {
          settingsTask.FinishRadius = radius;
          //        settingsTask.FinishMinHeight = Convert<unsigned>(taskParser.Value("Task", "TPWidth" + tpIdxStr));
          // AGL only in XCSoar ;-(
          settingsTask.FinishMinHeight = 0;
        }
      }
    }
    else if(sectorType == CCondor::SECTOR_WINDOW) {
      Translator().App().Warning() << "WARNING: " << name << ": " << Name() << " does not support window TP type. Circle TP will be used and you are responsible for reaching it on correct height and with correct heading." << std::endl;
    }
    else
      Translator().App().Error() << "ERROR: Unsupported sector type '" << sectorTypeStr << "' specified for TP '" << name << "'!!!";
  }

  if(!tpsValid)
    Translator().App().Warning() << "WARNING: " << Name() << " does not support different TPs types. FAI Sector will be used for all sectors. You may need to manualy advance a waypoint after reaching it in Condor." << std::endl;

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
  TaskDump(profileParser, taskParser, outputTaskFilePath, settingsTask, taskPointArray, startPointArray, waypointArray);
}


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
 */
void condor2nav::CTargetXCSoarCommon::PenaltyZonesProcess(CFileParserINI &profileParser,
                                                          const CFileParserINI &taskParser,
                                                          const CCondor::CCoordConverter &coordConv,
                                                          const boost::filesystem::path &pathPrefix,
                                                          const boost::filesystem::path &outputPathPrefix) const
{
  unsigned pzNum = Convert<unsigned>(taskParser.Value("Task", "PZCount"));
  if(pzNum == 0) {
    profileParser.Value("", "AirspaceFile", "\"\"");
    return;
  }
  
  profileParser.Value("", "AirspaceFile", "\"" + (pathPrefix / AIRSPACES_FILE_NAME).string() + std::string("\""));
  boost::filesystem::path airspacesFileName = outputPathPrefix / AIRSPACES_FILE_NAME;
  COStream airspacesFile(airspacesFileName);

  airspacesFile << "*******************************************************" << std::endl;
  airspacesFile << "* Condor Task Penalty Zones generated with Condor2Nav *" << std::endl;
  airspacesFile << "*******************************************************" << std::endl;
  for(size_t i=0; i<pzNum; i++) {
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
    
    for(size_t j=0; j<4; j++) {
      std::string tpCornerIdxStr(Convert(j));
      std::string x(taskParser.Value("Task", "PZPos" + tpCornerIdxStr + "X" + tpIdxStr));
      std::string y(taskParser.Value("Task", "PZPos" + tpCornerIdxStr + "Y" + tpIdxStr));
      airspacesFile << "DP " << coordConv.Latitude(x, y, CCondor::CCoordConverter::FORMAT_DDMMSS) <<
        " " << coordConv.Longitude(x, y, CCondor::CCoordConverter::FORMAT_DDMMSS) << std::endl;
    }
  }
}
