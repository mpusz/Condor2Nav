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
 * @file condor2nav.cpp
 *
 * @brief Implements the condor2nav::CCondor2Nav class. 
 */

#include "condor2nav.h"
#include "lkMapsDB.h"

const char *condor2nav::CCondor2Nav::CONFIG_FILE_NAME = "condor2nav.ini";

/**
 * @brief Class constructor. 
 *
 * @param type The logger type. 
 */
condor2nav::CCondor2Nav::CLogger::CLogger(TType type):
_type(type)
{

}


condor2nav::CCondor2Nav::CCondor2Nav():
_configParser(CONFIG_FILE_NAME)
{
}


void condor2nav::CCondor2Nav::OnStart() const
{
  if(_configParser.Value("Condor2Nav", "Target") == "LK8000" && _configParser.Value("LK8000", "CheckForMapUpdates") == "1") {
    LogHigh() << "LK8000 maps synchronization START" << std::endl;
    try {
      CLKMapsDB db(*this);
      auto newTemplates = db.LKMTemplatesSync();
      if(newTemplates.size()) {
        // new templates found - check if better maps can be used
        auto newMaps = db.LandscapesMatch(std::move(newTemplates));
        if(newMaps.size()) {
          db.LKMDownload(newMaps);
          db.ScenarioCSVUpdate();
        }
      }
      LogHigh() << "LK8000 maps synchronization FINISH" << std::endl;
    }
    catch(const std::exception &ex) {
      Error() << ex.what() << std::endl;
    }
  }
}
