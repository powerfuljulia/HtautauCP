#include "Htautau.h"

//const unsigned int nEvents = 5000000;
const unsigned int nEvents = 10000;

int main(int argc, char* argv[]) {

    std::cout << "Start H->tautau decay test" << std::endl;

    // Set seed
    pythia.readString("Random:setSeed = on");
    pythia.readString("Random:seed = 2585");

    // pp collisons at 13 TeV
    pythia.readString("Beams:idA = 2212");
    pythia.readString("Beams:idB = 2212");
    pythia.readString("Beams:eCM = 13000.");

    // ggF production
    pythia.readString("HiggsSM:gg2H = on");

    // Not so relevant and makes events bigger
    pythia.readString("PartonLevel:MPI = off");

    // H->tau tau decays
    pythia.readString("25:onMode = off");
    pythia.readString("25:onIfMatch = 15 15");

    //tau decays (both H -> tau nu)

    // --- Tau- (id = 15) ---
    pythia.readString("15:onMode = off");   // turn off all tau- decays
    pythia.readString("15:onIfMatch = -211 16"); // τ− → π− ντ
    // --- Tau+ (id = -15) ---
    pythia.readString("-15:onMode = off"); // turn off all tau+ decays
    pythia.readString("-15:onIfMatch = 211 -16"); // τ+ → π+ ντ 
    
    pythia.init();

    // Setup output
    OutputFile = new TFile("Output.root","recreate");

    OutputFile->cd();

    // Initialise histograms

    h_Higgs_Mass = new TH1D("h_Higgs_Mass","; m(H) [GeV]; Events / 10 GeV",20,0,200);
    h_Higgs_Pt = new TH1D("h_Higgs_Pt","; p_{T}(H) [GeV]; Events / 10 GeV",20,0,200);

    h_PosTau_VisPt =  new TH1D("h_PosTau_VisPt","; vis. p_{T}(#tau^{+}) [GeV]; Events / 10 GeV",20,0,200);
    h_NegTau_VisPt =  new TH1D("h_NegTau_VisPt","vis. p_{T}(#tau^{-}) [GeV];  Events / 10 GeV",20,0,200);

    h_DiTau_VisMass = new TH1D("h_DiTau_VisMass","; vis m(#tau^{+}#tau^{-}) [GeV]; Events / 10 GeV",20,0,200);
    h_DiTau_VisPt = new TH1D("h_DiTau_VisPt","; vis p_{T}(#tau^{+}#tau^{-}) [GeV]; Events / 10 GeV",20,0,200);

    h_Signed_Acoplanarity = new TH1D("h_Signed_Acoplanarity", "#phi^{*}_{CP}", 20, 0, 6.4);
    
    
    // Event Loop
    for(unsigned int iEvent = 0; iEvent < nEvents; ++iEvent) {

        //std::cout << "Event " <<  iEvent << std::endl;

        if (!pythia.next()) continue;
    
        unsigned int iH = -1;
        
        // Look for the H boson

        // Loop over particles in event 
        for(unsigned int i = 0; i < pythia.event.size(); ++i) {

            // Find final instance of the H 
            if( pythia.event[i].idAbs() ==  25 && pythia.event[i].statusAbs() == 62 ) {
                iH = i;
                break;
            }

        } // Loop over particles in event

        // Check we found the H
        if( iH ==  -1 ) continue;
	h_Higgs_Mass->Fill(pythia.event[iH].m());
	h_Higgs_Pt->Fill(pythia.event[iH].pT());

	unsigned int d1 = pythia.event[iH].daughter1();
	unsigned int d2 = pythia.event[iH].daughter2();
	unsigned int iTauMin = 0;
	unsigned int iTauPos = 0;

	unsigned int iPiMin = 0;
	unsigned int iPiPos = 0;

	
	if(pythia.event[d1].charge()< 0){
	  iTauMin = d1;
	  iTauPos = d2;
	}
	else if(pythia.event[d2].charge() > 0){
	  iTauMin = d2;
	  iTauPos = d1;
	}
	
	TLorentzVector PosPi;
	TLorentzVector NegPi;

	
	std::vector<unsigned int> decayProducts_tau1;
	std::vector<unsigned int> decayProducts_tau2;
        GetDecayProducts(d1,decayProducts_tau1);

	for(unsigned int n = 0; n < decayProducts_tau1.size(); ++n) {

            unsigned int prodIndex = decayProducts_tau1.at(n);

            TLorentzVector prodVec;
	    //	    std::cout<<"pythia.event[prodIndex].idAbs() = "<<pythia.event[prodIndex].idAbs()<<std::endl;
	    prodVec.SetPxPyPzE( pythia.event[prodIndex].px(), pythia.event[prodIndex].py(), pythia.event[prodIndex].pz(), pythia.event[prodIndex].e() );
	    if(pythia.event[prodIndex].idAbs()==211){
	      if(pythia.event[prodIndex].charge() > 0){
		PosPi = prodVec;
		iPiPos = prodIndex;
		h_PosTau_VisPt->Fill(pythia.event[prodIndex].pT());
	      }
	      else if(pythia.event[prodIndex].charge() < 0){
		NegPi = prodVec;
		h_NegTau_VisPt->Fill(pythia.event[prodIndex].pT());
		iPiMin = prodIndex;
	      }
	      
	      
	    }
	}
	
	GetDecayProducts(d2,decayProducts_tau2);

        for(unsigned int n = 0; n < decayProducts_tau2.size(); ++n) {

            unsigned int prodIndex = decayProducts_tau2.at(n);

            TLorentzVector prodVec;

            prodVec.SetPxPyPzE( pythia.event[prodIndex].px(), pythia.event[prodIndex].py(), pythia.event[prodIndex].pz(), pythia.event[prodIndex].e() );
            if(pythia.event[prodIndex].idAbs()==211){
	      //	      std::cout<<"pythia.event[prodIndex].charge() = "<<pythia.event[prodIndex].charge()<<std::endl;
              if(pythia.event[prodIndex].charge() > 0){
		PosPi = prodVec;
		iPiPos = prodIndex;
                h_PosTau_VisPt->Fill(pythia.event[prodIndex].pT());
              }
              else if(pythia.event[prodIndex].charge() < 0){
	        NegPi = prodVec;
                h_NegTau_VisPt->Fill(pythia.event[prodIndex].pT());
		iPiMin = prodIndex;
              }
		
		
            }
        }

	TLorentzVector DiTau = PosPi + NegPi;
	
	h_DiTau_VisMass->Fill(DiTau.M());
	h_DiTau_VisPt->Fill(DiTau.Pt());

	TLorentzVector Higgs;
	Higgs.SetPxPyPzE( pythia.event[iH].px(), pythia.event[iH].py(), pythia.event[iH].pz(), pythia.event[iH].e() );
	TVector3 BoostVector = Higgs.BoostVector();

	TLorentzVector TauMin_HRest;
	TauMin_HRest.SetPxPyPzE( pythia.event[iTauMin].px(), pythia.event[iTauMin].py(), pythia.event[iTauMin].pz(), pythia.event[iTauMin].e() );
	TauMin_HRest.Boost(-BoostVector);

	TLorentzVector TauPos_HRest;
	TauPos_HRest.SetPxPyPzE( pythia.event[iTauPos].px(), pythia.event[iTauPos].py(), pythia.event[iTauPos].pz(), pythia.event[iTauPos].e() );
	TauPos_HRest.Boost(-BoostVector);
	
	TLorentzVector PiMin_HRest = NegPi;
	PiMin_HRest.Boost(-BoostVector);

	TLorentzVector PiPos_HRest = PosPi;
	PiPos_HRest.Boost(-BoostVector);

	TVector3 TauMin_direction = TauMin_HRest.Vect().Unit();
	TVector3 TauPos_direction = TauPos_HRest.Vect().Unit();
	TVector3 PiMin_direction = PiMin_HRest.Vect().Unit();	
	TVector3 PiPos_direction = PiPos_HRest.Vect().Unit();
	
	TVector3 n1 = TauMin_direction.Cross(PiMin_direction).Unit();
	TVector3 n2 = TauPos_direction.Cross(PiPos_direction).Unit();

	double cosphi = n1.Dot(n2);
	cosphi = std::max(-1.0, std::min(1.0, cosphi));
	double phi = acos(cosphi);

	double sign = TauMin_direction.Dot(n1.Cross(n2));
	if (sign < 0) phi = 2.0 * M_PI - phi;

	h_Signed_Acoplanarity->Fill(phi);
	
    } // End of event loop. Statistics. Histogram. Done.
    
    pythia.stat();

    std::cout << "Write to file: " << OutputFile->GetName() << std::endl;

    OutputFile->cd();

    h_Higgs_Mass->Write();
    h_Higgs_Pt->Write();

    h_PosTau_VisPt->Write();
    h_NegTau_VisPt->Write();

    h_DiTau_VisMass->Write();
    h_DiTau_VisPt->Write();

    h_Signed_Acoplanarity->Write();
    

    std::cout << "Close file : " << OutputFile->GetName() << std::endl;

    OutputFile->Close();

    std::cout << "Done" << std::endl;

    return 0;

}


void GetDecayProducts(const unsigned int p, std::vector<unsigned int> & products) {

    if( pythia.event[p].isFinal() ) return;

    const unsigned int d1 = pythia.event[p].daughter1();
    const unsigned int d2 = pythia.event[p].daughter2();

    for(unsigned int i = d1; i <= d2; ++i) {


        if( pythia.event[i].isFinal() ) {

            if( CheckUnique(i,products) ) {
                products.push_back(i);
            }

        } else {

            GetDecayProducts(i,products);
        }

    }

}

bool CheckUnique(unsigned int p, std::vector<unsigned int> products) {

    for(int i = 0; i < products.size(); i++ ) {

        if( products.at(i) == p ) {
            return false;
        }

    }

    return true;

}

