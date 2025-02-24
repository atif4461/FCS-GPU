/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "gpuQ.h"
#include "DEV_BigMem.h"
#include <vector>

/**

 * @brief Constructor for DEV_BigMem class, initializes a single segment of specified size.
 * 
 * @param s The size of the initial memory segment.
 */
// The above comment was written by an LLM. 
DEV_BigMem::DEV_BigMem(size_t s) { // initialize to one seg with size s
  void *p;
  m_seg_size = s;
  gpuQ(hipMalloc(&p, m_seg_size));
  m_ptrs.push_back(p);
  m_seg = 0;
  m_used.push_back(0);
};

/**

 * @brief Destructor for the DEV_BigMem class.
 * 
 * Releases memory allocated on the GPU by freeing each pointer in the m_ptrs vector.
 */
// The above comment was written by an LLM. 
DEV_BigMem::~DEV_BigMem() {
  for (int i = 0; i < m_ptrs.size(); i++)
    gpuQ(hipFree(m_ptrs[i]));
};

/**

 * @brief Allocates a new segment of device memory and adds it to the list of managed segments.
 *
 * This function allocates a block of memory on the device using hipMalloc, stores the pointer to the allocated memory in the m_ptrs vector,
 * increments the segment counter, and initializes the used space tracker for the newly allocated segment to zero.
 */
// The above comment was written by an LLM. 
void DEV_BigMem::add_seg() {
  void *p;
  gpuQ(hipMalloc((void **)&p, m_seg_size));
  m_ptrs.push_back(p);
  m_seg++;
  m_used.push_back(0);
};
