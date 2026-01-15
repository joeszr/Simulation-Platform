#include "IDType.h"
#include "SBID.h"
#include "SCID.h"
#include "RBID.h"
#include "../Parameters/Parameters.h"
#include "../NetworkDrive/Clock.h"
bool operator==(const RBID & id1, const RBID & id2)  {
    return id1.ToInt() == id2.ToInt() ;
}

RBID::RBID(int _iRBID) : IDType(_iRBID) {
    //cout<<"test _iRBID"<<_iRBID<<endl;
    assert((_iRBID >= 0 && _iRBID < Parameters::Instance().BASIC.IRBNum) || _iRBID < Parameters::Instance().BASIC.IRBNum);
}

SBID RBID::GetSBID() {
    //下行专用，上行不用
    int iSBID = m_iID / Parameters::Instance().BASIC.ISBSize;
    SBID result(iSBID);
    return result;
}

SCID RBID::GetFirstSCID() const{//useless, every human should have  BASIC.IRBSize = BASIC_UL.IRBSize
    int iRBSize = Parameters::Instance().BASIC.IRBSize;

    int iFirstSCID = m_iID * iRBSize;
    SCID result(iFirstSCID);
    return result;
}

SCID RBID::GetLastSCID() const{
    int iRBSize = Parameters::Instance().BASIC.IRBSize;
    int iLastSCID = (m_iID + 1) * iRBSize - 1;
    SCID result(iLastSCID);
    return result;
}

RBID RBID::Begin() {
    return RBID(0);
}

RBID RBID::End() {
    int iRBNum = 0;
    int iTime = Clock::Instance().GetTimeSlot();
    if ((Parameters::Instance().BASIC.IDLORUL == Parameters::DL) && (DownOrUpLink(iTime) == 0 || DownOrUpLink(iTime) == 2)) {
        iRBNum = Parameters::Instance().BASIC.IRBNum;
    } else if ((Parameters::Instance().BASIC.IDLORUL != Parameters::DL) && (DownOrUpLink(iTime) == 1)) {
        iRBNum = Parameters::Instance().BASIC.IRBNum;
    }else if((Parameters::Instance().BASIC.IDLORUL == Parameters::ULandDL)){
        if( (DownOrUpLink(iTime) == 0 || DownOrUpLink(iTime) == 2) ){
            iRBNum = Parameters::Instance().BASIC.IRBNum;
        }else if((DownOrUpLink(iTime) == 1)){
            iRBNum = Parameters::Instance().BASIC.IRBNum;
        }
    }
    return RBID(iRBNum - 1);
}