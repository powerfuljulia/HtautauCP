#include "CreateNNInput.h"

 #include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream> 

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TLorentzVector.h"

#include "MicroTreeReader.h"


int main(int argc, char *argv[])
{



    std::string inputFileName = "";
    std::string treeName = "";
    std::string OutFileName = "";
      	treeName = "HtautauTree";
	  inputFileName = "/Users/ella/Library/CloudStorage/OneDrive-UniversityofEdinburgh/senior_honours_project/htautaucp_project/PythiaGeneration/Htautau_CPmix45.root";
	  OutFileName = "Htautau_CPmix45.csv";
	

    
    

    
    TFile* m_inputFile = TFile::Open(inputFileName.c_str());
    
    if(!m_inputFile)
      {
	std::cout << "Unable to open input file" << std::endl;
	return -1;
	
      }

    TTree * inTree_Signal_SM = dynamic_cast<TTree*>(m_inputFile->Get(treeName.c_str()));

    std::ofstream myfile;

    myfile.open (OutFileName);
    myfile << "SignedAcoplanarity,Weight\n";
    TreeReader * reader = new TreeReader(inTree_Signal_SM);
    Long64_t nentries = inTree_Signal_SM->GetEntries();

    int eventsPercent = nentries/100;
    
    if(eventsPercent == 0)
      {
	eventsPercent = 1;
      }


    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = reader->LoadTree(jentry);
      if (ientry < 0) break;
      reader->fChain->GetEntry(jentry);
      
      if((jentry % eventsPercent) == 0)
	{
	  std::cout << "Processing is: " << (jentry*100)/nentries << "\% complete" << std::endl;
    }
      
      myfile << reader->LeadingJet_Pt<<",";
      myfile << reader->LeadingJet_Eta<<",";
      myfile << reader->LeadingJet_Phi<<",";
      
      myfile << reader->SubLeadingJet_Pt<<",";
      myfile << reader->SubLeadingJet_Eta<<",";
      myfile << reader->SubLeadingJet_Phi<<",";
      
      
      myfile << reader->DiTau_Pt<<",";
      myfile << reader->DiTau_Eta<<",";
      myfile << reader->DiTau_Phi<<",";
      
      myfile << reader->DiJet_SignedDeltaPhi<<",";
      double rescale_factor = 1.;
      if(sample==2){
	if(reader->RecoFinalState==1) rescale_factor = (1171+73.5)/940.;
	else if(reader->RecoFinalState==2 || reader->RecoFinalState==3) rescale_factor = (3219+203)/3535.;
	else if(reader->RecoFinalState==4) rescale_factor= (3135+156)/4018.;
      }

      
      if(energy==13) myfile << reader->WeightSignal*139*rescale_factor<<"\n";
      else if(energy==100) myfile << reader->WeightSignal*30000*rescale_factor<<"\n";      
    }
    
    m_inputFile->Close();
    myfile.close();        

    return 0;
 
  

  }

  

