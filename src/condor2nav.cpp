//
// This file is part of Condor2Nav file formats translator.
//
// Copyright (C) 2009-2010 Mateusz Pusz
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
#include "translator.h"


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


/**
 * @brief Class destructor. 
 */
condor2nav::CCondor2Nav::CLogger::~CLogger()
{

}


/**
 * @brief Returns the type of the logger.
 *
 * @return The type of the logger.
 */
condor2nav::CCondor2Nav::CLogger::TType condor2nav::CCondor2Nav::CLogger::Type() const
{
  return _type;
}


/**
 * @brief Class destructor. 
 */
condor2nav::CCondor2Nav::~CCondor2Nav()
{

}
