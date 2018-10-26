float min_ = 5.2;
float max_ = 5.5;
int nBins_ = 100;
void fitPeak(TH1 *hist);

void cutFlow(TString fileName = "ntu2018Dv2.root". TCut cutEvt = "")
{
    TFile *f = new TFile(fileName);
    TTree *t = (TTree*)f->Get("PDsecondTree");

    TH1 *hNoCut     = new TH1F("hNoCut",    "", nBins_, min_, max_);
    TH1 *hJpsiCut   = new TH1F("hJpsiCut",  "", nBins_, min_, max_);
    TH1 *hPhiCut    = new TH1F("hPhiCut",   "", nBins_, min_, max_);
    TH1 *hBsCut     = new TH1F("hBsCut",    "", nBins_, min_, max_);
    TH1 *hCtCut     = new TH1F("hCtCut",    "", nBins_, min_, max_);


    TCut cNoCut = cutEvt;

    TCut cJpsiCut = cutEvt;
    cJpsiCut += "fabs(jpsiMass - 3.097) < 0.15";
    cJpsiCut += "jpsiPt>7.0";
    cJpsiCut += "muPlus_Pt > 4.0 && muMinus_Pt > 4.0";
    cJpsiCut += "fabs(muPlus_Eta) < 2.2 && fabs(muMinus_Eta) < 2.2";

    TCut cPhiCut = cutEvt;
    cPhiCut += "fabs(phiMass - 1.019) < 0.01";
    cPhiCut += "kaonPlus_Pt > 0.7 && kaonMinus_Pt > 0.7";
    cPhiCut += "fabs(kaonPlus_Eta) < 2.5 && fabs(kaonMinus_Eta) < 2.5";
    cPhiCut += "kaonPlus_Hits > 4 && kaonMinus_Hits > 4";

    TCut cBsCut = cutEvt;
    cBsCut += "bsMass > 5.2 && bsMass < 5.65";
    cBsCut += "bsPt > 8.0";
    cBsCut += "fabs(bsEta) < 2.4";

    TCut cCtCut = cutEvt;
    cCtCut += "bsCt2DSigmaUnit > 3.0";

    t->Project("hNoCut",    "bsMass", TString( cNoCut.GetTitle()    ));
    t->Project("hJpsiCut",  "bsMass", TString( cJpsiCut.GetTitle()  ));
    t->Project("hPhiCut",   "bsMass", TString( cPhiCut.GetTitle()   ));
    t->Project("hBsCut",    "bsMass", TString( cBsCut.GetTitle()    ));
    t->Project("hCtCut",    "bsMass", TString( cCtCut.GetTitle()    ));

    cout<<setw(5)<<left<< "nSgn" <<setw(5)<<left<< "nBkg" <<setw(5)<<left<< "S/(S+N)" <<endl;
    cout<<"---------------"<<endl;

    fitPeak(hNoCut);
    fitPeak(hJpsiCut);
    fitPeak(hPhiCut);
    fitPeak(hBsCut);
    fitPeak(hCtCut);

    return;

}

void fitPeak(TH1 *hist){

    ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls( 10000 );

    float mean = 5.366;
    float sigma = 0.015;

    TString sgnDef = "[1]*TMath::Gaus(x, [0], [4], true)";
    sgnDef +=       "+[2]*TMath::Gaus(x, [0], [5], true)";
    sgnDef +=       "+[3]*TMath::Gaus(x, [0], [6], true)";
    TString bkgDef = "[7]+[8]*TMath::Erfc([9]*(x-[10]))";
    TString funcDef = sgnDef + "+" + bkgDef;

    TF1 *func = new TF1("func", funcDef, min_, max_);

    func->SetParameter(0, mean);

    func->SetParameter(1, 1);
    func->SetParameter(2, 1);
    func->SetParameter(3, 1);

    func->SetParameter(4, sigma);
    func->SetParameter(5, sigma);
    func->SetParameter(6, sigma);

    func->SetParameter(7, hist->GetBinContent(nBins_-1));
    func->SetParameter(8, 1);
    func->SetParameter(9, 20);
    func->SetParameter(10, 5.25);

    func->SetParLimits(0, mean-sigma, mean+sigma);

    func->SetParLimits(1, 0, hist->GetEntries());
    func->SetParLimits(2, 0, hist->GetEntries());
    func->SetParLimits(3, 0, hist->GetEntries());

    func->SetParLimits(4, 0, sigma*2);
    func->SetParLimits(5, 0, sigma*2);
    func->SetParLimits(6, 0, sigma*2);

    func->SetParLimits(7, 0, hist->GetBinContent(nBins_-1)*1.5);
    func->SetParLimits(8, 0, hist->GetBinContent(nBins_-1));
    func->SetParLimits(9, 10, 1e3);
    func->SetParLimits(10, min_, mean);

    hist->SetMarkerStyle(20);
    hist->SetMarkerSize(.75);

    TCanvas c1;
    hist->Draw("PE");
    hist->Fit("func","MRLQ");
    hist->SetMinimum(0);

    TF1 *fit = hist->GetFunction("func");
    fit->Draw("same");
    
    TF1 *f1 = new TF1("f1","[0]*TMath::Gaus(x, [1], [2], true)", min_, max_);
    TF1 *f2 = new TF1("f2","[0]*TMath::Gaus(x, [1], [2], true)", min_, max_);
    TF1 *f3 = new TF1("f3","[0]*TMath::Gaus(x, [1], [2], true)", min_, max_);
    TF1 *f4 = new TF1("f4","[0]", min_, max_);
    TF1 *f5 = new TF1("f5","[0]*TMath::Erfc([1]*(x-[2]))", min_, max_);

    f1->SetParameters(fit->GetParameter(1),fit->GetParameter(0),fit->GetParameter(4));
    f2->SetParameters(fit->GetParameter(2),fit->GetParameter(0),fit->GetParameter(5));
    f3->SetParameters(fit->GetParameter(3),fit->GetParameter(0),fit->GetParameter(6));
    f4->SetParameter(0, fit->GetParameter(7));
    f5->SetParameters(fit->GetParameter(8),fit->GetParameter(9),fit->GetParameter(10));

    f1->SetLineColor(kBlue);
    f2->SetLineColor(kViolet);
    f3->SetLineColor(kAzure);
    f4->SetLineColor(kOrange);
    f5->SetLineColor(kGreen);
    f1->SetLineStyle(2);
    f2->SetLineStyle(2);
    f3->SetLineStyle(2);
    f4->SetLineStyle(2);
    f5->SetLineStyle(2);

    f1->Draw("same");
    f2->Draw("same");
    f3->Draw("same");
    f4->Draw("same");
    f5->Draw("same");

    float a = 5.32;
    float b = 5.42;

    float nSgn = 0.;
    nSgn += f1->Integral(a, b);
    nSgn += f2->Integral(a, b);
    nSgn += f3->Integral(a, b);

    float nBkg = 0.;
    nBkg += f4->Integral(a, b);
    nBkg += f5->Integral(a, b);

    cout<<setw(5)<<left<< nSgn <<setw(5)<<left<< nBkg <<setw(5)<<left<< nSgn/(nSgn+nBkg)<<endl;

    return;

};