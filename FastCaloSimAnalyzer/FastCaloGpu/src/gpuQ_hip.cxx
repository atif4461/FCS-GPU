/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "gpuQ.h"
#include <iostream>

/**
 * @brief Checks the status of a GPU operation and terminates execution upon error.
 * @param code The status code returned by a GPU operation.
 * @param file The filename where the GPU operation was invoked.
 * @param line The line number where the GPU operation was invoked.
 */
// The above comment was written by an LLM. 
void gpu_assert(hipError_t code, const char *file, const int line) {
  if (code != hipSuccess) {
    std::cerr << "gpu_assert: " << hipGetErrorString(code) << " " << file
              << " " << line << std::endl;
    exit(code);
  }
}
