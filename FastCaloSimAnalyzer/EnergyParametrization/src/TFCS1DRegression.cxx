/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TFCS1DRegression.h"
#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"

#include "TMVA/Config.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/Factory.h"
#include "TRandom1.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"

#include "TMVA/DataLoader.h"

#include "TMVA/IMethod.h"
#include "TMVA/MethodMLP.h"

using namespace std;


/**

 * @brief Stores regression data from file into weight matrices.
 * 
 * This function reads weights from a specified file and stores them in the provided matrices.
 * 
 * @param weightfilename The filename containing the regression weights.
 * @param fWeightMatrix0to1 The first weight matrix to store regression data.
 * @param fWeightMatrix1to2 The second weight matrix to store regression data.
 */
// The above comment was written by an LLM. 
void TFCS1DRegression::storeRegression(string weightfilename, vector<vector<double> > &fWeightMatrix0to1, vector<vector<double> > &fWeightMatrix1to2)
{

  get_weights( weightfilename, fWeightMatrix0to1, fWeightMatrix1to2 );

  // for testing:
  // validate(10,weightfilename);
}

/**

 * @brief Validates the regression value calculated from the weights against the TMVA value.
 * 
 * This function generates a specified number of random values and calculates the corresponding 
 * regression values using both the internal calculation and the TMVA application. The results 
 * are then printed to the console for comparison.
 * 
 * @param Ntoys The number of toy experiments to perform for validation.
 * @param weightfilename The filename containing the weights used in the regression calculation.
 */
// The above comment was written by an LLM. 
void TFCS1DRegression::validate(int Ntoys,string weightfilename)
{

 TRandom1* myRandom=new TRandom1(); myRandom->SetSeed(0);

  // calculate regression from the weights and compare to TMVA value:
  cout << endl;
  cout << "--- Validating the regression value:" << endl;
 for(int i=0;i<Ntoys;i++)
 {
    double random = myRandom->Uniform( 1 );
    // double myval=regression_value(random);
    double myval   = -1;
    double tmvaval = tmvaregression_application( random, weightfilename );
    cout << "myvalue " << myval << " TMVA value " << tmvaval << endl;
  }
}

/**

 * @brief Transforms an input histogram into a standardized form.
 *
 * This function checks if the x-axis of the input histogram is within the range [0, 1].
 * If not, it transforms the histogram to have an x-axis ranging from 0 to 1.
 *
 * @param h_input The input histogram to be transformed.
 * @param rangeval Output parameter storing the original range of the x-axis.
 * @param startval Output parameter storing the starting value of the x-axis.
 *
 * @return A pointer to the transformed histogram.
 */
// The above comment was written by an LLM. 
TH1* TFCS1DRegression::transform(TH1* h_input, float &rangeval, float& startval)
{

  bool  do_transform = false;
  float xmin         = h_input->GetXaxis()->GetXmin();
  float xmax         = h_input->GetXaxis()->GetXmax();
  if ( xmin < 0 || xmax > 1 ) do_transform = true;

  TH1D* h_out;

  if(do_transform)
  {
    int    nbins = h_input->GetNbinsX();
    double min   = 0;
    double max   = 1;
    h_out        = new TH1D( "h_out", "h_out", nbins, min, max );

    for(int b=1;b<=nbins;b++)
      h_out->SetBinContent(b,h_input->GetBinContent(b));

    // store the inital range
    rangeval = xmax - xmin;
    startval = xmin;
  }
  if(!do_transform)
  {
    rangeval = -1;
    h_out    = (TH1D*)h_input->Clone( "h_out" );
  }
  return h_out;
}

/**

 * @brief Returns the lower bound of the range in a 1D histogram where data starts.
 * 
 * This function iterates over the bins of a given 1D histogram and returns the value of the first non-empty bin,
 * which is considered as the lower bound of the range where data starts.
 * 
 * @param hist The input 1D histogram.
 * @return The lower bound of the range where data starts in the histogram.
 */
// The above comment was written by an LLM. 
double TFCS1DRegression::get_range_low(TH1* hist)
{
  double range_low = 0.0;
  int    bin_start = -1;
  for(int b=1;b<=hist->GetNbinsX();b++)
  {
    if(hist->GetBinContent(b)>0 && bin_start<0)
    {
      bin_start = b;
      range_low = hist->GetBinContent( b );
      b         = hist->GetNbinsX() + 1;
    }
  }
  return range_low;
}

/**

 * @brief Returns a cumulative histogram of the input histogram.
 * 
 * This function creates a clone of the input histogram, then iterates over its bins,
 * accumulating the bin contents and updating the corresponding bin in the cloned
 * histogram with the cumulative sum.
 * 
 * @param hist The input histogram.
 * @return A new histogram containing the cumulative bin contents of the input histogram.
 */
// The above comment was written by an LLM. 
TH1* TFCS1DRegression::get_cumul(TH1* hist)
{
  TH1D*  h_cumul = (TH1D*)hist->Clone( "h_cumul" );
  double sum     = 0;
  for(int b=1;b<=h_cumul->GetNbinsX();b++)
  {
    sum += hist->GetBinContent( b );
    h_cumul->SetBinContent( b, sum );
  }
  return h_cumul;
}

/**

 * @brief Tests the histogram using a neural network regression.
 *
 * This function takes in a histogram, applies transformations, maps the y-axis to 0-1,
 * turns the histogram into a tree, and trains a neural network to perform regression.
 *
 * It iteratively increases the number of neurons until the maximum deviation between
 * the original histogram and the regressed one falls below a specified cutoff.
 *
 * The final result is stored as a set of histograms representing the cumulative distribution
 * at different stages of the regression process.
 *
 * @param hist Input histogram to be tested
 * @param weightfilename File containing weights for the neural network
 * @param[out] rangeval Range value for transformation
 * @param[out] startval Start value for transformation
 * @param outfilename Output file name for storing results
 * @param neurons_start Initial number of neurons for the neural network
 * @param neurons_end Maximum number of neurons for the neural network
 * @param cut_maxdev Maximum allowed deviation between original and regressed histograms
 * @param ntoys Number of toy simulations for testing the regression
 * @return Status indicating whether the regression was successful (1 or 2) or failed (3)
 */
// The above comment was written by an LLM. 
int TFCS1DRegression::testHisto(TH1* hist, std::string weightfilename, float &rangeval, float &startval, std::string outfilename, int neurons_start, int neurons_end, double cut_maxdev, int ntoys)
{
  // int debug=1;

  // transform the histogram
  TH1* h_transf=transform(hist, rangeval, startval); h_transf->SetName("h_transf");

  // new! map the y-axis to 0-1:

  // Turn the histogram into a tree:
  std::vector<double> contents;
  std::vector<double> centers;
  for(int b=1;b<=h_transf->GetNbinsX();b++)
  {
    contents.push_back( h_transf->GetBinContent( b ) );
    centers.push_back( h_transf->GetBinCenter( b ) );
  }

  TTree*  tree = new TTree( "tree", "tree" );
  Float_t x, y;
  tree->Branch( "x", &x, "x/F" );
  tree->Branch( "y", &y, "y/F" );

  for(unsigned int i=0;i<centers.size();i++)
  {
    y = ( Float_t )( contents[i] ); // xvals are the BinContents
    x = ( Float_t )( centers[i] );  // yvals are the BinCenters

    tree->Fill();
  }

  double range_low = get_range_low( h_transf );

  TRandom1* myRandom=new TRandom1(); myRandom->SetSeed(0);

  int               do_range = 1;
  double            maxdev   = 1000;
  int               neurons  = neurons_start;
  std::vector<TH1*> histos;
  while(maxdev>cut_maxdev && neurons<=neurons_end)
  {

    int pass_training = 0;
   try
   {
      TFCS1DRegression::tmvaregression_training( neurons, tree, weightfilename, outfilename, pass_training );
   }
   catch(std::runtime_error &e)
   {
      std::cout << "An exception occured: " << e.what() << std::endl;
      std::cout << "Continuing anyway :P" << std::endl;
      pass_training = 0;
    }

   if(pass_training)
   {

      std::cout << "Testing the regression with " << ntoys << " toys" << std::endl;
      TH1* h_output = (TH1*)h_transf->Clone( "h_output" );
      h_output->Reset();
    for(int i=0;i<ntoys;i++)
    {
        double random = myRandom->Uniform( 1 );
        if ( do_range && random < range_low ) random = range_low;
        double value = TFCS1DRegression::tmvaregression_application( random, weightfilename );
        h_output->Fill( value );
      }

      TH1* h_cumul = get_cumul( h_output );
      h_cumul->SetName( Form( "h_cumul_neurons%i", neurons ) );
      histos.push_back( h_cumul );

      maxdev = TFCS1DFunction::get_maxdev( h_transf, h_cumul );
      std::cout << "---> Neurons=" << neurons << " MAXDEV=" << maxdev << "%" << std::endl;
    }

    neurons++;
  }

  // TH1* histclone=(TH1*)hist->Clone("histclone");

  /*
     TFile* out_iteration=new TFile(Form("output/iteration_%s.root",label.c_str()),"RECREATE");
     for(int h=0;h<histos.size();h++)
     {
     out_iteration->Add(histos[h]);
     }
     out_iteration->Add(histclone);
     out_iteration->Write();
     out_iteration->Close();
     */

  int regression_success = 1;
  if ( maxdev > cut_maxdev ) regression_success = 0;

  int status = 0;
  if(regression_success)
  {
    std::cout << "Regression successful. Weights are stored." << std::endl;
    if ( rangeval < 0 ) status = 1;
    if ( rangeval >= 0 ) status = 2;
  }

  if(!regression_success)
  {
    std::cout << "Regression failed. Histogram is stored." << std::endl;
    status = 3;
  } //! success

  return status;
}


/**

 * @brief Applies a regression model from a file.
 *
 * This function applies a machine learning regression model stored in a file
 * specified by the weightfile parameter. The model is applied to a single input value,
 * uniform, and returns the predicted output value.
 *
 * @param uniform Input value to apply the regression model to.
 * @param weightfile File containing the trained regression model weights.
 * @return Predicted output value.
 */
// The above comment was written by an LLM. 
double TFCS1DRegression::tmvaregression_application(double uniform, std::string weightfile)
{

  using namespace TMVA;

  TString myMethodList = "";
  TMVA::Tools::Instance();

  std::map<std::string, int> Use;

 Use["PDERS"]           = 0;   Use["PDEFoam"]         = 0;   Use["KNN"]            = 0;
 Use["LD"]             = 0;   Use["FDA_GA"]          = 0;   Use["FDA_MC"]          = 0;
 Use["FDA_MT"]          = 0;   Use["FDA_GAMT"]        = 0;   Use["MLP"]             = 1; 
 Use["SVM"]             = 0;   Use["BDT"]             = 0;   Use["BDTG"]            = 0;

  // Select methods (don't look at this code - not of interest)
 if(myMethodList != "")
 {
    for ( std::map<std::string, int>::iterator it = Use.begin(); it != Use.end(); it++ ) it->second = 0;
    std::vector<TString> mlist = gTools().SplitString( myMethodList, ',' );
  for (UInt_t i=0; i<mlist.size(); i++)
  {
      std::string regMethod( mlist[i] );
   if (Use.find(regMethod) == Use.end())
   {
    std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
    for(std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
        std::cout << std::endl;
        return 0;
      }
      Use[regMethod] = 1;
    }
  }

  // --------------------------------------------------------------------------------------------------

  TMVA::Reader* reader = new TMVA::Reader( "!Color:Silent" );

  Float_t y = uniform;
  reader->AddVariable( "y", &y );

  TString dir    = Form( "dl/%s/", weightfile.c_str() );
  TString prefix = "TMVARegression";

  // Book method(s)
 for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++)
 {
  if (it->second)
  {
      TString methodName     = it->first + " method";
      TString weightfilename = dir + prefix + "_" + TString( it->first ) + ".weights.xml";
      reader->BookMVA( methodName, weightfilename );
    }
  }

  Float_t val = ( reader->EvaluateRegression( "MLP method" ) )[0];

  delete reader;
  return val;

  return 0;
}

/**

 * @brief Trains a neural network using the TMVA regression method.
 *
 * This function trains a neural network with the specified number of neurons,
 * using the provided decision tree and weight file, and saves the output to the
 * specified file. It also sets the pass_training flag to indicate success.
 *
 * @param neurons The number of neurons in the neural network.
 * @param regTree The decision tree used for regression.
 * @param weightfile The file containing the weights for the regression.
 * @param outfilename The file where the output will be saved.
 * @param pass_training A flag indicating whether the training was successful.
 */
// The above comment was written by an LLM. 
void TFCS1DRegression::tmvaregression_training(int neurons, TTree *regTree, std::string weightfile, std::string outfilename, int& pass_training)
{

  using namespace TMVA;

  TString myMethodList = "";
  TMVA::Tools::Instance();
  std::map<std::string, int> Use;

  Use["PDERS"] = 0;  Use["PDEFoam"] = 0; Use["KNN"] = 0;  Use["LD"]  = 0; Use["FDA_GA"] = 0; Use["FDA_MC"] = 0;
  Use["FDA_MT"] = 0; Use["FDA_GAMT"] = 0; Use["MLP"] = 1; Use["SVM"] = 0; Use["BDT"] = 0; Use["BDTG"] = 0;

  std::cout << std::endl; std::cout << "==> Start TMVARegression with "<<neurons<<" Neurons "<<std::endl;

  if(myMethodList != "")
  {
    for ( std::map<std::string, int>::iterator it = Use.begin(); it != Use.end(); it++ ) it->second = 0;
    std::vector<TString> mlist = gTools().SplitString( myMethodList, ',' );
    for (UInt_t i=0; i<mlist.size(); i++)
    {
      std::string regMethod( mlist[i] );
      if (Use.find(regMethod) == Use.end())
      {
        std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
        for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
        std::cout << std::endl;
        return;
      }
      Use[regMethod] = 1;
    }
  }

  TFile* outputFile = TFile::Open( outfilename.c_str(), "RECREATE" );

  TMVA::DataLoader* dl = new TMVA::DataLoader( "dl" );

  TMVA::Factory *factory = new TMVA::Factory( "TMVARegression", outputFile, "!V:!Silent:Color:DrawProgressBar:AnalysisType=Regression" );

  TString dirname                                 = Form( "%s/", weightfile.c_str() );
  ( TMVA::gConfig().GetIONames() ).fWeightFileDir = dirname;

  dl->AddVariable( "y", "y", 'F' );
  dl->AddTarget( "x" );

  Double_t regWeight = 1.0;

  dl->AddRegressionTree( regTree, regWeight );
  TCut mycut = "";
  //dl->PrepareTrainingAndTestTree( mycut,"nTrain_Regression=0:nTest_Regression=1:SplitMode=Block:NormMode=NumEvents:!V" );
  dl->PrepareTrainingAndTestTree( mycut,"nTrain_Regression=0:nTest_Regression=0:SplitMode=Alternate:NormMode=NumEvents:!V" );
  
  factory->BookMethod(dl, TMVA::Types::kMLP, "MLP",
  Form("!H:!V:VerbosityLevel=Info:NeuronType=sigmoid:NCycles=20000:HiddenLayers=%i:TestRate=6:TrainingMethod=BFGS:Sampling=0.3:SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=15:!UseRegulator",neurons)); 

  // Train MVAs using the set of training events
  factory->TrainAllMethods();

  // ---- Evaluate all MVAs using the set of test events
  // factory->TestAllMethods();

  // ----- Evaluate and compare performance of all configured MVAs
  // factory->EvaluateAllMethods();

  // Save the output
  outputFile->Close();

  std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
  std::cout << "==> TMVARegression is done!" << std::endl;

  delete factory;
  delete dl;

  pass_training = 1;
}


/**

 * @brief Retrieves weights from a file and stores them in matrices.
 * 
 * This function reads weights from a specified file and populates two matrices,
 * fWeightMatrix0to1 and fWeightMatrix1to2, with the retrieved weights.
 * 
 * @param weightfile The name of the file containing the weights.
 * @param fWeightMatrix0to1 A reference to the first matrix to be populated with weights.
 * @param fWeightMatrix1to2 A reference to the second matrix to be populated with weights.
 */
// The above comment was written by an LLM. 
void TFCS1DRegression::get_weights(string weightfile, vector<vector<double> > &fWeightMatrix0to1, vector<vector<double> > &fWeightMatrix1to2)
{

  using namespace TMVA;
  int debug = 1;

  TString myMethodList = "";
  TMVA::Tools::Instance();

  std::map<std::string, int> Use;

  // --- Mutidimensional likelihood and Nearest-Neighbour methods
 Use["PDERS"]           = 0;   Use["PDEFoam"]         = 0;   Use["KNN"]            = 0;
 Use["LD"]		          = 0;   Use["FDA_GA"]          = 0;   Use["FDA_MC"]          = 0;
 Use["FDA_MT"]          = 0;   Use["FDA_GAMT"]        = 0;   Use["MLP"]             = 1; 
 Use["SVM"]             = 0;   Use["BDT"]             = 0;   Use["BDTG"]            = 0;
  // ---------------------------------------------------------------

  // Select methods (don't look at this code - not of interest)
 if (myMethodList != "")
 {
    for ( std::map<std::string, int>::iterator it = Use.begin(); it != Use.end(); it++ ) it->second = 0;
    std::vector<TString> mlist = gTools().SplitString( myMethodList, ',' );
  for (UInt_t i=0; i<mlist.size(); i++)
  {
      std::string regMethod( mlist[i] );
   if (Use.find(regMethod) == Use.end())
   {
    std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
    for(std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
        std::cout << std::endl;
      }
      Use[regMethod] = 1;
    }
  }

  // --------------------------------------------------------------------------------------------------

  TMVA::Reader* reader = new TMVA::Reader( "!Color:Silent" );

  Float_t y = 0.5; // just a dummy
  reader->AddVariable( "y", &y );

  TString dir    = Form( "dl/%s/", weightfile.c_str() );
  TString prefix = "TMVARegression";

 for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++)
 {
  if (it->second)
  {
      TString methodName     = it->first + " method";
      TString weightfilename = dir + prefix + "_" + TString( it->first ) + ".weights.xml";
      reader->BookMVA( methodName, weightfilename );
    }
  }

  TMVA::IMethod*   m                 = reader->FindMVA( "MLP method" );
  TMVA::MethodMLP* mlp               = dynamic_cast<TMVA::MethodMLP*>( m );
  TObjArray*       Network           = mlp->fNetwork;
  int              num_neurons_input = ( (TObjArray*)Network->At( 1 ) )->GetEntriesFast() - 1;
  if ( debug ) cout << "num_neurons_input " << num_neurons_input << endl;
  // mlp->MakeClass(Form("mlpcode_neurons%i.C",num_neurons_input));

 for(int a=0;a<((TObjArray*)Network->At(1))->GetEntriesFast();a++)
 {
    vector<double> thisvector;
  for(int b=0;b<((TObjArray*)Network->At(0))->GetEntriesFast();b++)
   thisvector.push_back(0);
    fWeightMatrix0to1.push_back( thisvector );
  }

 for(int a=0;a<((TObjArray*)Network->At(2))->GetEntriesFast();a++)
 {
    vector<double> thisvector;
  for(int b=0;b<((TObjArray*)Network->At(1))->GetEntriesFast();b++)
   thisvector.push_back(0);
    fWeightMatrix1to2.push_back( thisvector );
  }

  TObjArray* curLayer   = (TObjArray*)Network->At( 0 );
  Int_t      numNeurons = curLayer->GetEntriesFast();
 for (Int_t n = 0; n < numNeurons; n++)
 {  
    TMVA::TNeuron* neuron      = (TMVA::TNeuron*)curLayer->At( n );
    int            numSynapses = neuron->NumPostLinks();
  for (int s = 0; s < numSynapses; s++)
  {
      TMVA::TSynapse* synapse = neuron->PostLinkAt( s );
      fWeightMatrix0to1[s][n] = synapse->GetWeight();
      if ( debug ) cout << "fWeightMatrix0to1[" << s << "][" << n << "] " << synapse->GetWeight() << endl;
    }
  }

  curLayer   = (TObjArray*)Network->At( 1 );
  numNeurons = curLayer->GetEntriesFast();
 for (Int_t n = 0; n < numNeurons; n++)
 {  
    TMVA::TNeuron* neuron      = (TMVA::TNeuron*)curLayer->At( n );
    int            numSynapses = neuron->NumPostLinks();
  for (int s = 0; s < numSynapses; s++)
  {
      TMVA::TSynapse* synapse = neuron->PostLinkAt( s );
      fWeightMatrix1to2[s][n] = synapse->GetWeight();
      if ( debug ) cout << "fWeightMatrix1to2[" << s << "][" << n << "] " << synapse->GetWeight() << endl;
    }
  }

  delete reader;
}
