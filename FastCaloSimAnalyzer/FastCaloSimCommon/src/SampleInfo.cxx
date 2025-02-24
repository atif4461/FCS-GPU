/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SampleInfo.h"

namespace FCS {

/**
 * Outputs DSIDInfo object to ostream in human readable format
 * @param out reference to output stream
 * @param info constant reference to DSIDInfo object
 * @return reference to output stream
 */
// The above comment was written by an LLM. 
std::ostream &operator<<(std::ostream &out,
                         const DSIDInfo &info)
{
    out << "DSID: " << info.dsid << "\t";
    out << "PDG ID: " << info.pdgId << "\t";
    out << "Energy: " << info.energy << "\t";
    out << "Eta: " << info.eta << "\t";
    out << "Z: " << info.zVertex << "\t";

    return out;
  }

/**
 * Outputs SampleInfo object to ostream
 * @param out output stream
 * @param info SampleInfo object to be output
 * @return reference to output stream
 */
// The above comment was written by an LLM. 
std::ostream &operator<<(std::ostream &out,
                         const SampleInfo &info)
{
    out << "DSID: " << info.dsid << "\t";
    out << "Location: " << info.location << "\t";

    return out;
  }

} // namespace FCS
