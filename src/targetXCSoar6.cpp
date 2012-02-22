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
 * @brief Implements the condor2nav::CTargetXCSoar6 class. 
 */

#include "targetXCSoar6.h"
#include "imports/xcsoarTypes.h"
#include "ostream.h"


/**
 * @brief Class constructor.
 *
 * condor2nav::CTargetXCSoar6 class constructor.
 *
 * @param translator Configuration INI file parser.
 */
condor2nav::CTargetXCSoar6::CTargetXCSoar6(const CTranslator &translator):
CTargetXCSoar(translator)
{
}


/**
 * @brief Dumps waypoints in XCSoar v6 format. 
 *
 * Method dumps waypoints in XCSoar v6 format.
 * 
 * @param profileParser      XCSoar profile file parser.
 * @param taskParser         Condor task parser. 
 * @param settingsTask       Task settings
 * @param taskPointArray     Task points array
 * @param startPointArray    Task start points array
 * @param waypointArray      The array of waypoints data.
 */
void condor2nav::CTargetXCSoar6::TaskDump(CFileParserINI &profileParser,
                                          const CFileParserINI &taskParser,
                                          const xcsoar::SETTINGS_TASK &settingsTask,
                                          const xcsoar::TASK_POINT taskPointArray[],
                                          const xcsoar::START_POINT startPointArray[],
                                          const CWaypointArray &waypointArray) const
{
  using namespace xcsoar;

  COStream tskFile(_outputTaskFilePathList);  

  if(settingsTask.AATEnabled) {
    tskFile << "<Task type=\"AAT\" task_scored=\"1\" aat_min_time=\""
      << (settingsTask.AATTaskLength * 60) << "\""; 
  }
  else {
    tskFile << "<Task type=\"RT\" task_scored=\"1\" aat_min_time=\"0\"";
  }
  tskFile << " start_max_speed=\"0\" start_max_height=\"" << settingsTask.StartMaxHeight
    << "\" start_max_height_ref=\"1\" finish_min_height=\"" << settingsTask.FinishMinHeight
    << "\" fai_finish=\"0\" min_points=\"" << waypointArray.size() - 1
    << "\" max_points=\"10\" homogeneous_tps=\"0\" is_closed=\"0\">" << std::endl;

  for(size_t i=0; i<waypointArray.size(); i++) {
    tskFile << "\t<Point type=\"";

    if(i==0)
      tskFile << "Start";
    else if(i==(waypointArray.size()-1))
      tskFile << "Finish";
    else if(settingsTask.AATEnabled)
      tskFile << "Area";
    else
      tskFile << "Turn";

    tskFile << "\">" << std::endl;

    tskFile << "\t\t<Waypoint name=\"" << waypointArray[i].name << "\" id=\"0\" comment=\"" << waypointArray[i].comment <<
      "\" altitude=\"" << waypointArray[i].altitude << "\">" << std::endl;
    tskFile << "\t\t\t<Location longitude=\"" << waypointArray[i].longitude << "\" latitude=\""<< waypointArray[i].latitude << "\"/>" << std::endl;
    tskFile << "\t\t</Waypoint>" << std::endl;

    std::string tpIdxStr(Convert(i + 1));
    unsigned radius(Convert<unsigned>(taskParser.Value("Task", "TPRadius" + tpIdxStr)));
    unsigned angle(Convert<unsigned>(taskParser.Value("Task", "TPAngle" + tpIdxStr)));
    if(angle == 360)
      tskFile << "\t\t<ObservationZone type=\"Cylinder\" radius=\"" << radius <<"\"/>" << std::endl;
    else {
      if (angle==180 && (i == 0 || i == (waypointArray.size() - 1))) { 
        tskFile << "\t\t<ObservationZone type=\"Line\" length=\"" << radius * 2 <<"\"/>" << std::endl;
      }
      else {			
        unsigned angle1;
        unsigned angle2;
        if(i == 0)
          angle1 = WaypointBearing(waypointArray[i+1].longitude, waypointArray[i+1].latitude, waypointArray[i].longitude, waypointArray[i].latitude);
        else
          angle1 = WaypointBearing(waypointArray[i-1].longitude, waypointArray[i-1].latitude, waypointArray[i].longitude, waypointArray[i].latitude);                    
        if(i < (waypointArray.size()-1))
          angle2 = WaypointBearing(waypointArray[i+1].longitude, waypointArray[i+1].latitude, waypointArray[i].longitude, waypointArray[i].latitude);	  
        else
          angle2=angle1;

        unsigned halfAngle;
        if(angle1 == angle2)
          halfAngle = angle1;
        else {
          halfAngle = static_cast<unsigned>((angle1 + angle2) / 2.0);
          if((angle1 > angle2 && angle1 - angle2 > 180) || (angle1 < angle2 && angle2 - angle1 > 180))
            halfAngle = (halfAngle + 180) % 360;
        }
        double astart = static_cast<unsigned>(360 + halfAngle - angle / 2.0) % 360;
        double aend = static_cast<unsigned>(360 + halfAngle + angle / 2.0) % 360;
        tskFile << "<ObservationZone type=\"Sector\" radius=\"" << radius << "\" start_radial=\""<< astart <<"\" end_radial=\""<< aend <<"\" />\r\n";
      }
    }
    tskFile << "\t</Point>" << std::endl;
  }
  tskFile << "</Task>" << std::endl;
}
