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
 * @file condor2nav.h
 *
 * @brief Declares the condor2nav::CCondor2Nav class. 
**/

#ifndef __CONDOR2NAV_H__
#define __CONDOR2NAV_H__

#include <sstream>

/**
 * @brief Condor2Nav project namespace.
**/
namespace condor2nav {

  class CFileParserINI;

  /**
   * @brief Main project class.
   *
   * condor2nav::CCondor2Nav is a main project class. It is responsible for
   * command line handling and running the translation.
  **/
  class CCondor2Nav {
  public:
    /**
    * @brief Values that represent different types of supported flight plan types. 
    **/
    enum TFPLType {
      TYPE_DEFAULT,	          ///< @brief Default task name (from INI) file will be run. 
      TYPE_RESULT,	          ///< @brief Use last flown FPL from race result directory.
      TYPE_USER	              ///< @brief User provided exact path to FPL file. 
    };

    class CLogger {
    public:
      enum TType {
        TYPE_NORMAL,
        TYPE_WARNING,
        TYPE_ERROR
      };

    private:
      const TType _type;

      CLogger(const CLogger &);
      CLogger &operator=(const CLogger &);
      virtual void Dump(const std::string &str) const = 0;

    protected:
      TType Type() const { return _type; }

    public:
      CLogger(TType type): _type(type) {}
      virtual ~CLogger() {}
      
      /**
      * @brief Dumps new data to a stream. 
      *
      * Function writes new data to a stream.
      *
      * @param stream Stream to use. 
      * @param obj Data to write. 
      *
      * @return Stream instance.
      **/
      template<class T>
      friend const CLogger &operator<<(const CLogger &logger, const T &obj)
      {
        std::stringstream stream;
        stream << obj;
        logger.Dump(stream.str());
        return logger;
      }

      /**
      * @brief Writes functor (e.g. std::endl) to a stream.
      *
      * Method writes functor (e.g. std::endl) to a stream.
      *
      * @param stream Stream to use. 
      * @param f Functor to write. 
      *
      * @return Stream instance.
      **/
      friend const CLogger &operator<<(const CLogger &logger, std::ostream &(*f)(std::ostream &))
      {
        std::stringstream stream;
        stream << f;
        logger.Dump(stream.str());
        return logger;
      }
    };

  protected:
    static const char *CONFIG_FILE_NAME;          ///< @brief The name of the configuration INI file.

  public:
    virtual const CLogger &Log() const = 0;
    virtual const CLogger &Warning() const = 0;
    virtual const CLogger &Error() const = 0;
  };

}

#endif /* __CONDOR2NAV_H__ */