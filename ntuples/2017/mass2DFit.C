const float MassBs = 5.366;
const float sigma = 0.015;
const float massRange[2] = {5.0, 6.0};
const int nbins = 100;
const float dummy = 1.0;


void mass2DFit(TString fileName = "ntu2017C.root", TString varName = "bsPt")
{
	gStyle->SetOptStat(0);

	TFile *f 	= new TFile(fileName);
	TTree *t = (TTree*)f->Get("PDsecondTree");

	TString generalCuts = "";

	t->Draw(varName + " >> htemp ", generalCuts);
	TH1 *hPark = (TH1*)gDirectory->Get("htemp");
	hPark->Draw();
	float varRange[2] = {(float)hPark->GetXaxis()->GetXmin(), (float)hPark->GetXaxis()->GetXmax() };

	TH2* h2D = new TH2F("h2D", "bsMass:" + varName, nbins, massRange[0], massRange[1], nbins, varRange[0], varRange[1]);
	t->Draw(varName + ":bsMass >> h2D", generalCuts);

	TH1 *bsXProj = h2D->ProjectionX();
	TString fX = "[0]*exp(-0.5*((x-[1])/[2])**2)+[3]*exp(-0.5*((x-[1])/[4])**2)+[5]+[6]*x+[7]*x**2+[8]*x**3";
	TF1 *f1X = new TF1("f1X",fX , massRange[0], massRange[1]);

	f1X->SetParameters(dummy, MassBs, sigma, dummy, sigma, 
											dummy, dummy, dummy, dummy);

	f1X->SetParLimits(1, MassBs-sigma, MassBs+sigma);
	f1X->SetParLimits(2, 0, sigma*3);
	f1X->SetParLimits(4, 0, sigma*3);
	bsXProj->Fit("f1X","MRL");
	TF1 *fitMass = bsXProj->GetFunction("f1X");

	TString fY = "[9]+[10]*y+[11]*y**2+[12]*y**3+[13]*y**4+[14]*y**5";
	TF2 *f2 = new TF2("f2", fX + " + " + fY, massRange[0], massRange[1], varRange[0], varRange[1]);

	for(int i=0; i<=8; ++i) f2->SetParameter(i, fitMass->GetParameter(i) );
	for(int i=9; i<=14; ++i) f2->SetParameter(i, dummy );

	h2D->Fit("f2", "MRL");
	h2D->Draw("lego2");

	return;

}
