#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
#include <vector>
#include <algorithm>

#include "PDAnalyzer.h"

#include "TDirectory.h"
#include "TBranch.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TLorentzVector.h"
#include "TFile.h"
#include "TMatrixF.h"
#include "TVectorF.h"

// additional features
#include "PDSecondNtupleWriter.h"                               // second ntuple
//#include "DataSetFilter.cc"                                           // dataset filter
#include "PDMuonVar.cc"
#include "PDSoftMuonMvaEstimator.cc"
#include "AlbertoUtil.cc"

using namespace std;

int n1, n2;

//pdTreeAnalyze /lustre/cmswork/abragagn/ntuList/charmonium2017Lists/Charmonium_Run2017B-31Mar2018-v1_MINIAOD_DCAP.list hist.root -v outputFile ntu.root -v histoMode RECREATE -v use_gen f -n 10000

PDAnalyzer::PDAnalyzer() {

    std::cout << "new PDAnalyzer" << std::endl;

    // user parameters are set as names associated to a string, 
    // default values can be set in the analyzer class contructor

    setUserParameter( "verbose", "f" );
    setUserParameter( "minPtMuon", "2." );
    setUserParameter( "maxEtaMuon", "2.4" );
    setUserParameter( "outputFile", "ntu_skim.root" );



    setUserParameter( "ptCut", "40.0" ); //needed for paolo's code

}


PDAnalyzer::~PDAnalyzer() {
}



void PDAnalyzer::beginJob() {

    PDAnalyzerUtil::beginJob();

    // user parameters are retrieved as strings by using their names;
    // numeric parameters ( int, float or whatever ) can be directly set
    // by passing the corresponding variable,
    // e.g. getUserParameter( "name", x )

    getUserParameter( "verbose", verbose );
    getUserParameter( "minPtMuon", minPtMuon );
    getUserParameter( "maxEtaMuon", maxEtaMuon );

    getUserParameter( "ptCut", ptCut ); //needed for paolo's code for unknow reasons

// to skim the N-tuple "uncomment" the following lines
//  // dropBranch( "*tau*" ); // drop some branch if required
//  initWSkim( new TFile( "skim.root", "RECREATE" ) );


// additional features
//  DataSetFilter::beginJob();                                       // dataset filter
    tWriter = new PDSecondNtupleWriter;                     // second ntuple
    tWriter->open( getUserParameter("outputFile"), "RECREATE" ); // second ntuple
    
    n1 = 0;
    n2 = 0;

    return;

}


void PDAnalyzer::book() {

    // putting "autoSavedObject" in front of the histo creation 
    // it's automatically marked for saving on file; the option 
    // is uneffective when not using the full utility

    autoSavedObject =
    hTest   = new TH1D( "hTest", "hTest", 200, 0, 100 );

    autoSavedObject =
    hTest2   = new TH1D( "hTest2", "hTest2", 550, -0.05, 0.5 );

    return;

}


void PDAnalyzer::reset() {
// automatic reset
    autoReset();
    return;
}


bool PDAnalyzer::analyze( int entry, int event_file, int event_tot ) {

    if ( verbose ) {
        cout << " +++++++++++++++++++++++++++ " << endl;
        cout << "entry: "
                 << entry << " " << event_file << " " << event_tot << endl;
        cout << "run: " <<   runNumber << " , "
                 << "evt: " << eventNumber << endl;
    }
    else{

        if ( (!(event_tot%10)   && event_tot<100 ) || 
     (!(event_tot %100) && event_tot<1000 ) || 
     (!(event_tot %1000)    && event_tot<10000 ) || 
     (!(event_tot %10000)   && event_tot<100000 ) || 
     (!(event_tot %100000)  && event_tot<1000000 ) || 
     (!(event_tot %1000000) && event_tot<10000000 ) )
            cout << "   == at event " << event_file << " " << event_tot << endl;
    }

// additional features
    computeMuonVar();
    tWriter->Reset();
    convSpheCart(jetPt, jetEta, jetPhi, jetPx, jetPy, jetPz);
    convSpheCart(muoPt, muoEta, muoPhi, muoPx, muoPy, muoPz);
    convSpheCart(trkPt, trkEta, trkPhi, trkPx, trkPy, trkPz);

    bool jpsimu = false;
    bool jpsitktk = false;
    bool jpsitk = false;

    if(hlt(PDEnumString::HLT_Dimuon0_Jpsi3p5_Muon2_v)||hlt(PDEnumString::HLT_Dimuon0_Jpsi_Muon_v)) jpsimu = true;
    if(hlt(PDEnumString::HLT_DoubleMu4_JpsiTrkTrk_Displaced_v)) jpsitktk =  true;
    if(hlt(PDEnumString::HLT_DoubleMu4_JpsiTrk_Displaced_v)) jpsitk = true;
    

    int iSsB = GetBestBstrangeTest();
    if( !(jpsimu || jpsitktk || jpsitk) ) iSsB= -1;
    int FF = FFCode();

    if( !(jpsimu || jpsitktk || jpsitk) ) return false;
    if(iSsB<0) return false; 

    //if((FF<0 && iSsB>=0) || (FF>=0 && iSsB<0)) cout <<"debug "<<FF<<" "<<iSsB<<endl;

    bool _tight = false;
    int _utility = 0;
    
    int iSsBtight = GetBestBstrangeTight();
    if(iSsBtight>=0) {
        _tight = true;
        iSsB = iSsBtight;
    };

    vector <int> tkSsB = tracksFromSV(iSsB);
    int iJPsi = (subVtxFromSV(iSsB)).at(0);
    vector <int> tkJpsi = tracksFromSV(iJPsi);

    TLorentzVector tB(0,0,0,0);

    for(uint i=0; i<tkSsB.size(); ++i){
        int j = tkSsB.at(i);
        float m = MassK;
        if( (j==tkJpsi.at(0)) || (j==tkJpsi.at(1)) ) m = MassMu;
        TLorentzVector a;
        a.SetPtEtaPhiM( trkPt->at(j), trkEta->at(j), trkPhi->at(j), m );
        tB += a;
    }

    int iPV = GetBestPV(iSsB, tB);
    if(iPV<0) return false;

     if(iSsB>=0 || FF>=0) n2++;
     if(FF!=iSsB) {
         cout <<event_tot<<endl;
         cout<<FF<<" "<<svtMass->at(FF)<<" "<<svtChi2->at(FF)<<endl;
         cout<<iSsB<<" "<<svtMass->at(iSsB)<<" "<<svtChi2->at(iSsB)<<endl;
         n1++;
     }

    (tWriter->run) = runNumber;
    (tWriter->evt) = eventNumber;
    (tWriter->lumi) = lumiSection;

    (tWriter->bsMass) = svtMass->at(iSsB);
    (tWriter->bsPt) = tB.Pt();
    (tWriter->bsEta) = tB.Eta();
    (tWriter->bsPhi) = tB.Phi();

    (tWriter->bsLxy) = Lxy;
    (tWriter->bsCt2D) = GetCt2D(tb, iSsB);
    (tWriter->bsCt3D) = GetCt3D(tb, iSsB, iPV);

    (tWriter->isTight) = _tight;
    (tWriter->utility) = _utility;
    (tWriter->hltJpsiMu) = jpsimu;
    (tWriter->hltJpsiTrkTrk) = jpsitktk;
    (tWriter->hltJpsiTrk) = jpsitk;

    tWriter->fill();

/*    if( jpsitktk ){
        cout<<endl<<"run "<<runNumber<<" --- evt "<<eventNumber<<" --- iSvt "<<iSsB<<" --- HLT "<<jpsimu<<jpsitktk<<jpsitk<<endl;
        cout.precision(4);
        cout<<setw(6)<<left<< "svtX "   <<setw(5)<<   vSVT.X() <<endl;
        cout<<setw(6)<<left<< "svtY "   <<setw(5)<<   vSVT.Y() <<endl;
        cout<<setw(6)<<left<< "bsX "   <<setw(5)<<   vBeamSpot.X() <<endl;
        cout<<setw(6)<<left<< "bsY "   <<setw(5)<<   vBeamSpot.Y() <<endl;

        cout<<setw(6)<<left<< "pt "   <<setw(5)<<   tB.Pt() <<endl;
        cout<<setw(6)<<left<< "eta "  <<setw(5)<<   tB.Eta() <<endl;
        cout<<setw(6)<<left<< "Lxy "  <<setw(5)<<   Lxy     <<endl;
        cout<<setw(6)<<left<< "1/\u03B2\u03B3  "  <<setw(5)<<   MassBs/tB.Pt()     <<endl;
        cout<<setw(6)<<left<< "ct2D " <<setw(5)<<   ct      <<endl;
        cout<<setw(6)<<left<< "ct3D " <<setw(5)<<   ct3D    <<endl;        
    }

*/    if(use_gen){

        vector <int> ListLongLivedB;
        vector <int> ListB;
        int genBindex = -1;

        for( uint i=0 ; i<genId->size() ; ++i ){
            if(TagMixStatus( i ) == 2) continue;
            uint Code = abs(genId->at(i));
            if( Code == 511 || Code == 521 || Code == 531 || Code == 541 || Code == 5122 ) ListLongLivedB.push_back(i);
        }

        genBindex = GetClosestGenLongLivedB( tB.Eta(), tB.Phi(), tB.Pt(), &ListLongLivedB);

        if(genBindex>=0) cout<<setw(6)<<left<< "ctGen " <<setw(5)<<   GetGenCT(genBindex)    <<endl;

        if(genBindex>=0) hTest->Fill(GetGenCT(genBindex)-ct3D);


    }


// to skim the N-tuple "uncomment" the following line
//  if ( flag ) fillSkim();

    return true;

}


void PDAnalyzer::endJob() {
// to skim the N-tuple "uncomment" the following line
//  closeSkim();

// additional features
//  DataSetFilter::endJob();                                             // dataset filter
    tWriter->close();                                                           // second ntuple
    cout<<endl<<(float)n1/n2<<endl;
    return;
}


void PDAnalyzer::save() {
#   if UTIL_USE == FULL
    // explicit saving not necessary for "autoSavedObjects"
    autoSave();
#elif UTIL_USE == BARE
    // explicit save histos when not using the full utility

#endif

    return;
}


// ======MY FUNCTIONS===============================================================================
int PDAnalyzer::FFCode()
{
    int index = -1;
    float bestChi2 = 1e9;
    for ( int iSV = 0; iSV < nSVertices; ++iSV ){

        //Select the Bs vertex
        if ( svtType->at( iSV ) != PDEnumString::svtBsJPsiPhi) continue;

        int HLT_Jtktk = -1;
        int HLT_Jmu = -1;
        int HLT_Jtk = -1;
            
        for (int path=0; path<nHLTStatus; path++){
            if (!hltRun->at(path)) continue;

            if (!(hltPath->at(path)==PDEnumString::HLT_DoubleMu4_JpsiTrkTrk_Displaced_v 
            || hltPath->at(path)==PDEnumString::HLT_Dimuon0_Jpsi_Muon_v 
            || hltPath->at(path)==PDEnumString::HLT_DoubleMu4_JpsiTrk_Displaced_v 
            || hltPath->at(path)==PDEnumString::HLT_Dimuon0_Jpsi3p5_Muon2_v)) continue;

            if (hltPath->at(path)==PDEnumString::HLT_DoubleMu4_JpsiTrkTrk_Displaced_v) HLT_Jtktk=hltAccept->at(path);
            else if (hltPath->at(path)==PDEnumString::HLT_Dimuon0_Jpsi_Muon_v || hltPath->at(path)==PDEnumString::HLT_Dimuon0_Jpsi3p5_Muon2_v) HLT_Jmu=hltAccept->at(path);
            else if (hltPath->at(path)==PDEnumString::HLT_DoubleMu4_JpsiTrk_Displaced_v) HLT_Jtk=hltAccept->at(path);

        }

        if( !(HLT_Jtktk || HLT_Jmu || HLT_Jtk) ) continue;

        const vector<int>& tks = tracksFromSV( iSV );
        int n = tks.size();

        if (!n){cout<<"!n"<<endl; continue;}
        const vector<int>& sub = subVtxFromSV( iSV );
        if (sub.size() !=2){cout<<"sub.size() !=2"<<endl; continue;}

        TLorentzVector mu_p, mu_m, k_p, k_m, Bs, Jpsi, Phi, BsNoRefit, JpsiNoRefit, PhiNoRefit;
        std::vector<int> mu_idx, k_idx;
        TVector3 SVpos, PVpos;

        SVpos.SetXYZ(svtX->at( iSV ),svtY->at( iSV ),svtZ->at( iSV ));

        for (uint k=0; k<sub.size(); k++){
            if (svtType->at(sub[k]) == PDEnumString::svtJPsi){

                const vector<int>& mutks = tracksFromSV( sub[k] );
                for (uint t=0; t< mutks.size(); ++t) mu_idx.push_back(mutks[t]); 

            } else if (svtType->at(sub[k]) == PDEnumString::svtPhi){

                const vector<int>& ktks = tracksFromSV( sub[k] );
                for (uint t=0; t< ktks.size(); ++t) k_idx.push_back(ktks[t]); 
            }

        }

        if (k_idx.size()!=2 || mu_idx.size()!=2  ){cout<<"k_idx.size()!=2 || mu_idx.size()!=2"<<endl; continue;}

        if (trkCharge->at(mu_idx[0]) >0){
            mu_p.SetPtEtaPhiM(trkPt->at(mu_idx[0]),trkEta->at(mu_idx[0]),trkPhi->at(mu_idx[0]),MassMu);
            mu_m.SetPtEtaPhiM(trkPt->at(mu_idx[1]),trkEta->at(mu_idx[1]),trkPhi->at(mu_idx[1]),MassMu);
        } else if (trkCharge->at(mu_idx[0]) < 0){
            mu_m.SetPtEtaPhiM(trkPt->at(mu_idx[0]),trkEta->at(mu_idx[0]),trkPhi->at(mu_idx[0]),MassMu);
            mu_p.SetPtEtaPhiM(trkPt->at(mu_idx[1]),trkEta->at(mu_idx[1]),trkPhi->at(mu_idx[1]),MassMu);
        }

        if (trkCharge->at(k_idx[0]) >0){
            k_p.SetPtEtaPhiM(trkPt->at(k_idx[0]),trkEta->at(k_idx[0]),trkPhi->at(k_idx[0]),MassK);
            k_m.SetPtEtaPhiM(trkPt->at(k_idx[1]),trkEta->at(k_idx[1]),trkPhi->at(k_idx[1]),MassK);
        } else if (trkCharge->at(k_idx[0]) < 0){
            k_m.SetPtEtaPhiM(trkPt->at(k_idx[0]),trkEta->at(k_idx[0]),trkPhi->at(k_idx[0]),MassK);
            k_p.SetPtEtaPhiM(trkPt->at(k_idx[1]),trkEta->at(k_idx[1]),trkPhi->at(k_idx[1]),MassK);
        }

        Jpsi=mu_p + mu_m;
        Phi=k_p + k_m;
        Bs = Jpsi + Phi;

        int pvIndex=-999;
        FindPV(SVpos, PVpos, Bs, pvIndex);
               
        if (pvIndex < 0) continue;

        if( svtChi2->at(iSV)>bestChi2 ) continue;
        index = iSV;
        bestChi2 = svtChi2->at(iSV);
    }

    return index;
}


void PDAnalyzer::FindPV(TVector3 & sv, TVector3 & pv, TLorentzVector & BsP4, int & index) {
  
  TVector3 BsP3=BsP4.Vect();
  TVector3 tmpPV;
  float tmpCos=-999.;

  int iPV;
  for ( iPV = 0; iPV < nPVertices; ++iPV ) {
    tmpPV.SetXYZ(pvtX->at(iPV),pvtY->at(iPV),pvtZ->at(iPV));
    TVector3 diff= sv-tmpPV;
    if (abs(sv.Z()-tmpPV.Z())>0.5) continue;
    float cosPoint=cos(diff.Angle(BsP3));
    if (cosPoint > tmpCos){
      tmpCos=cosPoint;
      pv.SetXYZ(pvtX->at(iPV),pvtY->at(iPV),pvtZ->at(iPV));
      index=iPV;
    }
  }  

  return;
}

int PDAnalyzer::GetBestBstrangeTest()
{
    int index = -1;
    float bestChi2 = 1e9;
    for( uint iB=0; iB<nSVertices; ++iB ){

       if((svtType->at(iB)!=PDEnumString::svtBsJPsiPhi) ) continue;
        cout<<svtChi2->at(iB)<<endl;
       if( svtMass->at(iB)<BsMassRange[0] || svtMass->at(iB)>BsMassRange[1] ) continue;

       if( svtChi2->at(iB)>bestChi2 ) continue;
       index = iB;
       bestChi2 = svtChi2->at(iB);

    }
    return index;
}
