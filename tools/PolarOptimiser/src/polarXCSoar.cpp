//
// This file is part of PolarOptimiser gliders polar files optimisation helper.
//
// Copyright (C) 2009 Mateusz Pusz
//
//  XCSoar Glide Computer - http://xcsoar.sourceforge.net/
//  Copyright (C) 2000 - 2008
//
//  M Roberts (original release)
//  Robin Birch <robinb@ruffnready.co.uk>
//  Samuel Gisiger <samuel.gisiger@triadis.ch>
//  Jeff Goodenough <jeff@enborne.f2s.com>
//  Alastair Harrison <aharrison@magic.force9.co.uk>
//  Scott Penrose <scottp@dd.com.au>
//  John Wharington <jwharington@gmail.com>
//  Lars H <lars_hn@hotmail.com>
//  Rob Dunning <rob@raspberryridgesheepfarm.com>
//  Russell King <rmk@arm.linux.org.uk>
//  Paolo Ventafridda <coolwind@email.it>
//  Tobias Lohner <tobias@lohner-net.de>
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
 * @file polarXCSoar.cpp
 *
 * @brief XCSoar Polar class definition. 
**/

#include "polarXCSoar.h"
#include <string>
#include <cmath>


/**
 * @brief Class constructor
 *
 * polarOptimiser::CPolarXCSoar class constructor.
 *
 * @param speed An array of speed polar curve values.
 * @param sink An array of sink polar curve values.
 */
polarOptimiser::CPolarXCSoar::CPolarXCSoar(const double (&speed)[3], const double (&sink)[3])
{
  for(unsigned i=0; i<3; i++)
    _speed[i] = speed[i];

  // XCSoar specific calculations
  double d;
  double v1, v2, v3;
  double w1, w2, w3;

  v1 = speed[0]/3.6;
  v2 = speed[1]/3.6;
  v3 = speed[2]/3.6;
  w1 = sink[0];
  w2 = sink[1];
  w3 = sink[2];

  d = v1*v1*(v2-v3)+v2*v2*(v3-v1)+v3*v3*(v1-v2);
  if(d == 0.0)
    _polar[0] = 0;
  else
    _polar[0] = ((v2-v3)*(w1-w3)+(v3-v1)*(w2-w3))/d;

  d = v2-v3;
  if(d == 0.0)
    _polar[1] = 0;
  else
    _polar[1] = (w2-w3-_polar[0]*(v2*v2-v3*v3))/d;

  _polar[2] = (double)(w3-_polar[0]*v3*v3-_polar[1]*v3);
}


/**
 * @brief XCSoar speed polar curve equation
 *
 * Method implements XCSoar speed polar curve equation.
 *
 * @param a Equation argument
 * @param b Equation argument
 * @param c Equation argument
 * @param MC MC value to use for calculations
 * @param HW Horizontal wind value to be used for calculations
 * @param V Speed in m/s
 *
 * @return Calculated sink value in m/s
 */
double polarOptimiser::CPolarXCSoar::SinkRate(double a, double b, double c, double MC, double HW, double V) const
{
  double temp;

  // Quadratic form: w = c+b*(V)+a*V*V
  temp =  a*(V+HW)*(V+HW);
  temp += b*(V+HW);
  temp += c-MC;

  return temp;
}


/**
* @brief Returns XCSoar polar curve speed parameter value
*
* Method returns XCSOar polar curve speed parameter value. It is the value stored
* internally as a speed polar curve point.
*
* @param idx Index of a speed value to return.
*
* @return XCSoar polar curve speed parameter value
*/
double polarOptimiser::CPolarXCSoar::Speed(unsigned idx) const
{
  if(idx > 2)
    throw std::out_of_range("Error: Polar speed out of range (0-2)!!!");
  return _speed[idx];
}


/**
* @brief Calculates XCSoar sink polar curve value
*
* Method calculates XCSoar sink polar curve value.
*
* @param speed Speed value from polar curve to use
* @param weight Glider + pilot mass
* @param ballastLitres The amount of water ballast to set for calculations
*
* @return XCSoar polar curve sink value.
*/
double polarOptimiser::CPolarXCSoar::Sink(double speed, double weight, double ballastLitres) const
{
  // now scale off weight
  double weights[3] = {0};
  double polar[3];
  
  for(unsigned i=0; i<3; i++)
    polar[i] = _polar[i];

  // XCSoar specific calculations
  weights[0] = 70;                     // Pilot weight
  weights[1] = weight - weights[0];    // Glider empty weight
  weights[2] = ballastLitres;          // Ballast weight

  polar[0] = polar[0] * (double)sqrt(weights[0] + weights[1]);
  polar[2] = polar[2] / (double)sqrt(weights[0] + weights[1]);

  double ballast = sqrt(ballastLitres + weights[0] + weights[1]);

  double polar_a = polar[0] / ballast;
  double polar_b = polar[1];
  double polar_c = polar[2] * ballast;
  
  return SinkRate(polar_a, polar_b, polar_c, 0, 0, speed/3.6);
}
