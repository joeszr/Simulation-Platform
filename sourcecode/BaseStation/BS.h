///@file BS.h
///@brief  BS类函数声明
///@author wangxiaozhou

#pragma once
#include "../Utility/Include.h"
#include "../ChannelModel/Point.h"
#include "GroupID.h"
#include "Group.h"
#include "../SafeUnordered_map.h"
#include "MSRxBufferBS.h"
#include "../Utility/Random.h"
#include "BTS.h"
using namespace itpp;

class GroupID;
class Group;
class BTS;
class MSID;
class BTSID;
class MSRxBufferBS;

/// @brief 基站类声明

class BS final : public cm::Point {
private:
    Random random;
    /// BS的ID标示
    BSID m_ID;
    /// BS的激活集(本站服务的用户集合)
    vector<MSID> m_vActiveSet;
    /// 存在于BS上的MS接收缓存
    std::unordered_map<int, MSRxBufferBS> m_mMSRxBufferBS;
    /// BS包含的BTS集合
    vector<BTS> m_vBTS;
    ///
    vector<vector<Group> > m_vPicoGroupIn3Sectors;
    
    vector<int> m_vMultiplexID;

public:
    /// @brief 获得属于该BS的对应BTS的引用
    BTS& GetBTS(int _iID);

    int CountBTS();
    /// @brief 返回BSID
    BSID GetID()const;

    /// @brief BS每个TTI要执行的内容
    void WorkSlot();
    /// @brief BS的初始化
    void Initialize();
    /// @brief 获得BS激活集中MS的数量
    int CountMS();
    /// @brief 重置BS
    void Reset();
    /// @brief 添加一个BTS到当前BS
    void AddBTSs();

    void AddRISs();

    void AddGroups(const vector<Group>& _vGroup);
    Group& GetGroup(const GroupID& _groupID);

    void WorkSlotPerBs();

    double bs_map_height=0;//2022-09-08 tengshiming added
    string bs_attenna_name;//2022-09-08 tengshiming added

public:
    /// @brief 构造函数
    explicit BS(int _id);
    /// @brief 析构函数
    ~BS() override = default;
};