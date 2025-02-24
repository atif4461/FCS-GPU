/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DEV_BigMem.h"
#include <vector>
#include <omp.h>

/**
 * Constructor to initialize the DEVBigMem object with a specified segment size.
 * @param s The size of the initial segment.
 */
// The above comment was written by an LLM. 
DEV_BigMem::DEV_BigMem( size_t s ) { // initialize to one seg with size s
  void* p;
  m_seg_size = s;
  p          = (void*)omp_target_alloc( m_seg_size, m_default_device );
  if ( p == NULL ) { std::cout << " ERROR: No space left on device for p in big mem." << std::endl; }
  m_ptrs.push_back( p );
  m_seg = 0;
  m_used.push_back( 0 );
};

DEV_BigMem::~DEV_BigMem() {
  for ( int i = 0; i < m_ptrs.size(); i++ ) omp_target_free( m_ptrs[i], m_default_device );
};

/**
 * Adds a new segment to the big memory allocation on the default device.
 *
 * Allocates memory of size m_seg_size on the device specified by m_default_device,
 * stores the pointer in the list of pointers, increments the segment counter,
 * and initializes the used flag for the new segment to zero.
 */
// The above comment was written by an LLM. 
void DEV_BigMem::add_seg() {
  void* p;
  p = (void*)omp_target_alloc( m_seg_size, m_default_device );
  if ( p == NULL ) { std::cout << " ERROR: No space left on device for p in big mem." << std::endl; }
  m_ptrs.push_back( p );
  m_seg++;
  m_used.push_back( 0 );
};
