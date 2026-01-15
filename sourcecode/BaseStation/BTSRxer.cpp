///@file BTSRxer.cpp
///@brief  BTS接收机的基类实现
///@author zhengyi

#include "../Utility/IDType.h"
#include "../Utility/SCID.h"
#include "../Utility/RBID.h"
#include "../Utility/functions.h"
#include "BTSID.h"
#include "../MobileStation/MSID.h"
#include "../MobileStation/HARQRxStateMS.h"
#include "../MobileStation/HARQTxStateMS.h"
#include "../MobileStation/MS.h"
#include "HARQRxStateBTS.h"
#include "HARQTxStateBTS.h"
#include "MSRxBufferBTS.h"
#include "MSTxBufferBTS.h"
#include "BTS.h"
#include "BTSRxer.h"
#include "../DetectAlgorithm/Detector_UL.h"
#include "../DetectAlgorithm/MRCAlgorithm.h"
#include "../DetectAlgorithm/IRCAlgorithm.h"
#include "../DetectAlgorithm/MMSEAlgorithm.h"

#include "../DetectAlgorithm/SimpleMMSE_UL.h"
//#include "../DetectAlgorithm/SimpleMMSE.h"

#include "../DetectAlgorithm/MMSE_UL.h"
#include "ACKNAKMessageUL.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../ChannelModel/LinkMatrix.h"

BTSRxer::BTSRxer(const BTSID& _bstid) {
    m_BTSID = _bstid;
    if (Parameters::Instance().SIM_UL.UL.IMIMOMode == Parameters::IMIMOMode_VirtualMIMO) {//虚拟mimo的情况下不能选择MRC算法
        if (Parameters::Instance().BTS_UL.UL.IDetectorMode == Parameters::IDetectorMode_MRC) {
            cout << "inputs ERROR: if in Virtual-MMIMO Mode ,the IDetectorMode could not be MRC !" << endl;
            assert(false);
        }
    } //zhengyi
    switch (Parameters::Instance().BTS_UL.UL.IDetectorMode) {
        case Parameters::IDetectorMode_MRC://0
            m_pDetector = std::shared_ptr<Detector_UL > (new MRCAlgorithm());
            break;
        case Parameters::IDetectorMode_IRC://1
            m_pDetector = std::shared_ptr<Detector_UL > (new IRCAlgorithm());
            break;
        case Parameters::IDetectorMode_SimpleMMSE://2
            m_pDetector = std::shared_ptr<Detector_UL > (new SimpleMMSE_UL());
            break;
        case Parameters::IDetectorMode_MMSE://3 mark
            m_pDetector = std::shared_ptr<Detector_UL > (new MMSEAlgorithm());
            break;
        default:
            assert(false);
    }
}

void BTSRxer::Initialize(
        vector<MSID>* _pActiveSet,
        HARQRxStateBTS* _pHARQRxStateBTS,
        deque<std::shared_ptr<SchedulingMessageUL> > *_q,
        std::unordered_map<int, MSRxBufferBTS>* _pRxBufferBTS) {
    m_pActiveSet = _pActiveSet;
    m_pHARQRxState_BTS_UL = _pHARQRxStateBTS;
    m_pSchedulingMessageQueueUL = _q;
    m_pRxBufferBTS = _pRxBufferBTS;
    ////////zhengyi 下面这三个都是干什么用的？ 看着有些不是很整齐///////////////
    m_vIoT.resize(Parameters::Instance().BASIC.ISCNum, 0);
    m_vAveIoT.resize(Parameters::Instance().BASIC.ISCNum, -1); //如果是-1则赋值，防止重复计算（因为VMIMO配对）
    m_vReEstimate.resize(Parameters::Instance().BASIC.ISCNum, itpp::zeros_c(Parameters::Instance().MIMO_CTRL.Total_TXRU_Num, Parameters::Instance().MIMO_CTRL.Total_TXRU_Num)); //zhengyi
    ////////zhengyi 下面这三个都是干什么用的？ 看着有些不是很整齐，是否需要划归到一个算法当中？///////////////
}

vector<vector<mat > > BTSRxer::ComputeEstSINR4MultiUE(vector<MSID> _vmsid,
        RBID _rbid_1,
        RBID _rbid_2) {
    vector<vector<mat > > temp;
    assert(false);
    //return temp;
}