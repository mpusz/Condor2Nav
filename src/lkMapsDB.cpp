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
 * @file lkMapsDB.cpp
 *
 * @brief LK8000 maps database. 
 */

#include "lkMapsDB.h"
#include "condor2nav.h"
#include "fileParserCSV.h"
#include "translator.h"
#include "istream.h"
#include "tools.h"
#include <algorithm>

namespace condor2nav {

  unsigned MapScale(const CFileParserINI &map);

}

const boost::filesystem::path condor2nav::CLKMapsDB::CONDOR_TEMPLATES_DIR = "data/Landscapes";
const boost::filesystem::path condor2nav::CLKMapsDB::LK8000_TEMPLATES_DIR = "data/LK8000/LKMTemplates";
const boost::filesystem::path condor2nav::CLKMapsDB::LK8000_MAPS_DIR      = "data/LK8000/_Maps";


unsigned condor2nav::MapScale(const CFileParserINI &map)
{
  //if(map.Value("", "RES90") == "YES")
  //  return 90;
  if(map.Value("", "RES250") == "YES")
    return 250;
  if(map.Value("", "RES500") == "YES")
    return 500;
  if(map.Value("", "RES1000") == "YES")
    return 1000;
  throw EOperationFailed("ERROR: Unknown LK8000 map '" + map.Value("", "NAME") + "' scale!!!");
}


condor2nav::CLKMapsDB::CLKMapsDB(const CCondor2Nav &app):
_app(app), _sceneriesParser(CTranslator::DATA_PATH / _app.ConfigParser().Value("Condor2Nav", "Target") / CTranslator::SCENERIES_DATA_FILE_NAME)
{
  // fill the list of Condor landscapes templates
  std::for_each(boost::filesystem::directory_iterator(CONDOR_TEMPLATES_DIR), boost::filesystem::directory_iterator(),
    [this](const boost::filesystem::path &p) { _condor.push_back(p.filename().string().c_str()); });
  DirectoryCreate(LK8000_TEMPLATES_DIR);
}


auto condor2nav::CLKMapsDB::LKMTemplatesSync() const -> CNamesList
{
  // fill the list of already downloaded LKMaps templates
  CNamesList lkLocal;
  std::for_each(boost::filesystem::directory_iterator(LK8000_TEMPLATES_DIR), boost::filesystem::directory_iterator(),
    [&](const boost::filesystem::path &p) { lkLocal.push_back(p.filename().string().c_str()); });
  std::sort(lkLocal.begin(), lkLocal.end());

  // get the list of all LKMaps templates on LK8000 server
  _app.Log() << "Obtaining list of LK8000 maps templates..." << std::endl;
  CNamesList lkRemote;
  // do it here...
  lkRemote.push_back("CRO_CROATIA.TXT");
  lkRemote.push_back("POL.TXT");
  lkRemote.push_back("POL_TATRA.TXT");
  std::sort(lkRemote.begin(), lkRemote.end());

  // check if there are new LK8000 templates on the LK8000 server
  CNamesList result;
  std::set_difference(lkRemote.begin(), lkRemote.end(), lkLocal.begin(), lkLocal.end(), std::back_inserter(result));
  if(result.size()) {
    // download new templates from LK8000 server
    _app.Log() << "Downloading new LK8000 maps templates..." << std::endl;
    std::for_each(result.begin(), result.end(),
      [&](const CStringNoCase &name)
    { 
      _app.Log() << " - " << name << std::endl;
      Download("www.bware.it", std::string("/listing/LKMAPS/TEMPLATES/") + name.c_str(), LK8000_TEMPLATES_DIR / name.c_str());
    });
  }
  else {
    _app.Log() << "No new LK8000 maps templates found" << std::endl;
  }

  return result;
}


auto condor2nav::CLKMapsDB::LandscapesMatch(CNamesList &&newTemplates) -> CParsersMap
{
  _app.Log() << "Looking for new/better maps match..." << std::endl;

  CParsersMap result;

  // fill a list of Condor sceneries data
  CParsersMap condor;
  std::for_each(_condor.begin(), _condor.end(),
    [&](const CStringNoCase &n) { condor[n] = std::make_shared<CFileParserINI>(CONDOR_TEMPLATES_DIR / n.c_str()); });

  // prepare a list of freshly downloaded LKMaps templates data
  CParsersMap lk;
  std::for_each(newTemplates.begin(), newTemplates.end(),
    [&](CStringNoCase &n) { lk[std::move(n)] = std::make_shared<CFileParserINI>(LK8000_TEMPLATES_DIR / n.c_str()); });

  // do for all Condor maps
  for(auto landscape=condor.begin(); landscape!=condor.end(); ++landscape) {
    double lonMin = Convert<double>(landscape->second->Value("", "LONMIN"));
    double lonMax = Convert<double>(landscape->second->Value("", "LONMAX"));
    double latMin = Convert<double>(landscape->second->Value("", "LATMIN"));
    double latMax = Convert<double>(landscape->second->Value("", "LATMAX"));

    auto landscapeName(landscape->first, 0, landscape->first.find_last_of('_'));
    auto &landscapeData = _sceneriesParser.Row(landscapeName.c_str(), 0, true);
    std::shared_ptr<CFileParserINI> bestMatch;
    
    // assign previous LK map if found
    auto mapName = landscapeData[CTranslator::CTarget::SCENERY_MAP_FILE];
    if(!mapName.empty())
      bestMatch = std::make_shared<CFileParserINI>(LK8000_TEMPLATES_DIR / (mapName.substr(0, mapName.find(".LKM")) + ".TXT").c_str());

    // check for all new maps
    bool newMapFound = false;
    for(auto map=lk.begin(); map!=lk.end(); ++map) {
      // check if there is a new LK map that covers all landscape area
      if(InsideArea(
        Convert<double>(map->second->Value("", "LONMIN")), Convert<double>(map->second->Value("", "LONMAX")),
        Convert<double>(map->second->Value("", "LATMIN")), Convert<double>(map->second->Value("", "LATMAX")),
        lonMin, lonMax, latMin, latMax))
      {
        // check if that map is better than already found
        if(!bestMatch || MapScale(*map->second) < MapScale(*bestMatch)) {
          bestMatch = map->second;
          newMapFound = true;
        }
      }
    }

    if(newMapFound) {
      // set new map data in CSV file
      auto &newName = bestMatch->Value("", "NAME");
      _app.Log() << " - " << newName << " -> " << landscape->first << std::endl;
      landscapeData[CTranslator::CTarget::SCENERY_MAP_FILE] = newName + ".LKM";
      landscapeData[CTranslator::CTarget::SCENERY_TERRAIN_FILE] = newName + ".DEM";

      // store in results
      result[newName.c_str()] = bestMatch;
    }
  }

  if(result.empty())
    _app.Log() << "No new/better maps found" << std::endl;

  return result;
}


void condor2nav::CLKMapsDB::LKMDownload(const CParsersMap &maps) const
{
  _app.Log() << "Downloading new LK8000 maps..." << std::endl;
  for(auto map=maps.begin(); map!=maps.end(); ++map) {
    std::string path = "/listing/LKMAPS/" + map->second->Value("", "MAPZONE") + "/" + map->second->Value("", "DIR") + ".DIR/";
    std::string name = map->second->Value("", "NAME") + ".LKM";
    _app.Log() << " - " << name << std::endl;
    Download("www.bware.it", path + name, LK8000_MAPS_DIR / name, 180);

    name = map->second->Value("", "NAME") + "_" + Convert(MapScale(*map->second)) + ".DEM";
    _app.Log() << " - " << name << std::endl;
    Download("www.bware.it", path + name, LK8000_MAPS_DIR / name, 180);
  }
}


void condor2nav::CLKMapsDB::ScenarioCSVUpdate() const
{
  _app.Log() << "Updating '" << _sceneriesParser.Path() << "' file..." << std::endl;
  _sceneriesParser.Dump();
}
