#include "Analyser.h"

TVector3 calculateImpactParamVec_Truth(const TLorentzVector Pion, const TVector3 PiProductionVtx,
				 const TVector3 primaryVertex) {

  TVector3 pionVector(Pion.X(), Pion.Y(), Pion.Z());
  TVector3 initialPoint(PiProductionVtx.X(), PiProductionVtx.Y(), PiProductionVtx.Z());
  // solve equations in order to determine shortest distance from the PV to
  // the extrapolated pi-track
  double k = (primaryVertex - initialPoint) * pionVector / pionVector.Mag2();
  // calculate impact parameter vector
  TVector3 IPVector = initialPoint + k * pionVector - primaryVertex;
  return IPVector;
} //might need to add tauproduction vertices 




float Acoplanarity_IP(TLorentzVector piPlus, TLorentzVector ipVectorPlus,
                            TLorentzVector piMinus,
                            TLorentzVector ipVectorMinus,
                            const TLorentzVector& referenceFrame) {

  TVector3 boostIntoReferenceFrame = (-1) * referenceFrame.BoostVector();
  piPlus.Boost(boostIntoReferenceFrame);
  piMinus.Boost(boostIntoReferenceFrame);
  ipVectorPlus.Boost(boostIntoReferenceFrame);
  ipVectorMinus.Boost(boostIntoReferenceFrame);
  TVector3 ip3VectorPlus = ipVectorPlus.Vect();
  TVector3 ip3VectorMinus = ipVectorMinus.Vect();
  // parallel projections of impact vector onto pion direction
  TVector3 ipVectorParallelPlus =
    (ip3VectorPlus * piPlus.Vect().Unit()) * piPlus.Vect().Unit();
  TVector3 ipVectorParallelMinus =
    (ip3VectorMinus * piMinus.Vect().Unit()) * piMinus.Vect().Unit();
  // perpendicular component of impact vector
  TVector3 ipVectorPerpPlus = (ip3VectorPlus - ipVectorParallelPlus).Unit();
  TVector3 ipVectorPerpMinus =
    (ip3VectorMinus - ipVectorParallelMinus).Unit();
    // Triple-odd correlation
  float triplecorr =
    piMinus.Vect().Unit().Dot(ipVectorPerpPlus.Cross(ipVectorPerpMinus));
  
  float phistar = TMath::ACos(ipVectorPerpPlus * ipVectorPerpMinus);
  if (triplecorr < 0) {
    phistar = TMath::TwoPi() - phistar;
  }
  
  return phistar;
}



float Truth_Acoplanarity_IP(const TLorentzVector PiPlus, const TVector3 PiPlusProdVtx,
  const TLorentzVector PiMinus, const TVector3 PiMinusProdVtx,
  const TVector3 TauPlusProdVtx, const TVector3 TauMinusProdVtx) {

TLorentzVector ipVectorPlus(calculateImpactParamVec_Truth(PiPlus, PiPlusProdVtx, TauPlusProdVtx).Unit(), 0.);
TLorentzVector ipVectorMinus(calculateImpactParamVec_Truth(PiMinus, PiMinusProdVtx, TauMinusProdVtx).Unit(), 0.);
TLorentzVector referenceFrame = PiPlus + PiMinus;
float phistar = Acoplanarity_IP(PiPlus, ipVectorPlus, PiMinus, ipVectorMinus, referenceFrame);

return phistar;

}


void draw_histo(TH1D* h, TString process, TString filename){

  TCanvas* c = new TCanvas("canvas", "canvas", 600, 600);

  h->Sumw2();
  h->SetMaximum(1.1*h->GetMaximum());
  h->GetXaxis()->SetTitleOffset(1.1);
  h->GetYaxis()->SetTitleOffset(1.3);
  h->GetYaxis()->SetLabelSize(0.045);
  h->GetYaxis()->SetTitleSize(0.045);
  h->GetXaxis()->SetLabelSize(0.045);
  //h->GetXaxis()->SetMaxDigits(2);
  h->SetMarkerSize(0);
  h->SetLineColor(kRed+1);
  h->GetXaxis()->SetTitleSize(0.045);
  c->SetRightMargin(0.1);

  gPad->RedrawAxis();
  h->Draw("HIST E");
  TLatex t;
  t.SetNDC();
  t.SetTextColor(1);
  t.SetTextFont(42);
  t.SetTextSize(0.045);
  t.DrawLatex(0.17,.88,process);

  
  //  c->SetLogy();
  c->SaveAs(filename);

}


int main(int argc, char *argv[])
{

  std::string inFileName = ""; 
  int CPstate = 0;
  std::string phi = "0.785398"; 

  for(int i=1 ; i<argc ; ++i){
    std::string argument = argv[i];
    std::stringstream convert;

    if(argument == "--CPState"){
      convert << argv[++i];
      convert >> CPstate;
     
    }

    if(argument == "--phi"){
      convert << argv[++i];
      convert >> phi;
    }


    if(argument == "--inFile"){
      convert << argv[++i];
      convert >> inFileName;
    }

    
  } 


  SetAtlasStyle();
  rootlogon(); 

  std::string treeName = "HtautauTree";
  
  TFile* m_inputFile = TFile::Open(inFileName.c_str());
  //  TFile* m_inputFile = TFile::Open("../PythiaGeneration/Htautau.root");
    
  if(!m_inputFile)
    {
      std::cout << "Unable to open input file" << std::endl;
      return -1;
      
    }

  TTree * inTree = dynamic_cast<TTree*>(m_inputFile->Get(treeName.c_str()));

  TString modifier = "";
  if (CPstate == 0){
    modifier = "_CPeven";
  } else if (CPstate == 1){
    modifier = "_CPodd";
  } else {
    modifier = "_CPmix_phi"+phi;
  }
  
  h_Higgs_Mass = new TH1D("h_Higgs_Mass","; m(H) [GeV]; Events / 0.02 GeV",50,124.5,125.5);
  h_DiTau_VisMass = new TH1D("h_DiTau_VisMass","; visible m(#tau^{+}#tau^{-}) [GeV]; Events / 10 GeV",20,0,200);
  h_SignedAcoplanarity_IP = new TH1D("h_SignedAcoplanarity_IP"+modifier,"; #phi* [rad]; AU",18,0,TMath::TwoPi());
  
  TreeReader * reader = new TreeReader(inTree);
  Long64_t nentries = inTree->GetEntries();

  int eventsPercent = nentries/100;
  
  if(eventsPercent == 0)
    {
      eventsPercent = 1;
    }

  TLorentzVector PiPlus;
  TLorentzVector PiMinus;
  TLorentzVector Vis_DiTau;
  TVector3 PiPlusProdVtx;
  TVector3 PiMinusProdVtx;
  TVector3 TauPlusProdVtx;
  TVector3 TauMinusProdVtx; 

  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = reader->LoadTree(jentry);
    if (ientry < 0) break;
    reader->fChain->GetEntry(jentry);
    
    if((jentry % eventsPercent) == 0)
      {
	std::cout << "Processing is: " << (jentry*100)/nentries << "\% complete" << std::endl;
      }
    
    PiPlus.SetPtEtaPhiM(reader->Truth_PiPlus_Pt, reader->Truth_PiPlus_Eta, reader->Truth_PiPlus_Phi, 0.13957);
    PiMinus.SetPtEtaPhiM(reader->Truth_PiMinus_Pt, reader->Truth_PiMinus_Eta, reader->Truth_PiMinus_Phi, 0.13957);
    Vis_DiTau = PiPlus + PiMinus;
    h_Higgs_Mass->Fill(reader->Truth_Higgs_M);
    h_DiTau_VisMass->Fill(Vis_DiTau.M());

    PiPlusProdVtx.SetXYZ(reader->Truth_PiPlus_ProdVtx_X,
        reader->Truth_PiPlus_ProdVtx_Y,
        reader->Truth_PiPlus_ProdVtx_Z);
    PiMinusProdVtx.SetXYZ(reader->Truth_PiMinus_ProdVtx_X,
        reader->Truth_PiMinus_ProdVtx_Y,
        reader->Truth_PiMinus_ProdVtx_Z);
    TauPlusProdVtx.SetXYZ(reader->Truth_TauPlus_ProdVtx_X,
        reader->Truth_TauPlus_ProdVtx_Y,
        reader->Truth_TauPlus_ProdVtx_Z);   
    TauMinusProdVtx.SetXYZ(reader->Truth_TauMinus_ProdVtx_X,
        reader->Truth_TauMinus_ProdVtx_Y,
        reader->Truth_TauMinus_ProdVtx_Z);
    float acoplanarity_IP = Truth_Acoplanarity_IP(PiPlus, PiPlusProdVtx,
              PiMinus, PiMinusProdVtx, TauPlusProdVtx, TauMinusProdVtx);
    //fill histogram
    h_SignedAcoplanarity_IP->Fill(acoplanarity_IP);
  }    



  draw_histo(h_Higgs_Mass, "H#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_Higgs_Mass" + modifier +".pdf");
  draw_histo(h_DiTau_VisMass, "H#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_VisDiTau_Mass" + modifier +".pdf");
  draw_histo(h_SignedAcoplanarity_IP, "H#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_SignedAcoplanarity_IP_CPodd" + modifier + ".pdf");

  TFile* outputFile = new TFile("AnalyserOutput"+ modifier +".root", "RECREATE");
  outputFile->cd();
  h_SignedAcoplanarity_IP->Write();
  outputFile->Close();

  m_inputFile->Close();

  return 0;
 
  




}
