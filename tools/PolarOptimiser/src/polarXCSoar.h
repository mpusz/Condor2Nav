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
 * @file polarXCSoar.h
 *
 * @brief XCSoar Polar class declaration.
**/

#ifndef __POLARXCSOAR_H__
#define __POLARXCSOAR_H__

#include "polar.h"

namespace polarOptimiser {

  class CPolarXCSoar : public CPolar {
    double _speed[3];
    double _polar[3];
    double SinkRate(double a, double b, double c, double MC, double HW, double V) const;
  public:
    CPolarXCSoar(const double (&speed)[3], const double (&sink)[3]);
    virtual double Speed(unsigned idx) const;
    virtual double Sink(double speed, double weight, double ballastLitres) const;
  };

} // namespace polarOptimiser

#endif // __POLARXCSOAR_H__
