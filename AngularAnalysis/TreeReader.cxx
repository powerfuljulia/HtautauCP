#define TreeReader_cxx
#include "TreeReader.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void TreeReader::Loop()
{
//   In a ROOT session, you can do:
//      root> .L TreeReader.C
//      root> TreeReader t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}



TreeReader::TreeReader(TTree *tree) : fChain(0) 
{  
   Init(tree);
}

TreeReader::~TreeReader()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TreeReader::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TreeReader::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void TreeReader::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).


   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("Truth_Higgs_Pt", &Truth_Higgs_Pt, &b_Truth_Higgs_Pt);
   fChain->SetBranchAddress("Truth_Higgs_Eta", &Truth_Higgs_Eta, &b_Truth_Higgs_Eta);
   fChain->SetBranchAddress("Truth_Higgs_Phi", &Truth_Higgs_Phi, &b_Truth_Higgs_Phi);
   fChain->SetBranchAddress("Truth_Higgs_M", &Truth_Higgs_M, &b_Truth_Higgs_M);
   fChain->SetBranchAddress("Truth_TauMinus_Pt", &Truth_TauMinus_Pt, &b_Truth_TauMinus_Pt);
   fChain->SetBranchAddress("Truth_TauMinus_Eta", &Truth_TauMinus_Eta, &b_Truth_TauMinus_Eta);
   fChain->SetBranchAddress("Truth_TauMinus_Phi", &Truth_TauMinus_Phi, &b_Truth_TauMinus_Phi);
   fChain->SetBranchAddress("Truth_TauPlus_Pt", &Truth_TauPlus_Pt, &b_Truth_TauPlus_Pt);
   fChain->SetBranchAddress("Truth_TauPlus_Eta", &Truth_TauPlus_Eta, &b_Truth_TauPlus_Eta);
   fChain->SetBranchAddress("Truth_TauPlus_Phi", &Truth_TauPlus_Phi, &b_Truth_TauPlus_Phi);
   fChain->SetBranchAddress("Truth_PiMinus_Pt", &Truth_PiMinus_Pt, &b_Truth_PiMinus_Pt);
   fChain->SetBranchAddress("Truth_PiMinus_Eta", &Truth_PiMinus_Eta, &b_Truth_PiMinus_Eta);
   fChain->SetBranchAddress("Truth_PiMinus_Phi", &Truth_PiMinus_Phi, &b_Truth_PiMinus_Phi);
   fChain->SetBranchAddress("Truth_PiPlus_Pt", &Truth_PiPlus_Pt, &b_Truth_PiPlus_Pt);
   fChain->SetBranchAddress("Truth_PiPlus_Eta", &Truth_PiPlus_Eta, &b_Truth_PiPlus_Eta);  

   Notify();
}

Bool_t TreeReader::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TreeReader::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TreeReader::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

