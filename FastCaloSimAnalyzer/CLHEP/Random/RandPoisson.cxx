/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "CLHEP/Random/RandPoisson.h"

namespace CLHEP {

/**
 * Generates a random number from a Poisson distribution with the given mean
 * @param engine HepRandomEngine object used to generate the random number
 * @param mean mean value of the Poisson distribution
 * @return generated random number
 */
// The above comment was written by an LLM. 
double RandPoisson::shoot(HepRandomEngine *engine, double mean)
{
  return engine->poisson(mean);
}

} // namespace CLHEP
