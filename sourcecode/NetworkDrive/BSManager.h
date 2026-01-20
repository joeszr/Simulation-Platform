///@file BSManager.h
///@brief  基站管理类声明
///@author wangxiaozhou

#pragma once
#include "../Parameters/Parameters.h"
#include "../Utility/Include.h"
#include "../ChannelModel/Point.h"
#include "../ChannelModel/WrapAround.h"
#include"../BaseStation/BS.h"
#include "../Utility/Random.h"
class BTS;
class BS;
class BSID;
class BTSID;
//@threads
class Thread_control;

///基站管理类

class BSManager {
    //@threads
    friend Thread_control;
private:
    Random random;
public:
    ///存储所有基站的容器
    vector<BS> m_BSs;
    ///20260119
    vector<std::shared_ptr<BS> > m_vpBS;
    map<MacroID, vector<cm::Point> > mMacroID2ClustersPos2;
    std::unordered_map<int, vector<cm::Point> > mMacroID2ClustersPos;
    ///由BSID得到BS的引用

    BS& GetBS(const int& BSID) {
        return m_BSs[BSID];
    };

///20260119
    vector<std::shared_ptr<BS> >& GetvpBS() {
        return m_vpBS;
    };
    map<MacroID, vector<cm::Point> >& GetClusterPos() {
        return mMacroID2ClustersPos2;
    };

    ///由BTSID得到BTS引用
    BTS& GetBTS(const BTSID& _btsid);
    ///
    ///在系统中增加BS
    void AddBSs();
    ///在系统中增加RIS20260118
    void AddRISs();

    void DistributeRISs();

    ///初始化BS分布位置
    void DistributeBSs();
    ///基站初始化
    void InitializeBSs();
    ///获得系统中BS的数量

    int CountBS() const{
        return static_cast<int> (m_BSs.size());
    };
    ///获得BTS的数量


    ///
    void ConstructPicoGroups();

    ///基站管理类每TTI进行的操作
    void WorkSlot();
    ///重置BS管理类
    void Reset();
    void ReMove(){//重启一次仿真需要删除基站
        vector<BS> temp;
        m_BSs.swap(temp);
    }
    ///@brief 每个drop撒入Pico
    void DistributePicos_HetNet();
    ///@brief 具体确定每个Pico的位置
    void DistributeSinglePico_HetNet(const BTSID& btsid);
    ///@brief
    vector<cm::Point> DistributeOutdoorPicoClusters_ClusterHetNet(MacroID macroid, int _iClusterNumPerMacro);
    void DistributePicosInOneOutdoorCluster_ClusterHetNet(const MacroID& macroid, int _iClusterIndex, int _iPicoNumPerCluster);
    /// @brief 根据BTSID判断该BTS是否为Pico
    static bool IsPico(const BTSID& btsid);
    /// @brief 根据BTSID判断该BTS是否为Macro
    static bool IsMacro(const BTSID& btsid);
    ///@brief 取得pico所属的MacroID
    static MacroID GetMacroIDofPico(PicoID picoid);
    ///@brief 取得Macro的PicoIDs
    static vector<PicoID> GetPicoIDsofMacro(MacroID macroid);
    vector<cm::Point> GetClusterPos(const MacroID& macroid);
public:
    ///Singleton设计模式

    static BSManager& Instance() {
        static BSManager& m_BSM = *(new BSManager);
        return m_BSM;
    };
    BSManager(const BSManager&) = delete;
private:
    BSManager();
    BSManager & operator =(const BSManager&);
    ~BSManager() = default;
};
