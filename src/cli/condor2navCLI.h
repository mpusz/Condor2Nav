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
 * @file condor2navCLI.h
 *
 * @brief Declares the condor2nav::CCondor2NavCLI class. 
**/

#ifndef __CONDOR2NAV_CLI_H__
#define __CONDOR2NAV_CLI_H__

#include "condor2nav.h"

/**
* @brief Condor2Nav project namespace.
**/
namespace condor2nav {

  namespace cli {

    /**
    * @brief Main project class.
    *
    * condor2nav::CCondor2NavCLI is a main project class. It is responsible for
    * command line handling and running the translation.
    **/
    class CCondor2NavCLI : public CCondor2Nav {
    public:
      class CLogger : public CCondor2Nav::CLogger {
        virtual void Dump(const std::string &str) const;
      public:
        CLogger(TType type): CCondor2Nav::CLogger(type) {}
      };

    private:
      CLogger _normal;
      CLogger _warning;
      CLogger _error;

      void Usage() const;
      void CLIParse(int argc, const char *argv[], TFPLType &fplType, std::string &fplPath, unsigned &aatTime) const;

    public:
      CCondor2NavCLI();
      int Run(int argc, const char *argv[]) const;

      virtual const CLogger &Log() const { return _normal; }
      virtual const CLogger &Warning() const { return _warning; }
      virtual const CLogger &Error() const { return _error; }
    };

  } // namespace cli

} // namespace condor2nav

#endif /* __CONDOR2NAV_CLI_H__ */