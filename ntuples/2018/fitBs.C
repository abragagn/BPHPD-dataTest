float counting(TF1 *fit);
void fitPeak(TH1 *hist, TString name);

void fitBs(TString fileName = "ntu2018B.root"){

    gErrorIgnoreLevel = kFatal;
    gStyle->SetOptStat(0);

    TFile *f = new TFile(fileName);
    TString run = fileName(3,5);
    TTree *t = (TTree*)f->Get("PDsecondTree");

    vector<TString> cuts;
    vector<TString> names;
      cuts.clear(); 
      names.clear();

      cuts.push_back("(hltFired & 2)");
      names.push_back("JPsiMuon");

      cuts.push_back("(hltFired & 4)&&!(hltFired & 2)");
      names.push_back("JPsiTrkTrk");

      cuts.push_back("(hltFired & 8)&&!(hltFired & 2)&&!(hltFired & 4)");
      names.push_back("JPsiTrk");

      cuts.push_back("(hltFired & 8)&&!(hltFired & 2)");
      names.push_back("JPsiTrkInverted");

      cuts.push_back("(hltFired & 4)&&!(hltFired & 2)&&!(hltFired & 8)");
      names.push_back("JPsiTrkTrkInverted");

    for(int i=0; i<cuts.size(); ++i){

        TH1 *histMass = new TH1F("histMass","",250,5.0,5.5);
        TH1 *histCt = new TH1F("histCt","",100,0.0,0.5);
        TString cut = cuts[i];
        TString name = "ch" + run + "/" + "bsMass_" + names[i] + "_2018" + run;
        TString nameCt = "ch" + run + "/" + "bsCt_" + names[i] + "_2018" + run;
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

    TF1 *func = new TF1("func", funcDef, 5.0, 5.5);

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

    func->SetParLimits(4, 0, sigma*3);
    func->SetParLimits(5, 0, sigma*3);
    func->SetParLimits(6, 0, sigma*3);

    func->SetParLimits(7, 0, hist->GetBinContent(250-1)*1.5);
    func->SetParLimits(8, 0, hist->GetBinContent(250-1));
    func->SetParLimits(9, 10, 1e3);
    func->SetParLimits(10, 5.0, mean);

    TCanvas c1;
    hist->Draw("HIST");
    hist->Fit("func","MRLQ");
    hist->SetMinimum(0);

    TF1 *fit = hist->GetFunction("func");
    fit->Draw("SAME");

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

