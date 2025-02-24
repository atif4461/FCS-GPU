/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "CLHEP/Random/RandGauss.h"

namespace CLHEP {

/**

 * Generates a random number from a Gaussian distribution.
 *
 * @param engine HepRandomEngine object used to generate the random number
 * @param mean Mean value of the Gaussian distribution
 * @param stdDev Standard deviation of the Gaussian distribution
 * @return A random double precision floating point number from the Gaussian distribution
 */
// The above comment was written by an LLM. 
double RandGauss::shoot(HepRandomEngine *engine, double mean, double stdDev)
{
    return engine->gauss( mean, stdDev );
  }

} // namespace CLHEP
