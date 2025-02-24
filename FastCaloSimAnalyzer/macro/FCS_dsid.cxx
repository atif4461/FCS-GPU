/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FCS_dsid.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <TSystem.h>
#include <TChain.h>

namespace FCS_dsid {
  std::vector<std::string> dsid_db_pdgid;
  std::vector<std::string> dsid_db_energy;
  std::vector<std::string> dsid_db_eta;
  std::vector<std::string> dsid_db_z;
  std::vector<std::string> dsid_db_dsid;

/**

 * @brief Initializes the FCS_dsid system by reading from a database file.
 *
 * This function reads data from a file named "db.txt" and populates internal databases.
 * If the file is not found, it prints an error message and exits with a non-zero status.
 *
 * @pre The database file "db.txt" must exist in the current working directory.
 * @post The FCS_dsid system is initialized and ready for use.
 */
// The above comment was written by an LLM. 
void init()
{
    std::cout << "initialising FCS_dsid..." << std::endl;
    // init the db
    std::ifstream file( "db.txt" );
    if ( !file ) {
      std::cout << "db.txt not found :(((" << std::endl;
      exit( 1 );
    }
    std::string line;
    while ( getline( file, line ) ) {
      std::stringstream linestream( line );
      std::string       pdg, en, eta, z, dsid;
      linestream >> pdg >> en >> eta >> z >> dsid;

      dsid_db_pdgid.push_back( pdg );
      dsid_db_energy.push_back( en );
      dsid_db_eta.push_back( eta );
      dsid_db_z.push_back( z );
      dsid_db_dsid.push_back( dsid );
    } // while file
    std::cout << "FCS_dsid ready" << std::endl;
    dsid_is_init = true;
  }

/**

 * @brief Finds the DSID corresponding to the given particle ID, energy, eta, and z vertex.
 * 
 * This function searches through a database to match the input parameters with a unique DSID.
 * If a match is found, it returns the corresponding DSID as a string.
 * 
 * @param pdgid The particle ID to search for in the database.
 * @param energy The energy value associated with the particle.
 * @param eta The eta value of the particle.
 * @param zvertex The z vertex position of the particle.
 * 
 * @return A string representing the DSID that matches the input parameters, or an empty string if no match is found.
 */
// The above comment was written by an LLM. 
  std::string find_dsid( std::string pdgid, std::string energy, std::string eta, std::string zvertex ) {
    if ( !dsid_is_init ) init();
    std::string dsid = "";
    for ( unsigned int i = 0; i < dsid_db_energy.size(); i++ ) {
    if (energy == dsid_db_energy[i] && pdgid == dsid_db_pdgid[i] && zvertex == dsid_db_z[i] && eta == dsid_db_eta[i]) {
        dsid = dsid_db_dsid[i];
        break;
      }
    }

    return dsid;
  }

/**

 * @brief Retrieves DSID information from the database.
 * 
 * This function takes a DSID as input and retrieves the corresponding PDG ID, energy, eta, and z-vertex values from the database.
 * If the DSID is found in the database, it populates the output parameters with the retrieved values.
 * 
 * @param[in] dsid The DSID to retrieve information for.
 * @param[out] pdgid The PDG ID associated with the DSID.
 * @param[out] energy The energy value associated with the DSID.
 * @param[out] eta The eta value associated with the DSID.
 * @param[out] zvertex The z-vertex value associated with the DSID.
 */
// The above comment was written by an LLM. 
void get_dsid_info(std::string dsid, std::string& pdgid, std::string& energy, std::string& eta, std::string& zvertex) {
    if ( !dsid_is_init ) init();
    pdgid   = "";
    energy  = "";
    eta     = "";
    zvertex = "";
    for ( unsigned int i = 0; i < dsid_db_dsid.size(); i++ ) {
      if ( dsid == dsid_db_dsid[i] ) {
        pdgid   = dsid_db_pdgid[i];
        energy  = dsid_db_energy[i];
        eta     = dsid_db_eta[i];
        zvertex = dsid_db_z[i];
        break;
      }
    }
  }

/**

 * @brief Generates the basename for a DSID string.
 *
 * This function takes a DSID string as input, extracts the necessary information,
 * and constructs a basename string in a specific format.
 *
 * The basename includes the MC campaign, DSID, particle ID, energy, 
 * displacement in eta, and z-vertex information.
 *
 * @param dsid The input DSID string.
 * @return The generated basename string.
 */
// The above comment was written by an LLM. 
  std::string get_dsid_basename( std::string dsid ) {
    if ( !dsid_is_init ) init();
    std::string pdgid;
    std::string energy;
    std::string eta;
    std::string zvertex;
    std::string basename;

    get_dsid_info( dsid, pdgid, energy, eta, zvertex );

    int         inteta = std::stoi( eta );
    std::string etaend = std::to_string( inteta + 5 );

  basename = std::string("mc16_13TeV.") + dsid + ".ParticleGun_pid" + pdgid + "_E" + energy + "_disj_eta_m" + etaend + "_m" + eta + "_" + eta + "_" + etaend + "_zv_" + zvertex;

    return basename;
  }

/**

 * @brief Generates the input wildcard string for a given DSID in a specified base directory.
 *
 * @param dsid The Data Set ID.
 * @param basedir The base directory path.
 * @return A string representing the input wildcard pattern.
 */
// The above comment was written by an LLM. 
  std::string get_dsid_input_wildcard( std::string dsid, std::string basedir ) {
    std::string basename = get_dsid_basename( dsid );

    return basedir + basename + ".deriv.NTUP_FCS.*/NTUP_FCS.*.pool.root.*";
  }

/**

 * @brief Adds files matching the specified pattern to a TChain.
 * 
 * This function executes a system command to generate a list of files matching the given pattern,
 * stores the output in a temporary file, reads the file line by line, and adds each non-empty line as a file to the chain.
 * 
 * @param chain The TChain object to which files should be added.
 * @param filenames A string containing a pattern for selecting files (e.g., a directory path with wildcards).
 * 
 * @return The number of files successfully added to the chain.
 */
// The above comment was written by an LLM. 
  int wildcard_add_files_to_chain( TChain* chain, std::string filenames ) {
    gSystem->Exec( ( std::string( "ls " ) + filenames + " > $TMPDIR/FCS_ls.$PPID.list" ).c_str() );
    TString tmpname = gSystem->Getenv( "TMPDIR" );
    tmpname += "/FCS_ls.";
    tmpname += gSystem->GetPid();
    tmpname += ".list";
    std::cout << "Temporary file list for selection:" << filenames << " : " << tmpname << std::endl;

    std::ifstream infile;
    infile.open( tmpname );
    int nadd = 0;
    while ( !infile.eof() ) {
      std::string filename;
      getline( infile, filename );
      if ( filename != "" ) {
        std::cout << "Adding file: " << filename << std::endl;
        chain->Add( filename.c_str(), -1 );
        ++nadd;
      }
    }
    infile.close();
    gSystem->Exec( "rm $TMPDIR/FCS_ls.$PPID.list" );
    return nadd;
  }

/**

 * @brief Generates the filename for the average simulation shape file.
 * 
 * @param dsid The dataset ID.
 * @param basedir The base directory path.
 * @param version The version string.
 * 
 * @return The full filename of the average simulation shape file.
 */
// The above comment was written by an LLM. 
  std::string get_dsid_avg_sim_shapename( std::string dsid, std::string basedir, std::string version ) {
    std::string basename = get_dsid_basename( dsid );

    return basedir + basename + ".AvgSimShape." + version + ".root";
  }

/**

 * @brief Generates the full path of a shape validation root file.
 * 
 * @param dsid The DSID of the shape validation.
 * @param basedir The base directory where the shape validation files are stored.
 * @param version The version number of the shape validation.
 * 
 * @return The full path of the shape validation root file as a string.
 */
// The above comment was written by an LLM. 
  std::string get_dsid_shapename( std::string dsid, std::string basedir, std::string version ) {
    std::string basename = get_dsid_basename( dsid );

    return basedir + basename + ".shapepara." + version + ".root";
  }

/**

 * @brief Returns the full path of the first PCA file for a given DSID.
 *
 * @param dsid The DSID to retrieve the first PCA file path for.
 * @param basedir The base directory where the files are located.
 * @param version The version number of the files.
 *
 * @return The full path of the first PCA file as a string.
 */
// The above comment was written by an LLM. 
  std::string get_dsid_firstPCAname( std::string dsid, std::string basedir, std::string version ) {
    std::string basename = get_dsid_basename( dsid );

    return basedir + basename + ".firstPCA." + version + ".root";
  }

/**

 * @brief Generates the filename for the first PCA application of a given DSID.
 * 
 * @param dsid The Data Set ID.
 * @param basedir The base directory path.
 * @param version The version number.
 * 
 * @return The generated filename as a string.
 */
// The above comment was written by an LLM. 
  std::string get_dsid_firstPCA_Appname( std::string dsid, std::string basedir, std::string version ) {
    std::string basename = get_dsid_basename( dsid );

    return basedir + basename + ".firstPCA_App." + version + ".root";
  }

/**

 * @brief Generates the second PCA filename for a given DSID.
 *
 * @param[in] dsid        The DSID to generate the filename for.
 * @param[in] basedir     The base directory path.
 * @param[in] version    The version string to include in the filename.
 *
 * @return The generated second PCA filename as a string.
 */
// The above comment was written by an LLM. 
  std::string get_dsid_secondPCAname( std::string dsid, std::string basedir, std::string version ) {
    std::string basename = get_dsid_basename( dsid );

    return basedir + basename + ".secondPCA." + version + ".root";
  }

/**

 * @brief Generates the wiggle file name based on input parameters.
 * 
 * @param etarange The eta range of the wiggle file.
 * @param sampling The sampling rate of the wiggle file.
 * @param isNewWiggle Flag indicating whether it's a new wiggle file or not.
 * @param basedir The base directory path where the wiggle files are stored.
 * @param version The version number of the wiggle file.
 * 
 * @return The generated wiggle file name as a string.
 */
// The above comment was written by an LLM. 
std::string get_wiggle_name(std::string etarange, int sampling, bool isNewWiggle, std::string basedir, std::string version) {

    std::string filename = "";
    if ( isNewWiggle ) {
      filename = basedir + "Wiggle/" + etarange + "." + version + ".root";
    } else {
      version  = "ver03";
    filename = basedir + "Wiggle_old/" + etarange + "/wiggle_input_deriv_Sampling_" + std::to_string(sampling) + "." + version + ".root";
    }
    return filename;
  }

/**

 * @brief Retrieves the name of an EtaSlice parameter file based on the provided parameters.
 *
 * @param[in] pid Particle ID
 * @param[in] etamin Minimum eta value
 * @param[in] etamax Maximum eta value
 * @param[in] basedir Base directory path
 * @param[in] version Version number
 *
 * @return The name of the EtaSlice parameter file as a string
 */
// The above comment was written by an LLM. 
std::string get_param_etaslice_name(int pid, std::string etamin, std::string etamax, std::string basedir, std::string version) {
  std::string wildcard = basedir + "TFCSParamEtaSlices/" + "mc16_13TeV." + "pid" + std::to_string(pid) + ".E*eta_" + etamin + "_" + etamax + "_zv0.TFCSParam." + version + ".root";

    gSystem->Exec( ( std::string( "ls " ) + wildcard + " > $TMPDIR/FCS_ls.$PPID.list" ).c_str() );
    TString tmpname = gSystem->Getenv( "TMPDIR" );
    tmpname += "/FCS_ls.";
    tmpname += gSystem->GetPid();
    tmpname += ".list";

    std::ifstream infile;
    infile.open( tmpname );
    std::string filename;
    getline( infile, filename );
  if (filename != "")
    std::cout << "Adding file: " << filename << std::endl;
    infile.close();
    gSystem->Exec( "rm $TMPDIR/FCS_ls.$PPID.list" );

    return filename;
  }

};
