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

}

    int hltJpsiMu, hltJpsiTrkTrk, hltJpsiTrk, isTight, hasMuon, utility;
    int run, evt, lumi;
    float bsMass, bsPt, bsEta, bsPhi;
    float bsLxy, bsCt2D, bsCt2DErr, bsCt2DSigmaUnit, bsCt3D, bsCt3DErr, bsCt3DSigmaUnit;

    TBranch *b_hltJpsiMu, *b_hltJpsiTrkTrk, *b_hltJpsiTrk, *b_isTight, *b_hasMuon, *b_utility;
    TBranch *b_run, *b_evt, *b_lumi;
    TBranch *b_bsMass, *b_bsPt, *b_bsEta, *b_bsPhi;
    TBranch *b_bsLxy, *b_bsCt2D, *b_bsCt3D ,*b_bsCt2DErr, *b_bsCt2DSigmaUnit, *b_bsCt3DErr, *b_bsCt3DSigmaUnit;

    private:

    PDSecondNtupleData           ( const PDSecondNtupleData& a );
    PDSecondNtupleData& operator=( const PDSecondNtupleData& a );

};

#endif

