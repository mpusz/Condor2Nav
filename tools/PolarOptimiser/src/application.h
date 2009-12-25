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
 * @file application.h
 *
 * @brief Application class declaration.
**/

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "polarXCSoar.h"
#include <vector>

namespace polarOptimiser {

  class CApplication {
    typedef std::vector<double> CDataArray;

    double _massDryGross;
    unsigned _waterBallastLitersMax;
    std::auto_ptr<CPolar> _polar;

    void PolarHeader() const;
    void PolarLine(double speed, double ballast) const;
    void PolarFooter() const;

    void PolarFileRead(const std::string &fileName, CDataArray &data);

    void SpeedPolar() const;
    void Sink() const;
    double BestWeightCalculateUsingWaterBallast() const;
    void WinPilotDump() const;

  public:
    CApplication(const std::string &fileName);
    void Run();
  };

} // namespace polarOptimiser

#endif // __APPLICATION_H__
