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
#include <boost\filesystem\fstream.hpp>

namespace condor2nav {

  unsigned MapScale(const CFileParserINI &map);

}

const bfs::path   condor2nav::CLKMapsDB::CONDOR_TEMPLATES_DIR            = "data/Landscapes";
const bfs::path   condor2nav::CLKMapsDB::CONDOR2NAV_LK8000_TEMPLATES_DIR = "data/LK8000/LKMTemplates";
const bfs::path   condor2nav::CLKMapsDB::CONDOR2NAV_LK8000_MAPS_DIR      = "data/LK8000/_Maps/condor2nav";
const bfs::path   condor2nav::CLKMapsDB::LK8000_MAPS_URL                 = "/listing/LKMAPS";
const std::string condor2nav::CLKMapsDB::LKM_TEMPLATES_INDEX_SERVER      = "cloud.github.com";
const bfs::path   condor2nav::CLKMapsDB::LKM_TEMPLATES_INDEX_URL         = "/downloads/mpusz/Condor2Nav/LKMTemplates.txt";


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
  throw EOperationFailed{"ERROR: Unknown LK8000 map '" + map.Value("", "NAME") + "' scale!!!"};
}


condor2nav::CLKMapsDB::CLKMapsDB(const CCondor2Nav &app) :
  _app{app}, _sceneriesParser{CTranslator::DATA_PATH / _app.ConfigParser().Value("Condor2Nav", "Target") / CTranslator::SCENERIES_DATA_FILE_NAME}
{
  // fill the list of Condor landscapes templates
  std::for_each(bfs::directory_iterator(CONDOR_TEMPLATES_DIR), bfs::directory_iterator(),
    [this](const bfs::path &p) { _condor.emplace_back(p.filename().string().c_str()); });
  DirectoryCreate(CONDOR2NAV_LK8000_TEMPLATES_DIR);
}


auto condor2nav::CLKMapsDB::LKMTemplatesSync(const std::function<bool()> &abort) const -> CNamesList
{
  // fill the list of already downloaded LKMaps templates
  CNamesList lkLocal;
  std::for_each(bfs::directory_iterator(CONDOR2NAV_LK8000_TEMPLATES_DIR), bfs::directory_iterator(),
    [&](const bfs::path &p) { lkLocal.emplace_back(p.filename().string().c_str()); });
  sort(begin(lkLocal), end(lkLocal));

  // get the list of all LKMaps templates on LK8000 server
  _app.Log() << "Obtaining list of LK8000 maps templates..." << std::endl;
  CNamesList lkRemote;

  // temporary solution - read from a fixed file
  CIStream templates{LKM_TEMPLATES_INDEX_SERVER, LKM_TEMPLATES_INDEX_URL};
  while(templates.Buffer()) {
    std::string line;
    std::getline(templates.Buffer(), line);
    Trim(line);
    if(!line.empty())
      lkRemote.emplace_back(line.c_str());
  }
  sort(begin(lkRemote), end(lkRemote));

  // check if there are new LK8000 templates on the LK8000 server
  CNamesList diff;
  std::set_difference(begin(lkRemote), end(lkRemote), begin(lkLocal), end(lkLocal), std::back_inserter(diff));
  if(diff.size()) {
    // download new templates from LK8000 server
    _app.Log() << "Downloading new LK8000 maps templates..." << std::endl;
    CNamesList errors;
    for(auto &name : diff) {
      if(abort())
        break;
      try {
        _app.Log() << " - " << name << std::endl;
        Download("www.bware.it", LK8000_MAPS_URL / "TEMPLATES" / name.c_str(), CONDOR2NAV_LK8000_TEMPLATES_DIR / name.c_str());
      }
      catch(const EOperationFailed &ex) {
        _app.Error() << ex.what() << std::endl;
        errors.emplace_back(name);
      }
    }

    // remove errored templates if any
    for(auto &e : errors)
      lkRemote.erase(find(begin(lkRemote), end(lkRemote), e));
  }
  else {
    _app.Log() << "No new LK8000 maps templates found" << std::endl;
  }

  CNamesList result;
  set_union(begin(lkLocal), end(lkLocal), begin(lkRemote), end(lkRemote), std::back_inserter(result));
  return result;
}


auto condor2nav::CLKMapsDB::LandscapesMatch(CNamesList allTemplates) -> CParsersMap
{
  _app.Log() << "Looking for new/better maps match..." << std::endl;

  // fill a list of Condor sceneries data
  CParsersMap condor;
  for(const auto &name : _condor)
    condor[name] = std::make_shared<CFileParserINI>(CONDOR_TEMPLATES_DIR / name.c_str());

  // fill the list of already downloaded LKMaps
  CNamesList lkmLocal;
  CNamesList demLocal;
  if(exists(CONDOR2NAV_LK8000_MAPS_DIR)) {
    std::for_each(bfs::directory_iterator(CONDOR2NAV_LK8000_MAPS_DIR), bfs::directory_iterator(), [&](const bfs::path &p)
    {
      if(p.extension() == ".LKM")
        lkmLocal.emplace_back(p.stem().string().c_str());
      else if(p.extension() == ".DEM") {
        CStringNoCase file{p.stem().string().c_str()};
        demLocal.emplace_back(file.substr(0, file.find_last_of('_')));
      }
    });
  }
  sort(begin(lkmLocal), end(lkmLocal));
  sort(begin(demLocal), end(demLocal));

  CNamesList lkLocal;
  set_intersection(begin(lkmLocal), end(lkmLocal), begin(demLocal), end(demLocal), back_inserter(lkLocal));

  // prepare a list of all LKMaps templates data
  CParsersMap lk;
  for(const auto &name : allTemplates)
    lk[name] = std::make_shared<CFileParserINI>(CONDOR2NAV_LK8000_TEMPLATES_DIR / name.c_str());

  CParsersMap result;

  // do for all Condor maps
  for(auto &landscape : condor) {
    auto lonMin = Convert<double>(landscape.second->Value("", "LONMIN"));
    auto lonMax = Convert<double>(landscape.second->Value("", "LONMAX"));
    auto latMin = Convert<double>(landscape.second->Value("", "LATMIN"));
    auto latMax = Convert<double>(landscape.second->Value("", "LATMAX"));

    CStringNoCase landscapeName{landscape.first, 0, landscape.first.find_last_of('_')};
    auto &landscapeData = _sceneriesParser.Row(landscapeName.c_str(), 0, true);
    std::shared_ptr<CFileParserINI> bestMatch;
    
    // check for all maps
    for(auto &map : lk) {
      // check if there is a new LK map that covers all landscape area
      const bool insideArea = InsideArea(
        TLongitude{Convert<double>(map.second->Value("", "LONMIN"))}, TLongitude{Convert<double>(map.second->Value("", "LONMAX"))},
        TLatitude{Convert<double>(map.second->Value("", "LATMIN"))}, TLatitude{Convert<double>(map.second->Value("", "LATMAX"))},
        TLongitude{lonMin}, TLongitude{lonMax}, TLatitude{latMin}, TLatitude{latMax});
      if(insideArea) {
        // check if that map is better than already found
        if(!bestMatch || MapScale(*map.second) < MapScale(*bestMatch))
          bestMatch = map.second;
      }
    }

    if(bestMatch) {
      // set new map data in CSV file
      auto &newName = bestMatch->Value("", "NAME");
      landscapeData[CTranslator::CTarget::SCENERY_MAP_FILE] = newName + ".LKM";
      landscapeData[CTranslator::CTarget::SCENERY_TERRAIN_FILE] = newName + "_" + Convert(MapScale(*bestMatch)) + ".DEM";

      if(none_of(begin(lkLocal), end(lkLocal), [&](CStringNoCase &m){ return m == newName.c_str(); })) {
        _app.Log() << " - " << newName << " -> " << landscape.first << std::endl;
        // store in results
        result[newName.c_str()] = bestMatch;
      }
    }
  }

  if(result.empty())
    _app.Log() << "No new/better maps found" << std::endl;

  _app.Log() << "Updating '" << _sceneriesParser.Path() << "' file..." << std::endl;
  _sceneriesParser.Dump();

  return result;
}


void condor2nav::CLKMapsDB::LKMDownload(CParsersMap &maps, const std::function<bool()> &abort) const
{
  _app.Log() << "Downloading new LK8000 maps..." << std::endl;
  for(auto &map : maps) {
    try {
      bfs::path path = LK8000_MAPS_URL;
      if(map.second->Value("", "DIR") == "CONDOR")
        path /= "EUR/CONDOR.DIR";
      else
        path = path / map.second->Value("", "MAPZONE") / (map.second->Value("", "DIR") + ".DIR");

      auto download = [&](const std::string &name) {
        _app.Log() << " - " << name << std::endl;
        Download("www.bware.it", path / name, CONDOR2NAV_LK8000_MAPS_DIR / name, 180);
      };

      if(abort())
        return;
      download(map.second->Value("", "NAME") + ".LKM");

      if(abort())
        return;
      download(map.second->Value("", "NAME") + "_" + Convert(MapScale(*map.second)) + ".DEM");
    }
    catch(const EOperationFailed &ex) {
      _app.Error() << ex.what() << std::endl;
    }
  }
}
