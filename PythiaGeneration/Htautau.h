#include "TFile.h"
#include "TH2.h"
#include "TF1.h"
#include "TLorentzVector.h"
#include "TRandom3.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TMath.h"
#include "TColor.h"
#include "TStyle.h"
#include "TVector.h"
#include "TError.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TVector3.h"


#include <iostream>
#include <utility>
#include <vector>

#include "TROOT.h"
#include "TSystem.h"
#include "TApplication.h"

#include "TString.h"
#include "TRandom3.h"

#include "Pythia8/Pythia.h"

using namespace Pythia8;

Pythia pythia;

TFile * OutputFile;

TH1D* h_Higgs_Mass;
TH1D* h_Higgs_Pt;

TH1D* h_PosTau_VisPt;
TH1D* h_NegTau_VisPt;

TH1D* h_PosPi_Pt;
TH1D* h_NegPi_Pt;

TH1D* h_DiTau_VisMass;
TH1D* h_DiTau_VisPt;

TH1D* h_Signed_Acoplanarity;

void GetDecayProducts(const unsigned int p, std::vector<unsigned int> & products);

bool CheckUnique(unsigned int p, std::vector<unsigned int> products);

int main(int argc, char* argv[]);
