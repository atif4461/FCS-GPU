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
 * Initializes the FCS DSID database by reading from a file named db.txt.
 *
 * The function reads the file line by line, parsing each line into its components,
 * and stores them in separate vectors for later use.
 *
 * If the file does not exist, an error message is printed and the program exits.
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
 * Finds the dataset ID based on particle type, energy, pseudorapidity, and z-vertex position.
 *
 * @param pdgid Particle data group ID
 * @param energy Energy of the particle
 * @param eta Pseudorapidity of the particle
 * @param zvertex Z-position of the vertex
 * @return Dataset ID as a string
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
 * Retrieves DSID information from database.
 *
 * @param[in]  dsid  Dataset identifier
 * @param[out] pdgid  Particle data group identifier
 * @param[out] energy  Energy value associated with dataset
 * @param[out] eta     Pseudorapidity value of interaction point
 * @param[out] zvertex  Z-coordinate of vertex position
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
 * Returns the base name of a DSID string.
 *
 * @param dsid The input DSID string.
 * @return A string representing the base name of the DSID.
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
 * Returns the input wildcard string for a given DSID in a specified base directory.
 *
 * @param[in] dsid The DSID to generate the wildcard for.
 * @param[in] basedir The base directory path.
 * @return The generated wildcard string.
 */
// The above comment was written by an LLM. 
  std::string get_dsid_input_wildcard( std::string dsid, std::string basedir ) {
    std::string basename = get_dsid_basename( dsid );

    return basedir + basename + ".deriv.NTUP_FCS.*/NTUP_FCS.*.pool.root.*";
  }

/**
 * Adds files matching the specified pattern to a TChain object.
 *
 * @param chain pointer to the TChain object where files will be added
 * @param filenames string containing a wildcard pattern for selecting files
 * @return number of files successfully added to the chain
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
 * Returns the average simulation shape filename for a given DSID.
 *
 * @param dsid        Dataset identifier
 * @param basedir     Base directory path
 * @param version    Version string
 * @return Average simulation shape filename
 */
// The above comment was written by an LLM. 
  std::string get_dsid_avg_sim_shapename( std::string dsid, std::string basedir, std::string version ) {
    std::string basename = get_dsid_basename( dsid );

    return basedir + basename + ".AvgSimShape." + version + ".root";
  }

/**
 * Returns the full path of the shape validation root file 
 * for a given dataset identifier and software version.
 *
 * @param dsid Dataset identifier
 * @param basedir Base directory path
 * @param version Software version
 * @return Full path of the shape validation root file
 */
// The above comment was written by an LLM. 
  std::string get_dsid_shapename( std::string dsid, std::string basedir, std::string version ) {
    std::string basename = get_dsid_basename( dsid );

    return basedir + basename + ".shapepara." + version + ".root";
  }

/**
 * Returns the full path of the first PCA root file for a given DSID.
 *
 * @param dsid        The DSID to generate the path for
 * @param basedir     The base directory where the files are located
 * @param version    The version number of the files
 * @return The full path of the first PCA root file as a string
 */
// The above comment was written by an LLM. 
  std::string get_dsid_firstPCAname( std::string dsid, std::string basedir, std::string version ) {
    std::string basename = get_dsid_basename( dsid );

    return basedir + basename + ".firstPCA." + version + ".root";
  }

/**
 * Returns the full path of the first PCA App root file for a given DSID.
 *
 * @param dsid        The dataset identifier.
 * @param basedir     The base directory where the files are located.
 * @param version    The version number of the file.
 * @return The full path of the first PCA App root file.
 */
// The above comment was written by an LLM. 
  std::string get_dsid_firstPCA_Appname( std::string dsid, std::string basedir, std::string version ) {
    std::string basename = get_dsid_basename( dsid );

    return basedir + basename + ".firstPCA_App." + version + ".root";
  }

/**
 * Returns the full path of the second PCA root file for a given DSID.
 *
 * @param dsid        The DSID to generate the filename for
 * @param basedir     The base directory where the files are located
 * @param version    The version number of the file
 * @return The full path of the second PCA root file
 */
// The above comment was written by an LLM. 
  std::string get_dsid_secondPCAname( std::string dsid, std::string basedir, std::string version ) {
    std::string basename = get_dsid_basename( dsid );

    return basedir + basename + ".secondPCA." + version + ".root";
  }

/**
 * @brief Generates the wiggle file name based on input parameters
 * @param etarange ETARange value
 * @param sampling Sampling value
 * @param isNewWiggle Flag indicating new Wiggle
 * @param basedir Base directory path
 * @param version Version number
 * @return Wiggle file name as a string
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
 * @brief Retrieves the name of an EtaSlice parameter file based on input parameters.
 * @param pid Particle ID
 * @param etamin Minimum eta value
 * @param etamax Maximum eta value
 * @param basedir Base directory path
 * @param version Version number
 * @return Name of the corresponding EtaSlice parameter file
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
