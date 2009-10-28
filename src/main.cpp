//
// This file is a part of Condor2Nav file formats translator.
//
// Copyright (C) 2009 Mateusz Pusz
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Visit the project webpage (http://sf.net/projects/condor2nav) for more info.
//

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
    condor2nav::CCondor2Nav app(argc, argv);
    return app.Run();
  }
  catch(const std::exception &ex) {
    std::cerr << ex.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}
