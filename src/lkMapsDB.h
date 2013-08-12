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
 * @file lkMapsDB.h
 *
 * @brief LK8000 maps database.
 */

#ifndef __LKMAPSDB_H__
#define __LKMAPSDB_H__

#include "nonCopyable.h"
#include "traitsNoCase.h"
#include "fileParserCSV.h"
#include <vector>
#include <map>

namespace boost {
  namespace filesystem {
    class path;
  }
}

namespace condor2nav {

  class CCondor2Nav;
  class CFileParserINI;

  /**
   * @brief Input stream wrapper
   *
   * condor2nav::CIStream class is a wrapper for different stream types.
   */
  class CLKMapsDB : CNonCopyable {
  public:
    typedef std::vector<CStringNoCase> CNamesList;
    typedef std::map<CStringNoCase, std::shared_ptr<CFileParserINI>> CParsersMap;
  private:
    static const boost::filesystem::path CONDOR_TEMPLATES_DIR;
    static const boost::filesystem::path CONDOR2NAV_LK8000_TEMPLATES_DIR;
    static const boost::filesystem::path CONDOR2NAV_LK8000_MAPS_DIR;
    static const boost::filesystem::path LK8000_MAPS_URL;
    static const std::string             LKM_TEMPLATES_INDEX_SERVER;
    static const boost::filesystem::path LKM_TEMPLATES_INDEX_URL;

    const CCondor2Nav &_app;
    CFileParserCSV _sceneriesParser;
    CNamesList _condor;
  public:
    explicit CLKMapsDB(const CCondor2Nav &app);
    CNamesList LKMTemplatesSync(const std::function<bool()> &abort) const;
    CParsersMap LandscapesMatch(CNamesList allTemplates);
    void LKMDownload(CParsersMap &maps, const std::function<bool()> &abort) const;
  };

}

#endif /* __LKMAPSDB_H__ */
