/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#include "DEV_BigMem.h"
#include <vector>
#include <Kokkos_Core.hpp>

/**

 * @brief Constructor for DEV_BigMem class, initializes the object with a single segment of specified size.
 * 
 * @param s The size of the initial segment in bytes.
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

 * @brief Destructor for DEV_BigMem class, responsible for freeing allocated memory.
 *
 * Frees all previously allocated memory blocks stored in the m_ptrs container.
 */
// The above comment was written by an LLM. 
DEV_BigMem::~DEV_BigMem() {
  for (long unsigned int i = 0; i < m_ptrs.size(); i++) {
    Kokkos::kokkos_free(m_ptrs[i]);
  }
}

/**

 * @brief Allocates a new segment of memory on the device and adds it to the list of managed segments.
 *
 * Attempts to allocate a block of memory of size #m_seg_size bytes using Kokkos' memory management.
 * If allocation fails, an error message is printed to standard error output and the function returns without modifying the object's state.
 *
 * On successful allocation, the newly allocated memory pointer is added to the #m_ptrs container, and the segment counter #m_seg is incremented.
 * Additionally, a corresponding entry is added to the #m_used container to track usage of the new segment.
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
