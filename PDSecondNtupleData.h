#ifndef PDSecondNtupleData_h
#define PDSecondNtupleData_h
#include <vector>
#include "NtuTool/Common/interface/TreeWrapper.h"
using namespace std;

class PDSecondNtupleData: public virtual TreeWrapper {

public:

void Reset()  { autoReset(); }

PDSecondNtupleData() {


}
virtual ~PDSecondNtupleData() {}

void initTree() {
    treeName = "PDsecondTree";

    setBranch( "run", &run , "run/I" , &b_run );
    setBranch( "evt", &evt , "evt/I" , &b_evt );
    setBranch( "lumi", &lumi , "lumi/I" , &b_lumi );

    setBranch( "bsMass", &bsMass , "bsMass/F" , &b_bsMass );
    setBranch( "bsPt", &bsPt , "bsPt/F" , &b_bsPt );
    setBranch( "bsEta", &bsEta , "bsEta/F" , &b_bsEta );
    setBranch( "bsPhi", &bsPhi , "bsPhi/F" , &b_bsPhi );


    setBranch( "bsLxy", &bsLxy , "bsLxy/F" , &b_bsLxy );
    setBranch( "bsCt2D", &bsCt2D , "bsCt2D/F" , &b_bsCt2D );
    setBranch( "bsCt2DErr", &bsCt2DErr , "bsCt2DErr/F" , &b_bsCt2DErr );
    setBranch( "bsCt2DSigmaUnit", &bsCt2DSigmaUnit , "bsCt2DSigmaUnit/F" , &b_bsCt2DSigmaUnit );
    setBranch( "bsCt3D", &bsCt3D , "bsCt3D/F" , &b_bsCt3D );
    setBranch( "bsCt3DErr", &bsCt3DErr , "bsCt3DErr/F" , &b_bsCt3DErr );
    setBranch( "bsCt3DSigmaUnit", &bsCt3DSigmaUnit , "bsCt3DSigmaUnit/F" , &b_bsCt3DSigmaUnit );

    setBranch( "hltJpsiMu", &hltJpsiMu , "hltJpsiMu/I" , &b_hltJpsiMu );
    setBranch( "hltJpsiTrkTrk", &hltJpsiTrkTrk , "hltJpsiTrkTrk/I" , &b_hltJpsiTrkTrk );
    setBranch( "hltJpsiTrk", &hltJpsiTrk , "hltJpsiTrk/I" , &b_hltJpsiTrk );

    setBranch( "isTight", &isTight , "isTight/I" , &b_isTight );
    setBranch( "hasMuon", &hasMuon , "hasMuon/I" , &b_hasMuon );
    setBranch( "utility", &utility , "utility/I" , &b_utility );

    setBranch( "muPlus_Pt", &muPlus_Pt , "muPlus_Pt/F" , &b_muPlus_Pt );
    setBranch( "muMinus_Pt", &muMinus_Pt , "muMinus_Pt/F" , &b_muMinus_Pt );
    setBranch( "kaonPlus_Pt", &kaonPlus_Pt , "kaonPlus_Pt/F" , &b_kaonPlus_Pt );
    setBranch( "kaonMinus_Pt", &kaonMinus_Pt , "kaonMinus_Pt/F" , &b_kaonMinus_Pt );    
    setBranch( "muPlus_Eta", &muPlus_Eta , "muPlus_Eta/F" , &b_muPlus_Eta );
    setBranch( "muMinus_Eta", &muMinus_Eta , "muMinus_Eta/F" , &b_muMinus_Eta );
    setBranch( "kaonPlus_Eta", &kaonPlus_Eta , "kaonPlus_Eta/F" , &b_kaonPlus_Eta );
    setBranch( "kaonMinus_Eta", &kaonMinus_Eta , "kaonMinus_Eta/F" , &b_kaonMinus_Eta );
    setBranch( "kaonPlus_Hits", &kaonPlus_Hits , "kaonPlus_Hits/I" , &b_kaonPlus_Hits );
    setBranch( "kaonMinus_Hits", &kaonMinus_Hits , "kaonMinus_Hits/I" , &b_kaonMinus_Hits );
    setBranch( "jpsiMass", &jpsiMass , "jpsiMass/F" , &b_jpsiMass );
    setBranch( "jpsiPt", &jpsiPt , "jpsiPt/F" , &b_jpsiPt );
    setBranch( "jpsiEta", &jpsiEta , "jpsiEta/F" , &b_jpsiEta );
    setBranch( "phiMass", &phiMass , "phiMass/F" , &b_phiMass );
    setBranch( "phiPt", &phiPt , "phiPt/F" , &b_phiPt );
    setBranch( "phiEta", &phiEta , "phiEta/F" , &b_phiEta );

}

    int hltJpsiMu, hltJpsiTrkTrk, hltJpsiTrk, isTight, hasMuon, utility;
    int run, evt, lumi;
    float bsMass, bsPt, bsEta, bsPhi;
    float bsLxy, bsCt2D, bsCt2DErr, bsCt2DSigmaUnit, bsCt3D, bsCt3DErr, bsCt3DSigmaUnit;

    TBranch *b_hltJpsiMu, *b_hltJpsiTrkTrk, *b_hltJpsiTrk, *b_isTight, *b_hasMuon, *b_utility;
    TBranch *b_run, *b_evt, *b_lumi;
    TBranch *b_bsMass, *b_bsPt, *b_bsEta, *b_bsPhi;
    TBranch *b_bsLxy, *b_bsCt2D, *b_bsCt3D ,*b_bsCt2DErr, *b_bsCt2DSigmaUnit, *b_bsCt3DErr, *b_bsCt3DSigmaUnit;

    //selection variable

    float muPlus_Pt, muMinus_Pt, kaonPlus_Pt, kaonMinus_Pt;
    float muPlus_Eta, muMinus_Eta, kaonPlus_Eta, kaonMinus_Eta;
    float jpsiMass, jpsiPt, jpsiEta, phiMass, phiPt, phiEta, bsChi2;
    int kaonPlus_Hits, kaonMinus_Hits;

    TBranch *b_muPlus_Pt, *b_muMinus_Pt, *b_kaonPlus_Pt, *b_kaonMinus_Pt;
    TBranch *b_muPlus_Eta, *b_muMinus_Eta, *b_kaonPlus_Eta, *b_kaonMinus_Eta;
    TBranch *b_jpsiMass, *b_jpsiPt, *b_jpsiEta, *b_phiMass, *b_phiPt, *b_phiEta, *b_bsChi2;
    TBranch *b_kaonPlus_Hits, *b_kaonMinus_Hits;



    private:

    PDSecondNtupleData           ( const PDSecondNtupleData& a );
    PDSecondNtupleData& operator=( const PDSecondNtupleData& a );

};

#endif

