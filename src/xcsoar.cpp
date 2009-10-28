//
// This file is a part of Condor2Nav file formats translator.
//
// Copyright (C) 2009 Mateusz Pusz
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Visit the project webpage (http://sf.net/projects/condor2nav) for more info.
//

#include "xcsoar.h"
#include "condor2nav.h"
#include "condor.h"
#include "tools.h"
#include <fstream>
#include <iostream>


const char *condor2nav::CTranslatorXCSoar::XCSOAR_PROFILE_NAME = "xcsoar-registry.prf";

const char *condor2nav::CTranslatorXCSoar::OUTPUT_PROFILE_NAME = "Condor.prf";
const char *condor2nav::CTranslatorXCSoar::WP_FILE_NAME = "WP_CondorTask.dat";
const char *condor2nav::CTranslatorXCSoar::POLAR_FILE_NAME = "Polar_Condor.plr";


namespace xcsoar {

  const unsigned BINFILEMAGICNUMBER = 0x5c378fcf;
  const unsigned MAXTASKPOINTS = 10;
  const unsigned MAXSTARTPOINTS = 10;
  const unsigned MAXISOLINES = 32;

  struct GEOPOINT {
    double Longitude;
    double Latitude;
  };

  enum AATSectorType_t {
    AAT_CIRCLE=0,
    AAT_SECTOR
  };

  struct TASK_POINT {
    int Index;
    double InBound;
    double OutBound;
    double Bisector;
    double LegDistance;
    double LegBearing;
    GEOPOINT SectorStart;
    GEOPOINT SectorEnd;
    AATSectorType_t AATType;
    double AATCircleRadius;
    double AATSectorRadius;
    double AATStartRadial;
    double AATFinishRadial;
    GEOPOINT AATStart;
    GEOPOINT AATFinish;

    // from stats
    double AATTargetOffsetRadius;
    double AATTargetOffsetRadial;
    GEOPOINT AATTargetLocation;
    bool   AATTargetLocked;
    double LengthPercent;
    GEOPOINT IsoLine_Location[MAXISOLINES];
    bool IsoLine_valid[MAXISOLINES];
  };

  enum AutoAdvanceMode_t {
    AUTOADVANCE_MANUAL=0,
    AUTOADVANCE_AUTO,
    AUTOADVANCE_ARM,
    AUTOADVANCE_ARMSTART
  };

  enum ASTSectorType_t {
    AST_CIRCLE=0,
    AST_FAI,
    AST_DAE
  };

  enum StartSectorType_t {
    START_CIRCLE=0,
    START_LINE,
    START_SECTOR
  };

  enum FinishSectorType_t {
    FINISH_CIRCLE=0,
    FINISH_LINE,
    FINISH_SECTOR
  };

  struct SETTINGS_TASK {
    AutoAdvanceMode_t AutoAdvance;
    ASTSectorType_t SectorType;
    unsigned int SectorRadius;
    StartSectorType_t StartType;
    unsigned StartRadius;
    FinishSectorType_t FinishType;
    unsigned FinishRadius;
    double AATTaskLength;
    bool AATEnabled;
    bool EnableMultipleStartPoints;
    bool EnableFAIFinishHeight;
    unsigned FinishMinHeight;
    unsigned StartMaxHeight;
    unsigned StartMaxHeightMargin;
    unsigned StartMaxSpeed;
    unsigned StartMaxSpeedMargin;
    int StartHeightRef;
  };

  struct START_POINT {
    int Index;
    double OutBound;
    GEOPOINT SectorStart;
    GEOPOINT SectorEnd;
  };

}


condor2nav::CTranslatorXCSoar::CTranslatorXCSoar(const std::string &outputPath, const std::string &xcsoarDataPath):
CTranslator(outputPath),
_xcsoarDataPath(xcsoarDataPath),
_profileParser(CCondor2Nav::DATA_PATH + std::string("\\") + XCSOAR_PROFILE_NAME)
{
}


condor2nav::CTranslatorXCSoar::~CTranslatorXCSoar()
{
  _profileParser.Dump(OutputPath() + std::string("\\") + OUTPUT_PROFILE_NAME);
}


void condor2nav::CTranslatorXCSoar::SceneryMap(const CFileParserCSV::CStringArray &sceneryData)
{
  _profileParser.Value("", "MapFile", "\"" + _xcsoarDataPath + "\\" + sceneryData.at(CCondor2Nav::SCENERY_FILE) + "\"");
}


void condor2nav::CTranslatorXCSoar::SceneryTime(const CFileParserCSV::CStringArray &sceneryData)
{
  _profileParser.Value("", "UTCOffset", sceneryData.at(CCondor2Nav::SCENERY_UTC_OFFSET));
  _profileParser.Value("", "SetSystemTimeFromGPS", "1");
}


void condor2nav::CTranslatorXCSoar::Glider(const CFileParserCSV::CStringArray &gliderData)
{
  // set WinPilot Polar
  _profileParser.Value("", "Polar", "6");
  _profileParser.Value("", "PolarFile", "\"" + _xcsoarDataPath + std::string("\\") + POLAR_FILE_NAME + std::string("\""));

  _profileParser.Value("", "AircraftType", "\"" + gliderData.at(CCondor2Nav::GLIDER_NAME) + "\"");
  _profileParser.Value("", "SafteySpeed", Convert(KmH2MS(Convert<unsigned>(gliderData.at(CCondor2Nav::GLIDER_SPEED_MAX)))));
  _profileParser.Value("", "Handicap", gliderData.at(CCondor2Nav::GLIDER_DAEC_INDEX));
  _profileParser.Value("", "BallastSecsToEmpty", gliderData.at(CCondor2Nav::GLIDER_WATER_BALLAST_EMPTY_TIME));

  // create polar file
  std::string polarFileName = OutputPath() + std::string("\\") + POLAR_FILE_NAME;
  std::ofstream polarFile(polarFileName.c_str());
  if(!polarFile)
    throw std::runtime_error("ERROR: Couldn't open Polar file '" + polarFileName + "' for writing!!!");

  polarFile << "* " << gliderData.at(CCondor2Nav::GLIDER_NAME) << " WinPilot POLAR file" << std::endl;
  polarFile << "*" << std::endl;
  polarFile << "* MassDryGross[kg], MaxWaterBallast[liters], Speed1[km/h], Sink1[m/s], Speed2, Sink2, Speed3, Sink3" << std::endl;
  for(unsigned i=CCondor2Nav::GLIDER_MASS_DRY_GROSS; i<=CCondor2Nav::GLIDER_SINK_3; i++) {
    if(i > CCondor2Nav::GLIDER_MASS_DRY_GROSS)
      polarFile << ",";
    polarFile << gliderData.at(i);
  }
  polarFile << std::endl;
}


void condor2nav::CTranslatorXCSoar::Task(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv)
{
  using namespace xcsoar;

  // set task settings
  SETTINGS_TASK settingsTask;
  settingsTask.AutoAdvance           = static_cast<AutoAdvanceMode_t>(Convert<unsigned>(_profileParser.Value("", "AutoAdvance")));
  settingsTask.AATTaskLength         = 0;
  settingsTask.AATEnabled            = false;
  settingsTask.EnableMultipleStartPoints = false;
  settingsTask.EnableFAIFinishHeight = false;
  settingsTask.StartMaxHeightMargin  = 0;
  settingsTask.StartMaxSpeed         = 0;
  settingsTask.StartMaxSpeedMargin   = 0;
  settingsTask.StartHeightRef        = 0;

  TASK_POINT taskPointArray[MAXTASKPOINTS];
//  START_POINT startPointArray[MAXSTARTPOINTS];

  // reset all tasks
  for(unsigned i=0; i<MAXTASKPOINTS; i++)
    taskPointArray[i].Index = -1;


  _profileParser.Value("", "AdditionalWPFile", "\"" + _xcsoarDataPath + std::string("\\") + WP_FILE_NAME + std::string("\""));
  std::string wpFileName = OutputPath() + std::string("\\") + WP_FILE_NAME;
  std::ofstream wpFile(wpFileName.c_str());
  if(!wpFile)
    throw std::runtime_error("ERROR: Couldn't open file '" + wpFileName + "' for writing!!!");

  unsigned tpNum = condor2nav::Convert<unsigned>(taskParser.Value("Task", "Count"));
  // skip takeoff waypoint
  for(unsigned i=1; i<tpNum; i++){
    // dump WP file line
    std::string tpIdxStr(Convert(i));
    std::string x(taskParser.Value("Task", "TPPosX" + tpIdxStr));
    std::string y(taskParser.Value("Task", "TPPosY" + tpIdxStr));
    std::string name;
    if(i == 1)
      name = "START";
    else if(i == tpNum - 1)
      name = "FINISH";
    else
      name = "TP" + Convert(i - 1);
    wpFile << i << "," << coordConv.Latitude(x, y) << "," << coordConv.Longitude(x, y) << "," << taskParser.Value("Task", "TPPosZ" + tpIdxStr) << "M,T," << name << "," << taskParser.Value("Task", "TPName" + tpIdxStr) << std::endl;

    // dump Task File data
    std::string sectorTypeStr(taskParser.Value("Task", "TPSectorType" + tpIdxStr));
    unsigned sectorType(Convert<unsigned>(sectorTypeStr));
    if(sectorType == CCondor::SECTOR_CLASSIC) {
      unsigned angle(condor2nav::Convert<unsigned>(taskParser.Value("Task", "TPAngle" + tpIdxStr)));
      switch(angle) {
      case 90:
        if(i == 1)
          settingsTask.StartType = xcsoar::START_SECTOR;
        else if(i == tpNum - 1)
          settingsTask.FinishType = xcsoar::FINISH_SECTOR;
        else
          settingsTask.SectorType = xcsoar::AST_FAI;
        break;

      case 180:
        if(i == 1)
          settingsTask.StartType = xcsoar::START_LINE;
        else if(i == tpNum - 1)
          settingsTask.FinishType = xcsoar::FINISH_LINE;
        else {
          std::cerr << "WARNING: " << name << ": XCSoar does not support line TP type. FAI Sector will be used instead. You may need to manualy advance a waypoint after reaching it in Condor." << std::endl;
          settingsTask.SectorType = xcsoar::AST_FAI;
        }
        break;

      case 270:
        std::cerr << "WARNING: " << name << ": XCSoar does not support TP with angle '270'. Circle sector will be used instead. Be carefull to advance a waypoint in Condor after it has been advanced by the XCSoar." << std::endl;
      case 360:
        if(i == 1)
          settingsTask.StartType = xcsoar::START_CIRCLE;
        else if(i == tpNum - 1)
          settingsTask.FinishType = xcsoar::FINISH_CIRCLE;
        else
          settingsTask.SectorType = xcsoar::AST_CIRCLE;
        break;

      default:
        ;
      }
      
      unsigned radius(condor2nav::Convert<unsigned>(taskParser.Value("Task", "TPRadius" + tpIdxStr)));
      if(i == 1) {
        settingsTask.StartRadius = radius;
        settingsTask.StartMaxHeight = condor2nav::Convert<unsigned>(taskParser.Value("Task", "TPHeight" + tpIdxStr));
      }
      else if(i == tpNum - 1) {
        settingsTask.FinishRadius = radius;
        settingsTask.FinishMinHeight = condor2nav::Convert<unsigned>(taskParser.Value("Task", "TPWidth" + tpIdxStr));
      }
      else
        settingsTask.SectorRadius = radius;
    }
    else if(sectorType == CCondor::SECTOR_WINDOW) {
      std::cerr << "WARNING: " << name << ": XCSoar does not support window TP type. Circle TP will be used and you are responsible for reaching it on correct height and with correct heading." << std::endl;
    }
    else
      throw std::range_error("ERROR: Unsupported sector type '" + sectorTypeStr + "' specified for TP '" + name + "'!!!");
  }

  // set profile parameters
  // HomeWaypoint

  _profileParser.Value("", "StartLine", Convert(settingsTask.StartType));
  _profileParser.Value("", "StartMaxHeight", Convert(settingsTask.StartMaxHeight));
  _profileParser.Value("", "StartRadius", Convert(settingsTask.StartRadius));

  _profileParser.Value("", "FAISector", Convert(settingsTask.SectorType));
  _profileParser.Value("", "Radius", Convert(settingsTask.SectorRadius));

  _profileParser.Value("", "FinishLine", Convert(settingsTask.FinishType));
  _profileParser.Value("", "FinishMinHeight", Convert(settingsTask.FinishMinHeight));
  _profileParser.Value("", "FinishRadius", Convert(settingsTask.FinishRadius));
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
 

void condor2nav::CTranslatorXCSoar::Airspaces(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv)
{
  // AirspaceFile
  // AdditionalAirspaceFile
}


void condor2nav::CTranslatorXCSoar::Weather(const CFileParserINI &taskParser)
{
  unsigned dir = static_cast<unsigned>(Convert<float>(taskParser.Value("Weather", "WindDir")) + 0.5);
  unsigned speed = static_cast<unsigned>(Convert<float>(taskParser.Value("Weather", "WindSpeed")) + 0.5);
  _profileParser.Value("", "WindBearing", Convert(dir));
  _profileParser.Value("", "WindSpeed", Convert(speed));
}
