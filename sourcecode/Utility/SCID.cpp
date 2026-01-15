#include "IDType.h"
#include "RBID.h"
#include "SBID.h"
#include "SCID.h"
#include "../Parameters/Parameters.h"
#include "../NetworkDrive/Clock.h"

bool operator==(const SCID & __x, const SCID & __y)  {
    return __x.ToInt() == __y.ToInt() ;
}

SCID::SCID(int _iSCID) : IDType(_iSCID) {
    if(Parameters::Instance().BASIC.IDLORUL==Parameters::DL){
        assert((_iSCID >= 0 && _iSCID < Parameters::Instance().BASIC.ISCNum));
    }
    else if(Parameters::Instance().BASIC.IDLORUL==Parameters::UL){
        assert( _iSCID < Parameters::Instance().BASIC.ISCNum);
    }
    else if(Parameters::Instance().BASIC.IDLORUL==Parameters::ULandDL) {
        assert((_iSCID >= 0 && _iSCID < Parameters::Instance().BASIC.ISCNum) || _iSCID < Parameters::Instance().BASIC.ISCNum);
    }
}
RBID SCID::GetRBID() {
    int iRBSize = 0;
    int iTime = Clock::Instance().GetTimeSlot();
    if ((Parameters::Instance().BASIC.IDLORUL == Parameters::DL) && (DownOrUpLink(iTime) == 0 || DownOrUpLink(iTime) == 2)) {
        iRBSize = Parameters::Instance().BASIC.IRBSize;
    } else if ((Parameters::Instance().BASIC.IDLORUL != Parameters::DL) && (DownOrUpLink(iTime) == 1)) {
        iRBSize = Parameters::Instance().BASIC.IRBSize;
    }
    else if((Parameters::Instance().BASIC.IDLORUL == Parameters::ULandDL)){
        if((DownOrUpLink(iTime) == 0 || DownOrUpLink(iTime) == 2)){
            iRBSize = Parameters::Instance().BASIC.IRBSize;
        }else if((DownOrUpLink(iTime) == 1)){
            iRBSize = Parameters::Instance().BASIC.IRBSize;
        }
    }
    int iRBID = m_iID / iRBSize;
    RBID result(iRBID);
    return result;
}

SBID SCID::GetSBID() {
    //下行专用，上行不用
    int iSBID = m_iID / (Parameters::Instance().BASIC.ISBSize * Parameters::Instance().BASIC.IRBSize);
    SBID result(iSBID);
    return result;
}

SCID SCID::Begin() {
    return SCID(0);
}

SCID SCID::End() {
    int iSCNum = Parameters::Instance().BASIC.ISCNum;
//    int iTime = Clock::Instance().GetTimeSlot();
//    if ((Parameters::Instance().BASIC.IDLORUL == Parameters::DL) && (DownOrUpLink(iTime) == 0 || DownOrUpLink(iTime) == 2)) {
//        iSCNum = Parameters::Instance().BASIC.ISCNum;
//    } else if ((Parameters::Instance().BASIC.IDLORUL != Parameters::DL) && (DownOrUpLink(iTime) == 1)) {
//        iSCNum = Parameters::Instance().SIM_UL.UL.ISCNum;
//    }
//    else if((Parameters::Instance().BASIC.IDLORUL == Parameters::ULandDL)){
//        if((DownOrUpLink(iTime) == 0 || DownOrUpLink(iTime) == 2)){
//            iSCNum = Parameters::Instance().BASIC.ISCNum;
//        }else if((DownOrUpLink(iTime) == 1)){
//            iSCNum = Parameters::Instance().SIM_UL.UL.ISCNum;
//        }
//    }
    return SCID(iSCNum - 1);
}