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
 * @file PolarOptimiser/src/main.cpp
 *
 * @brief Implements the main() function. 
**/

#include "application.h"
#include <iostream>
#include <string>


/**
 * @brief Prints PolarOptimiser usage screen
 *
 * Function prints PolarOptimiser usage screen
 */
void Usage()
{
  std::cout << "PolarOptimiser 1.0 Copyright (C) 2009 Mateusz Pusz" << std::endl;
  std::cout << std::endl;
  std::cout << "This program comes with ABSOLUTELY NO WARRANTY. This is free software," << std::endl;
  std::cout << "and you are welcome to redistribute it under GNU GPL conditions." << std::endl;
  std::cout << std::endl;
  std::cout << "Usage:" << std::endl;
  std::cout << "  polarOptimier.exe [-h|<WINPILOT_POLAR_FILE>]" << std::endl;
  std::cout << std::endl;
  std::cout << "  -h                    - that help message" << std::endl;
  std::cout << "  <WINPILOT_POLAR_FILE> - glider polar file in WinPilot like format" << std::endl;
}


/**
 * @brief Main entry-point for this application.
 *
 * @param argc Number of command-line arguments. 
 * @param argv Array of command-line argument strings. 
 *
 * @return Exit-code for the process - 0 for success, else an error code. 
**/
int main(int argc, const char *argv[])
{
  try {
    if(argc == 1 || argc > 1 && std::string(argv[1]) == "-h") {
      Usage();
      return EXIT_SUCCESS;
    }

    polarOptimiser::CApplication app(argv[1]);
    app.Run();
    return EXIT_SUCCESS;
  }
  catch(const std::exception &ex) {
    std::cerr << ex.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}
