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

    setBranch( "bsMass", &bsMass , "bsMass/F" , &b_bsMass );
    
    setBranch( "bsLxy", &bsLxy , "bsLxy/F" , &b_bsLxy );
    setBranch( "bsCTxy", &bsCTxy , "bsCTxy/F" , &b_bsCTxy );
    setBranch( "bsCTxyz", &bsCTxyz , "bsCTxyz/F" , &b_bsCTxyz );

    setBranch( "bsPt", &bsPt , "bsPt/F" , &b_bsPt );
    setBranch( "bsEta", &bsEta , "bsEta/F" , &b_bsEta );
    setBranch( "bsPhi", &bsPhi , "bsPhi/F" , &b_bsPhi );

    setBranch( "hltFired", &hltFired , "hltFired/I" , &b_hltFired );
    setBranch( "isTight", &isTight , "isTight/I" , &b_isTight );
    setBranch( "hasMuon", &hasMuon , "hasMuon/I" , &b_hasMuon );
    setBranch( "utility", &utility , "utility/I" , &b_utility );

}

    int hltFired, isTight, hasMuon, utility;
    float bsMass, bsLxy, bsPt, bsEta, bsPhi, bsCTxy, bsCTxyz;

    TBranch *b_bsMass, *b_bsLxy, *b_bsPt, *b_bsEta, *b_bsPhi, *b_bsCTxy, *b_bsCTxyz;
    TBranch *b_hltFired, *b_isTight, *b_hasMuon, *b_utility;

    private:

    PDSecondNtupleData           ( const PDSecondNtupleData& a );
    PDSecondNtupleData& operator=( const PDSecondNtupleData& a );

};

#endif

