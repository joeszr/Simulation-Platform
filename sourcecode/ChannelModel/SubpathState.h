/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file SubpathState.h
///@brief  SubpathState类声明
///
///包括构造函数，析构函数，初始化子径状态和WorkSlot函数的声明
///
///@author wangfei
#pragma once

#include "libfiles.h"
#include "../Utility/Random.h"
#include <unordered_map>

using namespace std;
namespace cm {
    class PathState;

    class CTXRU;

    /// Subpath m_state class
    class SubpathState {
    public:
        Random random;
        /// The parent path m_state, this subpath belongs to it
        PathState *m_pPathState;

        bool m_bIsLOS_Subpath;

        //for ChannelInfo
        int m_Subpath_Index;

    public:     // init at step 10
        /// The power fraction
        double m_Power;
        /// The phase offset
        double m_PhaseDegXX;
        double m_PhaseDegXY;
        double m_PhaseDegYX;
        double m_PhaseDegYY;

    public:
        void Init_Step10();

    public:     // init at step 8
        // set by Path

        /// The angle of departure offset
        double m_AODOffsetDeg;
        /// The angle of arrival offset
        double m_AOAOffsetDeg;
        /// The elevation angle of departure offset
        double m_EODOffsetDeg;
        /// The elevation angle of arrival offset
        double m_EOAOffsetDeg;

        // interface
        /// The angle of departure
        double m_AODDeg;
        /// The angle of arrival
        double m_AOADeg;
        /// The elevation angle of departure        
        double m_EODDeg;
        /// The elevation angle of arrival
        double m_EOADeg;

    protected:      // init at step 11 (new)
        // <_BS_TXRUIndex, _UE_TXRUIndex> --> TempD
        vector<vector<std::complex<double> >> m_TXRUPairID_2_cTempD;
        std::complex<double> BS_AggregateGain;
        std::complex<double> UE_AggregateGain;
        // _UE_PanelIndex --> TempB
        std::vector<std::complex<double> > m_cTempB_with_UEAntennaPanel;

        // _UE_PanelIndex --> UE_dH/dV
        std::vector<std::complex<double> > m_cUE_dH_Unit_withAntennaPanel;
        std::vector<std::complex<double> > m_cUE_dV_Unit_withAntennaPanel;

        // _BS_PanelIndex --> BS_dH/dV
        std::vector<std::complex<double> > m_cBS_dH_Unit_withAntennaPanel;
        std::vector<std::complex<double> > m_cBS_dV_Unit_withAntennaPanel;

        // _BS/UE_PanelIndex --> PhiRAD
        std::vector<double> m_BS_PhiRAD_with_AntennaPanel;

        // _BS/UE_PanelIndex --> AOG
        std::vector<double> m_BS_AOG_with_AntennaPanel;

        // _BS/UE_PanelIndex --> PhiRAD
        std::vector<double> m_UE_PhiRAD_with_AntennaPanel;

        // _BS/UE_PanelIndex --> AOG
        std::vector<double> m_UE_AOG_with_AntennaPanel;

    public:
        /// Initialize
        void Initialize_step11A();

    protected:
        std::complex<double> GetTempD_for_TXRUPair(
                std::shared_ptr<CTXRU> &_pBS_TXRU,
                std::shared_ptr<CTXRU> &_pUE_TXRU);

    public:
        std::complex<double> CalcSubpath_TimeH_for_TXRUPair(
                int BS_BeamIndex, int UE_BeamIndex,
                std::shared_ptr<CTXRU> &_pBS_TXRU,
                std::shared_ptr<CTXRU> &_pUE_TXRU,
                int _AntIndex_in_BS_TXRU,
                int _AntIndex_in_UE_TXRU,
                double _time_s);

        double CalSubpathCouplingLoss_of_Beampair_using_36873_8_1_new(
                int BS_BeamIndex, int UE_BeamIndex,
                std::shared_ptr<cm::CTXRU> &pBS_TXRU,
                std::shared_ptr<cm::CTXRU> &pUE_TXRU);

    public:
        void Initialize_step11B();

    protected:      // inner function being called in Initialize_step11A()
//        void Init_PhiRAD_and_AOG();
        void Init_PhiRAD_and_AOG_new();

//        void InitUnitVector_withAntennaPanel();

        void InitUnitVector_withAntennaPanel_new();

//        void InitTempB_withAntennaPanel();
        void InitTempB_withAntennaPanel_new();

//        void InitTempD_withTXRUPair();
        void InitTempD_withTXRUPair_new();

    public:
        void Set_LOS_Subpath() {
            m_bIsLOS_Subpath = true;
        }

    public:
        // Initialize
        void Initialize();
        //chty 1111 b
    private:
        bool m_bOnlyFirst;
        //chty 1111 b
        //record temp value to avoid duplicate calculation
        vector<vector<vector<vector<std::complex<double>>>>>
                m_vcSubpath_TimeH;//BS_AntennaPanelIndex-UE_AntennaPanelIndex-BSTXRU_Index-UETXRU_Index-cSubpath_TimeH
        vector<std::complex<double>> m_vcDelta_cSubpath_TimeH;//used to avoid e^n
        vector<vector<vector<vector<double>>>> m_vdLastUpdate;//last time when the Subpath is updated
        double m_dUpdateInterval;//s, not ms
        bool HaveAllocate;
        //chty 1111 e
    public:
        std::complex<double> InitialCalcSubpath_TimeH_for_TXRUPair(int BS_BeamIndex, int UE_BeamIndex,
                                                                   std::shared_ptr<CTXRU> &_pBS_TXRU,
                                                                   std::shared_ptr<CTXRU> &_pUE_TXRU,
                                                                   int _AntIndex_in_BS_TXRU,
                                                                   int _AntIndex_in_UE_TXRU,
                                                                   double _time_s);
        //chty 1111 e

    public:
        /// constructor
        SubpathState();

        /// destructor
        ~SubpathState() = default;
    };
}