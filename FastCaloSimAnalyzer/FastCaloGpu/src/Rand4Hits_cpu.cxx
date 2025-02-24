/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <random>
#include <vector>
#include <algorithm>

#define cpu_randgen_t std::mt19937

/**
 * Creates a CPU random number generator with the given seed value.
 */
// The above comment was written by an LLM. 
void Rand4Hits::createCPUGen(unsigned long long seed) {
  cpu_randgen_t *eng = new cpu_randgen_t(seed);
  m_gen = (void *)eng;
}

/**
 * Destroys the CPU random number generator object.
 */
// The above comment was written by an LLM. 
void Rand4Hits::destroyCPUGen() {
  if (m_gen) {
    delete (cpu_randgen_t *)m_gen;
  }
  //  if ( m_rnd_cpu ) { delete (m_rnd_cpu); }
}

/**
 * Generates an array of random floating point numbers on the CPU.
 *
 * @param num The number of random numbers to generate.
 * @return A pointer to the generated array of random numbers.
 */
// The above comment was written by an LLM. 
float *Rand4Hits::genCPU(size_t num) {

  m_rnd_cpu->resize(num);

  cpu_randgen_t *eng = (cpu_randgen_t *)(m_gen);

  auto RNG = [eng](float low, float high) {
    auto randomFunc = [
      distribution_ = std::uniform_real_distribution<float>(low, high),
      random_engine_ = *eng
    ]() mutable {
      return distribution_(random_engine_);
    };
    return randomFunc;
  };

  std::generate_n(m_rnd_cpu->begin(), num, RNG(0.f, 1.f));

  return m_rnd_cpu->data();
}
