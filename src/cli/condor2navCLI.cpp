//
// This file is part of Condor2Nav file formats translator.
//
// Copyright (C) 2009-2011 Mateusz Pusz
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
 * @file condor2navCLI.cpp
 *
 * @brief Implements the condor2nav::CCondor2NavCLI class. 
 */

#include "condor2navCLI.h"
#include "translator.h"
#include "condor.h"
#include <iostream>


/**
 * @brief Class constructor. 
 *
 * @param type The logger type. 
 */
condor2nav::cli::CCondor2NavCLI::CLogger::CLogger(TType type):
CCondor2Nav::CLogger(type)
{

}


/**
 * @brief Dumps the text to the console output. 
 *
 * @param str The string to dump. 
 *
 * @exception EOperationFailed Thrown when operation failed to execute. 
 */
void condor2nav::cli::CCondor2NavCLI::CLogger::Dump(const std::string &str) const
{
  switch(Type()) {
  case TYPE_NORMAL:
    std::cout << str;
    break;
  case TYPE_WARNING:
  case TYPE_ERROR:
    std::cerr << str;
    break;
  default:
    throw EOperationFailed("ERROR: Unsupported logger type (" + Convert(Type()) + ")!!!");
  }
}


/**
 * @brief Default class constructor.
 */
condor2nav::cli::CCondor2NavCLI::CCondor2NavCLI():
_normal(CLogger::TYPE_NORMAL),
_warning(CLogger::TYPE_WARNING),
_error(CLogger::TYPE_ERROR)
{
}


/**
 * @brief Prints condor2nav executable usage help. 
 *
 * Method prints condor2nav executable usage help.
 */
void condor2nav::cli::CCondor2NavCLI::Usage() const
{
  Log() << "Condor2Nav 3.1 Copyright (C) 2009-2011 Mateusz Pusz" << std::endl;
  Log() << std::endl;
  Log() << "This program comes with ABSOLUTELY NO WARRANTY. This is free software," << std::endl;
  Log() << "and you are welcome to redistribute it under GNU GPL conditions." << std::endl;
  Log() << std::endl;
  Log() << "Usage:" << std::endl;
  Log() << "  condor2nav.exe [-h|--aat <TASK_MIN_TIME>][--default|--last-race|<FPL_PATH>]" << std::endl;
  Log() << std::endl;
  Log() << "  -h                    - that help message" << std::endl;
  Log() << "  --aat <TASK_MIN_TIME> - convert a task as AAT with provided Task Minimum Time" << std::endl;
  Log() << "                          in minutes. AAT task data is automatically detected for" << std::endl;
  Log() << "                          files directly downloaded from http://condor-club.eu" << std::endl;
  Log() << "                          so that parameter does not need to be provided." << std::endl;
  Log() << "  --default             - run translation for default FPL file" << std::endl;
  Log() << "                          (Default FPL file name is specified in condor2nav.ini file)" << std::endl;
  Log() << "  --last-race           - convert last flown race" << std::endl;
  Log() << "                          (Useful option for servers that does not allow to save" << std::endl;
  Log() << "                           task file on local disk. Join the race, exit after few" << std::endl;
  Log() << "                           seconds, run translation with --last-race option and" << std::endl;
  Log() << "                           join a race once again, but now with full PDA support)" << std::endl;
  Log() << "  <FPL_PATH>            - full path to Condor FPL file" << std::endl;
  Log() << "                          (The same result can be achieved i.e. by drag-and-drop" << std::endl;
  Log() << "                           of FPL file in Windows Explorer onto condor2nav.exe icon)" << std::endl;
}


/**
 * @brief Parse Command Line Interface parameters.
 *        
 * Method parses Command Line Interface parameters and returns user provided values. 
 *
 * @param argc             Number of command line arguments. 
 * @param argv             The array of command line arguments. 
 * @param [in,out] fplType Type of the FPL file.
 * @param [in,out] fplPath Full pathname of the FPL file. 
 * @param [in,out] aatTime Minimum time of the AAT task. 
 *
 * @exception std::runtime_error Thrown when parsing error occurred.
 */
void condor2nav::cli::CCondor2NavCLI::CLIParse(int argc, const char *argv[], TFPLType &fplType, boost::filesystem::path &fplPath, unsigned &aatTime) const
{
  fplType = TYPE_DEFAULT;
  fplPath = "";
  aatTime = 0;

  for(int i=1; i<argc; i++) {
    if(std::string(argv[i]) == "-h") {
      Usage();
      exit(EXIT_SUCCESS);
    }
    else if(std::string(argv[i]) == "--aat") {
      if(i + 1 == argc)
        throw EOperationFailed("ERROR: AAT TASK_MIN_TIME not provided!!!");
     
      std::stringstream stream(argv[++i]);
      stream >> aatTime;
      if(stream.fail())
        throw EOperationFailed("ERROR: Invalid AAT TASK_MIN_TIME!!!");
    }
    else if(std::string(argv[i]) == "--default") {
      // nothing needs to be done here
      fplType = TYPE_DEFAULT;
    }
    else if(std::string(argv[i]) == "--last-race") {
      fplType = TYPE_RESULT;
    }
    else if(argv[i][0] == '-') {
      throw EOperationFailed("ERROR: Unkown option '" + std::string(argv[i]) + "' provided!!!");
    }
    else {
      fplPath = argv[i];
      fplType = TYPE_USER;
    }
  }
}


/**
 * @brief Checks if condor-club AAT task file.
 *
 * Method Checks if condor-club AAT task file is provided. It looks for certain entries
 * that are added by http://condor-club.eu server to the file.
 *
 * @param condor Condor wrapper. 
 * @param aatTime AAT time provided from command line. 
 * 
 * @return Operation status.
 */
bool condor2nav::cli::CCondor2NavCLI::AATCheck(const CCondor &condor, unsigned &aatTime) const
{
  try {
    const CFileParserINI &taskParser = condor.TaskParser();
    if(taskParser.Value("Task", "AAT") == "Distance") {
      Error() << "ERROR: AAT/D tasks are not supported!!!" << std::endl;
    }
    else if(taskParser.Value("Task", "AAT") == "Speed") {
      try {
        unsigned time = Convert<unsigned>(taskParser.Value("Task", "DesignatedTime"));
        if(aatTime > 0 && aatTime != time) {
          Warning() << "WARNING: Provided AAT time (" << aatTime << ") is different than condor-club time (" << time << ")!" << std::endl;
        }
        else if(aatTime == 0) {
          aatTime = time;
          Log() << "Autodetected AAT time: " << aatTime << " minutes." << std::endl;
        }
      }
      catch(EOperationFailed &) {
        Error() << "ERROR: Corrupted condor-club task file!!!" << std::endl;
        return false;
      }
    }
  }
  catch(EOperationFailed &) {
  }

  return true;
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
 */
int condor2nav::cli::CCondor2NavCLI::Run(int argc, const char *argv[]) const
{
  // parse CLI options
  TFPLType fplType;
  boost::filesystem::path fplPath;
  unsigned aatTime;
  CLIParse(argc, argv, fplType, fplPath, aatTime);
  
  // obtain Condor installation path
  boost::filesystem::path condorPath = CCondor::InstallPath();
  
  // open configuration file
  CFileParserINI configParser(CONFIG_FILE_NAME);
  
  // create Condor FPL file path
  CCondor::FPLPath(configParser, fplType, condorPath, fplPath);
  
  // create Condor wrapper
  CCondor condor(condorPath, fplPath);
  if(!AATCheck(condor, aatTime))
    return EXIT_FAILURE;

  // run translation
  CTranslator translator(*this, configParser, condor, aatTime);
  translator.Run();
  
  return EXIT_SUCCESS;
}
