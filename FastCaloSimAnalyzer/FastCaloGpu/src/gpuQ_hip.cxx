/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "gpuQ.h"
#include <iostream>

/**

 * @brief Checks the status of a HIP operation and terminates the program if an error occurs.
 * 
 * @param[in] code The status code returned by a HIP function.
 * @param[in] file The filename where the assertion was triggered.
 * @param[in] line The line number where the assertion was triggered.
 */
// The above comment was written by an LLM. 
void gpu_assert(hipError_t code, const char *file, const int line) {
  if (code != hipSuccess) {
    std::cerr << "gpu_assert: " << hipGetErrorString(code) << " " << file
              << " " << line << std::endl;
    exit(code);
  }
}
