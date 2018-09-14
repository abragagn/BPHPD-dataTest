float counting(TF1 *fit);
void fitPeak(TH1 *hist, TString name);
float min_ = 5.1;
float max_ = 5.6;

void fitBs(TString fileName = "ntu2018B.root"){

    gErrorIgnoreLevel = kFatal;
    gStyle->SetOptStat(0);

    TFile *f = new TFile(fileName);
    TString run = fileName(3,5);
    TTree *t = (TTree*)f->Get("PDsecondTree");

    vector<pair<TString, TString>> cuts;

    cuts.clear();
    
    cuts.push_back(std::make_pair("(hltFired & 2)","JPsiMuon"));
    cuts.push_back(std::make_pair("(hltFired & 4)&&!(hltFired & 2)","JPsiTrkTrk"));
    cuts.push_back(std::make_pair("(hltFired & 8)&&!(hltFired & 2)&&!(hltFired & 4)","JPsiTrk"));
    
    cuts.push_back(std::make_pair("(hltFired & 8)&&!(hltFired & 2)","JPsiTrkInverted");
    cuts.push_back(std::make_pair("(hltFired & 4)&&!(hltFired & 2)&&!(hltFired & 8)","JPsiTrkTrkInverted");
                   
    cuts.push_back(std::make_pair("(hltFired & 4)","JPsiTrkTrkNoVeto");
    cuts.push_back(std::make_pair("(hltFired & 8)","JPsiTrkNoVeto");

    for(int i=0; i<cuts.size(); ++i){

        TH1 *histMass = new TH1F("histMass","",250,min_,max_);
        TH1 *histCt = new TH1F("histCt","",100,0.0,0.5);
        TString cut = cuts[i].first;
        TString name = "ch" + run + "/" + "bsMass_" + cuts[i].second + "_" + run;
        TString nameCt = "ch" + run + "/" + "bsCt_" + cuts[i].second + "_" + run;
        TCanvas c1;

        t->Project("histMass", "bsMass", cut);
        t->Project("histCt", "bsCt", cut);
        fitPeak(histMass, name); 
        histCt->Draw("HIST");
        c1.Print(nameCt + ".png");
        histMass->Reset("ICESM");
        histCt->Reset("ICESM");

        cut += "&&isTight==1";
        name += "_tight";
        nameCt += "_tight";
        t->Project("histMass", "bsMass", cut);
        t->Project("histCt", "bsCt", cut);
        fitPeak(histMass, name);
        histCt->Draw("HIST");
        c1.Print(nameCt + ".png");
        histMass->Reset("ICESM");
        histCt->Reset("ICESM");

        delete histMass;
        delete histCt;

        cout<<endl;

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

    func->SetParameter(7, hist->GetBinContent(250-1));
    func->SetParameter(8, 1);
    func->SetParameter(9, 20);
    func->SetParameter(10, 5.10);

    func->SetParLimits(0, mean-sigma, mean+sigma);

    func->SetParLimits(1, 0, hist->GetEntries());
    func->SetParLimits(2, 0, hist->GetEntries());
    func->SetParLimits(3, 0, hist->GetEntries());

    func->SetParLimits(4, 0, sigma*2);
    func->SetParLimits(5, 0, sigma*2);
    func->SetParLimits(6, 0, sigma*2);

    func->SetParLimits(7, 0, hist->GetBinContent(250-1)*1.5);
    func->SetParLimits(8, 0, hist->GetBinContent(250-1));
    func->SetParLimits(9, 10, 1e3);
    func->SetParLimits(10, min_, mean);

    TCanvas c1;
    hist->Draw("HIST");
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

    cout<<nEvt<<" ";

    auto legend = new TLegend(0.1,0.7,0.4,0.9);
    legend->SetHeader(nEvt_,"C");
    legend->SetTextSize(0.05);
    legend->Draw();

    c1.Print(name + ".png");

};

