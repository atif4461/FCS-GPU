/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSimAnalyzer/TFCSWriteCellsToTree.h"

#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include <iostream>

#include "TTree.h"

//=============================================
//======= TFCSWriteCellsToTree =========
//=============================================

TFCSWriteCellsToTree::TFCSWriteCellsToTree( const char* name, const char* title, TTree* tree )
    : TFCSParametrization( name, title ) {
  init_tree( tree );
}

/**

 * @brief Initializes the tree with the given TTree object.
 * 
 * This function sets up the tree structure by adding branches and leaves.
 * It creates a new branch for average cells and additional branches for cells in each layer.
 * 
 * @param tree The TTree object to be initialized.
 */
// The above comment was written by an LLM. 
void TFCSWriteCellsToTree::init_tree( TTree* tree ) {
  m_tree = tree;

  /** now add branches and leaves to the tree */
  if ( m_tree ) {
    m_oneeventcells = new FCS_matchedcellvector;
    m_tree->Branch( "AvgAllCells", &m_oneeventcells );

    // write cells per layer
    for ( Int_t i = 0; i < CaloCell_ID_FCS::MaxSample; i++ ) {
      TString branchname = "AvgSampling_";
      branchname += i;
      m_layercells[i] = new FCS_matchedcellvector;
      m_tree->Branch( branchname, &m_layercells[i] );
    }
  }
}

/**

 * @brief Simulates the writing of cells to a tree.
 *
 * This function takes a simulation state and writes the cells from that state to a tree.
 * It clears the vectors holding the cells, copies the cells into the tree,
 * and fills the tree with the copied data.
 *
 * @param[in] simulstate The simulation state containing the cells to be written.
 * @param[in] truth The truth state (not used in this implementation).
 * @param[in] extrapol The extrapolation state (not used in this implementation).
 *
 * @return The status code indicating success or failure of the operation.
 */
// The above comment was written by an LLM. 
FCSReturnCode TFCSWriteCellsToTree::simulate( TFCSSimulationState& simulstate, const TFCSTruthState* /*truth*/,
                                              const TFCSExtrapolationState* /*extrapol*/ ) {
  if ( !m_tree ) return FCSFatal;

  m_oneeventcells->m_vector.clear();
  for ( Int_t i = 0; i < CaloCell_ID_FCS::MaxSample; i++ ) m_layercells[i]->m_vector.clear();

  // Now copy all cells into the tree
  for ( const auto& iter : simulstate.cells() ) {
    const CaloDetDescrElement* theDDE = iter.first;
    int                        layer  = theDDE->getSampling();

    FCS_matchedcell cell;
    cell.cell.cell_identifier = theDDE->identify();
    cell.cell.sampling        = layer;
    cell.cell.energy          = iter.second;
    cell.cell.center_x        = theDDE->x();
    cell.cell.center_y        = theDDE->y();
    cell.cell.center_z        = theDDE->z();

    m_oneeventcells->push_back( cell );
    m_layercells[layer]->push_back( cell );
  }

  m_tree->Fill();

  return FCSSuccess;
}

/**

 * @brief Prints information about the object.
 * 
 * @param[in] option  The print options.
 */
// The above comment was written by an LLM. 
void TFCSWriteCellsToTree::Print( Option_t* option ) const {
  TString opt( option );
  bool    shortprint = opt.Index( "short" ) >= 0;
  bool    longprint  = msgLvl( MSG::DEBUG ) || ( msgLvl( MSG::INFO ) && !shortprint );
  TString optprint   = opt;
  optprint.ReplaceAll( "short", "" );

  TFCSParametrization::Print( option );

  if ( longprint ) ATH_MSG_INFO( optprint << "  tree ptr=" << m_tree );
}
