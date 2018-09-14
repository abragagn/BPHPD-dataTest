#ifndef PDSecondNtupleData_h
#define PDSecondNtupleData_h
#include <vector>
#include "NtuTool/Common/interface/TreeWrapper.h"
using namespace std;

class PDSecondNtupleData: public virtual TreeWrapper {

public:

void Reset()  { autoReset(); }

PDSecondNtupleData() {

    bsMass    = new vector <float>;
    bsCt      = new vector <float>;
    bsPt      = new vector <float>;
    bsEta     = new vector <float>;
    bsPhi     = new vector <float>;

    hltFired  = new vector <int>;
    isTight   = new vector <int>;
    hasMuon   = new vector <int>;




}
virtual ~PDSecondNtupleData() {}

void initTree() {
    treeName = "PDsecondTree";

    setBranch( "bsMass", &bsMass , 8192, 99, &b_bsMass );
    setBranch( "bsCt", &bsCt , 8192, 99, &b_bsCt );

    setBranch( "bsPt", &bsPt , 8192, 99, &b_bsPt );
    setBranch( "bsEta", &bsEta , 8192, 99, &b_bsEta );
    setBranch( "bsPhi", &bsPhi , 8192, 99, &b_bsPhi );

    setBranch( "hltFired", &hltFired , 8192, 99, &b_hltFired );
    setBranch( "isTight", &isTight , 8192, 99, &b_isTight );
    setBranch( "hasMuon", &hasMuon , 8192, 99, &b_hasMuon );

}

    vector <int> *hltFired, *isTight, *hasMuon;
    vector <float> *bsMass, *bsCt, *bsPt, *bsEta, *bsPhi;

    TBranch *b_bsMass, *b_bsCt, *b_bsPt, *b_bsEta, *b_bsPhi;
    TBranch *b_hltFired, *b_isTight, *b_hasMuon;

    private:

    PDSecondNtupleData           ( const PDSecondNtupleData& a );
    PDSecondNtupleData& operator=( const PDSecondNtupleData& a );

};

#endif

