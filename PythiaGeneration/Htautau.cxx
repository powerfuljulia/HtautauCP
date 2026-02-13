#include "Htautau.h"

// const unsigned int nEvents = 5000000;
const unsigned int nEvents = 100000;

int main(int argc, char* argv[]) {

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

    
    
  }

  TString outputName = "Htautau_CPeven.root";
    std::cout << "Start H->tautau decay test" << std::endl;

    // Set seed
    pythia.readString("Random:setSeed = on");
    pythia.readString("Random:seed = 2585");

    // pp collisions at 13 TeV
    pythia.readString("Beams:idA = 2212");
    pythia.readString("Beams:idB = 2212");
    pythia.readString("Beams:eCM = 13000.");

    // ggF production
    pythia.readString("HiggsSM:gg2H = on");

    // Not so relevant and makes events bigger
    pythia.readString("PartonLevel:MPI = off");

    // H->tau tau decays
    if(CPstate == 1) {
      outputName = "Htautau_CPodd.root";
      pythia.readString("HiggsH1:parity = 2"); // making things CP-odd
    }

    else if(CPstate == 2){
      outputName = std::string("Htautau_CPmix") + (std::stod(phi) < 0 ? "-45" : "45") +".root";
      pythia.readString("HiggsH1:parity = 4");
      pythia.readString("HiggsH1:phiParity = " + phi);
    }
    
    pythia.readString("25:onMode = off");
    pythia.readString("25:onIfMatch = 15 15");

    // tau decays (both H -> tau nu)
    // --- Tau- (id = 15) ---
    pythia.readString("15:onMode = off");   // turn off all tau- decays
    pythia.readString("15:onIfMatch = -211 16"); // τ− → π− ντ
    // --- Tau+ (id = -15) ---
    pythia.readString("-15:onMode = off"); // turn off all tau+ decays
    pythia.readString("-15:onIfMatch = 211 -16"); // τ+ → π+ ντ 

    pythia.init();

    // Setup output
    OutputFile = new TFile(outputName, "recreate");
    OutputFile->cd();

    // Initialise histograms
    h_Higgs_Mass = new TH1D("h_Higgs_Mass", "; m(H) [GeV]; Events / 10 GeV", 20, 0, 200);
    h_Higgs_Pt = new TH1D("h_Higgs_Pt", "; p_{T}(H) [GeV]; Events / 10 GeV", 20, 0, 200);

    h_PosTau_VisPt = new TH1D("h_PosTau_VisPt", "; vis. p_{T}(#tau^{+}) [GeV]; Events / 10 GeV", 20, 0, 200);
    h_NegTau_VisPt = new TH1D("h_NegTau_VisPt", "vis. p_{T}(#tau^{-}) [GeV]; Events / 10 GeV", 20, 0, 200);

    h_DiTau_VisMass = new TH1D("h_DiTau_VisMass", "; vis m(#tau^{+}#tau^{-}) [GeV]; Events / 10 GeV", 20, 0, 200);
    h_DiTau_VisPt = new TH1D("h_DiTau_VisPt", "; vis p_{T}(#tau^{+}#tau^{-}) [GeV]; Events / 10 GeV", 20, 0, 200);
    TString treeName = "HtautauTree";
    TTree * outputTree = new TTree(treeName, treeName);
    BranchMaker(outputTree);

    // Event Loop
    for (unsigned int iEvent = 0; iEvent < nEvents; ++iEvent) {
        ClearVars();

        if (!pythia.next()) continue;

        unsigned int iH = -1;

        // Look for the H boson
        // Loop over particles in event 
        for (unsigned int i = 0; i < pythia.event.size(); ++i) {
            // Find final instance of the H 
            if (pythia.event[i].idAbs() == 25 && pythia.event[i].statusAbs() == 62) {
                iH = i;
                break;
            }
        }

        // Check we found the H
        if (iH == -1) continue;

        h_Higgs_Mass->Fill(pythia.event[iH].m());
        h_Higgs_Pt->Fill(pythia.event[iH].pT());

        unsigned int d1 = pythia.event[iH].daughter1();
        unsigned int d2 = pythia.event[iH].daughter2();
        unsigned int iTauMin = 0;
        unsigned int iTauPos = 0;

        unsigned int iPiMin = 0;
        unsigned int iPiPos = 0;

        if (pythia.event[d1].charge() < 0) {
            iTauMin = d1;
            iTauPos = d2;
        } else if (pythia.event[d2].charge() > 0) {
            iTauMin = d2;
            iTauPos = d1;
        }

        TLorentzVector PiPos;
        TLorentzVector PiMin;

        std::vector<unsigned int> decayProducts_tau1;
        std::vector<unsigned int> decayProducts_tau2;
        GetDecayProducts(d1, decayProducts_tau1);

        for (unsigned int n = 0; n < decayProducts_tau1.size(); ++n) {
            unsigned int prodIndex = decayProducts_tau1.at(n);

            TLorentzVector prodVec;
            prodVec.SetPxPyPzE(pythia.event[prodIndex].px(), pythia.event[prodIndex].py(), pythia.event[prodIndex].pz(), pythia.event[prodIndex].e());

            if (pythia.event[prodIndex].idAbs() == 211) {
                if (pythia.event[prodIndex].charge() > 0) {
                    PiPos = prodVec;
                    iPiPos = prodIndex;
                    h_PosTau_VisPt->Fill(pythia.event[prodIndex].pT());
                } else if (pythia.event[prodIndex].charge() < 0) {
                    PiMin = prodVec;
                    h_NegTau_VisPt->Fill(pythia.event[prodIndex].pT());
                    iPiMin = prodIndex;
                }
            }
        }

        GetDecayProducts(d2, decayProducts_tau2);

        for (unsigned int n = 0; n < decayProducts_tau2.size(); ++n) {
            unsigned int prodIndex = decayProducts_tau2.at(n);

            TLorentzVector prodVec;
            prodVec.SetPxPyPzE(pythia.event[prodIndex].px(), pythia.event[prodIndex].py(), pythia.event[prodIndex].pz(), pythia.event[prodIndex].e());

            if (pythia.event[prodIndex].idAbs() == 211) {
                if (pythia.event[prodIndex].charge() > 0) {
                    PiPos = prodVec;
                    iPiPos = prodIndex;
                    h_PosTau_VisPt->Fill(pythia.event[prodIndex].pT());
                } else if (pythia.event[prodIndex].charge() < 0) {
                    PiMin = prodVec;
                    h_NegTau_VisPt->Fill(pythia.event[prodIndex].pT());
                    iPiMin = prodIndex;
                }
            }
        }

        TLorentzVector DiTau = PiPos + PiMin;

        h_DiTau_VisMass->Fill(DiTau.M());
        h_DiTau_VisPt->Fill(DiTau.Pt());

        TLorentzVector Higgs;
        Higgs.SetPxPyPzE(pythia.event[iH].px(), pythia.event[iH].py(), pythia.event[iH].pz(), pythia.event[iH].e());

        TLorentzVector TauMinus;
        TauMinus.SetPxPyPzE(pythia.event[iTauMin].px(), pythia.event[iTauMin].py(), pythia.event[iTauMin].pz(), pythia.event[iTauMin].e());
        
        TLorentzVector TauPlus;
        TauPlus.SetPxPyPzE(pythia.event[iTauPos].px(), pythia.event[iTauPos].py(), pythia.event[iTauPos].pz(), pythia.event[iTauPos].e());  

        m_T_Truth_Higgs_Pt = Higgs.Pt();
        m_T_Truth_Higgs_Eta = Higgs.Eta();
        m_T_Truth_Higgs_Phi = Higgs.Phi();
        m_T_Truth_Higgs_M = Higgs.M();

        m_T_Truth_TauMinus_Pt = TauMinus.Pt();
        m_T_Truth_TauMinus_Eta = TauMinus.Eta();
        m_T_Truth_TauMinus_Phi = TauMinus.Phi();

        m_T_Truth_TauMinus_ProdVtx_X = pythia.event[iTauMin].xProd();
        m_T_Truth_TauMinus_ProdVtx_Y = pythia.event[iTauMin].yProd();
        m_T_Truth_TauMinus_ProdVtx_Z = pythia.event[iTauMin].zProd();

        m_T_Truth_TauPlus_Pt = TauPlus.Pt();
        m_T_Truth_TauPlus_Eta = TauPlus.Eta();
        m_T_Truth_TauPlus_Phi = TauPlus.Phi();

        m_T_Truth_TauPlus_ProdVtx_X = pythia.event[iTauPos].xProd();
        m_T_Truth_TauPlus_ProdVtx_Y = pythia.event[iTauPos].yProd();
        m_T_Truth_TauPlus_ProdVtx_Z = pythia.event[iTauPos].zProd();

        m_T_Truth_PiMinus_Pt = PiMin.Pt();
        m_T_Truth_PiMinus_Eta = PiMin.Eta();
        m_T_Truth_PiMinus_Phi = PiMin.Phi();

        m_T_Truth_PiMinus_ProdVtx_X = pythia.event[iPiMin].xProd();
        m_T_Truth_PiMinus_ProdVtx_Y = pythia.event[iPiMin].yProd();
        m_T_Truth_PiMinus_ProdVtx_Z = pythia.event[iPiMin].zProd();

        m_T_Truth_PiPlus_Pt = PiPos.Pt();
        m_T_Truth_PiPlus_Eta = PiPos.Eta();
        m_T_Truth_PiPlus_Phi = PiPos.Phi();

        m_T_Truth_PiPlus_ProdVtx_X = pythia.event[iPiPos].xProd();
        m_T_Truth_PiPlus_ProdVtx_Y = pythia.event[iPiPos].yProd();
        m_T_Truth_PiPlus_ProdVtx_Z = pythia.event[iPiPos].zProd();

        outputTree->Fill();
    }

    // End of event loop. Statistics. Histogram. Done.
    pythia.stat();

    std::cout << "Write to file: " << OutputFile->GetName() << std::endl;

    OutputFile->cd();

    h_Higgs_Mass->Write();
    h_Higgs_Pt->Write();

    h_PosTau_VisPt->Write();
    h_NegTau_VisPt->Write();

    h_DiTau_VisMass->Write();
    h_DiTau_VisPt->Write();

    outputTree->Write();

    std::cout << "Close file : " << OutputFile->GetName() << std::endl;

    OutputFile->Close();

    std::cout << "Done" << std::endl;

    return 0;
}

void GetDecayProducts(const unsigned int p, std::vector<unsigned int>& products) {
    if (pythia.event[p].isFinal()) return;

    const unsigned int d1 = pythia.event[p].daughter1();
    const unsigned int d2 = pythia.event[p].daughter2();

    for (unsigned int i = d1; i <= d2; ++i) {
        if (pythia.event[i].isFinal()) {
            if (CheckUnique(i, products)) {
                products.push_back(i);
            }
        } else {
            GetDecayProducts(i, products);
        }
    }
}

bool CheckUnique(unsigned int p, std::vector<unsigned int> products) {
    for (int i = 0; i < products.size(); i++) {
        if (products.at(i) == p) {
            return false;
        }
    }
    return true;
}

void ClearVars() {

    m_T_Truth_Higgs_Pt = -99.;
    m_T_Truth_Higgs_Eta = -99.;
    m_T_Truth_Higgs_Phi = -99.;    
    m_T_Truth_Higgs_M = -99.;  
    
    m_T_Truth_TauMinus_Pt = -99.;
    m_T_Truth_TauMinus_Eta  = -99.;
    m_T_Truth_TauMinus_Phi = -99.;
    m_T_Truth_TauMinus_ProdVtx_X = -99.;
    m_T_Truth_TauMinus_ProdVtx_Y = -99.;
    m_T_Truth_TauMinus_ProdVtx_Z = -99.;
    
    m_T_Truth_TauPlus_Pt = -99.;
    m_T_Truth_TauPlus_Eta = -99.;
    m_T_Truth_TauPlus_Phi = -99.;
    m_T_Truth_TauPlus_ProdVtx_X = -99.;
    m_T_Truth_TauPlus_ProdVtx_Y = -99.;
    m_T_Truth_TauPlus_ProdVtx_Z = -99.;
    
    m_T_Truth_PiMinus_Pt = -99.;
    m_T_Truth_PiMinus_Eta = -99.;
    m_T_Truth_PiMinus_Phi = -99.;

    m_T_Truth_PiMinus_ProdVtx_X = -99.;
    m_T_Truth_PiMinus_ProdVtx_Y = -99.;
    m_T_Truth_PiMinus_ProdVtx_Z = -99.;
    
    m_T_Truth_PiPlus_Pt = -99.;
    m_T_Truth_PiPlus_Eta = -99.;
    m_T_Truth_PiPlus_Phi = -99.;

    m_T_Truth_PiPlus_ProdVtx_X = -99.;
    m_T_Truth_PiPlus_ProdVtx_Y = -99.;
    m_T_Truth_PiPlus_ProdVtx_Z = -99.;

}

void BranchMaker(TTree* Tree) {
    Tree->Branch("Truth_Higgs_Pt", &m_T_Truth_Higgs_Pt);
    Tree->Branch("Truth_Higgs_Eta", &m_T_Truth_Higgs_Eta);
    Tree->Branch("Truth_Higgs_Phi", &m_T_Truth_Higgs_Phi);
    Tree->Branch("Truth_Higgs_M", &m_T_Truth_Higgs_M);
    Tree->Branch("Truth_TauMinus_Pt", &m_T_Truth_TauMinus_Pt);
    Tree->Branch("Truth_TauMinus_Eta", &m_T_Truth_TauMinus_Eta);
    Tree->Branch("Truth_TauMinus_Phi", &m_T_Truth_TauMinus_Phi);
    Tree->Branch("Truth_TauMinus_ProdVtx_X", &m_T_Truth_TauMinus_ProdVtx_X);
    Tree->Branch("Truth_TauMinus_ProdVtx_Y", &m_T_Truth_TauMinus_ProdVtx_Y);
    Tree->Branch("Truth_TauMinus_ProdVtx_Z", &m_T_Truth_TauMinus_ProdVtx_Z);
    Tree->Branch("Truth_TauPlus_Pt", &m_T_Truth_TauPlus_Pt);
    Tree->Branch("Truth_TauPlus_Eta", &m_T_Truth_TauPlus_Eta);
    Tree->Branch("Truth_TauPlus_Phi", &m_T_Truth_TauPlus_Phi);
    Tree->Branch("Truth_TauPlus_ProdVtx_X", &m_T_Truth_TauPlus_ProdVtx_X);
    Tree->Branch("Truth_TauPlus_ProdVtx_Y", &m_T_Truth_TauPlus_ProdVtx_Y);
    Tree->Branch("Truth_TauPlus_ProdVtx_Z", &m_T_Truth_TauPlus_ProdVtx_Z);
    Tree->Branch("Truth_PiMinus_Pt", &m_T_Truth_PiMinus_Pt);
    Tree->Branch("Truth_PiMinus_Eta", &m_T_Truth_PiMinus_Eta);
    Tree->Branch("Truth_PiMinus_Phi", &m_T_Truth_PiMinus_Phi);
    Tree->Branch("Truth_PiMinus_ProdVtx_X", &m_T_Truth_PiMinus_ProdVtx_X);
    Tree->Branch("Truth_PiMinus_ProdVtx_Y", &m_T_Truth_PiMinus_ProdVtx_Y);
    Tree->Branch("Truth_PiMinus_ProdVtx_Z", &m_T_Truth_PiMinus_ProdVtx_Z);
    Tree->Branch("Truth_PiPlus_Pt", &m_T_Truth_PiPlus_Pt);
    Tree->Branch("Truth_PiPlus_Eta", &m_T_Truth_PiPlus_Eta);
    Tree->Branch("Truth_PiPlus_Phi", &m_T_Truth_PiPlus_Phi);
    Tree->Branch("Truth_PiPlus_ProdVtx_X", &m_T_Truth_PiPlus_ProdVtx_X);
    Tree->Branch("Truth_PiPlus_ProdVtx_Y", &m_T_Truth_PiPlus_ProdVtx_Y);
    Tree->Branch("Truth_PiPlus_ProdVtx_Z", &m_T_Truth_PiPlus_ProdVtx_Z);    
}
