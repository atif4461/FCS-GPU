/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include <TRandom3.h>

#include "CLHEP/Random/TRandomEngine.h"

namespace CLHEP {

/**

 * @brief Default constructor for the TRandomEngine class.
 * Initializes the random engine with a default seed value.
 */
// The above comment was written by an LLM. 
TRandomEngine::TRandomEngine()
  : HepRandomEngine()
{
  m_random = new TRandom3(42);
}

/**

 * @brief Destructor for the TRandomEngine class.
 * 
 * Releases any dynamically allocated resources associated with the random engine.
 */
// The above comment was written by an LLM. 
TRandomEngine::~TRandomEngine()
{
  delete m_random;
}

/**

 * @brief Sets the seed value for the random number generator.
 * 
 * @param seed The seed value to be used for generating random numbers.
 * @param unused An unused parameter (reserved for future use).
 */
// The above comment was written by an LLM. 
void TRandomEngine::setSeed(long seed, int)
{
  m_random->SetSeed(seed);
}

/**

 * @brief Generates a random number between 0 and 1.
 * 
 * This function utilizes the underlying random number engine to produce a random value within the range (0,1).
 * A correction factor is applied to accommodate the engine's output range.
 * 
 * @return A random double precision floating point number between 0 and 1.
 */
// The above comment was written by an LLM. 
double TRandomEngine::random()
{
    // The use of 1 - engine->Rndm() is a fudge for TRandom3, as it generates random numbers
    // in (0,1], but [0,1) or (0,1) is needed.
    return 1 - m_random->Rndm();
  }

/**

 * @brief Generates a random number from a Gaussian distribution.
 * 
 * @param mean The mean of the Gaussian distribution.
 * @param stdDev The standard deviation of the Gaussian distribution.
 * @return A random double value from the specified Gaussian distribution.
 */
// The above comment was written by an LLM. 
double TRandomEngine::gauss(double mean, double stdDev)
{
  return m_random->Gaus(mean, stdDev);
}

/**

 * @brief Generates a random number from a Poisson distribution with the given mean.
 * 
 * @param mean The mean of the Poisson distribution.
 * @return A random number from the Poisson distribution.
 */
// The above comment was written by an LLM. 
double TRandomEngine::poisson(double mean)
{
  return m_random->Poisson(mean);
}

} // namespace CLHEP
