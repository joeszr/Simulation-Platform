///@file SRSTxer.cpp
///@brief  SRS发射机的实现
///@author wangfei

#include "../Utility/IDType.h"
#include "MSID.h"
#include "../BaseStation/BTSID.h"
#include "../NetworkDrive/BSManager.h"
#include "../NetworkDrive/MSManager.h"
#include "ACKNAKMessageDL.h"
#include "../ChannelModel/LinkMatrix.h"
#include "CQIMessage.h"
#include "../BaseStation/BTS.h"
#include "MS.h"
#include "../Utility/RBID.h"
#include "SRSTxer.h"


    int SRSTxer::m_iCrashNum =-1;
    
SRSTxer::SRSTxer(const BTSID& _MainServBTSID, const MSID& _msid) : m_FirstRB(0), m_LastRB(0) {
    m_MainServBTSID = _MainServBTSID;
    m_MSID = _msid;
    m_dSRSTxPowerMW_per_SC = 0;
}

void SRSTxer::Initialize() {
    // 0----全干扰
    // 1----1/2干扰
    // 2----1/4干扰	
    // 3----1/8干扰	
    switch (Parameters::Instance().ERROR.ISRS_COMB) {
        case 0:
            m_iCombNum = 1;
            break;
        case 1:
            m_iCombNum = 2;
            break;
        case 2:
            m_iCombNum = 4;
            break;
        case 3:
            m_iCombNum = 8;
            break;
        default:
            assert(false);
            break;
    }

    m_iRefSRSPeried_slot = Parameters::Instance().ERROR.ISRS_PERIOD; // LTE: 20ms (20slot); NR: 20ms (40slot)

    // LTE: 2 OS per 5ms (5 slot); NR: 4 OS per 5ms (10 slot)
    m_dRefOSNum_per_slot = Parameters::Instance().ERROR.DSRS_OSNumPerSlot;

    // LTE: 20MHz ; NR: 100MHz 
    DSRS_SystemBW_MHz = Parameters::Instance().ERROR.DSRS_SystemBW_MHz;

    double SCS = Parameters::Instance().BASIC.DCarrierWidthHz;
    // LTE: 100RB ; NR: 250RB
    IRBNUM_in_SystemBW = 100 * DSRS_SystemBW_MHz / 20 / (SCS / 15000);

    // LTE: 5MHz ; NR: 20MHz 
    DSRS_SRS_UnitBW_MHz = Parameters::Instance().ERROR.DSRS_SRS_UnitBW_MHz;

    // LTE: 25RB ; NR: 50RB 
    m_iRB_per_SRS = IRBNUM_in_SystemBW / (DSRS_SystemBW_MHz / DSRS_SRS_UnitBW_MHz);

    m_iRxAntNum = Parameters::Instance().MSS.FirstBand.ITotalAntNumPerPanel; // LTE: 2; NR: 4

    // m_iCrashNum = m_iCombNum * (m_iRefSRSPeried_slot * m_iRefOSNum_per_slot) / m_iRxAntNum
    m_iCrashNum = m_iCombNum * (m_iRefSRSPeried_slot * m_dRefOSNum_per_slot) / m_iRxAntNum;

    // m_iMultiplexID = 0 ... m_iCrashNum - 1
    m_iMultiplexID = randi(0, m_iCrashNum - 1);
    //cout<<m_iMultiplexID<<endl;

    /*
    
    
        //注意：该函数必须在已经完全确定每个MS由哪个BTS服务以后才能执行，即在所有MS都执行完MS的Initialize()之后
        BTS& bts = BSManager::Instance().GetBTS(m_MainServBTSID);
        int iMSNum = bts.GetConnectedNum();
        vector<MSID> vActiveSet = bts.GetActiveSet();
        int iMSIndex = -1;
        for(int i=0;i<iMSNum;++i){
            if(m_MSID==vActiveSet[i]){
                iMSIndex = i;
                break;
            }
        }
        //初始化时用全带宽，但是m_iBandIndex需要根据配置区分
        m_FirstRB = RBID::Begin();
        m_LastRB = RBID::End();
        if(iMSIndex < iMSNum/2){
            m_iGroupID = 0;
            if(Parameters::Instance().ERROR.ISRS_WIDTH == 1){
                m_iBandIndex = 0;  
            }else if(Parameters::Instance().ERROR.ISRS_WIDTH == 2){
                if(iMSIndex<iMSNum/4){
                    m_iBandIndex = 0;
                }else{
                    m_iBandIndex = 1;
                }
            }else if(Parameters::Instance().ERROR.ISRS_WIDTH == 4){
                if(iMSIndex<iMSNum/8){
                    m_iBandIndex = 0;
                }
                else if(iMSIndex<(2*iMSNum)/8){
                    m_iBandIndex = 1;
                }
                else if(iMSIndex<(3*iMSNum)/8){
                    m_iBandIndex = 2;
                }
                else{
                    m_iBandIndex = 3;
                }
            }else{
                cout<<"Error! The parameter ERROR.ISRS_WIDTH is wrong!"<<endl;
                assert(false);
            }
        }else{
            m_iGroupID = 1;
            if(Parameters::Instance().ERROR.ISRS_WIDTH == 1){
                m_iBandIndex = 0;
            }else if(Parameters::Instance().ERROR.ISRS_WIDTH == 2){
                if(iMSIndex<(3*iMSNum)/4){
                    m_iBandIndex = 0;
                }else{
                    m_iBandIndex = 1;
                }
            }else if(Parameters::Instance().ERROR.ISRS_WIDTH == 4){
                if(iMSIndex<(5*iMSNum)/8){
                    m_iBandIndex = 0;
                }
                else if(iMSIndex<(6*iMSNum)/8){
                    m_iBandIndex = 1;
                }
                else if(iMSIndex<(7*iMSNum)/8){
                    m_iBandIndex = 2;
                }
                else{
                    m_iBandIndex = 3;
                }
            }else{
                cout<<"Error! The parameter ERROR.ISRS_WIDTH is wrong!"<<endl;
                assert(false);
            }
        }


        if(Parameters::Instance().ERROR.ISRS_COMB == 0){
            m_iGroupID = 0;
        }else if(Parameters::Instance().ERROR.ISRS_COMB == 1){
            if(iMSIndex < iMSNum/2){
                m_iGroupID = 0;
            }else{
                m_iGroupID = 1;
            }
        }else if(Parameters::Instance().ERROR.ISRS_COMB == 2){
            if(iMSIndex < iMSNum/4){
                m_iGroupID = 0;
            }else if(iMSIndex < iMSNum/2){
                m_iGroupID = 1;
            }else if(iMSIndex < (3*iMSNum)/4){
                m_iGroupID = 2;
            }else{
                m_iGroupID = 3;
            }
        }else if(Parameters::Instance().ERROR.ISRS_COMB == 3){
            if(iMSIndex < iMSNum/8){
                m_iGroupID = 0;
            }else if(iMSIndex < (2*iMSNum)/8){
                m_iGroupID = 1;
            }else if(iMSIndex < (3*iMSNum)/8){
                m_iGroupID = 2;
            }else if(iMSIndex < (4*iMSNum)/8){
                m_iGroupID = 3;
            }else if(iMSIndex < (5*iMSNum)/8){
                m_iGroupID = 4;
            }else if(iMSIndex < (6*iMSNum)/8){
                m_iGroupID = 5;
            }else if(iMSIndex < (7*iMSNum)/8){
                m_iGroupID = 6;
            }else{
                m_iGroupID = 7;
            }
        }else {
            cout<<"Error! The parameter ERROR.ISRS_WIDTH is wrong!"<<endl;
            assert(false);
        }
     */
}

void SRSTxer::UpdateMultiplexID() {
    // m_iMultiplexID = 0 ... m_iCrashNum - 1
    m_iMultiplexID = randi(0, m_iCrashNum - 1);
}

/*
int SRSTxer::GetGroupID(){
    return m_iGroupID;
}
 */
/*

int SRSTxer::GetBandIndex(){
    return m_iBandIndex;
}
 */

/*
void SRSTxer::SRSScheduling(){
    m_iBandIndex = (++m_iBandIndex)%Parameters::Instance().ERROR.ISRS_WIDTH;
    if(Parameters::Instance().ERROR.ISRS_WIDTH == 1){
        m_FirstRB = RBID::Begin();
        m_LastRB = RBID::End();
    }else if(Parameters::Instance().ERROR.ISRS_WIDTH == 2){
        if(m_iBandIndex == 0){
            m_FirstRB = RBID::Begin();
            m_LastRB = RBID((Parameters::Instance().BASIC.IRBNum)/2 - 1);
        }else{
            m_FirstRB = RBID((Parameters::Instance().BASIC.IRBNum)/2);
            m_LastRB = RBID::End();
        }
    }else if(Parameters::Instance().ERROR.ISRS_WIDTH == 4){
        if(m_iBandIndex == 0){
            m_FirstRB = RBID::Begin();
            m_LastRB = RBID((Parameters::Instance().BASIC.IRBNum)/4 - 1);
        }
        else if(m_iBandIndex == 1){
            m_FirstRB = RBID((Parameters::Instance().BASIC.IRBNum)/4);
            m_LastRB = RBID((Parameters::Instance().BASIC.IRBNum)/2 - 1);
        }
        else if(m_iBandIndex == 2){
            m_FirstRB = RBID((Parameters::Instance().BASIC.IRBNum)/2);
            m_LastRB = RBID(3*(Parameters::Instance().BASIC.IRBNum)/4 - 1);
        }
        else{
            m_FirstRB = RBID(3*(Parameters::Instance().BASIC.IRBNum)/4);
            m_LastRB = RBID::End();
        }
    }else{
        cout<<"Error! The parameter ERROR.ISRS_WIDTH is wrong!"<<endl;
        assert(false);
    }
}
 */

/*
RBID SRSTxer::GetFirstRB(){
    return m_FirstRB;
}

RBID SRSTxer::GetLastRB(){
    return m_LastRB;
}
 */

void SRSTxer::SRSPowerControl() {
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    BTS& bts = BSManager::Instance().GetBTS(m_MainServBTSID);
    MS& ms = MSManager::Instance().GetMS(m_MSID.ToInt());

    assert(lm.GetCouplingLossDB(bts, ms) < 0);
    double dRBSRSTxPowerMW_perPRB = DB2L(
            Parameters::Instance().MSS.FirstBand.UL.DP0Dbm4SRS
            + Parameters::Instance().MSS.FirstBand.UL.DAlfa4SRS
            * abs(lm.GetCouplingLossDB(bts, ms)));

    int iRBUsed = m_iRB_per_SRS;
    int iSRSSCNum = iRBUsed * Parameters::Instance().BASIC.IRBSize / m_iCombNum;

    double dTotalSRSTxPowerMW = dRBSRSTxPowerMW_perPRB * iRBUsed;
    dTotalSRSTxPowerMW = std::min(
            DB2L(Parameters::Instance().MSS_UL.UL.DMaxTxPowerDbm), dTotalSRSTxPowerMW);

    //输出功率到文档
    //if (iTime == 1) {
    //    Observer::Print("TXPower") << (Clock::Instance().GetTimeSlot())
    //            << setw(20) << m_MSID.ToInt()
    //            << setw(20) << dTotalSRSTxPowerMW
    //            << endl;
    //}
    m_dSRSTxPowerMW_per_SC = dTotalSRSTxPowerMW / iSRSSCNum;
}

double SRSTxer::GetSRSTxPowerMW_per_SC() {
    return m_dSRSTxPowerMW_per_SC;
}

int SRSTxer::GetCrashNum() {
    int iCombNum = -1;
    switch (Parameters::Instance().ERROR.ISRS_COMB) {
        case 0:
            iCombNum = 1;
            break;
        case 1:
            iCombNum = 2;
            break;
        case 2:
            iCombNum = 4;
            break;
        case 3:
            iCombNum = 8;
            break;
        default:
            assert(false);
            break;
    }
    int iRefSRSPeried_slot = Parameters::Instance().ERROR.ISRS_PERIOD;
    double dRefOSNum_per_slot = Parameters::Instance().ERROR.DSRS_OSNumPerSlot;
    int iRxAntNum = Parameters::Instance().MSS.FirstBand.ITotalAntNumPerPanel;
    m_iCrashNum = iCombNum * (iRefSRSPeried_slot * dRefOSNum_per_slot) / iRxAntNum;
    return m_iCrashNum;
}