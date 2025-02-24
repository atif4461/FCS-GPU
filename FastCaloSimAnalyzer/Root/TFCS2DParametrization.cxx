/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "FastCaloSimAnalyzer/TFCS2DParametrization.h"

#include "TROOT.h"
#include "TString.h"
#include "TMath.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TH2.h"
#include "TCanvas.h"

#include <iostream>

TFCS2DParametrization::TFCS2DParametrization() { m_debug = 0; }

/**
 * Constructor for the class 
 * @param tree pointer to the input TTree object
 * @param outputfile string specifying the output file name
 * @param vlayer vector of integers representing layer information
 */
// The above comment was written by an LLM. 
TFCS2DParametrization::TFCS2DParametrization( TTree* tree, std::string outputfile, std::vector<int> vlayer ) {

  m_debug      = 0;
  m_tree       = tree;
  m_outputFile = outputfile;
  m_vlayer     = vlayer;
}

TFCS2DParametrization::~TFCS2DParametrization() {}

/**
 * Creates shape histograms for 2D parametrization.
 *
 * @param cutoff  The cutoff value for histogram creation.
 * @param opt     Optional string parameter.
 */
// The above comment was written by an LLM. 
void TFCS2DParametrization::CreateShapeHistograms( double cutoff, std::string opt ) {

  std::vector<int> v_layer     = m_vlayer;
  std::string      outfileName = m_outputFile;

  auto fout = std::unique_ptr<TFile>( TFile::Open( outfileName.c_str(), "recreate" ) );
  if ( !fout ) {
    std::cerr << "Error: Could not create file '" << outfileName << "'" << std::endl;
    return;
  }
  fout->cd();

  for ( unsigned int ilayer = 0; ilayer < v_layer.size(); ilayer++ ) {
    int layer = v_layer.at( ilayer );
    int npca  = 6;

    for ( int ipca = 1; ipca < npca; ipca++ ) {
      int pca = ipca;

      std::cout << "layer, pca = " << layer << " , " << pca << std::endl;

      TH1F* h_radius       = GetHisto( "radius_mm", layer, pca, cutoff, opt );
      TH2F* h_radius_alpha = GetParametrization( h_radius, layer, pca, opt );

      for ( int ix = 1; ix <= h_radius_alpha->GetNbinsX(); ++ix ) {
        for ( int iy = 1; iy <= h_radius_alpha->GetNbinsY(); ++iy ) {
          if ( h_radius_alpha->GetBinContent( ix, iy ) < 0 ) {
            std::cout << "WARNING: Histo: " << h_radius_alpha->GetName() << " : " << h_radius_alpha->GetTitle()
                      << " : bin(" << ix << "," << iy << ")=" << h_radius_alpha->GetBinContent( ix, ix )
                      << " is negative. Fixing to 0!" << std::endl;
            h_radius_alpha->SetBinContent( ix, iy, 0 );
          }
        }
      }

      h_radius_alpha->Write();
    }
  }
  fout->Close();
}

/**
 * @brief Plots the shape in polar coordinates.
 *
 * This function generates plots of the shape in polar coordinates for each layer and PCA component.
 * It creates a directory for the output plots, opens the input shape file, and iterates over each layer and PCA component.
 * For each combination, it retrieves the corresponding histogram, plots it using PlotPolar, and saves the plot as a PDF file.
 *
 * @return None
 */
// The above comment was written by an LLM. 
void TFCS2DParametrization::PlotShapePolar() {

  std::vector<int> v_layer       = m_vlayer;
  std::string      shapefileName = m_outputFile;
  std::cout << "Running on = " << shapefileName.c_str() << std::endl;
  std::string outDir = "shape_polar_plots";
  auto r = system( ( "mkdir -p " + outDir ).c_str() );
  if ( r != 0 ) {
    std::cerr << "Error: Could not create directory \"" << outDir << "\"" << std::endl;
    return;
  }

  TString title = TFCSAnalyzerBase::GetLabel();

  auto fshape = std::unique_ptr<TFile>( TFile::Open( shapefileName.c_str() ) );
  if ( !fshape ) {
    std::cerr << "Error: Could not open file '" << shapefileName << "'" << std::endl;
    return;
  }

  for ( unsigned int ilayer = 0; ilayer < v_layer.size(); ilayer++ ) {
    int layer = v_layer.at( ilayer );
    int npca  = 6;

    for ( int ipca = 1; ipca < npca; ipca++ ) {
      int pca = ipca;
      std::cout << "layer, pca = " << layer << " , " << pca << std::endl;

      TString layerName = TFCSAnalyzerBase::GetLayerName( layer );
      TString label( Form( "%s, %s", title.Data(), layerName.Data() ) );
      // TString label(Form("%s layer %i pca %i", title.Data(), layer, pca));

      std::string xlabel = "x [mm]";
      std::string ylabel = "y [mm]";
      std::string zlabel = "Energy normalized to unity";

      TH2F* h = (TH2F*)fshape->Get( Form( "h_r_alpha_layer%i_pca%i", layer, pca ) );
      std::cout << "No of entries: " << h->GetEntries() << std::endl;
      TCanvas*    c       = TFCSAnalyzerBase::PlotPolar( h, label.Data(), xlabel, ylabel, zlabel, 4 );
      std::string outfile = outDir + "/" + "h_r_alpha_layer" + std::to_string( layer ) + "_pca" + std::to_string( pca );
      c->SaveAs( ( outfile + ".pdf" ).c_str() );

      delete c;
      delete h;
    }
  }

  fshape->Close();
}

/**
 * @brief Returns a 2D histogram representing the parametrization of the input data.
 * 
 * @param h_radius Input 1D histogram of radius values.
 * @param layer Layer number for which the parametrization is calculated.
 * @param pca Principal component analysis option.
 * @param opt Binning option, either "equal_energy" or "default".
 * 
 * @return A 2D histogram with the parametrized data.
 */
// The above comment was written by an LLM. 
TH2F* TFCS2DParametrization::GetParametrization( TH1F* h_radius, int layer, int pca, std::string opt ) {

  std::vector<double> v_xbins;

  if ( opt == "equal_energy" ) {
    v_xbins = CreateEqualEnergyBinning( h_radius, 20 );
  } else if ( opt == "default" ) {
    v_xbins = CreateBinning( h_radius );
  }

  int     nbinsy = v_xbins.size();
  int     nbinsx = 8;
  double  xmin   = 0.;
  double  xmax   = 2 * TMath::Pi();
  double* ybins  = new double[nbinsy];

  for ( unsigned int i = 0; i < v_xbins.size(); i++ ) {
    ybins[i] = v_xbins.at( i );
    // std::cout << "xbins[ " << i << " ] = " << xbins[i] << std::endl ;
  }

  TH2F* h_r_alpha = new TH2F( Form( "h_r_alpha_layer%i_pca%i", layer, pca ),
                              Form( "h_r_alpha_layer%i_pca%i", layer, pca ), nbinsx, xmin, xmax, nbinsy - 1, ybins );

  TString histname( Form( "h_r_alpha_layer%i_pca%i", layer, pca ) );
  TString varexp( Form( "%s>>%s", "radius_mm:alpha_mm", histname.Data() ) );
  TString cut( Form( "hit_energy*scale_factor*(layer==%i && pca==%i)", layer, pca ) );
  TString draw = "goff";

  TTree* tree = m_tree;
  tree->Draw( varexp, cut, draw );

  h_r_alpha->Sumw2();
  double integral = h_r_alpha->Integral();
  h_r_alpha->Scale( 1 / integral );

  delete[] ybins;

  return h_r_alpha;
}

/**
 * Creates binning for 2D parametrization based on input histogram.
 *
 * @return Vector of doubles representing bin edges.
 */
// The above comment was written by an LLM. 
std::vector<double> TFCS2DParametrization::CreateBinning( TH1F* h ) {

  std::vector<double> v_xbins;

  v_xbins.push_back( 0 );

  int value = 0;
  for ( int ibin = 1; ibin < h->GetNbinsX(); ibin++ ) {

    value = value + h->GetBinContent( ibin );

    if ( value > 1000 ) {
      double range = h->GetBinLowEdge( ibin ) + h->GetBinWidth( ibin );
      v_xbins.push_back( range );
      value = 0;
    }
  }

  int    nmax = h->FindLastBinAbove( 0 );
  double max  = h->GetBinLowEdge( nmax ) + h->GetBinWidth( nmax );

  v_xbins.push_back( max );

  return v_xbins;
}

/**
 * Creates equal energy binning for a given histogram.
 *
 * @param h input histogram
 * @param nbins number of bins
 * @return vector of bin edges with equal energy content
 */
// The above comment was written by an LLM. 
std::vector<double> TFCS2DParametrization::CreateEqualEnergyBinning( TH1F* h, int nbins ) {

  std::vector<double> xbins;

  int icount = 0;
  xbins.push_back( icount );

  double total_energy = h->Integral();
  std::cout << "total energy = " << total_energy << std::endl;

  double avg = total_energy / nbins;
  std::cout << "avg = " << avg << std::endl;

  double bin_energy = 0.;

  int NbinsX = h->GetNbinsX();
  std::cout << "NbinsX = " << NbinsX << std::endl;

  for ( int ibin = 0; ibin < NbinsX; ibin++ ) {
    if ( bin_energy < avg ) {
      bin_energy = bin_energy + h->GetBinContent( ibin );

    } else if ( bin_energy >= avg ) {
      icount++;

      double range = h->GetBinLowEdge( ibin ) + h->GetBinWidth( ibin );
      xbins.push_back( range );
      bin_energy = 0.;
    }
  }

  int    nmax = h->FindLastBinAbove( 0 );
  double max  = h->GetBinLowEdge( nmax ) + h->GetBinWidth( nmax );

  xbins.push_back( max );

  return xbins;
}

/**
 * @brief Returns a histogram with hits or energy for a given variable, layer, PCA, cutoff, and option.
 *
 * @param var The variable to be used in the histogram.
 * @param layer The layer number.
 * @param pca The PCA number.
 * @param cutoff The cutoff value.
 * @param opt The option to choose between 'energy' and 'hit'.
 * @return A pointer to the resulting histogram.
 */
// The above comment was written by an LLM. 
TH1F* TFCS2DParametrization::GetHisto( std::string var, int layer, int pca, double cutoff, std::string opt ) {

  // return a histogram with hits (to calculate bin boundary) or energy
  // either case requires creating a energy histogram to calculate the
  // distance corresponding to the cut-off energy.

  double merge = 5.;
  if ( layer == 1 or layer == 5 ) merge = 1.;

  double xmin    = 0.;
  double xmax_mm = 10000.;
  int    nbins   = (int)( xmax_mm / merge );

  TString histname_mm( Form( "hlmm%ipca%i", layer, pca ) );
  TString varexp_mm( Form( "%s>>hlmm%ipca%i(%i, %f, %f)", var.c_str(), layer, pca, nbins, xmin, xmax_mm ) );
  TString cut( Form( "hit_energy*scale_factor*(layer==%i && pca==%i)", layer, pca ) );

  TString draw = "goff";

  TTree* tree = m_tree;
  tree->Draw( varexp_mm, cut, draw );
  TH1F*  histo_mm = (TH1F*)gROOT->FindObject( histname_mm );
  double rmax_mm  = TFCSAnalyzerBase::GetBinUpEdge( histo_mm, cutoff );

  int nbinsr = (int)( rmax_mm / merge );

  TString hr_name( Form( "h_r_l%ipca%i", layer, pca ) );
  TString exp( Form( "%s>>h_r_l%ipca%i(%i, %f, %f)", "radius_mm", layer, pca, nbinsr, xmin, rmax_mm ) );
  TString cut2 = "";

  if ( opt == "energy" ) {
    cut2 = Form( "hit_energy*scale_factor*(layer==%i && pca==%i)", layer, pca );
  } else if ( opt == "hit" ) {
    cut2 = Form( "1*(layer==%i && pca==%i)", layer, pca );
  }
  tree->Draw( exp, cut2, draw );

  TH1F* h_radius = (TH1F*)gROOT->FindObject( hr_name );

  // for (int i = 0; i < h_radius->GetNbinsX(); i++) {

  //    std::cout << " bin = " << i << " up edge = " << h_radius->GetXaxis()->GetBinUpEdge(i) << " width = " <<
  //    h_radius->GetBinWidth(i) << std::endl;
  // }

  return h_radius;
}

/**
 * Returns the number of bins in the histogram after applying merging rules.
 *
 * @param h     input histogram
 * @param layer layer number
 * @return number of bins
 */
// The above comment was written by an LLM. 
int TFCS2DParametrization::GetNbins( TH1F* h, int layer ) {
  int nbins = 50;

  std::vector<double> bins = CreateEqualEnergyBinning( h, nbins );

  double bin0_width = bins.at( 1 ) - bins.at( 0 );
  // delete [] bins;

  // std::cout << "bin0_width = " << bin0_width << std::endl ;

  double merge = -1.;
  if ( layer == 1 or layer == 5 )
    merge = 1.;
  else
    merge = 5.;

  // std::cout << "layer, merge = " << layer << " , " << merge << std::endl ;

  if ( bin0_width < merge ) {
    while ( bin0_width < merge ) {
      nbins--;
      std::vector<double> xbins = CreateEqualEnergyBinning( h, nbins );
      for ( unsigned int i = 0; i < xbins.size(); i++ )
        // std::cout << "====> xbins = " << *(xbins + i) << std::endl ;

        bin0_width = xbins.at( 1 ) - xbins.at( 0 );
      // std::cout << "in < merge bin0_width = " << bin0_width << std::endl ;
    }
  } else if ( bin0_width > 2 * merge ) {
    while ( 1.2 * merge < bin0_width && bin0_width < 2 * merge ) {
      nbins++;
      std::vector<double> ybins = CreateEqualEnergyBinning( h, nbins );
      bin0_width                = ybins.at( 1 ) - ybins.at( 0 );
      // std::cout << "in > merge bin0_width = " << bin0_width << std::endl ;
    }
  }

  // std::cout << "nbins = " << nbins << std::endl ;
  return nbins;
}
