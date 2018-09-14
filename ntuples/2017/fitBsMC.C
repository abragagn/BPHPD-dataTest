float counting(TF1 *fit);
void fitPeak(TH1 *hist, TString name);

void fitBsMC(TString fileName = "ntuBsJpsiPhi.root"){

	gErrorIgnoreLevel = kFatal;
	gStyle->SetOptStat(0);

	TFile *f = new TFile(fileName);
	TString sample = fileName;
	int nameLength = sample.Length();
	sample.Remove(nameLength-5,5);
	sample.Remove(0,3);

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


  cuts.push_back("");
  names.push_back("Inclusive");

	for(int i=0; i<cuts.size(); ++i){

		TH1 *histMass = new TH1F("histMass","",100,5.0,6.0);
		TH1 *histCt = new TH1F("histCt","",100,0.0,0.5);
		TString cut = cuts[i];
		TString name = sample + "/" + "bsMass_" + names[i] + "_2017" + sample;
		TString nameCt = sample + "/" + "bsCt_" + names[i] + "_2017" + sample;
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

	float mean = 5.366;
	float sigma = 0.015;

	TF1 *func = new TF1("func","[0]*exp(-0.5*((x-[1])/[2])**2)+[3]*exp(-0.5*((x-[1])/[4])**2)+[5]*exp([6]*x)",5.0, 6.0);

	func->SetParameter(0, 1);
	func->SetParameter(3, 1);

	func->SetParameter(1, mean);
	func->SetParameter(2, sigma);
	func->SetParameter(4, sigma);

	func->SetParameter(6, -1);

	func->SetParLimits(0, 0, 1e6);
	func->SetParLimits(3, 0, 1e6);

	func->SetParLimits(1, mean-sigma, mean+sigma);
	func->SetParLimits(2, sigma/2, sigma*2);
	func->SetParLimits(4, sigma/2, sigma*2);

	func->SetParLimits(5, 0, 1e9);
	func->SetParLimits(6, -1e2, 0);

	TCanvas c1;
	hist->Draw("HIST");
	hist->Fit("func","MRLQ");
	hist->SetMinimum(0);

	TF1 *fit = hist->GetFunction("func");
	fit->Draw("SAME");

	float nEvt = counting(hist->GetFunction("func"))/hist->GetBinWidth(0);
	TString nEvt_;
	nEvt_.Form("# of Bs = %.0f", nEvt);

	cout<<nEvt<<" ";

	auto legend = new TLegend(0.9,0.7,0.6,0.9);
	legend->SetHeader(nEvt_,"C");
	legend->SetTextSize(0.05);
	legend->Draw();

	c1.Print(name + ".png");

};

float counting(TF1 *fit){
	float a1 = fit->GetParameter(0);
	float a2 = fit->GetParameter(3);
	float sigma1 = fit->GetParameter(2);
	float sigma2 = fit->GetParameter(4);
	return TMath::Sqrt(2*TMath::Pi())*(a1*sigma1+a2*sigma2);
};

