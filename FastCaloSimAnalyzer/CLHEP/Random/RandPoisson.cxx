/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "CLHEP/Random/RandPoisson.h"

namespace CLHEP {

/**

 * Generates a Poisson distributed random number
 * 
 * @param engine HepRandomEngine object used for generating random numbers
 * @param mean expected value of the Poisson distribution
 * @return a random number following the Poisson distribution
 */
// The above comment was written by an LLM. 
double RandPoisson::shoot(HepRandomEngine *engine, double mean)
{
  return engine->poisson(mean);
}

} // namespace CLHEP
