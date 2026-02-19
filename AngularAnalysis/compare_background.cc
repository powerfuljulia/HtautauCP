void compare_histos(std::vector<TH1D*> histos, std::vector<TString> legend_labels, TString file_name){

    for (int i = 0; i < histos.size(); i++) {
        if (!histos[i]) {
            std::cerr << "Error: Histogram at index " << i << " is null!" << std::endl;
            return;
        }
        if (histos[i]->Integral() == 0) {
            std::cerr << "Error: Histogram at index " << i << " has zero integral!" << std::endl;
            return;
        }
        histos[i]->Sumw2();
        histos[i]->Scale(1 / histos[i]->Integral());
    }
    TCanvas* c = new TCanvas("canvas", "canvas", 600, 600);

    for(int i = 0; i<histos.size(); i++){
        histos[i]->Sumw2();
        histos[i]->Scale(1/histos[i]->Integral());

        }

    double max = 0;
    for(int i = 0; i<histos.size(); i++){
        if(histos[i]->GetMaximum()>max) max = histos[i]->GetMaximum();

        }

    vector<int> colors = {1,633,875,419};
    vector<int> styles = {20,21,22,23};


    double leg_x1 = 0.6;
    double leg_x2 = 0.8;

    double leg_y1 = 0.93;
    double leg_y2 = 0.75;


    TLegend *leg = new TLegend(leg_x1,leg_y1,leg_x2,leg_y2,NULL,"brNDC");

    gStyle->SetOptTitle(1); // Enable titles
    gStyle->SetTitleFontSize(0.05); // Adjust title font size
    gStyle->SetTitleAlign(23); // Center align the title
    gStyle->SetTitleX(0.53); // Set title position (X-axis)
    gStyle->SetTitleY(0.995); // Set title position (Y-axis)

    leg->SetBorderSize(0);
    leg->SetTextSize(0.042);
    leg->SetTextFont(42);
    leg->SetFillStyle(1000);

    for(int i = 0; i<histos.size(); i++){
        histos[i]->SetMarkerSize(1);
        histos[i]->SetMarkerStyle(styles[i]);
        histos[i]->SetMarkerColor(colors[i]);
        histos[i]->GetYaxis()->SetTitleOffset(1.5);
        histos[i]->GetXaxis()->SetTitleOffset(1.1);
        histos[i]->GetYaxis()->SetLabelSize(0.045);
        histos[i]->GetYaxis()->SetTitleSize(0.045);
        histos[i]->GetXaxis()->SetLabelSize(0.045);
        histos[i]->GetXaxis()->SetTitleSize(0.045);
        histos[i]->GetYaxis()->SetTitle("A.U.");

        //histos[i]->GetXaxis()-> SetNdivisions(503);
        histos[i]->SetLineWidth(1);
        histos[i]->SetLineColor(colors[i]);
        histos[i]->SetLineStyle(1);
        // Dynamically set the x-axis title based on the histogram name or title
        TString histoName = histos[i]->GetName(); // Get the histogram's name

        if (histoName.Contains("Acoplanarity")) {
            histos[i]->GetXaxis()->SetTitle("#varphi*_{CP} [rad]"); // signed acoplanarity
            histos[i]->SetTitle("Signed Acoplanarity for CP-even and background (#pi Decay Product)");
            histos[i]->SetMinimum(0.);
            histos[0]->SetMaximum(1.4*max);
        } else if (histoName.Contains("Phi")) {
            histos[i]->GetXaxis()->SetTitle("#phi [rad]"); // azimuthal angle
            histos[i]->SetTitle("Phi for CP-even and background (#pi Decay Produt)");
            histos[i]->SetMinimum(0.0535);
            histos[0]->SetMaximum(1.01*max);
        } else if (histoName.Contains("Eta")) {
            histos[i]->GetXaxis()->SetTitle("#eta"); // Pseudorapidity
            histos[i]->SetTitle("Eta for CP-even and background (#pi Decay Product)");
            histos[i]->SetMinimum(0.);
            histos[0]->SetMaximum(1.4*max);
        } else if (histoName.Contains("Pt")) {
            histos[i]->GetXaxis()->SetTitle("p_{T} [GeV]"); // Transverse momentum
            histos[i]->SetTitle("p_{T} for CP-even and background (#pi Decay Product)");
            histos[i]->SetMinimum(0.);
            histos[0]->SetMaximum(1.03*max);
        } else if (histoName.Contains("DiTau")) {
            histos[i]->GetXaxis()->SetTitle("m_{#tau#tau}^{vis} [GeV]"); // Visible mass of the di-tau system
            histos[i]->SetTitle("Visible Mass of the Di-tau System for CP-even and background (#pi Decay Product)");
            histos[i]->SetMinimum(0.);
            histos[0]->SetMaximum(1.4*max);
        } else if (histoName.Contains("_Mass")) {
            double integral = histos[i]->Integral(); // Calculate the integral of the histogram
            if (integral > 0) { // Avoid division by zero
                histos[i]->Scale(1.0 / integral); // Normalise the histogram
            }
            histos[i]->GetXaxis()->SetTitle("Truth Mass [GeV]");
            histos[i]->SetTitle("Truth Mass of Higgs and background Z boson (#pi Decay Product)");
            histos[i]->SetMinimum(0.);
            histos[0]->SetMaximum(1.4*max);
            histos[i]->Draw("PE L");
        }
    

    }
    for(int i = 0; i<histos.size(); i++){
        TString histoName = histos[i]->GetName();
        if (histoName.Contains("_Mass")) {
            if (i == 0) histos[i]->Draw("HIST E");
            else histos[i]->Draw("same HIST E");
        } else {
            if(i==0) histos[i]->Draw("PE");
            else histos[i]->Draw("same PE");
        }
        leg->AddEntry(histos[i], legend_labels[i],"lp");
    }


    gPad->RedrawAxis();


    TLatex t;
    t.SetNDC();
    t.SetTextColor(1);
    t.SetTextFont(42);
    t.SetTextSize(0.045);



    leg->Draw("same");
    //c->SetLogy();                                                                                                                                                                                                    \
                                                                                                                                                                                                                        
    c->SaveAs(file_name);

}


void compare_background(TString inFileName, TString inBackgroundFileName){

        rootlogon();
        TFile* InFile = TFile::Open(inFileName);

        if(!InFile)
        {
            std::cout << "Unable to open input file" << std::endl;
            return;
            
        }   

        TFile* inBackgroundFile = TFile::Open(inBackgroundFileName);
        if(!inBackgroundFile)
        {
            std::cout << "Unable to open background input file" << std::endl;
            return;
            
        }

        // retrieve CP-even histograms
        TH1D * h_SignedAcoplanarity_CPeven = dynamic_cast<TH1D*>(InFile->Get("h_SignedAcoplanarity_IP_CPeven"));
        TH1D * h_Phi_CPeven = dynamic_cast<TH1D*>(InFile->Get("h_Phi_CPeven"));
        TH1D * h_Eta_CPeven = dynamic_cast<TH1D*>(InFile->Get("h_Eta_CPeven"));
        TH1D * h_Pt_CPeven = dynamic_cast<TH1D*>(InFile->Get("h_Pt_CPeven"));
        TH1D * h_DiTau_VisMass_CPeven = dynamic_cast<TH1D*>(InFile->Get("h_DiTau_VisMass_CPeven"));
        TH1D * h_Higgs_Mass_CPeven = dynamic_cast<TH1D*>(InFile->Get("h_Higgs_Mass_CPeven"));


        // retrieve background histograms
        TH1D * h_SignedAcoplanarity_Bkg = dynamic_cast<TH1D*>(inBackgroundFile->Get("h_SignedAcoplanarity_IP_Bkg"));
        TH1D * h_Phi_Bkg = dynamic_cast<TH1D*>(inBackgroundFile->Get("h_Phi_Bkg"));
        TH1D * h_Eta_Bkg = dynamic_cast<TH1D*>(inBackgroundFile->Get("h_Eta_Bkg"));
        TH1D * h_Pt_Bkg = dynamic_cast<TH1D*>(inBackgroundFile->Get("h_Pt_Bkg"));
        TH1D * h_DiTau_VisMass_Bkg = dynamic_cast<TH1D*>(inBackgroundFile->Get("h_DiTau_VisMass_Bkg"));
        TH1D * h_Z_Mass = dynamic_cast<TH1D*>(inBackgroundFile->Get("h_Z_Mass"));

 
        std::vector<TString> labels = {"H #rightarrow #tau#tau CP-even", "Z #rightarrow #tau#tau background "};

        //compare signed acoplanarity
        std::vector<TH1D*> histos_HvsZ_SignedAcoplanarity;
        histos_HvsZ_SignedAcoplanarity.push_back(h_SignedAcoplanarity_CPeven);
        histos_HvsZ_SignedAcoplanarity.push_back(h_SignedAcoplanarity_Bkg);

        compare_histos(histos_HvsZ_SignedAcoplanarity, labels, "background_comparison_SignedAcoplanarity.pdf");

        // compare phi
        std::vector<TH1D*> histos_HvsZ_Phi;
        histos_HvsZ_Phi.push_back(h_Phi_CPeven);
        histos_HvsZ_Phi.push_back(h_Phi_Bkg);

        compare_histos(histos_HvsZ_Phi, labels, "background_comparison_Phi.pdf");

        // compare eta
        std::vector<TH1D*> histos_HvsZ_Eta;
        histos_HvsZ_Eta.push_back(h_Eta_CPeven);
        histos_HvsZ_Eta.push_back(h_Eta_Bkg);

        compare_histos(histos_HvsZ_Eta, labels, "background_comparison_Eta.pdf");

        // compare pt
        std::vector<TH1D*> histos_HvsZ_Pt;
        histos_HvsZ_Pt.push_back(h_Pt_CPeven);
        histos_HvsZ_Pt.push_back(h_Pt_Bkg);

        compare_histos(histos_HvsZ_Pt, labels, "background_comparison_Pt.pdf");

        // compare visible mass of the di-tau system
        std::vector<TH1D*> histos_HvsZ_DiTauVisMass;
        histos_HvsZ_DiTauVisMass.push_back(h_DiTau_VisMass_CPeven);
        histos_HvsZ_DiTauVisMass.push_back(h_DiTau_VisMass_Bkg);

        compare_histos(histos_HvsZ_DiTauVisMass, labels, "background_comparison_DiTauVisMass.pdf");

        // compare truth mass of Higgs and Z boson
        std::vector<TH1D*> histos_HvsZ_TruthMass;
        histos_HvsZ_TruthMass.push_back(h_Higgs_Mass_CPeven);
        histos_HvsZ_TruthMass.push_back(h_Z_Mass);

        compare_histos(histos_HvsZ_TruthMass, labels, "background_comparison_TruthMass.pdf");
    }


