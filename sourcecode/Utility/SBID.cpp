#include "IDType.h"
#include "RBID.h"
#include "SBID.h"
#include "SCID.h"
#include "../Parameters/Parameters.h"
bool operator==(const SBID & __x, const SBID & __y)  {
    return __x.ToInt() == __y.ToInt() ;
}
size_t SBIDHash(const SBID & p1)  {
    return std::hash<int>()(p1.ToInt()) ;
}
bool operator==(const std::pair<SBID, int> & __x, const std::pair<SBID, int> & __y)  {
    return __x.first.ToInt() == __y.first.ToInt()&&__x.second==__y.second ;
}
size_t pair_int_SBIDHash(const std::pair<SBID, int> & p1)  {
    return std::hash<int>()(p1.first.ToInt())^std::hash<int>()(p1.second) ;
}
SBID::SBID(int _iSBID) : IDType(_iSBID) {
    m_iFirstRBIndex=-1;
    m_iRBNum=-1;
    //下行专用，上行不用
    assert(_iSBID >= 0 && _iSBID < Parameters::Instance().BASIC.ISBNum);
}

SBID::SBID(int _iSBID, int _iFirstRBIndex, int _iRBNum)
: IDType(_iSBID), m_iFirstRBIndex(_iFirstRBIndex), m_iRBNum(_iRBNum) {
}

RBID SBID::GetFirstRBID() const{
    //下行专用，上行不用
    int iFirstRBID = m_iID * Parameters::Instance().BASIC.ISBSize;
    RBID result(iFirstRBID);
    return result;
}

RBID SBID::GetLastRBID() const{
    //下行专用，上行不用
    int iLastRBID = (m_iID + 1) * Parameters::Instance().BASIC.ISBSize - 1;
    int iRBNum = Parameters::Instance().BASIC.IRBNum;
    iLastRBID = iLastRBID > (iRBNum - 1) ? (iRBNum - 1) : iLastRBID;
    RBID result(iLastRBID);
    return result;
}

SCID SBID::GetFirstSCID() const{
    return GetFirstRBID().GetFirstSCID();
}

SCID SBID::GetLastSCID() const{
    return GetLastRBID().GetLastSCID();
}

SBID SBID::Begin() {
    return SBID(0);
}

SBID SBID::End() {
    //下行专用，上行不用
    return SBID(Parameters::Instance().BASIC.ISBNum - 1);
}

