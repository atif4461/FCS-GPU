/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoLoadGpu.h"

/**

 * @brief Loads the GPU resources.
 *
 * This function serves as an entry point for loading GPU resources. It dispatches the actual loading task to a specialized function based on the defined macro (USE_STDPAR, USE_KOKKOS, USE_ALPAKA, USE_OMPGPU), defaulting to CUDA if none are defined.
 *
 * @return True if the loading is successful, false otherwise.
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

 * @brief Returns a pointer to the sample index on the host.
 *
 * @return A pointer to the sample index on the host, or nullptr if USE_ALPAKA is not defined.
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

 * @brief Returns a pointer to the regions stored on the GPU.
 *
 * This function provides access to the regions loaded onto the GPU.
 * If the USE_ALPAKA flag is defined, it calls the alpaka-specific implementation,
 * otherwise it returns a null pointer.
 *
 * @return A pointer to the regions on the GPU, or nullptr if not available.
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

 * @brief Returns a pointer to the cell grid on the GPU.
 *
 * This function returns a pointer to the cell grid on the GPU, 
 * but only if the USE_ALPAKA flag is defined. Otherwise, it returns nullptr.
 *
 * @param[in] neta Number of eta bins in the grid.
 * @param[in] nphi Number of phi bins in the grid.
 * @return Pointer to the cell grid on the GPU, or nullptr if USE_ALPAKA is not defined.
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
