#include "Analyser.h"
#include <iostream>


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
}




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
  std::string inBackgroundFileName = "";
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

    if(argument == "--inBackgroundFile"){
      convert << argv[++i];
      convert >> inBackgroundFileName;
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

  std::string backgroundTreeName = "ZtautauTree";

  TFile* m_background_inputFile = nullptr;
  TTree* backgroundTree = nullptr;

  // check if background file name is provided
  if(!inBackgroundFileName.empty()) {
    m_background_inputFile = TFile::Open(inBackgroundFileName.c_str());
    if (m_background_inputFile) {
      backgroundTree = dynamic_cast<TTree*>(m_background_inputFile->Get(backgroundTreeName.c_str()));
    }
    else {
      std::cout << "Unable to open background input file" << std::endl;
      
    }
  } else {
    std::cout << "No background file provided, proceeding without background comparison" << std::endl;
  }
  

  TString modifier = "";
  if (CPstate == 0){
    modifier = "_CPeven";
  } else if (CPstate == 1){
    modifier = "_CPodd";
  } else {
    modifier = "_CPmix_phi"+phi;
  }
  
  h_Higgs_Mass = new TH1D("h_Higgs_Mass","; m(H) [GeV]; Events / 0.02 GeV",60,80,130);
  h_DiTau_VisMass = new TH1D("h_DiTau_VisMass","; visible m(#tau^{+}#tau^{-}) [GeV]; Events / 10 GeV",20,0,200);
  h_SignedAcoplanarity_IP = new TH1D("h_SignedAcoplanarity_IP"+modifier,"; #phi* [rad]; AU",18,0,TMath::TwoPi());
  h_Phi = new TH1D("h_Phi"+modifier,"; #phi [rad]; AU", 18,-TMath::Pi(),TMath::Pi());
  h_Eta = new TH1D("h_Eta"+modifier,"; #eta ; AU", 18,-5,5);
  h_Pt = new TH1D("h_Pt"+modifier,"; p_{T} [GeV]; AU", 18,0,200);
  
  TreeReader * reader = new TreeReader(inTree);
  Long64_t nentries = inTree->GetEntries();

  // check if backgroundTree is provided
  TreeReader * backgroundReader = nullptr;
  Long64_t backgroundNEntries = 0;

  if (backgroundTree != nullptr) {
    backgroundReader = new TreeReader(backgroundTree);
    backgroundNEntries = backgroundTree->GetEntries();

  // create new histograms for background data only if background file is provided
  h_Z_Mass = new TH1D("h_Z_Mass","; m(Z) [GeV]; Events / 1 GeV",60,80,130);
  h_DiTau_VisMass_Bkg = new TH1D("h_DiTau_VisMass_Bkg","; visible m(#tau^{+}#tau^{-}) [GeV]; Events / 10 GeV",20,0,200);
  h_SignedAcoplanarity_IP_Bkg = new TH1D("h_SignedAcoplanarity_IP_Bkg"+modifier,"; #phi* [rad]; AU",18,0,TMath::TwoPi());
  h_Phi_Bkg = new TH1D("h_Phi_Bkg"+modifier,"; #phi [rad]; AU", 18,-TMath::Pi(),TMath::Pi());
  h_Eta_Bkg = new TH1D("h_Eta_Bkg"+modifier,"; #eta ; AU", 18,-5,5);
  h_Pt_Bkg = new TH1D("h_Pt_Bkg"+modifier,"; p_{T} [GeV]; AU", 18,0,200);
 
  }

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

  std::ofstream myfile;

    // open CSV file for output
    myfile.open ("Htautau_output"+ modifier +".csv");
    std::cout << "Writing output to Htautau_output"+ modifier +".csv" << std::endl;
    // write headers to CSV file
    myfile << "SignedAcoplanarity,Weight,Phi_PiPlus,Phi_PiMinus,Eta_PiPlus,Eta_PiMinus,Pt_PiPlus,Pt_PiMinus,ipX_PiPlus,ipY_PiPlus,ipZ_PiPlus,ipX_PiMinus,ipY_PiMinus,ipZ_PiMinus\n";  

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
    myfile << acoplanarity_IP << ",";
    
    float weight = 0;

    // convert phi to a number 
    float phi_num = std::stof(phi);

    if (phi_num < 0){
      weight = -1;
    } else if (phi_num > 0){
      weight = 1;
    } else if (phi_num == 0){
      weight = 0;
    }
    myfile << weight << ",";
  

    // extract Phi for PiPlus and PiMinus
    float Phi_PiPlus = PiPlus.Phi();
    float Phi_PiMinus = PiMinus.Phi();

    // extract Eta for PiPlus and PiMinus
    float Eta_PiPlus = PiPlus.Eta();
    float Eta_PiMinus = PiMinus.Eta();

    // extract Pt for PiPlus and PiMinus
    float Pt_PiPlus = PiPlus.Pt();
    float Pt_PiMinus = PiMinus.Pt();

    // fill histograms
    h_Phi->Fill(Phi_PiPlus);
    h_Phi->Fill(Phi_PiMinus);

    h_Eta->Fill(Eta_PiPlus);
    h_Eta->Fill(Eta_PiMinus);

    h_Pt->Fill(Pt_PiPlus);
    h_Pt->Fill(Pt_PiMinus);

    // calculate impact parameter vector for PiPlus and PiMinus using the production vertex of the pion and the production vertex of the tau as the primary vertex
    // use TVector3
    
  /*
    TVector3 ipVector_PiPlus3 = calculateImpactParamVec_Truth(PiPlus, PiPlusProdVtx, TauPlusProdVtx);
    TVector3 ipVector_PiMinus3 = calculateImpactParamVec_Truth(PiMinus, PiMinusProdVtx, TauMinusProdVtx);
    std::cout << "ipVector_PiPlus3: " << ipVector_PiPlus3.X() << ", " << ipVector_PiPlus3.Y() << ", " << ipVector_PiPlus3.Z() << std::endl;
*/

    TLorentzVector ipVector_PiPlus(calculateImpactParamVec_Truth(PiPlus, PiPlusProdVtx, TauPlusProdVtx).Unit(), 0.);
    TLorentzVector ipVector_PiMinus(calculateImpactParamVec_Truth(PiMinus, PiMinusProdVtx, TauMinusProdVtx).Unit(), 0.); 
   // std::cout << "ipVector_PiPlus: " << ipVector_PiPlus.X() << ", " << ipVector_PiPlus.Y() << ", " << ipVector_PiPlus.Z() << std::endl;

    // boost vectors into rest frame
    /*
    TLorentzVector referenceFrame = PiPlus + PiMinus;
    PiPlus.Boost(-referenceFrame.BoostVector());
    PiMinus.Boost(-referenceFrame.BoostVector());
    ipVector_PiPlus.Boost(-referenceFrame.BoostVector());
    ipVector_PiMinus.Boost(-referenceFrame.BoostVector());
    */


    // extract impact parameter components for PiPlus and PiMinus
    float ipX_PiPlus = ipVector_PiPlus.X();
    float ipY_PiPlus = ipVector_PiPlus.Y();
    float ipZ_PiPlus = ipVector_PiPlus.Z();

    float ipX_PiMinus = ipVector_PiMinus.X();
    float ipY_PiMinus = ipVector_PiMinus.Y();
    float ipZ_PiMinus = ipVector_PiMinus.Z();


    // set pion mass explicitly
    float PionMass = 0.13957;
    // set visible di-tau mass explicitly
    float Vis_DiTau_Mass = Vis_DiTau.M();


    // write values to CSV file
    myfile << Phi_PiPlus << ","
            << Phi_PiMinus << ","
            << Eta_PiPlus << ","
            << Eta_PiMinus << ","
            << Pt_PiPlus << ","
            << Pt_PiMinus << ","
            << ipX_PiPlus << ","
            << ipY_PiPlus << ","
            << ipZ_PiPlus << ","
            << ipX_PiMinus << ","
            << ipY_PiMinus << ","
            << ipZ_PiMinus << "\n";

  }    

  myfile.close();

  // event loop for background data, if background file is provided
  if (backgroundTree != nullptr) {
    for (Long64_t jentry=0; jentry<backgroundNEntries;jentry++) {
      Long64_t ientry = backgroundReader->LoadTree(jentry);
      if (ientry < 0) break;
      backgroundReader->fChain->GetEntry(jentry);
      
      if((jentry % eventsPercent) == 0)
      {
    std::cout << "Processing is: " << (jentry*100)/nentries << "\% complete" << std::endl;
      }
        
      PiPlus.SetPtEtaPhiM(backgroundReader->Truth_PiPlus_Pt, backgroundReader->Truth_PiPlus_Eta, backgroundReader->Truth_PiPlus_Phi, 0.13957);
      PiMinus.SetPtEtaPhiM(backgroundReader->Truth_PiMinus_Pt, backgroundReader->Truth_PiMinus_Eta, backgroundReader->Truth_PiMinus_Phi, 0.13957);
      Vis_DiTau = PiPlus + PiMinus;
      h_Z_Mass->Fill(backgroundReader->Truth_Z_M);
      h_DiTau_VisMass_Bkg->Fill(Vis_DiTau.M());
    
      PiPlusProdVtx.SetXYZ(backgroundReader->Truth_PiPlus_ProdVtx_X,
          backgroundReader->Truth_PiPlus_ProdVtx_Y,
          backgroundReader->Truth_PiPlus_ProdVtx_Z);
      PiMinusProdVtx.SetXYZ(backgroundReader->Truth_PiMinus_ProdVtx_X,
          backgroundReader->Truth_PiMinus_ProdVtx_Y,
          backgroundReader->Truth_PiMinus_ProdVtx_Z);
      TauPlusProdVtx.SetXYZ(backgroundReader->Truth_TauPlus_ProdVtx_X,
          backgroundReader->Truth_TauPlus_ProdVtx_Y,
          backgroundReader->Truth_TauPlus_ProdVtx_Z);   
      TauMinusProdVtx.SetXYZ(backgroundReader->Truth_TauMinus_ProdVtx_X,
          backgroundReader->Truth_TauMinus_ProdVtx_Y,
          backgroundReader->Truth_TauMinus_ProdVtx_Z);
      float acoplanarity_IP = Truth_Acoplanarity_IP(PiPlus, PiPlusProdVtx,
                PiMinus, PiMinusProdVtx, TauPlusProdVtx, TauMinusProdVtx);
      //fill histogram
      h_SignedAcoplanarity_IP_Bkg->Fill(acoplanarity_IP);

      // extract Phi for PiPlus and PiMinus
      float Phi_PiPlus = PiPlus.Phi();
      float Phi_PiMinus = PiMinus.Phi();

      // extract Eta for PiPlus and PiMinus
      float Eta_PiPlus = PiPlus.Eta();
      float Eta_PiMinus = PiMinus.Eta();

      // extract Pt for PiPlus and PiMinus
      float Pt_PiPlus = PiPlus.Pt();
      float Pt_PiMinus = PiMinus.Pt();

      // fill histograms
      h_Phi_Bkg->Fill(Phi_PiPlus);
      h_Phi_Bkg->Fill(Phi_PiMinus);

      h_Eta_Bkg->Fill(Eta_PiPlus);
      h_Eta_Bkg->Fill(Eta_PiMinus);

      h_Pt_Bkg->Fill(Pt_PiPlus);
      h_Pt_Bkg->Fill(Pt_PiMinus);
        

    } 
  }


  draw_histo(h_Higgs_Mass, "H#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_Higgs_Mass" + modifier +".pdf");
  draw_histo(h_DiTau_VisMass, "H#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_VisDiTau_Mass" + modifier +".pdf");
  draw_histo(h_SignedAcoplanarity_IP, "H#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_SignedAcoplanarity_IP_CPodd" + modifier + ".pdf");
  draw_histo(h_Phi, "H#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_Phi" + modifier + ".pdf");
  draw_histo(h_Eta, "H#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_Eta" + modifier + ".pdf");
  draw_histo(h_Pt, "H#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_Pt" + modifier + ".pdf");


  TFile* outputFile = new TFile("AnalyserOutput"+ modifier +".root", "RECREATE");
  outputFile->cd();
  h_SignedAcoplanarity_IP->Write("h_SignedAcoplanarity_IP"+modifier);
  h_Phi->Write("h_Phi"+modifier);
  h_Eta->Write("h_Eta"+modifier);
  h_Pt->Write("h_Pt"+modifier);
  h_DiTau_VisMass->Write("h_DiTau_VisMass"+modifier);
  h_Higgs_Mass->Write("h_Higgs_Mass"+modifier);
  outputFile->Close();

  if (m_background_inputFile != nullptr) {
        // Check if all background histograms are initialized
    if (h_Z_Mass && h_DiTau_VisMass_Bkg && h_SignedAcoplanarity_IP_Bkg && h_Phi_Bkg && h_Eta_Bkg && h_Pt_Bkg && h_DiTau_VisMass_Bkg && h_Z_Mass) {
    // draw histograms for background data comparison
      draw_histo(h_Z_Mass, "Z#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_Z_Mass_Background.pdf");
      draw_histo(h_DiTau_VisMass_Bkg, "Z#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_VisDiTau_Mass_Background.pdf");
      draw_histo(h_SignedAcoplanarity_IP_Bkg, "Z#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_SignedAcoplanarity_IP_Background.pdf");
      draw_histo(h_Phi_Bkg, "Z#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_Phi_Background.pdf");
      draw_histo(h_Eta_Bkg, "Z#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_Eta_Background.pdf");
      draw_histo(h_Pt_Bkg, "Z#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_Pt_Background.pdf");

      // save histograms for main input and background data
      TFile* background_outputFile = new TFile("AnalyserOutput_Background.root", "RECREATE");
      background_outputFile->cd();
      h_SignedAcoplanarity_IP_Bkg->Write("h_SignedAcoplanarity_IP_Bkg");
      h_Phi_Bkg->Write("h_Phi_Bkg");
      h_Eta_Bkg->Write("h_Eta_Bkg");
      h_Pt_Bkg->Write("h_Pt_Bkg");
      h_DiTau_VisMass_Bkg->Write("h_DiTau_VisMass_Bkg");
      h_Z_Mass->Write("h_Z_Mass");

      background_outputFile->Close();
      m_background_inputFile->Close();
    } else {
        std::cerr << "Error: One or more background histograms are null!" << std::endl;
    }
  }
  else {
    std::cout << "No background file provided, skipping background histogram drawing and saving" << std::endl;
  }
  m_inputFile->Close();




  return 0;


}
