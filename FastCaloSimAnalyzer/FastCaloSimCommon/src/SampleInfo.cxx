/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SampleInfo.h"

namespace FCS {

/**

 * @brief Overloads the << operator to output DSIDInfo objects in a human-readable format.
 *
 * Outputs the contents of the DSIDInfo object, including DSID, PDG ID, Energy, Eta, and Z vertex.
 *
 * @param out The output stream where the formatted data will be written.
 * @param info The DSIDInfo object being output.
 *
 * @return A reference to the output stream after writing the formatted data.
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

 * @brief Overloads the << operator to output SampleInfo objects to an ostream.
 * 
 * Outputs the DSID and location of the SampleInfo object in a tab-separated format.
 * 
 * @param out The ostream to which the SampleInfo object is to be output.
 * @param info The SampleInfo object being output.
 * 
 * @return A reference to the ostream after outputting the SampleInfo object.
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
