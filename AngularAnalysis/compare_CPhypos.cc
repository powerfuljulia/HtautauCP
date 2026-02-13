void compare_histos(std::vector<TH1D*> histos, std::vector<TString> legend_labels, TString file_name, TString process){

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
  gStyle->SetTitleX(0.5); // Set title position (X-axis)
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
    // Dynamically set the x-axis title based on the histogram name or title
    TString histoName = histos[i]->GetName(); // Get the histogram's name

    if (histoName.Contains("Acoplanarity")) {
      histos[i]->GetXaxis()->SetTitle("#varphi*_{CP} [rad]"); // signed acoplanarity
      histos[i]->SetTitle("Signed Acoplanarity by CP Hypothesis (#pi Decay Mode)");
      histos[i]->SetMinimum(0.);
      histos[0]->SetMaximum(1.4*max);
    } else if (histoName.Contains("Phi")) {
      histos[i]->GetXaxis()->SetTitle("#phi [rad]"); // azimuthal angle
      histos[i]->SetTitle("Phi by CP Hypothesis (#pi Decay Mode)");
      histos[i]->SetMinimum(0.0545);
      histos[0]->SetMaximum(1.01*max);
    } else if (histoName.Contains("Eta")) {
      histos[i]->GetXaxis()->SetTitle("#eta"); // Pseudorapidity
      histos[i]->SetTitle("Eta by CP Hypothesis (#pi Decay Mode)");
      histos[i]->SetMinimum(0.);
      histos[0]->SetMaximum(1.4*max);
    } else if (histoName.Contains("Pt")) {
      histos[i]->GetXaxis()->SetTitle("p_{T} [GeV]"); // Transverse momentum
      histos[i]->SetTitle("p_{T} by CP Hypothesis (#pi Decay Mode)");
      histos[i]->SetMinimum(0.);
      histos[0]->SetMaximum(1.4*max);
    }
   

    //histos[i]->GetXaxis()-> SetNdivisions(503);
    histos[i]->SetLineWidth(1);
    histos[i]->SetLineColor(colors[i]);
    histos[i]->SetLineStyle(1);
  }
  for(int i = 0; i<histos.size(); i++){
    if(i==0) histos[i]->Draw("PE");
    else histos[i]->Draw("same PE");
    leg->AddEntry(histos[i], legend_labels[i],"lp");
  }


  gPad->RedrawAxis();


  TLatex t;
  t.SetNDC();
  t.SetTextColor(1);
  t.SetTextFont(42);
  t.SetTextSize(0.045);
  t.DrawLatex(0.17,.87,process);


  leg->Draw("same");
  //c->SetLogy();                                                                                                                                                                                                    \
                                                                                                                                                                                                                      
  c->SaveAs(file_name);

}


void compare_CPhypos(TString inFileName){

    rootlogon();
    TFile* InFile = TFile::Open(inFileName);

    if(!InFile)
    {
      std::cout << "Unable to open input file" << std::endl;
      return;
      
    }   

    TH1D * h_SignedAcoplanarity_CPeven = dynamic_cast<TH1D*>(InFile->Get("h_SignedAcoplanarity_IP_CPeven"));
    TH1D * h_SignedAcoplanarity_CPodd = dynamic_cast<TH1D*>(InFile->Get("h_SignedAcoplanarity_IP_CPodd"));
    TH1D * h_SignedAcoplanarity_CPmix = dynamic_cast<TH1D*>(InFile->Get("h_SignedAcoplanarity_IP_CPmix_phi45"));
    TH1D * h_SignedAcoplanarity_CPmixneg = dynamic_cast<TH1D*>(InFile->Get("h_SignedAcoplanarity_IP_CPmix_phi-45"));

    TH1D * h_Phi_CPeven = dynamic_cast<TH1D*>(InFile->Get("h_Phi_CPeven"));
    TH1D * h_Phi_CPodd = dynamic_cast<TH1D*>(InFile->Get("h_Phi_CPodd"));
    TH1D * h_Phi_CPmix = dynamic_cast<TH1D*>(InFile->Get("h_Phi_CPmix_phi45"));
    TH1D * h_Phi_CPmixneg = dynamic_cast<TH1D*>(InFile->Get("h_Phi_CPmix_phi-45"));

    TH1D * h_Eta_CPeven = dynamic_cast<TH1D*>(InFile->Get("h_Eta_CPeven"));
    TH1D * h_Eta_CPodd = dynamic_cast<TH1D*>(InFile->Get("h_Eta_CPodd"));
    TH1D * h_Eta_CPmix = dynamic_cast<TH1D*>(InFile->Get("h_Eta_CPmix_phi45"));
    TH1D * h_Eta_CPmixneg = dynamic_cast<TH1D*>(InFile->Get("h_Eta_CPmix_phi-45"));

    TH1D * h_Pt_CPeven = dynamic_cast<TH1D*>(InFile->Get("h_Pt_CPeven"));
    TH1D * h_Pt_CPodd = dynamic_cast<TH1D*>(InFile->Get("h_Pt_CPodd"));
    TH1D * h_Pt_CPmix = dynamic_cast<TH1D*>(InFile->Get("h_Pt_CPmix_phi45"));
    TH1D * h_Pt_CPmixneg = dynamic_cast<TH1D*>(InFile->Get("h_Pt_CPmix_phi-45"));

    std::vector<TString> labels = {"Scalar", "PseudoScalar", "CP-mix (#phi_{#tau}=45^{#circ})", "CP-mix (#phi_{#tau}=-45^{#circ})"};

    //compare signed acoplanarity
    std::vector<TH1D*> histos_CPhypos_SignedAcoplanarity;
    histos_CPhypos_SignedAcoplanarity.push_back(h_SignedAcoplanarity_CPeven);
    histos_CPhypos_SignedAcoplanarity.push_back(h_SignedAcoplanarity_CPodd);
    histos_CPhypos_SignedAcoplanarity.push_back(h_SignedAcoplanarity_CPmix);
    histos_CPhypos_SignedAcoplanarity.push_back(h_SignedAcoplanarity_CPmixneg);

    compare_histos(histos_CPhypos_SignedAcoplanarity, labels, "comparison_SignedAcoplanarity.pdf", "H #rightarrow #tau#tau #rightarrow #pi#pi#nu#nu");

    // compare Phi
    std::vector<TH1D*> histos_CPhypos_Phi;
    histos_CPhypos_Phi.push_back(h_Phi_CPeven);
    histos_CPhypos_Phi.push_back(h_Phi_CPodd);
    histos_CPhypos_Phi.push_back(h_Phi_CPmix);
    histos_CPhypos_Phi.push_back(h_Phi_CPmixneg);

    compare_histos(histos_CPhypos_Phi, labels, "comparison_Phi.pdf", "H #rightarrow #tau#tau #rightarrow #pi#pi#nu#nu");

    // compare Eta
    std::vector<TH1D*> histos_CPhypos_Eta;
    histos_CPhypos_Eta.push_back(h_Eta_CPeven);
    histos_CPhypos_Eta.push_back(h_Eta_CPodd);
    histos_CPhypos_Eta.push_back(h_Eta_CPmix);
    histos_CPhypos_Eta.push_back(h_Eta_CPmixneg);

    compare_histos(histos_CPhypos_Eta, labels, "comparison_Eta.pdf", "H #rightarrow #tau#tau #rightarrow #pi#pi#nu#nu");

    // compare Pt
    std::vector<TH1D*> histos_CPhypos_Pt;
    histos_CPhypos_Pt.push_back(h_Pt_CPeven);
    histos_CPhypos_Pt.push_back(h_Pt_CPodd);
    histos_CPhypos_Pt.push_back(h_Pt_CPmix);
    histos_CPhypos_Pt.push_back(h_Pt_CPmixneg);

    compare_histos(histos_CPhypos_Pt, labels, "comparison_Pt.pdf", "H #rightarrow #tau#tau #rightarrow #pi#pi#nu#nu");
  }