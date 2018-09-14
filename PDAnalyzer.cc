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

    setupReaderBarrel( weightFileBarrel );
    setupReaderEndcap( weightFileEndcap );

    nselMu = 0;
    nselFake = 0;

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
    if(GetBestBstrangeTight()>=0) tight = true;

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


    (tWriter->bsMass)->push_back( svtMass->at(iSsB) );
    (tWriter->bsCt)->push_back( svtDist3D->at(iSsB) );

    (tWriter->bsPt)->push_back( t.Pt() );
    (tWriter->bsEta)->push_back( t.Eta() );
    (tWriter->bsPhi)->push_back( t.Phi() );

    (tWriter->isTight)->push_back( tight );

    int hltfired_ = 0;

    for(int i=0; i<nHLTStatus; ++i){
        if(!hltAccept->at( i )) continue;

        if((hltPath->at( i ) == PDEnumString::HLT_Dimuon0_Jpsi3p5_Muon2_v) || (hltPath->at( i ) == PDEnumString::HLT_Dimuon0_Jpsi_Muon_v)) 
            hltfired_ += pow(2, 1);

        if(hltPath->at( i ) == PDEnumString::HLT_DoubleMu4_JpsiTrkTrk_Displaced_v) 
            hltfired_ += pow(2, 2);

        if(hltPath->at( i ) == PDEnumString::HLT_DoubleMu4_JpsiTrk_Displaced_v)
            hltfired_ += pow(2, 3);

        if(hltPath->at( i ) == PDEnumString::HLT_Dimuon20_Jpsi_Barrel_Seagulls_v)
            hltfired_ += pow(2, 4);

        if(hltPath->at( i ) == PDEnumString::HLT_Dimuon25_Jpsi_noCorrL1_v)
            hltfired_ += pow(2, 5);

        if(hltPath->at( i ) == PDEnumString::HLT_Dimuon25_Jpsi_v)
            hltfired_ += pow(2, 6);

        if(hltPath->at( i ) == PDEnumString::HLT_DoubleMu2_Jpsi_DoubleTrk1_Phi_v)
            hltfired_ += pow(2, 7);

        if(hltPath->at( i ) == PDEnumString::HLT_DoubleMu4_3_Jpsi_Displaced_v)
            hltfired_ += pow(2, 8);

    }

    (tWriter->hltFired)->push_back( hltfired_ );

    //-----------------------------------------OPPOSITE SIDE-----------------------------------------

    //-----------------------------------------SELECTION--------------------------------------------
    bool muFlag = false;

    for(int iMuon = 0; iMuon < nMuons; ++iMuon ){

        if( !( muoType->at(iMuon) & PDEnumString::global ) ) continue;
        int itkmu = muonTrack( iMuon, PDEnumString::muInner );
        if(itkmu<0) continue;
        if(std::find(tkSsB.begin(), tkSsB.end(), itkmu) != tkSsB.end()) continue;

        if(muoPt->at( iMuon )<minPtMuon) continue;
        if(abs(muoEta->at( iMuon ))>maxEtaMuon) continue;

        int muoLund = 0, genMuIndex = -1;
        if(use_gen){
            int genMuIndex = GetClosestGen( muoEta->at(iMuon), muoPhi->at(iMuon), muoPt->at(iMuon) );
            if( genMuIndex >= 0 )   muoLund = genId->at(genMuIndex);
            if(muoLund==0) continue;
        }
        
        if(!isMvaMuon(iMuon, 0.81, 0.82)) continue;

        //dZ wrt ssB
        float muoDz_wrtB = trkDz->at(itkmu) - ssBDz;
        if(abs(muoDz_wrtB)>1.0) continue;

        muFlag = true;
        if(use_gen) {if(abs(muoLund)!=13) ++nselFake;}

        break;

    }

    if(muFlag) ++nselMu;

    (tWriter->hasMuon)->push_back( muFlag );


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
    cout<<"nselMu "<<nselMu<<endl;
    cout<<"nselFake "<<nselFake<<endl;
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


// to plot some histogram immediately after the ntuple loop
// "uncomment" the following lines
/*
void PDAnalyzer::plot() {
    TCanvas* can = new TCanvas( "muoPt", "muoPt", 800, 600 );
    can->cd();
    can->Divide( 1, 2 );
    can->cd( 1 );
    hptmumax->Draw();
    hptmu2nd->Draw();
    return;
}
*/


// ======MY FUNCTIONS===============================================================================