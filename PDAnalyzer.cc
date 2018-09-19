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

// additional features
#include "PDSecondNtupleWriter.h"                               // second ntuple
//#include "DataSetFilter.cc"                                           // dataset filter
#include "PDMuonVar.cc"
#include "PDSoftMuonMvaEstimator.cc"
#include "AlbertoUtil.cc"

using namespace std;

//pdTreeAnalyze /lustre/cmswork/abragagn/ntuList/charmonium2017Lists/Charmonium_Run2017B-31Mar2018-v1_MINIAOD_DCAP.list hist.root -v outputFile ntu.root -v histoMode RECREATE -v use_gen f -n 10000

PDAnalyzer::PDAnalyzer() {

    std::cout << "new PDAnalyzer" << std::endl;

    // user parameters are set as names associated to a string, 
    // default values can be set in the analyzer class contructor

    setUserParameter( "verbose", "f" );
    setUserParameter( "minPtMuon", "2." );
    setUserParameter( "maxEtaMuon", "2.4" );
    setUserParameter( "outputFile", "ntu_skim.root" );
    setUserParameter( "use_only_gen", "f" );



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
    getUserParameter( "use_only_gen", use_only_gen );

    getUserParameter( "ptCut", ptCut ); //needed for paolo's code for unknow reasons

// to skim the N-tuple "uncomment" the following lines
//  // dropBranch( "*tau*" ); // drop some branch if required
//  initWSkim( new TFile( "skim.root", "RECREATE" ) );


// additional features
//  DataSetFilter::beginJob();                                       // dataset filter
    tWriter = new PDSecondNtupleWriter;                     // second ntuple
    tWriter->open( getUserParameter("outputFile"), "RECREATE" ); // second ntuple

    return;

}


void PDAnalyzer::book() {

    // putting "autoSavedObject" in front of the histo creation 
    // it's automatically marked for saving on file; the option 
    // is uneffective when not using the full utility

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
    else {

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


    int iSsB = GetBestBstrange();
    if(iSsB<0) return false;

    bool tight = false;
    int utility = 0;
    int iSsBtight = GetBestBstrangeTight();
    if(iSsBtight>=0) {
        tight = true;
        iSsB = iSsBtight;
    };
    if(francescoSelectionTest1()>=0) utility += 1<<1;

    vector <int> tkSsB = tracksFromSV(iSsB);
    int iJPsi = (subVtxFromSV(iSsB)).at(0);
    vector <int> tkJpsi = tracksFromSV(iJPsi);

    TLorentzVector t(0,0,0,0);

    for(unsigned int i=0; i<tkSsB.size(); ++i){
        int j = tkSsB.at(i);
        float m = MassK;
        if( (j==tkJpsi.at(0)) || (j==tkJpsi.at(1)) ) m = MassMu;
        TLorentzVector a;
        a.SetPtEtaPhiM( trkPt->at(j), trkEta->at(j), trkPhi->at(j), m );
        t += a;
    }

    float ssBDz = 0;
    for( unsigned int i=0; i<tkSsB.size(); ++i ) ssBDz+=trkDz->at(tkSsB[i]);
    ssBDz/=tkSsB.size();

    int hltfired_ = 0;

    if(hlt(PDEnumString::HLT_Dimuon0_Jpsi3p5_Muon2_v)||hlt(PDEnumString::HLT_Dimuon0_Jpsi_Muon_v)) hltfired_ += 1<<1;
    if(hlt(PDEnumString::HLT_DoubleMu4_JpsiTrkTrk_Displaced_v)) hltfired_ += 1<<2;
    if(hlt(PDEnumString::HLT_DoubleMu4_JpsiTrk_Displaced_v)) hltfired_ += 1<<3;

    (tWriter->bsMass)->push_back( svtMass->at(iSsB) );
    (tWriter->bsCt)->push_back( svtDist3D->at(iSsB) );

    (tWriter->bsPt)->push_back( t.Pt() );
    (tWriter->bsEta)->push_back( t.Eta() );
    (tWriter->bsPhi)->push_back( t.Phi() );

    (tWriter->isTight)->push_back( tight );
    (tWriter->utility)->push_back( utility );
    (tWriter->hltFired)->push_back( hltfired_ );

    tWriter->fill();


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
int PDAnalyzer::francescoSelectionTest1()
{
    int index = -1;
    float bestChi2 = 1e9;
    for( unsigned short int iB=0; iB<nSVertices; ++iB ){

       if((svtType->at(iB)!=PDEnumString::svtBsJPsiPhi) ) continue;

       int iJPsi = (subVtxFromSV(iB)).at(0);
       int iPhi = (subVtxFromSV(iB)).at(1);
       vector <int> tkSsB = tracksFromSV(iB);
       vector <int> tkJpsi = tracksFromSV(iJPsi);
       vector <int> tkPhi = tracksFromSV(iPhi);

       //JPSI
        if(fabs(svtMass->at(iJPsi) - MassJPsi) > 0.15 ) continue;

        if(trkPt->at(tkJpsi[0]) < 4.0 ) continue;
        if(trkPt->at(tkJpsi[1]) < 4.0 ) continue;
        if(fabs(trkEta->at(tkJpsi[0])) > 2.1) continue;
        if(fabs(trkEta->at(tkJpsi[1])) > 2.1) continue;

        TLorentzVector tJPsi(0,0,0,0);
        for( unsigned int i=0; i<tkJpsi.size(); ++i ){
           int j = tkJpsi[i];
           TLorentzVector a;
           a.SetPtEtaPhiM( trkPt->at(j), trkEta->at(j), trkPhi->at(j), MassMu );
           tJPsi += a;
        }


        if(tJPsi.Pt() < 7.0) continue;

       //PHI
        if(fabs(svtMass->at(iPhi) - MassPhi) > 0.01 ) continue;
        if(trkPt->at(tkPhi[0]) < 0.7) continue;
        if(trkPt->at(tkPhi[1]) < 0.7) continue;
        if(fabs(trkEta->at(tkPhi[0])) > 2.5) continue;
        if(fabs(trkEta->at(tkPhi[1])) > 2.5) continue;

        int Kp_Hits = trkHitPattern->at(tkPhi[0]);
        int Km_Hits = trkHitPattern->at(tkPhi[1]);

        int kpnpst = ( int(Kp_Hits) / 100 ) % 10000;
        int kpntrk = kpnpst % 100;

        int kmnpst = ( int(Km_Hits) / 100 ) % 10000;
        int kmntrk = kmnpst % 100;

        if (kmntrk<4 || kpntrk<4 ) continue;

        //BS
        if( svtMass->at(iB)<5.2 || svtMass->at(iB)>5.65 ) continue;
        if((svtDist2D->at(iB)/svtSigma2D->at(iB)) < 3) continue;
        if((svtDist2D->at(iB)) < 0.02) continue;

        TLorentzVector tB(0,0,0,0);

        for( unsigned int i=0; i<tkSsB.size(); ++i ){
            int j = tkSsB[i];
            float m = MassK;
            if( j == tkJpsi[0] || j == tkJpsi[1] ) m = MassMu;
            TLorentzVector a;
            a.SetPtEtaPhiM( trkPt->at(j), trkEta->at(j), trkPhi->at(j), m );
            tB += a;
        }

        if(tB.Pt() < 8.0) continue;
        if(fabs(tB.Eta()) > 2.4) continue;

        if( ChiSquaredProbability( svtChi2->at(iB), svtNDOF->at(iB) ) < 0.02 ) continue;

        if( svtChi2->at(iB)>bestChi2 ) continue;
        index = iB;
        bestChi2 = svtChi2->at(iB);

        }
        return index;
}
