/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file SpaceChannelState.h
///@brief  SpaceChannelState类声明
///
///包括构造函数，析构函数，空时信道的初始化和每个时隙的更新，初始化每条径的延时和AOD/AOA的函数
///以及子载波上快衰信道的更新函数的声明
///
///@author wangfei
#pragma once
#include<map>
#include<vector>
#include "libfiles.h"
#include "../Utility/Random.h"
using namespace itpp;
namespace cm {
    class CTXRU;
    class AntennaPanel;
    
    class PathState;

    class CAntMatrix_per_TXRU_pair;
    class CTXRUPairMatrix_per_subcarrier;
    class BasicChannelState;
    class ITU;
    class SpaceChannelState {
         typedef std::vector <PathState> VECPS;
        typedef std::vector <itpp::cmat> VECCMAT;

    public:
        // <itpp::cmat > --> H1 dim(RxAntNum_per_TXRU, TxAntNum_per_TXRU)
        // itpp::Mat<itpp::cmat > -->  X1 = <_BS_TXRUIndex, _UE_TXRUIndex> -> H1   
        // std::vector < itpp::Mat<itpp::cmat > > --> scid --> X1 
//        std::vector < CTXRUPairMatrix_per_subcarrier > m_TXRUPairID_2_FreqH;


        std::vector < itpp::cmat > m_TXRUPairID_2_FreqH_Matrix;
        std::map<std::pair<int, int>, std::vector<std::complex<double> > > m_TXRUPairID_2_FreqH; //n个频点
        std::map<std::pair<int, int>, std::vector<std::complex<double> > > m_TXRUPairID_2_FreqH1;
        std::map<std::pair<int, int>, std::vector<std::complex<double> > > m_TXRUPairID_2_FreqH_RIS;
        std::vector< std::map<std::pair<int, int>, std::vector<std::complex<double> > > > m_TXRUPairID_2_FreqH_BS2RIS;
        Random random;
        //服务RIS的振子相位
        itpp::cmat tempRIS;

    protected:
        // ABF = analog beamforming
        std::complex<double> GetH_after_ABF(int _scid, 
            std::shared_ptr<cm::CTXRU> _pBS_TXRU, 
            std::shared_ptr<cm::CTXRU> _pUE_TXRU,
            int _iBSBeamIndex, int _iUEPanelIndex);

        std::complex<double> GetH_after_ABF_with_BestBeams(int _scid,
            std::shared_ptr<cm::CTXRU> _pBS_TXRU,
            std::shared_ptr<cm::CTXRU> _pUE_TXRU);
        std::complex<double> GetH_after_ABF_with_BestBeams_RISonly(int _scid,
                std::shared_ptr<cm::CTXRU> _pBS_TXRU,
                std::shared_ptr<cm::CTXRU> _pUE_TXRU);
        
    public:
        itpp::cmat GetH_after_ABF_for_active_TXRU_Pairs_per_PanelPair(
            int _scid,
            AntennaPanel* _pBS_Panel,
            AntennaPanel* _pUE_Panel);

        itpp::cmat GetH_after_ABF_for_all_active_TXRU_Pairs(int _scid);
        itpp::cmat Get_mD_after_ABF_for_all_active_TXRU_Pairs(int _scid);
        itpp::cmat GetH_after_ABF_for_all_active_TXRU_Pairs_RISIntf_BestPanel(int _scid);



     public:
        /// Multi-path
        VECPS m_vPath;
        
        BasicChannelState* m_pBCS;

        int m_iStrongestUEPanelIndex;
        
    public:
        double m_dStrongestCouplingLoss_Linear;
        AntennaPanel* m_pBest_BS_Panel;
        AntennaPanel* m_pBest_UE_Panel;
        
        // <_BS_PanelIndex, _UE_PanelIndex> --> StrongestBSBeamInde/StrongestUEBeamInde
        itpp::Mat<int> m_PanelPairID_2_StrongestBSBeamIndex;
        itpp::Mat<int> m_PanelPairID_2_StrongestUEBeamIndex;
        std::map<std::pair<int,int>, double> mBeampair2Couplingloss_Linear;
    public:

        double GetCouplingloss(std::pair<int,int> beampair);
        //std::vector<std::pair<int, int> > GetAvailableBeampair();

        int GetStrongestBSBeamIndex(
                AntennaPanel* _pBS_Panel,
                AntennaPanel* _pUE_Panel);
        
        int GetStrongestUEBeamIndex(
                AntennaPanel* _pBS_Panel,
                AntennaPanel* _pUE_Panel);
        
    public:

        /// Time-driven interface, implements the process of each slot
        void WorkSlot(double _dTimeSec);
        void WorkSlot_RIS(double _dTimeSec, std::pair<int, int> beampair);
         void UpdateH(double time);

        /// Initialize
        void Initialize();

        void CalH();
        void CalH(std::pair<int, int> beampair);
    private:
        /// Initialize the delay for each path.
        void InitializePathDelay();
        /// Initialize the power for each path.
        void InitializePathPower();
        /// Initialize the angle of departure.
        ///for ITU implementation
        void InitializeAOD(ITU);
        /// Initialize the angle of arrival
        ///for ITU implementation
        void InitializeAOA(ITU);
        /// Initialize the elevation angle of departure.
        ///for ITU implementation
        void InitializeEOD(ITU);
        /// Initialize the elevation angle of arrival
        ///for ITU implementation
        void InitializeEOA();
        ////for step8//////////
        void step8(int path_index);
        
        void step9(int path_index);
        
        void step10(int path_index);
        
        void step11_A(int path_index);
        
        void step11_B(int path_index);
        
        double CalCouplingLoss_of_Beampair_using_36873_8_1_new(
            int BS_BeamIndex, int UE_BeamIndex,
            std::shared_ptr<cm::CTXRU>& pBS_TXRU,
            std::shared_ptr<cm::CTXRU>& pUE_TXRU );
        
        void CalculateRSRP_new();
        void CalculateRSRP_RIS();
        void StrongBeam(std::map<std::pair<int,int>,double> &mBeam2Couplingloss);
    protected:
        //void CalH
        void UpdateFreqH(double _dTimeSec, pair<int, int> beampair);

     public:
   	double CalCouplingLoss_linear_u36873_w_BestBeamPair_4_BestPanelPair();
        
        double CalCouplingLoss_linear_u36873_w_RandomBSBeam_4_BestPanelPair();
        
    public:    ///20171116 slimed H
        void UpdateH(double _dTimeSec, std::pair<int, int> beampair);
        void UpdateH_RIS(double _dTimeSec, std::pair<int, int> beampair);
        
    public:
        /// constructor
        explicit SpaceChannelState(BasicChannelState* _pBCS);
        /// copy constructor
//        SpaceChannelState(const SpaceChannelState&);
        /// destructor
        ~SpaceChannelState();
        
    protected:
        double m_H_updated_period_ms;
        double m_LastUpdateTime_ms;
        
//        void CalcH(double _dTimeSec); 
        void CalcFreqH(double _dTimeSec);

         vector<complex<double>> CalcFreqH1(double _dTimeSec, std::shared_ptr<cm::CTXRU> _pBS_TXRU,
                                            std::shared_ptr<cm::CTXRU> _pUE_TXRU, pair<int, int> beampair);

         void CalcFreqH_RIS(double _dTimeSec, std::pair<int, int> beampair);
        //chty 1111 b
    private:
        std::vector<std::vector<std::complex<double>>> m_vdTempFromDelayQua;//PathIndex-tn
        std::vector<cmat> m_vH;//scid-H
        std::vector<cmat> m_vD; //svd分解后的右奇异矩阵 暂时只用于下行
        std::vector<bool> m_vbIsLatest;//is m_vH up to date for given scid
        void UpdateH4AllSC();
        //chty 1111 e
    };
    
    /// Space channel m_state
    class CAntMatrix_per_TXRU_pair {
    protected:
        itpp::cmat m_mAntElements;
    public:
        CAntMatrix_per_TXRU_pair()=default;
        explicit CAntMatrix_per_TXRU_pair(int n) {assert(false);};
        CAntMatrix_per_TXRU_pair(
                int _RxAntNum_per_TXRU, int _TxAntNum_per_TXRU)
                : m_RxAntNum_per_TXRU (_RxAntNum_per_TXRU), 
                m_TxAntNum_per_TXRU (_TxAntNum_per_TXRU) {
            
            m_mAntElements = itpp::cmat( 
                _RxAntNum_per_TXRU, _TxAntNum_per_TXRU );
        }
        
        ~CAntMatrix_per_TXRU_pair() = default;
        
        std::complex<double>& operator() (
                int _RxAntIndex_per_TXRU, int _TxAntIndex_per_TXRU)  {
            return m_mAntElements(_RxAntIndex_per_TXRU, _TxAntIndex_per_TXRU);
        }
        
        operator itpp::cmat() {
            return m_mAntElements;
        }
        
    protected:
        int m_RxAntNum_per_TXRU;
        int m_TxAntNum_per_TXRU;
    };
    
    class CTXRUPairMatrix_per_subcarrier {
    protected:
        itpp::Mat<CAntMatrix_per_TXRU_pair > m_mTXRU_pairs;

    public:
        CTXRUPairMatrix_per_subcarrier(
                int _BS_TXRU_Num, int _UE_TXRU_Num, 
                int _RxAntNum_per_TXRU, int _TxAntNum_per_TXRU)
                : m_BS_TXRU_Num (_BS_TXRU_Num),
                m_UE_TXRU_Num (_UE_TXRU_Num) {
                    
            m_mTXRU_pairs.set_size(_BS_TXRU_Num, _UE_TXRU_Num, false);
            
            for(int i = 0; i < _BS_TXRU_Num; i++) {
                for(int j = 0; j < _UE_TXRU_Num; j++) {
                    m_mTXRU_pairs(i, j) = CAntMatrix_per_TXRU_pair(
                            _RxAntNum_per_TXRU, _TxAntNum_per_TXRU);
                }
            }
        }
                
        CAntMatrix_per_TXRU_pair& operator() (
                int _BS_TXRU_Index, int _UE_TXRU_Index)  {
            return m_mTXRU_pairs(_BS_TXRU_Index, _UE_TXRU_Index);
        }
        
        ~CTXRUPairMatrix_per_subcarrier() = default;
    protected:
        int m_BS_TXRU_Num;
        int m_UE_TXRU_Num;

    };
}