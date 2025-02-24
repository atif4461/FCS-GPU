/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HepPDT/ParticleID.hh"

#include <cmath>
#include <iostream>
#include <limits>

namespace HepPDT
{

/**

 * @brief Constructor for ParticleID class, initializes charge based on particle ID.
 * 
 * @param pdgID The particle ID used to determine the charge.
 * 
 * @details Sets the charge (m_charge) according to the following rules:
 * - Positive charge (+1) for pdgIDs: 11, 211, 2212
 * - Negative charge (-1) for pdgIDs: -11, -211, -2212
 * - Zero charge (0) for pdgIDs: 22, +/-2112, 111
 * - For unsupported pdgIDs, sets charge to NaN and prints an error message.
 */
// The above comment was written by an LLM. 
ParticleID::ParticleID(const int pdgID)
{
    if ( pdgID == 11 || pdgID == 211 || pdgID == 2212 )
      m_charge = 1.;
    else if ( pdgID == -11 || pdgID == -211 || pdgID == -2212 )
      m_charge = -1.;
    else if ( pdgID == 22 || std::abs( pdgID ) == 2112 || pdgID == 111 )
      m_charge = 0;
    else {
      std::cerr << "Error: This pdgID is not supported: " << std::endl;
      m_charge = std::numeric_limits<double>::quiet_NaN();
    }
  }

} // namespace HepPDT
