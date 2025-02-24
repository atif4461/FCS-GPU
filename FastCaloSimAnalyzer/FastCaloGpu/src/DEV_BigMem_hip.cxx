/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "gpuQ.h"
#include "DEV_BigMem.h"
#include <vector>

/**
 * Constructor to initialize the DEVBigMem object with a specified segment size.
 * Initializes the memory allocation with a single segment of the given size.
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
 * Destructor to release device memory allocated for big memory objects
 */
// The above comment was written by an LLM. 
DEV_BigMem::~DEV_BigMem() {
  for (int i = 0; i < m_ptrs.size(); i++)
    gpuQ(hipFree(m_ptrs[i]));
};

/**
 * Adds a new segment to the big memory allocation
 */
// The above comment was written by an LLM. 
void DEV_BigMem::add_seg() {
  void *p;
  gpuQ(hipMalloc((void **)&p, m_seg_size));
  m_ptrs.push_back(p);
  m_seg++;
  m_used.push_back(0);
};
