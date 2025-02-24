/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#include "DEV_BigMem.h"
#include <vector>
#include <Kokkos_Core.hpp>

/**
 * Constructor initializing the DEVBigMem object with a specified segment size.
 *
 * @param s The initial size of the memory segment.
 */
// The above comment was written by an LLM. 
DEV_BigMem::DEV_BigMem(size_t s) { // initialize to one seg with size s
  m_seg_size = s;

  void *p{ nullptr };
  try {
    p = Kokkos::kokkos_malloc("bigmem buffer", m_seg_size);
  }
  catch (...) {
    std::cerr << "unable to allocate " << m_seg_size
              << " bytes for bigmem buffer\n";
    return;
  }
  m_ptrs.push_back(p);
  m_seg = 0;
  m_used.push_back(0);
}

/**
 * Destructor to release allocated memory resources
 */
// The above comment was written by an LLM. 
DEV_BigMem::~DEV_BigMem() {
  for (long unsigned int i = 0; i < m_ptrs.size(); i++) {
    Kokkos::kokkos_free(m_ptrs[i]);
  }
}

/**
 * Allocates a new segment of memory for the big memory buffer.
 *
 * Attempts to allocate a block of memory of size m_seg_size bytes using Kokkos.
 * If allocation fails, an error message is printed to standard error.
 * On success, the newly allocated memory pointer is added to the list of pointers.
 * The segment counter and used memory tracker are updated accordingly.
 */
// The above comment was written by an LLM. 
void DEV_BigMem::add_seg() {

  void *p{ nullptr };
  try {
    p = Kokkos::kokkos_malloc("bigmem buffer", m_seg_size);
  }
  catch (...) {
    std::cerr << "unable to allocate " << m_seg_size
              << " bytes for bigmem buffer\n";
    return;
  }
  // std::cout << "DEV_BM add_seg() " << m_seg << " " << p << std::endl;
  m_ptrs.push_back(p);
  m_seg++;
  m_used.push_back(0);
};
