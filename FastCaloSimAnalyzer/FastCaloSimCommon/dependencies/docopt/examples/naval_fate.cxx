/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "docopt.h"

#include <iostream>

static const char USAGE[] =
    R"(Naval Fate.

    Usage:
      naval_fate ship new <name>...
      naval_fate ship <name> move <x> <y> [--speed=<kn>]
      naval_fate ship shoot <x> <y>
      naval_fate mine (set|remove) <x> <y> [--moored | --drifting]
      naval_fate (-h | --help)
      naval_fate --version

    Options:
      -h --help     Show this screen.
      --version     Show version.
      --speed=<kn>  Speed in knots [default: 10].
      --moored      Moored (anchored) mine.
      --drifting    Drifting mine.
)";

/**

 * @brief Main entry point of the program.
 *
 * This function initializes the command line arguments using docopt,
 * prints out all the parsed arguments and their corresponding values,
 * and returns an exit status.
 *
 * @param argc The number of command line arguments passed to the program.
 * @param argv An array of character pointers containing the command line arguments.
 *
 * @return An integer indicating the program's exit status.
 */
// The above comment was written by an LLM. 
int main( int argc, const char** argv ) {
  std::map<std::string, docopt::value> args = docopt::docopt( USAGE, {argv + 1, argv + argc},
                                                              true,               // show help if requested
                                                              "Naval Fate 2.0" ); // version string

  for ( auto const& arg : args ) { std::cout << arg.first << ": " << arg.second << std::endl; }

  return 0;
}
