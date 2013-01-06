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

const boost::filesystem::path condor2nav::CLKMapsDB::CONDOR_TEMPLATES_DIR            = "data/Landscapes";
const boost::filesystem::path condor2nav::CLKMapsDB::CONDOR2NAV_LK8000_TEMPLATES_DIR = "data/LK8000/LKMTemplates";
const boost::filesystem::path condor2nav::CLKMapsDB::CONDOR2NAV_LK8000_MAPS_DIR      = "data/LK8000/_Maps/condor2nav";
const boost::filesystem::path condor2nav::CLKMapsDB::LK8000_MAPS_URL                 = "/listing/LKMAPS";
const std::string             condor2nav::CLKMapsDB::LKM_TEMPLATES_INDEX_SERVER      = "cloud.github.com";
const boost::filesystem::path condor2nav::CLKMapsDB::LKM_TEMPLATES_INDEX_URL         = "/downloads/mpusz/Condor2Nav/LKMTemplates.txt";


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
  DirectoryCreate(CONDOR2NAV_LK8000_TEMPLATES_DIR);
}


auto condor2nav::CLKMapsDB::LKMTemplatesSync(const std::function<bool()> &abort) const -> CNamesList
{
  // fill the list of already downloaded LKMaps templates
  CNamesList lkLocal;
  std::for_each(boost::filesystem::directory_iterator(CONDOR2NAV_LK8000_TEMPLATES_DIR), boost::filesystem::directory_iterator(),
    [&](const boost::filesystem::path &p) { lkLocal.push_back(p.filename().string().c_str()); });
  std::sort(lkLocal.begin(), lkLocal.end());

  // get the list of all LKMaps templates on LK8000 server
  _app.Log() << "Obtaining list of LK8000 maps templates..." << std::endl;
  CNamesList lkRemote;

  // temporary solution - read from a fixed file
  CIStream templates(LKM_TEMPLATES_INDEX_SERVER, LKM_TEMPLATES_INDEX_URL);
  while(templates.Buffer()) {
    std::string line;
    std::getline(templates.Buffer(), line);
    Trim(line);
    if(!line.empty())
      lkRemote.push_back(line.c_str());
  }
  std::sort(lkRemote.begin(), lkRemote.end());

  // check if there are new LK8000 templates on the LK8000 server
  CNamesList diff;
  std::set_difference(lkRemote.begin(), lkRemote.end(), lkLocal.begin(), lkLocal.end(), std::back_inserter(diff));
  if(diff.size()) {
    // download new templates from LK8000 server
    _app.Log() << "Downloading new LK8000 maps templates..." << std::endl;
    CNamesList errors;
    std::for_each(diff.begin(), diff.end(),
      [&](const CStringNoCase &name)
    {
      if(abort())
        return;
      try {
        _app.Log() << " - " << name << std::endl;
        Download("www.bware.it", LK8000_MAPS_URL / "TEMPLATES" / name.c_str(), CONDOR2NAV_LK8000_TEMPLATES_DIR / name.c_str());
      }
      catch(const EOperationFailed &ex) {
        _app.Log() << ex.what() << std::endl;
        errors.push_back(name);
      }
    });

    // remove errored templates if any
    std::for_each(errors.begin(), errors.end(),
          [&](const CStringNoCase &error) { lkRemote.erase(std::find(lkRemote.begin(), lkRemote.end(), error)); });
  }
  else {
    _app.Log() << "No new LK8000 maps templates found" << std::endl;
  }

  CNamesList result;
  std::set_union(lkLocal.begin(), lkLocal.end(), lkRemote.begin(), lkRemote.end(), std::back_inserter(result));
  return result;
}


auto condor2nav::CLKMapsDB::LandscapesMatch(CNamesList &&allTemplates) -> CParsersMap
{
  _app.Log() << "Looking for new/better maps match..." << std::endl;

  CParsersMap result;

  // fill a list of Condor sceneries data
  CParsersMap condor;
  std::for_each(_condor.begin(), _condor.end(),
    [&](const CStringNoCase &n) { condor[n] = std::make_shared<CFileParserINI>(CONDOR_TEMPLATES_DIR / n.c_str()); });

  // fill the list of already downloaded LKMaps
  CNamesList lkmLocal;
  CNamesList demLocal;
  if(boost::filesystem::exists(CONDOR2NAV_LK8000_MAPS_DIR))
    std::for_each(boost::filesystem::directory_iterator(CONDOR2NAV_LK8000_MAPS_DIR), boost::filesystem::directory_iterator(),
      [&](const boost::filesystem::path &p)
  {
    if(p.extension() == ".LKM")
      lkmLocal.push_back(p.stem().string().c_str());
    else if(p.extension() == ".DEM") {
      CStringNoCase file = p.stem().string().c_str();
      demLocal.push_back(file.substr(0, file.find_last_of('_')));
    }
  });
  std::sort(lkmLocal.begin(), lkmLocal.end());
  std::sort(demLocal.begin(), demLocal.end());

  CNamesList lkLocal;
  std::set_intersection(lkmLocal.begin(), lkmLocal.end(), demLocal.begin(), demLocal.end(), std::back_inserter(lkLocal));

  // prepare a list of all LKMaps templates data
  CParsersMap lk;
  std::for_each(allTemplates.begin(), allTemplates.end(),
    [&](CStringNoCase &n) { lk[std::move(n)] = std::make_shared<CFileParserINI>(CONDOR2NAV_LK8000_TEMPLATES_DIR / n.c_str()); });

  // do for all Condor maps
  for(auto landscape=condor.begin(); landscape!=condor.end(); ++landscape) {
    double lonMin = Convert<double>(landscape->second->Value("", "LONMIN"));
    double lonMax = Convert<double>(landscape->second->Value("", "LONMAX"));
    double latMin = Convert<double>(landscape->second->Value("", "LATMIN"));
    double latMax = Convert<double>(landscape->second->Value("", "LATMAX"));

    CStringNoCase landscapeName(landscape->first, 0, landscape->first.find_last_of('_'));
    auto &landscapeData = _sceneriesParser.Row(landscapeName.c_str(), 0, true);
    std::shared_ptr<CFileParserINI> bestMatch;
    
    // check for all maps
    for(auto map=lk.begin(); map!=lk.end(); ++map) {
      // check if there is a new LK map that covers all landscape area
      if(InsideArea(
        Convert<double>(map->second->Value("", "LONMIN")), Convert<double>(map->second->Value("", "LONMAX")),
        Convert<double>(map->second->Value("", "LATMIN")), Convert<double>(map->second->Value("", "LATMAX")),
        lonMin, lonMax, latMin, latMax))
      {
        // check if that map is better than already found
        if(!bestMatch || MapScale(*map->second) < MapScale(*bestMatch))
          bestMatch = map->second;
      }
    }

    if(bestMatch) {
      // set new map data in CSV file
      auto &newName = bestMatch->Value("", "NAME");
      landscapeData[CTranslator::CTarget::SCENERY_MAP_FILE] = newName + ".LKM";
      landscapeData[CTranslator::CTarget::SCENERY_TERRAIN_FILE] = newName + "_" + Convert(MapScale(*bestMatch)) + ".DEM";

      if(std::none_of(lkLocal.begin(), lkLocal.end(), [&](CStringNoCase &m){ return m == newName.c_str(); })) {
        _app.Log() << " - " << newName << " -> " << landscape->first << std::endl;

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
  for(auto map=maps.begin(); map!=maps.end(); ++map) {
    try {
      boost::filesystem::path path = LK8000_MAPS_URL;
      if(map->second->Value("", "DIR") == "CONDOR")
        path /= "EUR/CONDOR.DIR";
      else
        path = path / map->second->Value("", "MAPZONE") / (map->second->Value("", "DIR") + ".DIR");

      if(abort())
        return;
      std::string name = map->second->Value("", "NAME") + ".LKM";
      _app.Log() << " - " << name << std::endl;
      Download("www.bware.it", path / name, CONDOR2NAV_LK8000_MAPS_DIR / name, 180);

      if(abort())
        return;
      name = map->second->Value("", "NAME") + "_" + Convert(MapScale(*map->second)) + ".DEM";
      _app.Log() << " - " << name << std::endl;
      Download("www.bware.it", path / name, CONDOR2NAV_LK8000_MAPS_DIR / name, 180);
    }
    catch(const EOperationFailed &ex) {
      _app.Log() << ex.what() << std::endl;
    }
  }
}
