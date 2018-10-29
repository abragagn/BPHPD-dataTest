float counting(TF1 *fit);
void fitPeak(TH1 *hist, TString name);
float min_ = 5.2;
float max_ = 5.5;
int nBins_ = 50;

void fitBsMc17(TString fileName = "ntuMC2017.root"){

    gErrorIgnoreLevel = kFatal;
    //gStyle->SetOptStat(0);

    TFile *f = new TFile(fileName);
    TTree *t = (TTree*)f->Get("PDsecondTree");

    vector<pair<TString, TString>> cuts;

    cuts.clear();

    cuts.push_back(std::make_pair("hltJpsiMu&&isTight","JpsiMu_tight"));
    cuts.push_back(std::make_pair("hltJpsiTrkTrk&&!hltJpsiMu&&isTight&&bsCt2DSigmaUnit>3.","JpsiTrkTrk_tight_ct3p0s"));
    cuts.push_back(std::make_pair("hltJpsiTrk&&!hltJpsiMu&&!hltJpsiTrkTrk&&isTight&&bsCt2DSigmaUnit>3.","JpsiTrk_tight_ct3p0s"));
    
    for(int i=0; i<cuts.size(); ++i){

        TH1 *histMass = new TH1F("histMass","",nBins_,min_,max_);
        TH1 *histCt = new TH1F("histCt","",100,0.0,0.25);
        TString cut = cuts[i].first + "&&1"; 
        TString name = "mcBsJpsiPhi/bsMass_" + cuts[i].second + "_" + "mc2017";
        TString nameCt = "mcBsJpsiPhi/bsCt_" + cuts[i].second + "_" + "mc2017";
        TCanvas c1;

        t->Project("histMass", "bsMass", cut);
        t->Project("histCt", "bsCt2D", cut);
        fitPeak(histMass, name);

        //histMass->Draw("HIST");
        //c1.Print(name + ".png");
        histCt->Draw("HIST");
        c1.Print(nameCt + ".png");
        histMass->Reset("ICESM");
        histCt->Reset("ICESM");

/*
        cut += "&&isTight";
        name += "_tight";
        nameCt += "_tight";
        t->Project("histMass", "bsMass", cut);
        t->Project("histCt", "bsCt2D", cut);
        fitPeak(histMass, name);
        //histMass->Draw("HIST");
        //c1.Print(name + ".png");
        histCt->Draw("HIST");
        c1.Print(nameCt + ".png");
*/
        histMass->Reset("ICESM");
        histCt->Reset("ICESM");

        delete histMass;
        delete histCt;

        cout<<" --- "<<cuts[i].first<<endl;

    }

};

void fitPeak(TH1 *hist, TString name){

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

    float nEvt = fit->GetParameter(1);
    nEvt += fit->GetParameter(2);
    nEvt += fit->GetParameter(3);
    nEvt/=hist->GetBinWidth(0);

    TString nEvt_;
    nEvt_.Form("# of Bs = %.0f", nEvt);

    cout<<"# "<<nEvt;

    auto legend = new TLegend(0.1,0.8,0.3,0.9);
    legend->SetHeader(nEvt_,"C");
    legend->SetTextSize(0.03);
    legend->Draw();

    c1.Print(name + ".png");

};

