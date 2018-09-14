#define nan std::numeric_limits<double>::quiet_NaN()

const float MassBs = 5.366;
const float StdDev = 0.015;
const float MassRange[2] = {5.1, 5.7};
const int NbinsMass = 100;
const float Dummy = 1.0;
const float NSigmaSignal = 3.0;
const float NSigmaSide = NSigmaSignal*2;
const float OffsetSide = 1.0;

void sideBandSub(TString fileName = "ntu2017C.root", TString varName = "bsPt", 
									int Nbins = 100, float xmin = nan, float xmax = nan,
									TString generalCuts = "(hltFired & 4)&&!(hltFired & 2)&&bsPt>8" )
{
	gStyle->SetOptStat(0);
	TH1::SetDefaultSumw2(kTRUE);

	TFile *f 	= new TFile(fileName);
	TTree *t = (TTree*)f->Get("PDsecondTree");
	TCanvas c1;

	float varRange[2];
	if( isnan(xmin) || isnan(xmax) ){
		t->Draw(varName + " >> htemp ", generalCuts, "Q");
		TH1 *hPark = (TH1*)gDirectory->Get("htemp");
		hPark->Draw();
		varRange[0] = (float)hPark->GetXaxis()->GetXmin(); 
		varRange[1] = (float)hPark->GetXaxis()->GetXmax();
	}else{
		varRange[0] = xmin;
		varRange[1] = xmax;
	}

	TH1 *hMass = new TH1F("hMass", "hMass", NbinsMass, MassRange[0], MassRange[1]);
	TH1 *hVar = new TH1F("hVar", "hVar", Nbins, varRange[0], varRange[1]);
	TH1 *hVarSideBand = new TH1F("hVarSideBand", "hVarSideBand", Nbins, varRange[0], varRange[1]);
	TH1 *hVarSignalRegion = new TH1F("hVarSignalRegion", "hVarSignalRegion", Nbins, varRange[0], varRange[1]);

	t->Project("hMass", "bsMass", generalCuts);

	TCanvas *c2 = new TCanvas();

	TString fsgn = "[0]*exp(-0.5*((x-[1])/[2])**2)+[3]*exp(-0.5*((x-[1])/[4])**2)";
	TString fbkg = "[5]*exp([6]*x)+[7]";

	TF1 *fMass = new TF1("fMass", fsgn + "+" + fbkg, MassRange[0], MassRange[1]);

	float x[2] = {5.2,5.6};
	float y[2];

	for(int i=0;i<2;++i) y[i] = hMass->GetBinContent(hMass->FindBin(x[i]));

	float expSlope = TMath::Log(y[0]/y[1])/(x[0]-x[1]);
	float expNorm = y[0]*exp(-x[0]*expSlope);

	fMass->SetNpx(5000);
	fMass->SetParameter(1, MassBs);
	fMass->SetParameter(2, StdDev);
	fMass->SetParameter(4, StdDev);
	fMass->SetParameter(5, expNorm);
	fMass->SetParameter(6, expSlope);
	fMass->SetParLimits(1, MassBs-StdDev, MassBs+StdDev);
	fMass->SetParLimits(2, 0, StdDev*3);
	fMass->SetParLimits(4, 0, StdDev*3);

	hMass->Fit("fMass","MRL");
	hMass->Draw("PE1");
	hMass->SetMinimum(1);
	hMass->SetLineWidth(2);
	hMass->GetFunction("fMass")->SetLineColor(kBlue);
	hMass->GetFunction("fMass")->SetLineWidth(2);
	hMass->SetMarkerColor(1);
	hMass->SetMarkerStyle(20);
	hMass->SetMarkerSize(0.8);
	hMass->SetTitle("Mass Distribution");

	TF1 *fit = hMass->GetFunction("fMass");

	float a1 = sqrt(2*TMath::Pi())*(fit->GetParameter(0)*fit->GetParameter(2));
	float a2 = sqrt(2*TMath::Pi())*(fit->GetParameter(3)*fit->GetParameter(4));

	float fitWeightFactor = a1 / (a1+a2);
	float peakMean	= fit->GetParameter(1);
	float peakWidth	= sqrt( fitWeightFactor*(pow(fit->GetParameter(2), 2) 
																	- pow(fit->GetParameter(4), 2)) + pow(fit->GetParameter(4), 2) );

	float edgeL[2] = {peakMean - (NSigmaSignal+NSigmaSide+OffsetSide)*peakWidth, peakMean - (NSigmaSignal + OffsetSide)*peakWidth };
	float edgeR[2] = {peakMean + (NSigmaSignal+OffsetSide)*peakWidth, peakMean + (NSigmaSignal+NSigmaSide+OffsetSide)*peakWidth};
	float edgeS[2] = {peakMean - NSigmaSignal*peakWidth, peakMean + NSigmaSignal*peakWidth};

	TF1 *signal = new TF1("signal", fsgn, MassRange[0], MassRange[1]);
	TF1 *bkg = new TF1("bkg", fbkg, MassRange[0], MassRange[1]);

	int nParSgn = signal->GetNumberFreeParameters();
	int nParBkg = bkg->GetNumberFreeParameters();

	for(int i=0;i<nParSgn;++i) signal->SetParameter(i, fit->GetParameter(i));
	for(int i=nParSgn;i<nParBkg;++i) bkg->SetParameter(i, fit->GetParameter(i));

	signal->SetLineColor(8);
	signal->SetLineWidth(2);
	signal->SetNpx(5000);
	signal->Draw("SAME");

	TF1* sideL = (TF1*)fit->Clone("sideL");
  sideL->SetRange(edgeL[0], edgeL[1]);
  sideL->SetFillColor(kRed);
  sideL->SetFillStyle(3244);
  sideL->Draw("SAME FC");

	TF1* sideR = (TF1*)fit->Clone("sideR");
  sideR->SetRange(edgeR[0], edgeR[1]);
  sideR->SetFillColor(kRed);
  sideR->SetFillStyle(3244);
  sideR->Draw("SAME FC");

	TLegend *legend1 = new TLegend(0.55, 0.65, 0.85, 0.85);
	legend1->AddEntry(hMass,"Data","p");
	legend1->AddEntry("fMass","Fitted Distribution","l");
	legend1->AddEntry("signal","Fitted Signal","l");
	legend1->AddEntry("sideL","Side Bands","f");
	legend1->Draw();

	c2->Print("massBs.pdf");

	TCanvas *c3 = new TCanvas();

	TString cutSideBand, cutSignalRegion;
	cutSideBand.Form("(((bsMass>%f)&&(bsMass<%f)) || ((bsMass>%f)&&(bsMass<%f)))", edgeL[0], edgeL[1],  edgeR[0], edgeR[1]);
	cutSignalRegion.Form("((bsMass>%f)&&(bsMass<%f))", edgeS[0], edgeS[1]);

	t->Project("hVarSideBand", varName, generalCuts + "&&" + cutSideBand);
	t->Project("hVarSignalRegion", varName, generalCuts + "&&" + cutSignalRegion);

	ROOT::Math::WrappedTF1 wf1(*bkg);
	ROOT::Math::GSLIntegrator ig(ROOT::Math::IntegrationOneDim::kADAPTIVE);
	ig.SetFunction(wf1);
	ig.SetRelTolerance(0.001);
	
	int nSideBand = (ig.Integral(edgeL[0], edgeL[1]) + ig.Integral(edgeR[0], edgeR[1])) / hMass->GetBinWidth(1);
	int nSignalRegion = ig.Integral(edgeS[0], edgeS[1]) / hMass->GetBinWidth(1);
	float MF = (float)nSignalRegion/nSideBand;

	hVar->Add(hVarSignalRegion, hVarSideBand, 1, -MF);

	hVarSignalRegion->SetLineColor(kBlue);
	hVarSignalRegion->SetLineWidth(2);
	hVarSignalRegion->SetTitle(varName);
	hVarSignalRegion->Draw("HIST");

	hVarSideBand->SetLineColor(kGreen);
	hVarSideBand->SetLineWidth(2);
	hVarSideBand->Scale(MF);
	hVarSideBand->Draw("HIST SAME");

	hVar->SetLineColor(kRed);
	hVar->SetLineWidth(2);
	hVar->Draw("HIST SAME");



	float max = hVarSignalRegion->GetMaximum();
	if( hVarSideBand->GetMaximum() > max ) max = hVarSideBand->GetMaximum();

	hVarSignalRegion->GetYaxis()->SetRangeUser(1, max*1.05);

	TLegend *legend2 = new TLegend(0.55, 0.65, 0.85, 0.85);
	legend2->AddEntry(hVar,"Signal","l");
	legend2->AddEntry(hVarSideBand,"Background","l");
	legend2->AddEntry(hVarSignalRegion,"Signal + Background","l");
	legend2->Draw();

	c3->Print(varName + "_Sgn.pdf");

	return;

}

