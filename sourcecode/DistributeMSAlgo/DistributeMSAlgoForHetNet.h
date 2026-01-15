/* 
 * File:   DistributeMSAlgoForPico.h
 * Author: zxy
 *
 * Created on September 17, 2010, 11:22 AM
 */

#pragma once
#include "DistributeMSAlgo.h"

///@zxy 在系统中加入picoout的情况下MS的撒点

class DistributeMSAlgoForHetNet  final: public DistributeMSAlgo {
public:
    ///@zxy 正六边形撒点算法的成员函数
    void DistributeMS(MSManager& _msm) final;
    ///单个MS撒点
    void DistributeMS(MS& _ms) final;
    ///@zxuy normal场景下MS的撒点
    void DistributeMSNormal(MSManager& _msm);
    ///@zxy 4b场景下MS的撒点
    void DistributeMS4b(MSManager& _msm);
    ///@zxy 4b场景下，picoout区域内部的MS撒点
    void DistributeMSInPicoZone(MS& ms,int iBSID,int iBTSID );
    ///@zxy 4b场景下PicoOut区域外部的MS撒点
    void DistributeMSOutPicoZone(MS& ms,int iBSID,int iBTSID );
    ///@zxy 单个MS的六边形撒点（与上面的单个MS撒点有微小差别）
    void DistributeMS(MS& _ms,int ibsid,int ibtsid,int iPicoOutNumPerBTS);
    ///@xlong 20110829
    void DistributeMSNormalHexagon(MSManager& _msm);
    void DistributeMSHexagon(MS& _ms);
    ///
    void DistributeMS_Cluster(MSManager& _msm);
    void DistributeMSInCluster_Cluster(MS& ms, const MacroID& _macroid, const cm::Point& _clusterPos);
    void DistributeMSInMacro_Cluster(MS& ms, const MacroID& _macroid);
public:
    ///此类的构造函数
    DistributeMSAlgoForHetNet() = default;
    ///此类的析构函数
    ~DistributeMSAlgoForHetNet() override = default;
};


