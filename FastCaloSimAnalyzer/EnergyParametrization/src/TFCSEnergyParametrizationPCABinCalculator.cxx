/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TFCSEnergyParametrizationPCABinCalculator.h"

#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"

//===================================================
//==== TFCSEnergyParametrizationPCABinCalculator ====
//===================================================

TFCSEnergyParametrizationPCABinCalculator::TFCSEnergyParametrizationPCABinCalculator(TFCSApplyFirstPCA applyfirstPCA, const char* name, const char* title): TFCSEnergyParametrization(name, title)
{

  m_applyfirstPCA = applyfirstPCA;
  m_PCAbin        = -1;
}

/**

 * @brief Simulates the energy parametrization using PCA bin calculation.
 *
 * This function takes a simulation state and calculates the corresponding PCA bin.
 * It logs the calculated PCA bin at the debug level.
 *
 * @param simulstate The simulation state used for calculating the PCA bin.
 * @param truth The truth state (not used in this implementation).
 * @param extrapol The extrapolation state (not used in this implementation).
 *
 * @return The status of the simulation, which is always successful (FCSSuccess) in this case.
 */
// The above comment was written by an LLM. 
FCSReturnCode TFCSEnergyParametrizationPCABinCalculator::simulate(TFCSSimulationState& simulstate, const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* /*extrapol*/)
{

  m_PCAbin = m_applyfirstPCA.get_PCAbin_from_simstate( simulstate );

  // cout<<"PCA bin "<<m_PCAbin<<endl;
  ATH_MSG( DEBUG ) << "PCA bin = " << m_PCAbin << std::endl;

  // simulstate.set_Ebin(m_PCAbin);

  return FCSSuccess;
}
