///@file BS.cpp
///@brief  BS类函数实现
///@author wangxiaozhou

#include "../MobileStation/MSID.h"
#include "./MSRxBufferBTS.h"
#include "BTSID.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "ACKNAKMessageUL.h"
#include "BTS.h"
#include "GroupID.h"
#include "Group.h"
#include "../Utility/SCID.h"
#include "../Utility/RBID.h"
#include "BS.h"
//#include "../Statistician/Observer.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../NetworkDrive/Clock.h"
#include"../MobileStation/SRSTxer.h"
using namespace itpp;

BS::BS(int _id) : m_ID(_id), m_mMSRxBufferBS(Parameters::Instance().BASIC.ITotalMSNum){
}

void BS::AddBTSs() {
    m_vBTS.clear();
    for (int i = 0; i < Parameters::Instance().BASIC.ITotalBTSNumPerBS; ++i)
        m_vBTS.emplace_back(BTS(m_ID, i));
}

//20260115
void BS::AddRISs() {
    for (int i = 0; i < Parameters::Instance().BASIC.ITotalBTSNumPerBS; ++i){
        m_vBTS[i].AddRISs();
    }
}

void BS::AddGroups(const vector<Group>& _vGroup) {
    m_vPicoGroupIn3Sectors.push_back(_vGroup);
}

Group& BS::GetGroup(const GroupID& _groupID) {
    return m_vPicoGroupIn3Sectors[_groupID.GetSectorIndex()][_groupID.GetGroupIndex()];
}

void BS::Reset() {
    for( auto& bts: m_vBTS) {
        bts.Reset();
    }

    if (Parameters::Instance().BASIC.DWorkingMode
            == Parameters::WorkingMode_Normal) {
        m_mMSRxBufferBS.clear();
        m_vActiveSet.clear();
    }
}

void BS::Initialize() {
    for( auto& bts: m_vBTS) {
        bts.Initialize();
    }
}

void BS::WorkSlot() {
    if (Parameters::Instance().ERROR.ISRS_Error == 1) {
        int iCrashNum = SRSTxer::GetCrashNum();
        int iTime = Clock::Instance().GetTimeSlot();
        assert(Parameters::Instance().ERROR.ISRS_PERIOD % Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot == 0);
        int ISRS_OFFSET = Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLSlot;
        if (iTime % Parameters::Instance().ERROR.ISRS_PERIOD == ISRS_OFFSET) {
            m_vMultiplexID.clear();
            int iFoundRandNum = 0;
            const int MaxNum = 3;
            while (iFoundRandNum < MaxNum) {
                int iRandom = random.xUniformInt(0, iCrashNum - 1);
                if (find(m_vMultiplexID.begin(), m_vMultiplexID.end(), iRandom) == m_vMultiplexID.end()) {
                    m_vMultiplexID.push_back(iRandom);
                    iFoundRandNum++;
                }
            }

            for (int i = 0; i<static_cast<int> (m_vBTS.size()); ++i) {
                m_vBTS[i].m_iMultiplexID = m_vMultiplexID[i];
            }
        }
    }
    for( auto& bts: m_vBTS) {
        bts.WorkSlot();
    }
}

//void BS::operator()() {
//    WorkSlot();
//}

BTS& BS::GetBTS(int _iID) {
    return m_vBTS[_iID];
}

BSID BS::GetID()const {
    return m_ID;
}

int BS::CountMS() {
    return static_cast<int> (m_vActiveSet.size());
}

int BS::CountBTS() {
    return static_cast<int> (m_vBTS.size());
}

void BS::WorkSlotPerBs(){
    if (Parameters::Instance().ERROR.ISRS_Error == 1) {
        int iCrashNum = SRSTxer::GetCrashNum();
        int iTime = Clock::Instance().GetTimeSlot();
        assert(Parameters::Instance().ERROR.ISRS_PERIOD % Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot == 0);
        int ISRS_OFFSET = Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLSlot;
        if (iTime % Parameters::Instance().ERROR.ISRS_PERIOD == ISRS_OFFSET) {
            m_vMultiplexID.clear();
            int iFoundRandNum = 0;
            const int MaxNum = 3;
            while (iFoundRandNum < MaxNum) {
                int iRandom = random.xUniformInt(0, iCrashNum - 1);
                if (find(m_vMultiplexID.begin(), m_vMultiplexID.end(), iRandom) == m_vMultiplexID.end()) {
                    m_vMultiplexID.push_back(iRandom);
                    iFoundRandNum++;
                }
            }

            for (int i = 0; i<static_cast<int> (m_vBTS.size()); ++i) {
                m_vBTS[i].m_iMultiplexID = m_vMultiplexID[i];
            }
        }
    }
}