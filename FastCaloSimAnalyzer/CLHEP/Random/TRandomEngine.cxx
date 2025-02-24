/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include <TRandom3.h>

#include "CLHEP/Random/TRandomEngine.h"

namespace CLHEP {

/**

 * Default constructor initializing the random engine with a fixed seed value
 */
// The above comment was written by an LLM. 
TRandomEngine::TRandomEngine()
  : HepRandomEngine()
{
  m_random = new TRandom3(42);
}

/**

 * Destructor for TRandomEngine class deletes dynamically allocated random object */
// The above comment was written by an LLM. 
TRandomEngine::~TRandomEngine()
{
  delete m_random;
}

/**

 * Sets the seed value for the random number generator.
 *
 * @param seed   the seed value to be used for generating random numbers
 * @param unused reserved for future use
 */
// The above comment was written by an LLM. 
void TRandomEngine::setSeed(long seed, int)
{
  m_random->SetSeed(seed);
}

/**

 * Returns a random number in the range [0,1) 
 */
// The above comment was written by an LLM. 
double TRandomEngine::random()
{
    // The use of 1 - engine->Rndm() is a fudge for TRandom3, as it generates random numbers
    // in (0,1], but [0,1) or (0,1) is needed.
    return 1 - m_random->Rndm();
  }

/**

 * Returns a random number from a Gaussian distribution with specified mean and standard deviation
 * @param mean the mean of the Gaussian distribution
 * @param stdDev the standard deviation of the Gaussian distribution
 * @return a random number from the Gaussian distribution
 */
// The above comment was written by an LLM. 
double TRandomEngine::gauss(double mean, double stdDev)
{
  return m_random->Gaus(mean, stdDev);
}

/**

 * Generates a random number from a Poisson distribution with the given mean
 * @param mean the average rate of events
 * @return a random number from the Poisson distribution
 */
// The above comment was written by an LLM. 
double TRandomEngine::poisson(double mean)
{
  return m_random->Poisson(mean);
}

} // namespace CLHEP
