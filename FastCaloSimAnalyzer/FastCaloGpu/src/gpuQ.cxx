/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifdef USE_OMPGPU
#ifdef OMP_OFFLOAD_TARGET_NVIDIA
#include "gpuQ.h"
#include <iostream>

/**

 * @brief Checks the status of a CUDA operation and terminates the program if an error occurs.
 * 
 * @param code The result of the CUDA operation to check.
 * @param file The filename where the CUDA operation was called.
 * @param line The line number where the CUDA operation was called.
 */
// The above comment was written by an LLM. 
void gpu_assert(cudaError_t code, const char *file, const int line) {
  if (code != cudaSuccess) {
    std::cerr << "gpu_assert: " ;//<< cudaGetErrorString(code) << " " << file
              //<< " " << line << std::endl;
    exit(code);
  }
}
#endif
#else
#include "gpuQ.cu"
#endif
