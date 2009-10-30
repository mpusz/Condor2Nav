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
 * @file main.cpp
 *
 * @brief Implements the main() function. 
**/

#include "condor2nav.h"
#include <iostream>


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
    if(argc > 1 && std::string(argv[1]) == "-h") {
      std::cout << "Condor2Nav 0.1 Copyright (C) 2009 Mateusz Pusz" << std::endl;
      std::cout << std::endl;
      std::cout << "This program comes with ABSOLUTELY NO WARRANTY. This is free software," << std::endl;
      std::cout << "and you are welcome to redistribute it under GNU GPL conditions." << std::endl;
      std::cout << std::endl;
      std::cout << "Usage:" << std::endl;
      std::cout << "  condor2nav.exe [-h|<CONDOR_TASK_NAME>]" << std::endl;
      std::cout << std::endl;
      std::cout << "  -h                 - that help message" << std::endl;
      std::cout << "  <CONDOR_TASK_NAME> - Task name as seen in the Condor GUI" << std::endl;
      std::cout << "                       (without file extension)" << std::endl;
  
      return EXIT_SUCCESS;
    }

    condor2nav::CCondor2Nav app(argc, argv);
    return app.Run();
  }
  catch(const std::exception &ex) {
    std::cerr << ex.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}
