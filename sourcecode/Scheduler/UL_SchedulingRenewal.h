//
// Created by LAI on 2023/2/16.
//
#pragma once
#include "../Utility/Include.h"
#include "../SafeUnordered_map.h"
#include "PDCCH_Allocator.h"

class MSID;
class RBID;
class MSRxBufferBTS;
class HARQRxStateBTS;
class SchedulingMessageUL;
class SchedulerUL;

class Competor : public vector<MSID>{
public:
    vector<int> RBNeed;
    int RBNumAllocated;
    vector<RBID> vRBAllocated;
public:
    Competor():RBNumAllocated(0){
        RBNeed.resize(4, 0);
    }
    Competor(const vector<MSID>& _v){
        copy(_v.begin(), _v.end(), back_inserter(*this));
        RBNeed.resize(4, 0);
        RBNumAllocated = 0;
    }
};

typedef int RBSID;
typedef pair<MSID, RBSID> MSRBSIdPair;
//typedef pair<MSID, RBSID> MSRBSIDPair;

class UL_SchedulingRenewal: public SchedulerUL{

public:

private:
    bool mDebug;

    //上行能够使用的RB数量
    int iRBNum;

    //PDCCH分配（cwq）
    PDCCH_Allocator m_PdcchAllocator;
    unordered_map<int, int>MSID2CCELevel;
    unordered_map<int, vector<int>>MSID2CCESB;

    //暂时仿照（信道相关性）对MU进行配对的Pathloss门限
    double PLWindowForPairing;

    //MUMIMO最大用户配对数
    int m_iMUNum;

    //逻辑信道组的数目
    int LCGNum;

//    //UE在不同LCG上的优先级（废弃）
    vector<pair<int, pair<int, double>>> MSID2LCG_Priority;
//    //LCG上UE的优先级（废弃）
    unordered_map<int,unordered_map<int,double>> LCG2MSID_Priority;
//    //LCG上UE的需要的RB数（废弃）
    unordered_map<int,unordered_map<int,int>> LCG2MSID_RBNeed;

    //UE的RB限制
    unordered_map<int, int> MSID2RBLimit;

    //UE的优先级队列
    unordered_map<int, vector<double>> MSID2Priority;


    //UE在不同LCG上的历史吞吐量,维护在MAC_BTS
//    unordered_map<int,unordered_map<int,double>> LCG2MSID_HistoryThroughput;
    unordered_map<int,unordered_map<int,double>> LCG2MSID_HistoryThroughput;

    //获取基站的BSR，MSID-LCG-Buffer
    unordered_map<int,unordered_map<int,double>> MSID2LCGBuffer;

    //每个UE需要的RB数，每个LCG上需要的RB数之和
    unordered_map<int, int> MSID2RBNeed;

    //每个UE每个LCG上需要的RENum（废弃）
    unordered_map<int, unordered_map<int, int>> MSID2LCGRB_Need;

    //每个UE分配的新传的HARQID
    std::unordered_map<int, int> mMSID2AvaiHARQID;

    //每个UE的MCS
    unordered_map<int, itpp::imat> MSID2MCS;

    //每个UE的MCS
    unordered_map<int, int> MSID2iMCS;

    //每个UE的预估SINR
    unordered_map<int, double> MSID2EstimatedSINR;

    //MU配对主用户(废弃）
    vector<MSID> MUMainMS;

    //MU配对低优先级用户，用于保存已经被配对用户，防止重复配对
    vector<MSID> MUPairMS;

    //利用竞争体保存MU相关信息
    vector<Competor> vCompetors;

    //MU配对flag矩阵
    unordered_map<int, unordered_map<int, bool>> MUAvaliableFlag;  //MUAvaliableFlag[i][j] = 0 表示主用户i和配对用户j不能配对， 1表示可以配对, 这里没有用vector因为MSID不是连续的

    //用以存储两两用户之间的路损差值(废弃）
    unordered_map<int, unordered_map<int, double>> MSID2MSID_PLDiffer;

    vector<MSID> vReMSID;
    vector<RBID> vReRBID; //重传的RB集合的ID

    //小区资源位图
    vector<vector<vector<bool>>> vRBUsedFlag;

    //UE最高的逻辑信道组ID
    unordered_map<int, int> MSID2LCGIndex;

    unordered_map<int, int> MSID2K2SlotAllocated;

    deque<MSID>m_MSScheduleListRR; //用于轮询调度

public:
    void Scheduling(vector<MSID> &_vActiveSet,
                    HARQRxStateBTS &_HARQRxStateBTS,
                    std::unordered_map<int, MSRxBufferBTS> &_mMSRxBuffer,
                    deque<std::shared_ptr<SchedulingMessageUL> > &_qSchM,
                    vector<vector<vector<bool>>>& _vRBUsedFlagUL, vector<vector<bool>>& _vRBUsedFlagDL);

protected:

    void Reset();

    //记录该时刻的BSR信息
    void LogBSRInfo(int _iTime);

/**********************************************************************
* 函数名称：UpdateSRFlag
* 功能描述：对发送了SR的用户发送上行授权
* 输入参数：_vActiveSet：待调度的用户集合
          _vRBUsedFlagDL:下行资源位图
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void UpdateSRFlag(vector<MSID>& _vActiveSet, vector<vector<bool>>& _vRBUsedFlagDL);

/**********************************************************************
* 函数名称：DeleteUnAvaliableMS_DRX
* 功能描述：删除DRX非激活状态的用户
* 输入参数：_vMSSet：待调度的用户集合
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void DeleteUnAvaliableMS_DRX(vector<MSID>& _vMSSet);

/**********************************************************************
* 函数名称：ScheduleReTranList
* 功能描述：重传操作
* 输入参数：_vActiveSet：待调度的用户集合
          _HARQRxStateBTS：上行重传信息缓存
* 输出参数：vReMSID：重传用户
          vReRBID：重传用户占用的RB
          vRBUsedFlagUL：上行资源位图
          _vRBUsedFlagDL：下行资源位图
          _qSchM：调度信息队列
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void ScheduleReTranList(vector<MSID> &_vActiveSet,
                            HARQRxStateBTS& _HARQRxStateBTS,
                            vector<MSID> &vReMSID,
                            vector<RBID> &vReRBID,
                            vector<vector<vector<bool>>>& vRBUsedFlagUL,
                            vector<vector<bool>>& _vRBUsedFlagDL,
                            deque<std::shared_ptr<SchedulingMessageUL> >& _qSchM);

/**********************************************************************
* 函数名称：AvaiHARQIDState
* 功能描述：给新传用户分配HARQID
* 输入参数：_vActiveSet：待调度的用户集合
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void AvaiHARQIDState(vector<MSID> &_vActiveSet);

/**********************************************************************
* 函数名称：CalculatePriority
* 功能描述：计算用户在各个逻辑信道组的优先级
* 输入参数：_vActiveSet：待调度的用户集合
          _mMSRxBuffer：上行接收缓存
* 输出参数：_vActiveSet：按优先级排序后的用户队列
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void CalculatePriority(vector<MSID>& _vActiveSet,
                           std::unordered_map<int, MSRxBufferBTS>& _mMSRxBuffer);


/**********************************************************************
* 函数名称：CalculateMSID2EstimatedSINR
* 功能描述：根据SRS SINR计算用户的MCS
* 输入参数：_vActiveSet：待调度的用户集合
          _mMSRxBuffer：上行接收缓存
* 输出参数：MSID2MCS：用户的MCS等级
          MSID2EstimatedSINR：用户预估SINR
* 返 回 值：无
* 全局变量：MSID2MCS
          MSID2EstimatedSINR
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void CalculateMSID2EstimatedSINR(vector<MSID> &_vActiveSet,
                                     std::unordered_map<int, MSRxBufferBTS>& _mMSRxBuffer);

/**********************************************************************
* 函数名称：SortPriorityList
* 功能描述：按照优先级对用户队列进行排序
* 输入参数：_vActiveSetUpdate：待调度的用户集合
* 输出参数：_vActiveSetUpdate：排序后的用户队列
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void SortPriorityList(vector<MSID>& _vActiveSetUpdate);

/**********************************************************************
* 函数名称：UpdateQoS
* 功能描述：删除重传用户和没有buffer的用户
* 输入参数：_vActiveSet：待调度的用户集合
          _vReMSID：重传用户
* 输出参数：_vActiveSet：更新后的用户集合
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void UpdateQoS(vector<MSID>& _vActiveSet,
                   vector<MSID> _vReMSID
    );

/**********************************************************************
* 函数名称：CalculatePUSCHRBNum
* 功能描述：计算用户需要的RB数
* 输入参数：_vActiveSet：待调度的用户集合
          _mMSRxBuffer：上行接收信息缓存
* 输出参数：MSID2RBNeed：更新后的用户集合
* 返 回 值：无
* 全局变量：MSID2RBNeed
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void CalculatePUSCHRBNum(vector<MSID>& _vActiveSet,
                             std::unordered_map<int, MSRxBufferBTS>& _mMSRxBuffer);

/**********************************************************************
* 函数名称：AllocatePDCCH
* 功能描述：给待调度的用户分配PDCCH CCE
* 输入参数：_vActiveSet：待调度的用户集合
          _vRBUsedFlagDL：下行资源位图
* 输出参数：无
* 返 回 值：无
* 全局变量：
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void AllocatePDCCH(vector<MSID>& _vActiveSet, vector<vector<bool>>& _vRBUsedFlagDL);

    //PUCCH 保留
    void PUCCH_Allocator();

/**********************************************************************
* 函数名称：AllocateRBPosition
* 功能描述：给待调度的用户分配RB
* 输入参数：_vActiveSet：待调度的用户集合
          _vRBUsedFlagUL：上行资源位图
          _vRBUsedFlagDL：下行资源位图
* 输出参数：vCompetors：分配资源成功的竞争体（配对的用户组）集合
* 返 回 值：无
* 全局变量：vCompetors
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void AllocateRBPosition(vector<MSID>& _vActiveSet, vector<vector<vector<bool>>>& _vRBUsedFlagUL, vector<vector<bool>>& _vRBUsedFlagDL);
/**********************************************************************
* 函数名称：MakeMUPair
* 功能描述：MU配对
* 输入参数：_vActiveSet：待调度的用户集合
          _mMSRxBuffer：上行接收信息缓存
* 输出参数：MUAvaliableFlag：二维矩阵，两个用户能否配对的标识
* 返 回 值：无
* 全局变量：MUAvaliableFlag
* 修改记录：
* 其他说明：暂时采用通过PL配对，应采用信道相关性进行配对
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void MakeMUPair(vector<MSID>& _vActiveSet,
                    std::unordered_map<int, MSRxBufferBTS>& _mMSRxBuffer);

/**********************************************************************
* 函数名称：GenerateSchedulingMessage
* 功能描述：生成新传的调度信息
* 输入参数：_mMSRxBuffer：上行接收信息缓存
* 输出参数：_qSchM：调度信息队列
* 返 回 值：无
* 全局变量：
* 修改记录：
* 其他说明：暂时采用通过PL配对，应采用信道相关性进行配对
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void GenerateSchedulingMessage(std::unordered_map<int, MSRxBufferBTS>& _mMSRxBuffer,
                                   deque<std::shared_ptr<SchedulingMessageUL> >& _qSchM);

/**********************************************************************
* 函数名称：AllocateRBPositionPerCompetor
* 功能描述：新传用户RB分配
* 输入参数：vRBUsedFlagUL：上行资源位图
          _competor：待分配资源的竞争体（配对的用户组）
* 输出参数：_qSchM：调度信息队列
* 返 回 值：bool值，分配是否成功的flag
* 全局变量：
* 修改记录：
* 其他说明：暂时采用通过PL配对，应采用信道相关性进行配对
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    bool AllocateRBPositionPerCompetor(vector<vector<bool>> &vRBUsedFlagUL, Competor& _competor);
/**********************************************************************
* 函数名称：AllocateRBPositionPerCompetor
* 功能描述：重传用户RB分配
* 输入参数：vRBUsedFlagUL：上行资源位图
          _vReTxRBID：重传需要占用的RB
* 输出参数：
* 返 回 值：bool值，分配是否成功的flag
* 全局变量：
* 修改记录：
* 其他说明：暂时采用通过PL配对，应采用信道相关性进行配对
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    bool AllocateRBPositionPerCompetorReTx(vector<vector<bool>> &vRBUsedFlagUL, vector<RBID>& _vReTxRBID);

public:
    explicit UL_SchedulingRenewal(const BTSID& _MainServBTSID);

    ~UL_SchedulingRenewal() = default;
};
