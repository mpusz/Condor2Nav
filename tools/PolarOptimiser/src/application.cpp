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
 * @file application.cpp
 *
 * @brief Application class definition.
**/

#include "application.h"
#include "tools.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <set>


/**
 * @brief Class constructor
 *
 * polarOptimiser::CApplication class constructor responsible for polar curve creation.
 *
 * @param fileName WinPilot polar file
 */
polarOptimiser::CApplication::CApplication(const std::string &fileName)
{
  // get polar curve data
  CDataArray data;
  PolarFileRead(fileName, data);

  if(data.size() < 8 || data.size() % 2 != 0)
    throw std::runtime_error("ERROR: Invalid number of data specified in POLAR file!!!");

  // set equation parameters
  _massDryGross = data.at(0);
  _waterBallastLitersMax = static_cast<unsigned>(data.at(1));

  double speed[3];
  double sink[3];
  unsigned idx0 = 2, idx1 = 4, idx2 = 6;
  double bestError = 0xFFFF;
  if(data.size() > 8) {
    // look for the best 3 polar curve points
    for(unsigned i=2; i<data.size(); i+=2) {
      speed[0] = data.at(i);
      sink[0] = data.at(i + 1);
      for(unsigned j=i + 2; j<data.size(); j+=2) {
        speed[1] = data.at(j);
        sink[1] = data.at(j + 1);
        for(unsigned k=j + 2; k<data.size(); k+=2) {
          speed[2] = data.at(k);
          sink[2] = data.at(k + 1);

          _polar = std::auto_ptr<CPolarXCSoar>(new CPolarXCSoar(speed, sink));
          double error = 0;
          for(unsigned l=0; l<(data.size() - 2) / 2; l++)
            error += abs(_polar->Sink(data.at(l * 2 + 2), data.at(0), 0) - data.at(l * 2 + 3));
          if(error < bestError) {
            bestError = error;
            idx0 = i;
            idx1 = j;
            idx2 = k;
          }
        }
      }
    }
  }

  // assign 3 polar curve points
  speed[0] = data[idx0];
  sink[0] = data[idx0 + 1];
  speed[1] = data[idx1];
  sink[1] = data[idx1 + 1];
  speed[2] = data[idx2];
  sink[2] = data[idx2 + 1];
  _polar = std::auto_ptr<CPolarXCSoar>(new CPolarXCSoar(speed, sink));

  if(data.size() > 8) {
    // print calculation values and errors
    std::cout << "Best polar" << std::endl;
    PolarHeader(true);
    for(unsigned l=0; l<(data.size() - 2) / 2; l++)
      PolarLine(data.at(l * 2 + 2), _massDryGross, 0, true, data.at(l * 2 + 3));
    PolarFooter(true);
    std::cout << std::endl;
    std::cout << "Gross sink error: " << bestError << "m/s" << std::endl;
  }
}


/**
 * @brief Prints speed polar table header
 *
 * Method prints speed polar table header.
 *
 * @param sinkError Specifies if sink calculation error should be printed
 */
void polarOptimiser::CApplication::PolarHeader(bool sinkError /* = false */) const
{
  std::cout << std::setfill('-') << std::setw(16 * (sinkError ? 4 : 3) + 1) << "" << std::endl;
  std::cout << "| Speed [km/h]  |   Sink [m/s]  |       LD      |";
  if(sinkError)
    std::cout << "   Sink Error  |";
  std::cout << std::endl;
  std::cout << std::setfill('-') << std::setw(16 * (sinkError ? 4 : 3) + 1) << "" << std::endl;
}


/**
 * @brief Prints one speed polar table line
 * 
 * Method prints one speed polar table line.
 *
 * @param speed Speed to be used for calculation
 * @param weight Weight to be used for calculation
 * @param ballast Water ballast to be used for calculation
 * @param sinkError Specifies if sink calculation error should be printed
 * @param expSink Expected sink rate to be used for error calculation
 */
void polarOptimiser::CApplication::PolarLine(double speed, double weight, double ballast, bool sinkError /* = false */, double expSink /* = 0 */) const
{
  double sink = _polar->Sink(speed, weight, ballast);
  std::cout << std::setfill(' ') << "| " << std::setw(13) << speed << " | " <<
    std::fixed << std::setprecision(3) << std::setw(13) << sink << " | " <<
    std::setprecision(2) << std::setw(13) << (speed / 3.6 / (-sink)) << " |";
  if(sinkError)
    std::cout << " " << std::setw(13) << std::setprecision(4) << sink - expSink << " |";
  std::cout.unsetf(std::ios_base::fixed);
  std::cout.precision(0);
  std::cout << std::endl;
}


/**
 * @brief Prints speed polar table footer
 *
 * Method prints speed polar table footer.
 *
 * @param sinkError Specifies if sink calculation error should be printed
 */
void polarOptimiser::CApplication::PolarFooter(bool sinkError /* = false */) const
{
  std::cout << std::setfill('-') << std::setw(16 * (sinkError ? 4 : 3) + 1) << "" << std::endl;
}


/**
 * @brief Reads WinPilot style polar file
 *
 * Method reads WinPilot style polar file.
 *
 * @param fileName The name of polar file
 * @param data An array to store read data.
 */
void polarOptimiser::CApplication::PolarFileRead(const std::string &fileName, CDataArray &data)
{
  // open Polar file
  std::ifstream inputStream(fileName.c_str());
  if(!inputStream)
    throw std::runtime_error("ERROR: Couldn't open Polar file '" + fileName + "' for reading!!!");

  data.clear();
  data.reserve(8);

  // parse all lines
  std::string line;
  while(getline(inputStream, line)) {
    if(line.empty() || line[0] == '*')
      continue;

    size_t pos = 0;
    do {
      size_t posOld = pos;
      pos = line.find_first_of(",", posOld);
      size_t len = (pos != std::string::npos) ? (pos - posOld) : pos;
      std::string value = line.substr(posOld, len);
      data.push_back(Convert<double>(value));
      if(pos != std::string::npos)
        pos++;
    }
    while(pos != std::string::npos);
  }
}


/**
 * @brief Prints speed polar table
 *
 * Method prints speed polar table.
 */
void polarOptimiser::CApplication::SpeedPolar() const
{
  double ballast = 0;
  if(_waterBallastLitersMax > 0) {
    std::cout << "Water Ballast <0, " << _waterBallastLitersMax << "> [liters]:" << std::endl;
    std::cin >> ballast;
    if(ballast > _waterBallastLitersMax)
      throw std::out_of_range("ERROR: Maximum water ballast exceeded!!!");
  }

  unsigned speedMin;
  std::cout << "Speed Low [km/h]:" << std::endl;
  std::cin >> speedMin;

  unsigned speedMax;
  std::cout << "Speed High [km/h]:" << std::endl;
  std::cin >> speedMax;
  std::cout << std::endl;

  // find best LD
  double ld = 0;
  unsigned ldSpeed;
  for(ldSpeed=speedMin; ldSpeed<=speedMax; ldSpeed+=1) {
    double ldLast = ld;
    ld = ldSpeed / 3.6 / (-_polar->Sink(ldSpeed, _massDryGross, ballast));
    if(ld < ldLast) {
      ldSpeed--;
      ld = ldLast;
      break;
    }
  }
  
  // prepare speed range
  std::set<unsigned> speeds;
  for(unsigned speed=speedMin; speed<=speedMax; speed+=5)
    speeds.insert(speed);
  for(unsigned speed=ldSpeed-5; speed<=ldSpeed+5; speed+=1)
    speeds.insert(speed);

  PolarHeader();
  for(std::set<unsigned>::iterator it=speeds.begin(); it!=speeds.end(); ++it) {
    PolarLine(*it, _massDryGross, ballast);
  }
  PolarFooter();

  std::cout << std::endl;
  std::cout << "Best LD = " << ld << " (Speed: " << ldSpeed << "km/h; Sink: " << _polar->Sink(ldSpeed, _massDryGross, ballast) << "m/s)" << std::endl;
}


/**
 * @brief Calculates sink for provided speed
 *
 * Method calculates sink for provided speed
 */
void polarOptimiser::CApplication::Sink() const
{
  double ballast = 0;
  if(_waterBallastLitersMax > 0) {
    std::cout << "Water Ballast <0, " << _waterBallastLitersMax << "> [liters]:" << std::endl;
    std::cin >> ballast;
    if(ballast > _waterBallastLitersMax)
      throw std::out_of_range("ERROR: Maximum water ballast exceeded!!!");
  }

  unsigned speed;
  std::cout << "Speed [km/h]:" << std::endl;
  std::cin >> speed;

  double sink = _polar->Sink(speed, _massDryGross, ballast);
  std::cout << std::endl;
  std::cout << "Sink = " << sink << "m/s" << std::endl;
  std::cout << "LD   = " << speed / 3.6 / (-sink) << std::endl;
}


/**
 * @brief Calculates the best MassDryGross value based on MAX Water Ballast speed polar
 *
 * Method calculates the best MassDryGross value based on MAX Water Ballast speed polar.
 *
 * @return The best MAssDryGross.
 */
double polarOptimiser::CApplication::BestWeightCalculateUsingWaterBallast() const
{
  std::cout << "Please provide 3 polar points for the \"max water ballast\" curve" << std::endl;
  std::cout << "Sink speeds should be provided as negative values" << std::endl;
  std::cout << std::endl;

  double speedFull[3] = {0};
  double sinkFull[3] = {0};
  for(unsigned i=0; i<3; i++) {
    std::cout << "Speed " << i << " [km/h]: ";
    std::cin >> speedFull[i];
    std::cout << "Sink " << i << " [m/s]: ";
    std::cin >> sinkFull[i];
  }

  unsigned weight;
  double error = 0xFFFF;
  for(weight=10; weight<500; weight++) {
    double errorLast = error;
    error = 0;
    for(unsigned i=0; i<3; i++)
      error += abs(_polar->Sink(speedFull[i], weight, _waterBallastLitersMax) - sinkFull[i]);
    if(error > errorLast) {
      error = errorLast;
      weight--;
      break;
    }
  }
  std::cout << std::endl;
  std::cout << "The best weight [kg]: " << weight << std::endl;
  std::cout << "Gross sink error: " << error << "m/s" << std::endl;
  std::cout << std::endl;

  std::cout << "Full ballast calculated polar" << std::endl;
  PolarHeader(true);
  for(unsigned i=0; i<3; i++)
    PolarLine(speedFull[i], weight, _waterBallastLitersMax, true, sinkFull[i]);
  PolarFooter(true);
  std::cout << std::endl;

  return weight;
}


/**
 * @brief Dumps polar curve in WinPilot file format
 *
 * Method dumps polar curve in WinPilot file format.
 */
void polarOptimiser::CApplication::WinPilotDump() const
{
  std::cout << "***************************************************************************************************" << std::endl;
  std::cout << "* WinPilot POLAR file generated with PolarOptimiser PolarOptimiser" << std::endl;
  std::cout << "*" << std::endl;
  std::cout << "* MassDryGross[kg], MaxWaterBallast[liters], Speed1[km/h], Sink1[m/s], Speed2, Sink2, Speed3, Sink3" << std::endl;
  std::cout << "***************************************************************************************************" << std::endl;
  std::cout << _massDryGross << "," << _waterBallastLitersMax << "," <<
    _polar->Speed(0) << "," << _polar->Sink(_polar->Speed(0)) << "," <<
    _polar->Speed(1) << "," << _polar->Sink(_polar->Speed(1)) << "," <<
    _polar->Speed(2) << "," << _polar->Sink(_polar->Speed(2)) << std::endl;
}


/**
 * @brief Application main loop
 *
 * Method implements application main loop.
 */
void polarOptimiser::CApplication::Run()
{
  bool exit = false;

  while(!exit) {
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Actions:" << std::endl;
    std::cout << "1. Speed polar curve" << std::endl;
    std::cout << "2. Sink for specific speed" << std::endl;
    std::cout << "3. Best MassDryGross calculation based on full water ballast polar curve" << std::endl;
    std::cout << "4. Dump WinPilot polar file" << std::endl;
    std::cout << "5. Exit"  << std::endl;
    char option;
    std::cin >> option;
    switch(option) {
    case '1':
      SpeedPolar();
      break;

    case '2':
      Sink();
      break;

    case '3':
      BestWeightCalculateUsingWaterBallast();
      break;

    case '4':
      WinPilotDump();
      break;

    case '5':
      exit = true;
      break;

    default:
      break;
    }
  }
}
