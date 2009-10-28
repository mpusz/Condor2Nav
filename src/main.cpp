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
