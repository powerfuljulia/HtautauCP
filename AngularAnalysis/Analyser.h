#ifndef ANALYSER_H
#define ANALYSER_H

#include <iostream>
#include <fstream>
#include "TreeReader.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include "TH1D.h"
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TLatex.h"
#include "TLegend.h"
#include "RooPlot.h"
#include "TStyle.h"
#include "rootlogon.C"
#include "AtlasStyle.h"
TH1D* h_Higgs_Mass;
TH1D* h_DiTau_VisMass;
TH1D* h_SignedAcoplanarity_IP;
TH1D* h_Phi;
TH1D* h_Eta;
TH1D* h_Pt;

// create histograms for background data
TH1D* h_Z_Mass;
TH1D* h_DiTau_VisMass_Bkg;
TH1D* h_SignedAcoplanarity_IP_Bkg;
TH1D* h_Phi_Bkg;
TH1D* h_Eta_Bkg;
TH1D* h_Pt_Bkg;

#endif//ANALYSER_H
