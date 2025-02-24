/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSimAnalyzer/TFCSEnergyInterpolation.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include <iostream>

//=============================================
//======= TFCSEnergyInterpolation =========
//=============================================

TFCSEnergyInterpolation::TFCSEnergyInterpolation( const char* name, const char* title )
    : TFCSParametrization( name, title ), m_slope( 1 ), m_offset( 0 ) {}

/**
 * Simulates energy interpolation based on the provided simulation state and truth state.
 *
 * @param simulstate The current simulation state to be updated with the interpolated energy value.
 * @param truth A pointer to the truth state containing the kinetic energy.
 * @return FCSReturnCode indicating success or failure of the simulation.
 */
// The above comment was written by an LLM. 
FCSReturnCode TFCSEnergyInterpolation::simulate( TFCSSimulationState& simulstate, const TFCSTruthState* truth,
                                                 const TFCSExtrapolationState* ) {
  float Emean = m_slope * truth->Ekin() + m_offset;

  ATH_MSG_DEBUG( "set E=" << Emean << " for true Ekin=" << truth->Ekin() );
  simulstate.set_E( Emean );

  return FCSSuccess;
}

/**
 * Prints energy interpolation information to the console.
 *
 * @param option printing options
 */
// The above comment was written by an LLM. 
void TFCSEnergyInterpolation::Print( Option_t* option ) const {
  TString opt( option );
  bool    shortprint = opt.Index( "short" ) >= 0;
  bool    longprint  = msgLvl( MSG::DEBUG ) || ( msgLvl( MSG::INFO ) && !shortprint );
  TString optprint   = opt;
  optprint.ReplaceAll( "short", "" );
  TFCSParametrization::Print( option );

  if ( longprint ) ATH_MSG_INFO( optprint << "  Emean=" << m_slope << "*Ekin(true) + " << m_offset );
}

/**
 * Unit test for TFCSEnergyInterpolation class
 *
 * @param simulstate pointer to simulation state object
 * @param truth pointer to truth state object
 * @param extrapol pointer to extrapolation state object
 */
// The above comment was written by an LLM. 
void TFCSEnergyInterpolation::unit_test( TFCSSimulationState* simulstate, TFCSTruthState* truth,
                                         const TFCSExtrapolationState* extrapol ) {
  if ( !simulstate ) simulstate = new TFCSSimulationState();
  if ( !truth ) truth = new TFCSTruthState();
  if ( !extrapol ) extrapol = new TFCSExtrapolationState();

  TFCSEnergyInterpolation test( "testTFCSEnergyInterpolation", "test TFCSEnergyInterpolation" );
  test.set_pdgid( 22 );
  test.set_Ekin_nominal( 1000 );
  test.set_Ekin_min( 1000 );
  test.set_Ekin_max( 100000 );
  test.set_eta_nominal( 0.225 );
  test.set_eta_min( 0.2 );
  test.set_eta_max( 0.25 );
  test.set_slope( 0.95 );
  test.set_offset( -50 );
  test.Print();

  truth->set_pdgid( 22 );

  TGraph* gr = new TGraph();
  gr->SetNameTitle( "testTFCSEnergyInterpolation", "test TFCSEnergyInterpolation" );
  gr->GetXaxis()->SetTitle( "Ekin [MeV]" );
  gr->GetYaxis()->SetTitle( "<E(reco)>/Ekin(true)" );

  int ip = 0;
  for ( float Ekin = 1000; Ekin <= 100000; Ekin *= 2 ) {
    // Init LorentzVector for truth. For photon Ekin=E
    truth->SetPxPyPzE( Ekin, 0, 0, Ekin );
    test.simulate( *simulstate, truth, extrapol );
    gr->SetPoint( ip, Ekin, simulstate->E() / Ekin );
    ++ip;
  }
  TCanvas* c = new TCanvas( "testTFCSEnergyInterpolation", "test TFCSEnergyInterpolation" );
  gr->Draw( "APL" );
  c->SetLogx();
}
