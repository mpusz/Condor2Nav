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
 * @file condor2navCLI.h
 *
 * @brief Declares the condor2nav::CCondor2NavCLI class. 
 */

#ifndef __CONDOR2NAV_CLI_H__
#define __CONDOR2NAV_CLI_H__

#include "condor2nav.h"
#include <boost/filesystem.hpp>

/**
 * @brief Condor2Nav project namespace.
 */
namespace condor2nav {

  class CCondor;

  /**
   * @brief Condor2Nav CLI interface namespace.
   */
  namespace cli {

    /**
     * @brief Main CLI project class.
     *
     * condor2nav::CCondor2NavCLI is a main CLI project class. It is responsible for
     * command line handling and running the translation.
     */
    class CCondor2NavCLI : public CCondor2Nav {
    public:

      /**
       * @brief Logger class
       *
       * Class is responsible for logging Condor2Nav traces on the console output
       */
      class CLogger : public CCondor2Nav::CLogger {
        virtual void Dump(const std::string &str) const override;
      public:
        CLogger(TType type);
      };

    private:
      CLogger _normal;              ///< @brief Normal logging level logger
      CLogger _warning;             ///< @brief Warning logging level logger
      CLogger _error;               ///< @brief Error logging level logger

      void Usage() const;
      void CLIParse(int argc, const char *argv[], TFPLType &fplType, boost::filesystem::path &fplPath, unsigned &aatTime) const;
      bool AATCheck(const CCondor &condor, unsigned &aatTime) const;

    public:
      CCondor2NavCLI();

      virtual const CLogger &Log() const override { return _normal; }
      virtual const CLogger &Warning() const override { return _warning; }
      virtual const CLogger &Error() const override { return _error; }

      int Run(int argc, const char *argv[]) const;
    };

  } // namespace cli

} // namespace condor2nav

#endif /* __CONDOR2NAV_CLI_H__ */
