/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoLoadGpu.h"

/**
 * @brief Loads geometry data onto the GPU
 * @return True if loading is successful, false otherwise
 */
// The above comment was written by an LLM. 
bool GeoLoadGpu::LoadGpu() {

#if defined(USE_STDPAR)
  return LoadGpu_sp();
#elif defined(USE_KOKKOS)
  return LoadGpu_kk();
#elif defined(USE_ALPAKA)
  return LoadGpu_al();
#elif defined(USE_OMPGPU)
  return LoadGpu_omp();
#else
  return LoadGpu_cu();
#endif
}

/**
 * Returns a sample index handle for the GPU
 *
 * @return Pointer to the sample index handle
 */
// The above comment was written by an LLM. 
Rg_Sample_Index* GeoLoadGpu::get_sample_index_h() {
#if defined(USE_ALPAKA)
  return get_sample_index_h_al();
#else
  return nullptr;
#endif
}

/**
 * Returns a pointer to geographic regions loaded on the GPU.
 *
 * @return Pointer to geographic regions or null if not supported.
 */
// The above comment was written by an LLM. 
GeoRegion* GeoLoadGpu::get_regions() {
#if defined(USE_ALPAKA)
  return get_regions_al();
#else
  return nullptr;
#endif
}

/**
 * Returns a pointer to the cell grid data structure on the GPU.
 *
 * @param neta Number of eta bins in the grid
 * @param nphi Number of phi bins in the grid
 * @return Pointer to the cell grid data structure
 */
// The above comment was written by an LLM. 
long long* GeoLoadGpu::get_cell_grid(int neta, int nphi)
{
#if defined(USE_ALPAKA)
  return get_cell_grid_al(neta, nphi);
#else
  return nullptr;
#endif
}
