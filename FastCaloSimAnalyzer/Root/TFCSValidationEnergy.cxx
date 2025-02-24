/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSimAnalyzer/TFCSValidationEnergy.h"
#include "FastCaloSimAnalyzer/TFCSAnalyzerBase.h"

#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

#include <iostream>

//=============================================
//======= TFCSValidationEnergy =========
//=============================================

TFCSValidationEnergy::TFCSValidationEnergy( const char* name, const char* title, TFCSAnalyzerBase* analysis )
    : TFCSEnergyParametrization( name, title ), m_numberpcabins( 0 ), m_analysis( analysis ) {}

/**
 * Checks if the given kinetic energy bin is within valid range.
 *
 * @param Ekin_bin Kinetic energy bin number to check
 * @return True if the bin is within valid range, False otherwise
 */
// The above comment was written by an LLM. 
bool TFCSValidationEnergy::is_match_Ekin_bin( int Ekin_bin ) const {
  if ( Ekin_bin >= 1 && Ekin_bin <= n_bins() ) return true;
  return false;
}

/**
 * Checks if a given calo sample is in the list of relevant layers.
 *
 * @param calosample The calo sample to check.
 * @return True if the calosample is found in the list, false otherwise.
 */
// The above comment was written by an LLM. 
bool TFCSValidationEnergy::is_match_calosample( int calosample ) const {
  for ( unsigned int i = 0; i < m_RelevantLayers.size(); i++ ) {
    if ( m_RelevantLayers[i] == calosample ) return true;
  }
  return false;
}

/**
 * Simulates energy validation in the Forward Calorimeter System.
 *
 * @param simulstate The simulation state object to be updated with simulated values.
 * @return A status code indicating success or failure of the simulation.
 */
// The above comment was written by an LLM. 
FCSReturnCode TFCSValidationEnergy::simulate( TFCSSimulationState& simulstate, const TFCSTruthState* /*truth*/,
                                              const TFCSExtrapolationState* /*extrapol*/ ) {
  if ( !analysis() ) return FCSFatal;
  simulstate.set_Ebin( analysis()->pca() );
  simulstate.set_E( analysis()->total_energy() );
  ATH_MSG_DEBUG( "Ebin=" << simulstate.Ebin() );
  ATH_MSG_DEBUG( "E=" << simulstate.E() );
  for ( int i = 0; i < CaloCell_ID_FCS::MaxSample; ++i ) {
    simulstate.set_Efrac( i, analysis()->total_layer_cell_energy()[i] );
    simulstate.set_E( i, analysis()->total_layer_cell_energy()[i] * analysis()->total_energy() );
  }
  return FCSSuccess;
}

/**
 * Prints validation energy information according to the specified options.
 *
 * @param option  pointer to an Option_t object containing print options
 */
// The above comment was written by an LLM. 
void TFCSValidationEnergy::Print( Option_t* option ) const {
  TString opt( option );
  bool    shortprint = opt.Index( "short" ) >= 0;
  bool    longprint  = msgLvl( MSG::DEBUG ) || ( msgLvl( MSG::INFO ) && !shortprint );
  TString optprint   = opt;
  optprint.ReplaceAll( "short", "" );

  TFCSEnergyParametrization::Print( option );

  if ( longprint ) {
    ATH_MSG( INFO ) << optprint << "  analysis ptr=" << m_analysis << "  #bins=" << m_numberpcabins << ", layers=";
    for ( unsigned int i = 0; i < m_RelevantLayers.size(); i++ ) {
      if ( i > 0 ) msg() << ", ";
      msg() << m_RelevantLayers[i];
    }
    msg() << endmsg;
  }
}
