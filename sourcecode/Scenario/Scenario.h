///@brief  场景类
///
///根据场景配置信道参数
///
///@author wanghanning

#pragma once
#include "../ChannelModel/Tx.h"
#include "../ChannelModel/Rx.h"
#include "../ChannelModel/PathLoss.h"
#include "../ChannelModel/BasicChannelState.h"
#include "../ChannelModel/SpaceChannelState.h"
#include "../ChannelModel/GaussianMap.h"
#include"../Statistician/Observer.h"
#include "../MobileStation/MS.h"

namespace cm{
    typedef std::pair<double, double> POS;
    typedef std::pair<POS, POS> KEY;
    class Scenario {
        friend class BasicChannelState;
        friend class SpaceChannelState;
    protected:
        std::shared_ptr<PathLoss> m_pPL;
        /// 用来产生可视径时延扩展相关性的随机分布地图
        std::shared_ptr<GaussianMap> m_pDSMapLOS;
        /// 用来产生可视径离开角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pAODMapLOS;
        /// 用来产生可视径到达角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pAOAMapLOS;
        /// 用来产生可视径垂直离开角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pEODMapLOS;
        /// 用来产生可视径垂直到达角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pEOAMapLOS;
        /// 用来产生可视径阴影衰落的随机分布地图
        std::shared_ptr<GaussianMap> m_pSFMapLOS;
        /// 用来产生可视径K值（LOS径功率分配相关）的随机分布地图
        std::shared_ptr<GaussianMap> m_pKMapLOS;
        /// 用来产生非可视径时延扩展相关性的随机分布地图
        std::shared_ptr<GaussianMap> m_pDSMapNLOS;
        /// 用来产生非可视径离开角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pAODMapNLOS;
        /// 用来产生非可视径到达角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pAOAMapNLOS;
        /// 用来产生非可视径垂直离开角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pEODMapNLOS;
        /// 用来产生非可视径垂直到达角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pEOAMapNLOS;
        /// 用来产生非可视径阴影衰落的随机分布地图
        std::shared_ptr<GaussianMap> m_pSFMapNLOS;
        /// 用来产生室外到室内时延扩展相关性的随机分布地图
        std::shared_ptr<GaussianMap> m_pDSMapO2I;
        /// 用来产生室外到室内离开角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pAODMapO2I;
        /// 用来产生室外到室内到达角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pAOAMapO2I;
        /// 用来产生室外到室内垂直离开角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pEODMapO2I;
        /// 用来产生室外到室内垂直到达角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pEOAMapO2I;
        /// 用来产生室外到室内阴影衰落的随机分布地图
        std::shared_ptr<GaussianMap> m_pSFMapO2I;
         /// 用来产生可视径时延扩展相关性的随机分布地图
        std::shared_ptr<GaussianMap> m_pDSMapPicoToUELOS;
        /// 用来产生可视径离开角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pAODMapPicoToUELOS;
        /// 用来产生可视径到达角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pAOAMapPicoToUELOS;
        /// 用来产生可视径阴影衰落的随机分布地图
        std::shared_ptr<GaussianMap> m_pSFMapPicoToUELOS;
        /// 用来产生可视径K值（LOS径功率分配相关）的随机分布地图
        std::shared_ptr<GaussianMap> m_pKMapPicoToUELOS;
        /// 用来产生非可视径时延扩展相关性的随机分布地图
        std::shared_ptr<GaussianMap> m_pDSMapPicoToUENLOS;
        /// 用来产生非可视径离开角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pAODMapPicoToUENLOS;
        /// 用来产生非可视径到达角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pAOAMapPicoToUENLOS;
        /// 用来产生非可视径阴影衰落的随机分布地图
        std::shared_ptr<GaussianMap> m_pSFMapPicoToUENLOS;
        /// 用来产生室外到室内时延扩展相关性的随机分布地图
        std::shared_ptr<GaussianMap> m_pDSMapPicoToUEO2I;
        /// 用来产生室外到室内离开角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pAODMapPicoToUEO2I;
        /// 用来产生室外到室内到达角角度扩展的随机分布地图
        std::shared_ptr<GaussianMap> m_pAOAMapPicoToUEO2I;
        /// 用来产生室外到室内阴影衰落的随机分布地图
        std::shared_ptr<GaussianMap> m_pSFMapPicoToUEO2I;
        /// 用来记录LOS或NLOS的标记量
        std::map<KEY, bool> m_LOSORNLOS;
    public:
        virtual void SetMSPara(MS* ms) = 0;
        /////////////////////////////////////LinkMatrix调用////////////////////////////////////////////////
        virtual double GenPos2Din(std::pair<int, int> pos);
        virtual std::shared_ptr<PathLoss> GetPathLossPtr(Tx& _tx, Rx& _rx, bool _bIsLOS) = 0;
        virtual std::shared_ptr<PathLoss> GetPathLossPtr(double _dInDoorDisM, bool _bIsLOS, int _iIslowloss, double _dPenetrationSFdb) = 0;
        ////////////////////////////////////////BCS调用////////////////////////////////////////////////////
        /// @brief 初始化地图变量
        virtual void InitializeMap();
        virtual bool DecideLOS(double _dDisM_2D, bool _bIsMacro2UE, double _h_UT = 1.5) = 0;
        virtual void ReadMapPos(Point& _tx, Rx& _rx, BasicChannelState* const bcs) = 0;
        virtual void SetSCSPara(BasicChannelState* const bcs, double _dTxHeight, double _dRxHeight) = 0;
        ////////////////////////////////////////SCS调用////////////////////////////////////////////////////
        virtual void InitializeAOD(SpaceChannelState* pSCS) = 0;
        virtual void InitializeAOA(SpaceChannelState* pSCS) = 0;
        virtual void InitializeEOD(SpaceChannelState* pSCS) = 0;
        virtual void InitializeEOA(SpaceChannelState* pSCS) = 0;

    protected:
        static Scenario* m_pScene;
    public:
        /// @brief 构造函数
        Scenario(void);
        /// @breif 析构函数
        ~Scenario(void);
        //单例
        static Scenario& Instance();
    };
}