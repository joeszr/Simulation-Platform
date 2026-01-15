///@file MSRxer.cpp
///@brief  MS接收机的基类实现
///@author zhangyong

#include "../Utility/IDType.h"
#include "../Utility/SCID.h"
#include "../BaseStation/BTSID.h"
#include "../DetectAlgorithm/Detector_DL.h"
#include "../DetectAlgorithm/MRC_DL.h"
#include "../DetectAlgorithm/IRC.h"
#include "../DetectAlgorithm/SimpleMMSE_DL.h"
#include "../DetectAlgorithm/MMSE_DL.h"
#include "MSRxer.h"
#include "MSID.h"
#include "MS.h"
#include"../ChannelModel/LinkMatrix.h"
#include"../BaseStation/BTS.h"
//#include"../BaseStation/HARQRxStateBTS.h"

MSRxer::MSRxer(const BTSID& _MainservBTSID, const MSID& _msid, vector<mat> *_pSINR, HARQRxStateMS* _pHARQRxStateMS) {
    m_MainServBTS = _MainservBTSID;
    m_MSID = _msid;
    m_pSINR = _pSINR;
    m_pHARQRxStateMS = _pHARQRxStateMS;
    //m_dOLLAOffsetDB = 0.0;
    /*
        for(int i=0;i<Parameters::Instance().MIMO_CTRL.IMaxRankNum;i++){
            v_dOLLAOffsetDB.push_back(0);
        }
     */
    int iMaxRankNum = Parameters::Instance().MIMO_CTRL.IMaxRankNum;
    m_dOLLAOffsetDB.set_size(iMaxRankNum, 1);
    m_dOLLAOffsetDB = itpp::zeros(iMaxRankNum, 1);

    m_dOLLAOffset2DB = 0.0;
    m_vCovR = vector<cmat>(Parameters::Instance().BASIC.ISCNum / Parameters::Instance().LINK_CTRL.IFrequencySpace, zeros_c(Parameters::Instance().MSS.FirstBand.ITotalAntNumPerPanel, Parameters::Instance().MSS.FirstBand.ITotalAntNumPerPanel));
    switch (Parameters::Instance().MIMO_CTRL.IDetectorMode) {
        case Parameters::IDetectorMode_MRC:
            m_pDetector = std::shared_ptr<Detector_DL > (new MRC_DL());
            break;
        case Parameters::IDetectorMode_IRC:
            m_pDetector = std::shared_ptr<Detector_DL > (new IRC());
            break;
        case Parameters::IDetectorMode_SimpleMMSE:
            m_pDetector = std::shared_ptr<Detector_DL > (new SimpleMMSE_DL());
            break;
        case Parameters::IDetectorMode_MMSE:
            m_pDetector = std::shared_ptr<Detector_DL > (new MMSE_DL());
            break;
        default:
            assert(false);
            break;
    }
}

MSRxer::~MSRxer() {
    m_vCovR.clear();
}

void MSRxer::ReceiveScheduleMessage(std::shared_ptr<SchedulingMessageDL> _pSchM) {
    m_pScheduleMesDL = _pSchM;
}


void MSRxer::ApSounding() {

}

void MSRxer::CalInterference(vector<cmat>& vSIS, vector<cmat>& vSIH, vector<cmat>& vSIP, vector<cmat>& vWIS, vector<double>& vWIL,
        BTSID& _MainServBTS, MSID& _msid, SCID& _scid) {
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    MS& ms = _msid.GetMS();
    for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
        if (btsid == _MainServBTS)
            continue;
        ///按一定比例决定是否存在该干扰
        if (Parameters::Instance().BASIC.DInterfProb < 0.99) {
            if (random.xUniform() > Parameters::Instance().BASIC.DInterfProb)
                continue;
        }

        if (!btsid.GetBTS().IsExistTraffic())
            continue;

        BTS& bts = btsid.GetBTS();
        if (!bts.GetSBUsedFlag(_scid.GetSBID()))
            continue;
        if (lm.IsStrong(bts, ms)) {
            vSIS.push_back(bts.GetTxDataSymbol(_scid));

            vSIH.push_back(
                    lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs2(
                    bts, ms, _scid.ToInt(),
                    btsid.GetBTS().GetvUEPanelAndvBSBeam()));
            vSIP.push_back(bts.GetPrecodeCodeWord(_scid.GetSBID()));
        } else {
            vWIS.push_back(bts.GetTxDataSymbol(_scid));
            vWIL.push_back(DB2L(lm.GetCouplingLossDB(bts, ms)));
        }
    }
}

void MSRxer::CalStrongInterference(vector<cmat>& vSIS, vector<cmat>& vSIH, vector<cmat>& vSIP,
        BTSID& _MainServBTS, MSID& _msid, SCID& _scid) {
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    MS& ms = _msid.GetMS();
    for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
        if (btsid == _MainServBTS)
            continue;
        ///按一定比例决定是否存在该干扰
        if (Parameters::Instance().BASIC.DInterfProb < 0.99) {
            if (random.xUniform() > Parameters::Instance().BASIC.DInterfProb)
                continue;
        }

        if (!btsid.GetBTS().IsExistTraffic())
            continue;

        BTS& bts = btsid.GetBTS();
        if (!bts.GetSBUsedFlag(_scid.GetSBID()))
            continue;
        if (lm.IsStrong(bts, ms)) {
            //获取强干扰信号
            cmat mSIS = bts.GetTxDataSymbol(_scid);
//            cmat mSIS = bts.GetTxCRS(1);
            vSIS.push_back(mSIS);

            //获取强干扰信道
            cmat mSIH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs2(
                    bts, ms, _scid.ToInt(),
                    btsid.GetBTS().GetvUEPanelAndvBSBeam());
            vSIH.push_back(mSIH);

            //获取强干扰的预编码码字
            cmat mSIP = bts.GetPrecodeCodeWord(_scid.GetSBID());
//            cmat mSIP = 1 / sqrt(Parameters::Instance().MIMO_CTRL.Total_TXRU_Num) * itpp::ones_c(Parameters::Instance().MIMO_CTRL.Total_TXRU_Num, 1);
            vSIP.push_back(mSIP);
        }
    }
}

void MSRxer::CalWeakInterference(vector<cmat>& vWIS, vector<double>& vWIL,
        BTSID& _MainServBTS, MSID& _msid, SCID& _scid) {

    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    MS& ms = _msid.GetMS();

    for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
        if (btsid == _MainServBTS)
            continue;
        ///按一定比例决定是否存在该干扰
        if (Parameters::Instance().BASIC.DInterfProb < 0.99) {
            if (random.xUniform() > Parameters::Instance().BASIC.DInterfProb)
                continue;
        }

        if (!btsid.GetBTS().IsExistTraffic())
            continue;

        BTS& bts = btsid.GetBTS();
        if (!bts.GetSBUsedFlag(_scid.GetSBID()))
            continue;
        if (!lm.IsStrong(bts, ms)) {

            //获取弱干扰的信号
            cmat mWIS = bts.GetTxDataSymbol(_scid);
//            cmat mWIS = bts.GetTxCRS(1);
            vWIS.push_back(mWIS);

            //获取弱干扰大尺度衰落
            double dPLI = DB2L(lm.GetCouplingLossDB(bts, ms));
            vWIL.push_back(dPLI);
        }
    }
}