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
 * @file polar.h
 *
 * @brief Polar class declaration.
**/

#ifndef __POLAR_H__
#define __POLAR_H__

namespace polarOptimiser {

  /**
   * @brief Polar curve equation implementation.
   *
   * polarOptimiser::CPolar class is responsible for glider speed polar curve equation
   * implementation. Such an equation should take speed and weights as inputs and produce
   * sink value as an output.
   */
  class CPolar {
  public:
    virtual ~CPolar();

    /**
     * @brief Returns polar curve speed parameter value
     *
     * Method returns polar curve speed parameter value. It is the value stored internally
     * as a speed polar curve point.
     *
     * @param idx Index of a speed value to return.
     *
     * @return Polar curve speed parameter value
     */
    virtual double Speed(unsigned idx) const = 0;

    /**
     * @brief Calculates sink polar curve value
     *
     * Method calculates sink polar curve value.
     *
     * @param speed Speed value from polar curve to use
     * @param weight Glider + pilot mass
     * @param ballastLitres The amount of water ballast to set for calculations
     *
     * @return Polar curve sink value.
     */
    virtual double Sink(double speed, double weight, double ballastLitres) const = 0;
    double Sink(double speed) const;
  };

} // namespace polarOptimiser

#endif // __POLAR_H__
