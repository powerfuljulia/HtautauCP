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

  histos[0]->SetMaximum(1.4*max);
  vector<int> colors = {1,633,875,419};
  vector<int> styles = {20,21,22,23};


  double leg_x1 = 0.6;
  double leg_x2 = 0.8;

  double leg_y1 = 0.93;
  double leg_y2 = 0.75;


  TLegend *leg = new TLegend(leg_x1,leg_y1,leg_x2,leg_y2,NULL,"brNDC");

  leg->SetBorderSize(0);
  leg->SetTextSize(0.042);
  leg->SetTextFont(42);
  leg->SetFillStyle(1000);
  for(int i = 0; i<histos.size(); i++){
    histos[i]->SetMinimum(0.);
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
    histos[i]->GetXaxis()->SetTitle("#varphi*_{CP} [rad]");
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

    std::vector<TString> labels = {"Scalar", "PseudoScalar", "CP-mix (#phi_{#tau}=45^{#circ})", "CP-mix (#phi_{#tau}=-45^{#circ})"};

    std::vector<TH1D*> histos_CPhypos;
    histos_CPhypos.push_back(h_SignedAcoplanarity_CPeven);
    histos_CPhypos.push_back(h_SignedAcoplanarity_CPodd);
    histos_CPhypos.push_back(h_SignedAcoplanarity_CPmix);
    histos_CPhypos.push_back(h_SignedAcoplanarity_CPmixneg);

    compare_histos(histos_CPhypos, labels, "comparison_SignedAcoplanarity.pdf", "H #rightarrow #tau#tau #rightarrow #pi#pi#nu#nu");
}