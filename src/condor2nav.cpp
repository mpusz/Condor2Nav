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
 * @file condor2nav.cpp
 *
 * @brief Implements the condor2nav::CCondor2Nav class. 
**/

#include "condor2nav.h"
#include "translator.h"
#include <iostream>


void condor2nav::CCondor2Nav::Usage() const
{
  std::cout << "Condor2Nav 1.0 Copyright (C) 2009-2010 Mateusz Pusz" << std::endl;
  std::cout << std::endl;
  std::cout << "This program comes with ABSOLUTELY NO WARRANTY. This is free software," << std::endl;
  std::cout << "and you are welcome to redistribute it under GNU GPL conditions." << std::endl;
  std::cout << std::endl;
  std::cout << "Usage:" << std::endl;
  std::cout << "  condor2nav.exe [-h|<CONDOR_TASK_FULL_PATH>]" << std::endl;
  std::cout << std::endl;
  std::cout << "  -h                      - that help message" << std::endl;
  std::cout << "  <CONDOR_TASK_FULL_PATH> - full path to Condor Task file" << std::endl;
}


/**
 * @brief Runs translation.
 *
 * Method is responsible for command line handling and running the translation.
 *
 * @param argc   Number of command-line arguments. 
 * @param argv   Array of command-line argument strings. 
 * 
 * @return Application execution result.
**/
int condor2nav::CCondor2Nav::Run(int argc, const char *argv[]) const
{
  if(argc > 1 && std::string(argv[1]) == "-h") {
    Usage();
    return EXIT_SUCCESS;
  }

  // obtain Condor installation path
  HKEY hTestKey;
  if((RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Condor: The Competition Soaring Simulator", 0, KEY_READ, &hTestKey)) != ERROR_SUCCESS)
    throw std::runtime_error("ERROR: Condor installation not found!!!");
  DWORD bufferSize = 0;
  RegQueryValueEx(hTestKey, "DisplayIcon", NULL, NULL, NULL, &bufferSize);
  std::auto_ptr<char> buffer = std::auto_ptr<char>(new char[bufferSize]);
  RegQueryValueEx(hTestKey, "DisplayIcon", NULL, NULL, reinterpret_cast<BYTE *>(buffer.get()), &bufferSize);
  std::string path(buffer.get());
  RegCloseKey(hTestKey);
  size_t pos = path.find("Condor.exe");
  path = path.substr(0, pos - 1);

  std::string taskName((argc > 1) ? argv[1] : "");
  CTranslator translator(path, taskName);
  translator.Run();
  
  return EXIT_SUCCESS;
}
