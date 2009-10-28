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
 * @file translator.cpp
 *
 * @brief Implements the translators hierarchy base class (condor2nav::CTranslator). 
**/

#include "translator.h"
#include "tools.h"


/**
 * @brief Class constructor.
 *
 * condor2nav::CTranslator class constructor.
 *
 * @param outputPath Translation output directory.
**/
condor2nav::CTranslator::CTranslator(const std::string &outputPath):
_outputPath(outputPath)
{
  DirectoryCreate(_outputPath);
}


/**
 * @brief Class destructor.
 *
 * condor2nav::CTranslator class destructor.
**/
condor2nav::CTranslator::~CTranslator()
{
}


/**
 * @brief Returns translation output directory.
 *
 * Method returns translation output directory.
 *
 * @return Translation output directory. 
**/
const std::string &condor2nav::CTranslator::OutputPath() const
{
  return _outputPath;
}
