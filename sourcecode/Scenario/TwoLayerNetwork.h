///@brief  双层网络
///
///针对不同网络架构，实现不同初始化流程
///
///@author wanghanning

#pragma once
#include "Framework.h"

class TwoLayerNetwork : public Framework{
public:
    virtual void DistributeBSs();
    virtual void ConstructPicoGroups();
    ///@brief 每个drop撒入Pico
    void DistributePicos_HetNet();
    ///@brief 具体确定每个Pico的位置
    void DistributeSinglePico_HetNet(BTSID btsid);
    std::vector<cm::Point> OutdoorPicoClusters(MacroID macroid, int _iClusterNumPerMacro);
    void PicosInOutdoorCluster(MacroID macroid, int _iClusterIndex);
public:
    /// @brief 构造函数
    TwoLayerNetwork(void);
    /// @breif 析构函数
    ~TwoLayerNetwork(void);
};