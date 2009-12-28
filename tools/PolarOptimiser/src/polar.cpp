//
// This file is part of PolarOptimiser gliders polar files optimisation helper.
//
// Copyright (C) 2009 Mateusz Pusz
//
// PolarOptimiser is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// PolarOptimiser is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with PolarOptimiser. If not, see <http://www.gnu.org/licenses/>.
//
// Visit the project webpage (http://sf.net/projects/condor2nav) for more info.
//

/**
 * @file polar.cpp
 *
 * @brief Polar class definition. 
**/

#include "polar.h"

/**
 * @brief Class destructor
 *
 * polarOptimiser::CPolar class destructor.
 */
polarOptimiser::CPolar::~CPolar()
{
}


/**
 * @brief Obtains sink for specified speed from polar curve
 *
 * Method obtains sink for specified speed from polar curve.
 *
 * @param speed Speed to be used for polar curve point
 *
 * @return Sink for specified speed from polar curve.
 */
double polarOptimiser::CPolar::Sink(double speed) const
{
  return Sink(speed, 200, 0);  // any weight is good here
}
