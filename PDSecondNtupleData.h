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
    bsPt      = new vector <float>;
    bsEta     = new vector <float>;
    bsPhi     = new vector <float>;

    bsLxy     = new vector <float>;
    bsCtau    = new vector <float>;
    bsCtau3D  = new vector <float>;

    hltFired  = new vector <int>;
    isTight   = new vector <int>;
    hasMuon   = new vector <int>;
    utility   = new vector <int>;

}
virtual ~PDSecondNtupleData() {}

void initTree() {
    treeName = "PDsecondTree";

    setBranch( "bsMass", &bsMass , 8192, 99, &b_bsMass );
    
    setBranch( "bsLxy", &bsLxy , 8192, 99, &b_bsLxy );
    setBranch( "bsCtau", &bsCtau , 8192, 99, &b_bsCtau );
    setBranch( "bsCtau3D", &bsCtau3D , 8192, 99, &b_bsCtau3D );

    setBranch( "bsPt", &bsPt , 8192, 99, &b_bsPt );
    setBranch( "bsEta", &bsEta , 8192, 99, &b_bsEta );
    setBranch( "bsPhi", &bsPhi , 8192, 99, &b_bsPhi );

    setBranch( "hltFired", &hltFired , 8192, 99, &b_hltFired );
    setBranch( "isTight", &isTight , 8192, 99, &b_isTight );
    setBranch( "hasMuon", &hasMuon , 8192, 99, &b_hasMuon );
    setBranch( "utility", &utility , 8192, 99, &b_utility );

}

    vector <int> *hltFired, *isTight, *hasMuon, *utility;
    vector <float> *bsMass, *bsLxy, *bsPt, *bsEta, *bsPhi, *bsCtau, *bsCtau3D;

    TBranch *b_bsMass, *b_bsLxy, *b_bsPt, *b_bsEta, *b_bsPhi, *b_bsCtau, *b_bsCtau3D;
    TBranch *b_hltFired, *b_isTight, *b_hasMuon, *b_utility;

    private:

    PDSecondNtupleData           ( const PDSecondNtupleData& a );
    PDSecondNtupleData& operator=( const PDSecondNtupleData& a );

};

#endif

