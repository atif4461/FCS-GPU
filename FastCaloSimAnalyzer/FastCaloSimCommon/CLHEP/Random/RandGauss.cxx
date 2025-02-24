/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "CLHEP/Random/RandGauss.h"

namespace CLHEP {

/**

 * @brief Generates a random number from a Gaussian distribution.
 * 
 * @param engine Pointer to the HepRandomEngine instance used for generating the random number.
 * @param mean The mean of the Gaussian distribution.
 * @param stdDev The standard deviation of the Gaussian distribution.
 * 
 * @return A random double value from the specified Gaussian distribution.
 */
// The above comment was written by an LLM. 
double RandGauss::shoot(HepRandomEngine *engine, double mean, double stdDev)
{
    return engine->gauss( mean, stdDev );
  }

} // namespace CLHEP
