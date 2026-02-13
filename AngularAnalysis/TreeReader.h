#ifndef TreeReader_h
#define TreeReader_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
using namespace std;

class TreeReader {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types

   Double_t Truth_Higgs_Pt;
   Double_t Truth_Higgs_Eta;
   Double_t Truth_Higgs_Phi;
   Double_t Truth_Higgs_M;
   Double_t Truth_TauMinus_Pt;
   Double_t Truth_TauMinus_Eta;
   Double_t Truth_TauMinus_Phi;
   Double_t Truth_TauMinus_ProdVtx_X;
   Double_t Truth_TauMinus_ProdVtx_Y;
   Double_t Truth_TauMinus_ProdVtx_Z;
   Double_t Truth_TauPlus_Pt;
   Double_t Truth_TauPlus_Eta;
   Double_t Truth_TauPlus_Phi;
   Double_t Truth_TauPlus_ProdVtx_X;
   Double_t Truth_TauPlus_ProdVtx_Y;
   Double_t Truth_TauPlus_ProdVtx_Z;
   Double_t Truth_PiMinus_Pt;
   Double_t Truth_PiMinus_Eta;
   Double_t Truth_PiMinus_Phi;
   Double_t Truth_PiMinus_ProdVtx_X;
   Double_t Truth_PiMinus_ProdVtx_Y;
   Double_t Truth_PiMinus_ProdVtx_Z;
   Double_t Truth_PiPlus_Pt;
   Double_t Truth_PiPlus_Eta;
   Double_t Truth_PiPlus_Phi;
   Double_t Truth_PiPlus_ProdVtx_X;
   Double_t Truth_PiPlus_ProdVtx_Y;
   Double_t Truth_PiPlus_ProdVtx_Z;
   Double_t Truth_Z_M;
   Double_t Truth_Z_Pt;
   Double_t Truth_Z_Eta;
   Double_t Truth_Z_Phi;


   // List of branches
   TBranch        *b_Truth_Higgs_Pt;   //!
   TBranch        *b_Truth_Higgs_Eta;   //!
   TBranch        *b_Truth_Higgs_Phi;   //!
   TBranch        *b_Truth_Higgs_M;   //!
   TBranch        *b_Truth_TauMinus_Pt;   //!
   TBranch        *b_Truth_TauMinus_Eta;   //!
   TBranch        *b_Truth_TauMinus_Phi;   //!
   TBranch        *b_Truth_TauMinus_ProdVtx_X;   //!
   TBranch        *b_Truth_TauMinus_ProdVtx_Y;   //!
   TBranch        *b_Truth_TauMinus_ProdVtx_Z;   //!
   TBranch        *b_Truth_TauPlus_Pt;   //!
   TBranch        *b_Truth_TauPlus_Eta;   //! 
   TBranch        *b_Truth_TauPlus_Phi;   //!
   TBranch        *b_Truth_TauPlus_ProdVtx_X;   //!
   TBranch        *b_Truth_TauPlus_ProdVtx_Y;   //!
   TBranch        *b_Truth_TauPlus_ProdVtx_Z;   //!
   TBranch        *b_Truth_PiMinus_Pt;   //!
   TBranch        *b_Truth_PiMinus_Eta;   //!
   TBranch        *b_Truth_PiMinus_Phi;   //!
   TBranch        *b_Truth_PiMinus_ProdVtx_X;   //!
   TBranch        *b_Truth_PiMinus_ProdVtx_Y;   //!
   TBranch        *b_Truth_PiMinus_ProdVtx_Z;   //!
   TBranch        *b_Truth_PiPlus_Pt;   //!
   TBranch        *b_Truth_PiPlus_Eta;   //!
   TBranch        *b_Truth_PiPlus_Phi;   //!
   TBranch        *b_Truth_PiPlus_ProdVtx_X;   //!
   TBranch        *b_Truth_PiPlus_ProdVtx_Y;   //!
   TBranch        *b_Truth_PiPlus_ProdVtx_Z;   //
   TBranch        *b_Truth_Z_M;   //!
   TBranch        *b_Truth_Z_Pt;   //!
   TBranch        *b_Truth_Z_Eta;   //!
   TBranch        *b_Truth_Z_Phi;   //!


   TreeReader(TTree *tree=0);
   virtual ~TreeReader();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif


