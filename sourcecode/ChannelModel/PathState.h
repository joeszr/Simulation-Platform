/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathState.h
///@brief  PathState类声明
///
///包括构造函数，析构函数，SCM和ITU模型下各条径的状态初始化函数，WorkSlot函数以及信道矩阵的初始化和更新函数的声明

#pragma once
#include "classholder.h"
#include "../Utility/Random.h"
namespace cm{
    class BasicChannelState;
    class SubpathState;
    
    class CTXRU;
    
    /// Path State class
    class PathState{
    public:
        Random random;
        std::vector<SubpathState> m_vSubpath; 
        /// The parent space channel m_state, this path belongs to it
        BasicChannelState* m_pBCS; 
        
        std::shared_ptr<SubpathState> m_pLOS_Subpath;
        
        //for ChannelInfo
        int m_Path_Index;
        
    public:
        bool m_bIsFirstPath;
        
    public:     // init at Step9
        /// Interim variable
        double m_PowerPerRay;
        ///The coupled power P2 of each sub-path in the horizontal orientation
        ///is set relative to the power P1 of each sub-path in the vertical orientation
        ///according to the XPD ratio, defined as XPD = P1/P2. A single XPD ratio applies
        ///to all sub-paths of a given path
        double m_dXPD1; 
        double m_dXPD2;
        
    public:
        void Init_Step9();
        
    public:
        /// The non-quantized path delay
        double m_DelayRaw; 
        /// The path power fraction
        double m_Power; 
        double m_PowerNlos; 
        /// The quantized path delay
        double m_DelayQua; 
        
    public:    
        /// The path angle of departure
        double m_AODDeg_Path; 
        /// The path angle of arrival
        double m_AOADeg_Path;
        /// The path elevation angle of departure
        double m_EODDeg_Path;
        /// The path elevation angle of arrival
        double m_EOADeg_Path;
        
    public:

        void Build_LOS_Subpath();
        
    public:
        void SetFirstPath(bool _bIsFirstPath) {
            m_bIsFirstPath = _bIsFirstPath;
        }

    public:     
        std::complex<double> CalcPath_TimeH_for_TXRUPair(
            int BS_BeamIndex, int UE_BeamIndex,
            std::shared_ptr<CTXRU>& _pBS_TXRU,
            std::shared_ptr<CTXRU>& _pUE_TXRU,
            int _AntIndex_in_BS_TXRU, 
            int _AntIndex_in_UE_TXRU,
            double _time_s);
        
    public:
        void Init_Step8();
        
    public:
        void Initialize();

    public:
        /// constructor
        PathState();
        /// copy constructor
        PathState( const PathState& );
        /// destructor
        ~PathState() = default;
    };
}
