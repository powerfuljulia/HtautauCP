#include "Analyser.h"

void draw_histo(TH1D* h, TString process, TString filename){

  TCanvas* c = new TCanvas("canvas", "canvas", 600, 600);

  h->Sumw2();
  h->SetMaximum(1.2*h->GetMaximum());
  h->GetXaxis()->SetTitleOffset(1.3);
  h->GetYaxis()->SetTitleOffset(1.6);
  h->GetYaxis()->SetLabelSize(0.045);
  h->GetYaxis()->SetTitleSize(0.045);
  h->GetXaxis()->SetLabelSize(0.045);
  //h->GetXaxis()->SetMaxDigits(2);
  h->GetXaxis()-> SetNdivisions(503);
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

  SetAtlasStyle();
  rootlogon(); 

  std::string treeName = "HtautauTree";
  
  TFile* m_inputFile = TFile::Open("../PythiaGeneration/Htautau.root");
    
  if(!m_inputFile)
    {
      std::cout << "Unable to open input file" << std::endl;
      return -1;
      
    }

  TTree * inTree = dynamic_cast<TTree*>(m_inputFile->Get(treeName.c_str()));
  
  TreeReader * reader = new TreeReader(inTree);
  Long64_t nentries = inTree->GetEntries();

  int eventsPercent = nentries/100;
  
  if(eventsPercent == 0)
    {
      eventsPercent = 1;
    }


  h_Higgs_Mass = new TH1D("h_Higgs_Mass","; m(H) [GeV]; Events / 0.02 GeV",50,124.5,125.5);
  h_DiTau_VisMass = new TH1D("h_DiTau_VisMass","; visible m(#tau^{+}#tau^{-}) [GeV]; Events / 10 GeV",20,0,200);
  TLorentzVector PiPlus;
  TLorentzVector PiMinus;
  TLorentzVector Vis_DiTau;

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
  }    


  draw_histo(h_Higgs_Mass, "H#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_Higgs_Mass.pdf");
  draw_histo(h_DiTau_VisMass, "H#rightarrow#tau(#rightarrow#pi#nu)#tau(#rightarrow#pi#nu)", "Truth_VisDiTau_Mass.pdf");
  m_inputFile->Close();

  return 0;
 
  




}
