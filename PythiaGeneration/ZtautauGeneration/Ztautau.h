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
#include "TTree.h"


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

TH1D* h_Z_Mass;
TH1D* h_Z_Pt;

TH1D* h_PosTau_VisPt;
TH1D* h_NegTau_VisPt;

TH1D* h_PosPi_Pt;
TH1D* h_NegPi_Pt;

TH1D* h_DiTau_VisMass;
TH1D* h_DiTau_VisPt;


double m_T_Truth_Z_Pt;
double m_T_Truth_Z_Eta;
double m_T_Truth_Z_Phi;
double m_T_Truth_Z_M;

double m_T_Truth_TauMinus_Pt;
double m_T_Truth_TauMinus_Eta;
double m_T_Truth_TauMinus_Phi;
double m_T_Truth_TauMinus_ProdVtx_X;
double m_T_Truth_TauMinus_ProdVtx_Y;
double m_T_Truth_TauMinus_ProdVtx_Z;

double m_T_Truth_TauPlus_Pt;
double m_T_Truth_TauPlus_Eta;
double m_T_Truth_TauPlus_Phi;
double m_T_Truth_TauPlus_ProdVtx_X;
double m_T_Truth_TauPlus_ProdVtx_Y;
double m_T_Truth_TauPlus_ProdVtx_Z;

double m_T_Truth_PiMinus_Pt;
double m_T_Truth_PiMinus_Eta;
double m_T_Truth_PiMinus_Phi;
double m_T_Truth_PiMinus_ProdVtx_X;
double m_T_Truth_PiMinus_ProdVtx_Y;
double m_T_Truth_PiMinus_ProdVtx_Z;

double m_T_Truth_PiPlus_Pt;
double m_T_Truth_PiPlus_Eta;
double m_T_Truth_PiPlus_Phi;
double m_T_Truth_PiPlus_ProdVtx_X;
double m_T_Truth_PiPlus_ProdVtx_Y;
double m_T_Truth_PiPlus_ProdVtx_Z;



void GetDecayProducts(const unsigned int p, std::vector<unsigned int> & products);

bool CheckUnique(unsigned int p, std::vector<unsigned int> products);

void BranchMaker(TTree* Tree);

void ClearVars();

int main(int argc, char* argv[]);
