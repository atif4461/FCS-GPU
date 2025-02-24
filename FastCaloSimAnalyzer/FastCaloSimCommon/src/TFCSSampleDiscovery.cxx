/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <iostream>
#include <sstream>

#include "TFCSSampleDiscovery.h"

std::string TFCSSampleDiscovery::m_baseDir = "";

/**
 * Constructor for the class 
 * Initializes member variables and checks if base directory is set
 */
// The above comment was written by an LLM. 
TFCSSampleDiscovery::TFCSSampleDiscovery() :
  m_invalid( FCS::DSIDInfo( -1 ) ) {

  if (m_baseDir == "") {
    throw std::runtime_error("DataDir not set for input files either via arg or env var");
  }

}

/**
 * Constructor for TFCSSampleDiscovery class.
 *
 * @param dir The directory path where input files are located.
 * @param fileName The name of the file used to initialize the DSID database.
 * @param debug A flag indicating whether to print debugging information.
 */
// The above comment was written by an LLM. 
TFCSSampleDiscovery::TFCSSampleDiscovery(const std::string& dir, const std::string& fileName,
                                         bool debug)
  : m_invalid(FCS::DSIDInfo(-1)), m_dsidDB(fileName) {

  m_baseDir = dir;

  std::cout << "Reading input files from " << m_baseDir << std::endl;
  std::cout << "Initialising DSID DB..." << std::endl;

  // init the DB
  std::ifstream file = openFile( fileName );
  if ( !file ) {
    std::cerr << "DB initialisation failed: " << fileName << " not found :((("
              << std::endl;
    exit( 1 );
  }

  std::string line;
  while ( getline( file, line ) ) {
    std::stringstream linestream( line );
    int               pdgId, energy, z, dsid;
    float             eta;
    linestream >> pdgId >> energy >> eta >> z >> dsid;

    m_dbDSID.emplace_back( dsid, pdgId, energy, eta, z );
  }

  if ( debug ) {
    for (const FCS::DSIDInfo &info : m_dbDSID) {
      std::cout << info << std::endl;
    }
  }

  std::cout << "DB ready" << std::endl;
}

/**
 * Finds the DSID information that matches the specified particle ID, energy, eta, and z vertex.
 *
 * @param pdgId The particle ID to search for.
 * @param energy The energy of the particle.
 * @param eta The eta value of the particle.
 * @param zVertex The z vertex of the particle.
 * @return A constant reference to the matching DSID information, or an invalid DSID if no match is found.
 */
// The above comment was written by an LLM. 
const FCS::DSIDInfo &TFCSSampleDiscovery::findDSID(int pdgId, int energy,
                                                   float eta, int zVertex) const
{
  for ( const FCS::DSIDInfo& info : m_dbDSID ) {
    if (pdgId == info.pdgId && energy == info.energy && eta == info.eta &&
        zVertex == info.zVertex) {
      return info;
    }
  }

  std::cerr << " Error: no DSID matching pid " << pdgId << " energy " << energy
            << " eta " << eta << " zVertex " << zVertex << " found in DSID DB \'"
            << m_dsidDB << "\'" << std::endl;
  
  return m_invalid;
}

/**
 * Retrieves the energy associated with a given dataset identifier.
 *
 * @param dsid The dataset identifier to retrieve the energy for.
 * @return The energy value corresponding to the specified dataset identifier, or 0 if not found.
 */
// The above comment was written by an LLM. 
int TFCSSampleDiscovery::getEnergy(int dsid) const
{
  for ( const FCS::DSIDInfo& info : m_dbDSID ) {
    if (info.dsid == dsid) {
      return info.energy;
    }
  }

  return 0;
}

/**
 * Retrieves the particle data group ID associated with the specified dataset ID.
 *
 * @param dsid The dataset ID to retrieve the PDG ID for.
 * @return The PDG ID corresponding to the dataset ID, or -1 if not found.
 */
// The above comment was written by an LLM. 
int TFCSSampleDiscovery::getPdgId(int dsid) const
{
  for ( const FCS::DSIDInfo& info : m_dbDSID ) {
    if (info.dsid == dsid) {
      return info.pdgId;
    }
  }

  return -1;
}

/**
 * Finds sample information from an input file based on the given dataset ID.
 *
 * @param inDSID The ID of the dataset to search for.
 * @param fileName The name of the file containing the sample data.
 * @return A SampleInfo object containing the details of the matching sample, or an empty object if not found.
 */
// The above comment was written by an LLM. 
FCS::SampleInfo TFCSSampleDiscovery::findSample(int inDSID, const std::string &fileName) const {
  int         dsid, pdgId, energy, zVertex;
  std::string location, label;
  float       etaMin, etaMax;

  std::ifstream file = openFile(fileName);
  if (!file) {
    std::cerr << " Error: Unable to open file \'" << fileName
              << "\' with input samples data" << std::endl;
    exit(1);
  }

  while ( !file.eof() ) {
    file >> dsid >> location >> label >> pdgId >> energy
          >> etaMin >> etaMax >> zVertex;

    if ( dsid == inDSID ) {
      return FCS::SampleInfo( dsid, m_baseDir + "/" + FCS::DIR_INPUTS + location, label,
                              pdgId, energy, etaMin, etaMax, zVertex );
    }
  }

  std::cerr << " Error: DSID " << inDSID << " not found in input sample file \'"
            << fileName << "\'" << std::endl;
  
  return FCS::SampleInfo(-1);
}

/**
 * Returns the base name of a sample discovery dataset based on the given DSID.
 *
 * @param dsid The DSID of the sample discovery dataset.
 * @return The base name of the sample discovery dataset as a string.
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getBaseName(int dsid) const
{
  for ( const FCS::DSIDInfo& info : m_dbDSID ) {
    if ( info.dsid == dsid ) {
      std::stringstream s;
      s << "mc16_13TeV." << dsid << ".ParticleGun_pid" << info.pdgId
        << "_E" << info.energy << "_disj_eta_m" << (info.eta + 5)
        << "_m" << info.eta << "_" << info.eta << "_"
        << (info.eta + 5) << "_zv_" << info.zVertex;
      return s.str();
    }
  }

  return "";
}

/**
 * Returns the first PCA application name for a given data set ID and version.
 *
 * @param dsid Data set ID
 * @param version Version string
 * @return First PCA application name as a string
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getFirstPCAAppName(int dsid,
                                                    const std::string &version) const {
  return getName( dsid, "firstPCA_App", m_baseDir + "/" + FCS::DIR_FIRSTPCA, version );
}

/**
 * Returns the second PCA name for a given dataset identifier and version.
 *
 * @param dsid Dataset identifier
 * @param version Version string
 * @return Second PCA name as a string
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getSecondPCAName(int dsid,
                                                  const std::string &version) const
{
  return getName( dsid, "secondPCA", m_baseDir + "/" + FCS::DIR_DSID, version );
}

/**
 * Returns the shape name for a given dataset identifier and version.
 *
 * @param dsid Dataset identifier
 * @param version Version string
 * @return Shape name as a string
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getShapeName(int dsid,
                                              const std::string &version) const
{
  return getName( dsid, "shapepara", m_baseDir + "/" + FCS::DIR_DSID, version );
}

/**
 * Returns the average simulation shape name for a given dataset identifier and version.
 *
 * @param dsid Dataset identifier
 * @param version Version string
 * @return Average simulation shape name as a string
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getAvgSimShapeName(int dsid,
                                                    const std::string &version) const
{
  return getName( dsid, "AvgSimShape", m_baseDir + "/" + FCS::DIR_DSID, version );
}

/**
 * Returns the file name of the Einterpol mean for a given particle data group ID and version.
 *
 * @param pdgId Particle Data Group ID
 * @param version Version string
 * @return File name of the Einterpol mean
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getEinterpolMeanName(int pdgId,
                                                      const std::string &version) const
{
  return m_baseDir + "/" + FCS::DIR_INTERPOLATION + "mc16_13TeV.pid" + std::to_string( pdgId ) + ".EinterpolMean." + version + ".root";
}

/**
 * Returns the full path of the parametrization file for a given version.
 *
 * @param version The version number of the parametrization file.
 * @return The full path of the parametrization file as a string.
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getParametrizationName(const std::string &version)
{
  return m_baseDir + "/" + FCS::DIR_PARAMETRIZATION  + "TFCSparam_" + version + ".root";
}

/**
 * Returns the Wiggle file name based on the provided parameters.
 *
 * @param etaRange The eta range of the Wiggle file.
 * @param sampling The sampling value used in the Wiggle file.
 * @param isNewWiggle Flag indicating whether it is a new Wiggle file.
 * @param version The version number of the Wiggle file.
 * @return The constructed Wiggle file name as a string.
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getWiggleName(const std::string &etaRange,
                                               int sampling,
                                               bool isNewWiggle,
                                               const std::string &version)
{
  if ( isNewWiggle ) {
    return m_baseDir + "/" + FCS::DIR_WIGGLE + "Wiggle/" + etaRange + "." + version + ".root";
  } else {
    return m_baseDir + "/" + FCS::DIR_WIGGLE + "Wiggle_old/" + etaRange +
      "/wiggle_input_deriv_Sampling_" + std::to_string( sampling ) + ".ver03.root";    
  }
}

/**
 * Returns the geometry tree of the sample discovery
 *
 * @return The geometry tree as a string
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::geometryTree()
{
  return "ATLAS-R2-2016-01-00-01";
}

/**
 * Returns the geometry file name for the sample discovery.
 *
 * @return The full path of the geometry file as a string.
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::geometryName()
{
  return m_baseDir + "/" + FCS::DIR_GEOMETRY + "Geometry-ATLAS-R2-2016-01-00-01.root";
}

/**
 * Returns an array of geometry file names for FCAL electrodes
 */
// The above comment was written by an LLM. 
std::array<std::string, 3> TFCSSampleDiscovery::geometryNameFCal() {
  return {
    m_baseDir + "/" + FCS::DIR_GEOMETRY + "FCal1-electrodes.sorted.HV.09Nov2007.dat",
    m_baseDir + "/" + FCS::DIR_GEOMETRY + "FCal2-electrodes.sorted.HV.April2011.dat",
    m_baseDir + "/" + FCS::DIR_GEOMETRY + "FCal3-electrodes.sorted.HV.09Nov2007.dat",
  };
}

std::string TFCSSampleDiscovery::geometryMap() {
  return m_baseDir + "/" + FCS::DIR_GEOMETRY + "cellId_vs_cellHashId_map.txt";
}

/**
 * Opens a file with the specified filename, attempting to locate it in the current directory 
 * and then in a common data folder if necessary.
 *
 * @param fileName The name of the file to be opened.
 * @return An ifstream object representing the opened file.
 */
// The above comment was written by an LLM. 
std::ifstream TFCSSampleDiscovery::openFile(const std::string &fileName) const
{
  // first try directly
  std::ifstream file( fileName );
  // if not found try common data folder
  if ( !file && fileName.find( "/" ) == std::string::npos ) {
    std::string path = std::getenv( "FCSSTANDALONE" );
    return std::ifstream( path + "/data/" + fileName );
  }

  return file;
}

/**
 * Returns the full path of the sample discovery file 
 * @param dsid dataset identifier
 * @param label sample label
 * @param basedir base directory path
 * @param version software version
 * @return string representing the full path of the sample discovery file
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getName(int dsid, const std::string &label,
                                         const std::string &basedir,
                                          const std::string& version ) const {
  return basedir + getBaseName( dsid ) + "." + label + "." + version + ".root";
}
