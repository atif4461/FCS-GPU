/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <iostream>
#include <sstream>

#include "TFCSSampleDiscovery.h"

std::string TFCSSampleDiscovery::m_baseDir = "";

/**

 * @brief Constructor for the TFCSSampleDiscovery class.
 * 
 * Initializes the object with default values and checks if the base directory is set.
 * If the base directory is not set, it throws a runtime error.
 */
// The above comment was written by an LLM. 
TFCSSampleDiscovery::TFCSSampleDiscovery() :
  m_invalid( FCS::DSIDInfo( -1 ) ) {

  if (m_baseDir == "") {
    throw std::runtime_error("DataDir not set for input files either via arg or env var");
  }

}

/**

 * @brief Constructor for TFCSSampleDiscovery class.
 * 
 * Initializes the object with the directory path and file name of the DSID database.
 * It reads the input files from the specified directory, initializes the DSID database,
 * and stores the data in the m_dbDSID vector.
 * 
 * @param dir The directory path where the input files are located.
 * @param fileName The name of the file containing the DSID database.
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

 * @brief Finds a DSIDInfo object that matches the specified particle ID, energy, eta, and z-vertex.
 * 
 * @param pdgId The particle ID to search for.
 * @param energy The energy value to match.
 * @param eta The eta value to match.
 * @param zVertex The z-vertex value to match.
 * 
 * @return A constant reference to the matching DSIDInfo object, or a default invalid object if no match is found.
 * 
 * @note If no matching DSIDInfo object is found, an error message is printed to the standard error stream.
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

 * @brief Retrieves the energy associated with a given dataset identifier.
 *
 * This function iterates through the database of DSID information to find a match
 * for the specified dataset identifier and returns the corresponding energy value.
 *
 * If no matching DSID is found, the function returns 0.
 *
 * @param dsid The dataset identifier to search for in the database.
 * @return The energy associated with the given dataset identifier, or 0 if not found.
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

 * @brief Retrieves the PDG ID associated with a given DSID.
 * 
 * Iterates through the database of DSID information to find a match for the provided DSID,
 * returning the corresponding PDG ID if found.
 * 
 * @param dsid The DSID to search for in the database.
 * @return The PDG ID associated with the given DSID, or -1 if no match is found.
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

 * @brief Finds a sample from an input file based on the provided dataset ID.
 *
 * This function opens the specified file, reads its contents line by line, and searches for a match with the given dataset ID.
 * If a match is found, it constructs and returns an FCS::SampleInfo object containing the sample's details.
 * If no match is found after reading the entire file, an error message is printed and an FCS::SampleInfo object with default values is returned.
 *
 * @param[in] inDSID The dataset ID to search for in the input file.
 * @param[in] fileName The path to the input file containing sample data.
 * @return An FCS::SampleInfo object representing the found sample, or an object with default values if the sample is not found.
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

 * @brief Retrieves the base name of a sample discovery dataset.
 *
 * This method iterates over the list of DSID information in the database and returns
 * the corresponding base name string when a match is found with the provided DSID.
 *
 * The base name string is constructed from various attributes such as the energy,
 * particle ID, eta values, and z-vertex position.
 *
 * If no matching DSID is found, an empty string is returned.
 *
 * @param dsid The DSID to search for in the database.
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

 * @brief Retrieves the first PCA application name for a given dataset ID and version.
 *
 * @param dsid The dataset ID.
 * @param version The version string.
 * @return The first PCA application name as a string.
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getFirstPCAAppName(int dsid,
                                                    const std::string &version) const {
  return getName( dsid, "firstPCA_App", m_baseDir + "/" + FCS::DIR_FIRSTPCA, version );
}

/**

 * @brief Retrieves the second PCA name for a given dataset ID and version.
 * 
 * @param dsid The dataset ID.
 * @param version The version string.
 * @return The second PCA name as a string.
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getSecondPCAName(int dsid,
                                                  const std::string &version) const
{
  return getName( dsid, "secondPCA", m_baseDir + "/" + FCS::DIR_DSID, version );
}

/**

 * @brief Retrieves the shape name associated with a given dataset ID and version.
 *
 * @param dsid The dataset ID.
 * @param version The version string.
 * @return A string representing the shape name.
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getShapeName(int dsid,
                                              const std::string &version) const
{
  return getName( dsid, "shapepara", m_baseDir + "/" + FCS::DIR_DSID, version );
}

/**

 * @brief Retrieves the average simulation shape name for a given dataset ID and version.
 *
 * @param dsid The dataset ID.
 * @param version The version string.
 * @return The average simulation shape name as a string.
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getAvgSimShapeName(int dsid,
                                                    const std::string &version) const
{
  return getName( dsid, "AvgSimShape", m_baseDir + "/" + FCS::DIR_DSID, version );
}

/**

 * @brief Returns the file name of the Einterpol Mean data for a given particle ID and version.
 *
 * @param pdgId The Particle Data Group ID.
 * @param version The version string of the data.
 * @return A string representing the full path to the Einterpol Mean data file.
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getEinterpolMeanName(int pdgId,
                                                      const std::string &version) const
{
  return m_baseDir + "/" + FCS::DIR_INTERPOLATION + "mc16_13TeV.pid" + std::to_string( pdgId ) + ".EinterpolMean." + version + ".root";
}

/**

 * @brief Returns the full path of the parametrization file for a given version.
 * 
 * @param version The version number of the parametrization file.
 * @return A string containing the full path of the parametrization file.
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getParametrizationName(const std::string &version)
{
  return m_baseDir + "/" + FCS::DIR_PARAMETRIZATION  + "TFCSparam_" + version + ".root";
}

/**

 * @brief Returns the Wiggle file name based on the input parameters.
 * 
 * @param[in] etaRange The eta range of the Wiggle file.
 * @param[in] sampling The sampling value used in the Wiggle file.
 * @param[in] isNewWiggle A flag indicating whether it's a new Wiggle file or not.
 * @param[in] version The version number of the Wiggle file.
 * 
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

 * @brief Returns the geometry tree of the TFCSSampleDiscovery object.
 * @return A string representing the geometry tree.
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::geometryTree()
{
  return "ATLAS-R2-2016-01-00-01";
}

/**

 * @brief Returns the geometry file name for the ATLAS R2 2016 setup.
 * @return The full path of the geometry file as a string.
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::geometryName()
{
  return m_baseDir + "/" + FCS::DIR_GEOMETRY + "Geometry-ATLAS-R2-2016-01-00-01.root";
}

/**

 * @brief Returns an array of file names for FCAL geometry discovery.
 * @return A std::array of three strings representing file paths for FCAL geometry data.
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

 * @brief Opens a file with the specified filename, attempting to find it in the current directory and then in the FCSSTANDALONE/data directory if necessary.
 *
 * @param[in] fileName The name of the file to be opened.
 *
 * @return A reference to an ifstream object associated with the opened file.
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

 * @brief Returns the full file path of a sample discovery file.
 * 
 * @param dsid Dataset identifier.
 * @param label Sample label.
 * @param basedir Base directory path.
 * @param version Version string.
 * 
 * @return The constructed file path as a string.
 */
// The above comment was written by an LLM. 
std::string TFCSSampleDiscovery::getName(int dsid, const std::string &label,
                                         const std::string &basedir,
                                          const std::string& version ) const {
  return basedir + getBaseName( dsid ) + "." + label + "." + version + ".root";
}
