/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "CLHEP/Random/RandPoisson.h"

namespace CLHEP {

/**

 * @brief Generates a Poisson-distributed random number.
 *
 * @param engine Pointer to the HepRandomEngine instance used for generating the random number.
 * @param mean The expected value of the Poisson distribution.
 *
 * @return A random number following a Poisson distribution with the given mean.
 */
// The above comment was written by an LLM. 
double RandPoisson::shoot(HepRandomEngine *engine, double mean)
{
  return engine->poisson(mean);
}

} // namespace CLHEP
