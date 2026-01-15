#include "../Utility/Include.h"
#include "PDCCH_Allocator.h"
class MSID;
class SchedulerDL;
class NewCompetor : public vector<MSID>{
public:
    vector<int> SBNumNeed;
    int SBNumAllocated;
    vector<SBID> vSBAllocated;
public:
    NewCompetor():SBNumAllocated(0){
        SBNumNeed.resize(4, 0);
    }
    NewCompetor(const vector<MSID>& _v){
        copy(_v.begin(), _v.end(), back_inserter(*this));
        SBNumNeed.resize(4, 0);
        SBNumAllocated = 0;
    }
};
class Mixed_DL_Scheduler : public SchedulerDL {
private:
    int RR_Index; //用于轮询调度
    int m_iMUNum;
    double PLWindowForPairing;
    int LCNum;

    PDCCH_Allocator m_PdcchAllocator;
    unordered_map<int, int>MSID2CCELevel;
    unordered_map<int, vector<int>>MSID2CCESB;
    deque<MSID>m_MSScheduleListRR;
private:
    int Getk1_slot(int _iTime);
    int DownOrUpLink_OS(int _iTime_OS);
/**********************************************************************
* 函数名称：Calc_UE_Priority
* 功能描述：计算每个用户在每个逻辑信道上的优先级
* 输入参数：_vMSSet：待调度的用户集合
          _MSTxBuffer：用户的下行状态信息缓存
* 输出参数：_vMSSet：按优先级排序后的用户队列
          _MSID2Priority：用户各个逻辑信道的优先级
          _MSID2MCS：用户本次调度的MCS等级
          _UnAvaliableMSID：没有业务的用户
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void Calc_UE_Priority(vector<MSID>& _vMSSet, std::unordered_map<int, MSTxBufferBTS>& _mMSTxBuffer, unordered_map<int, vector<double>>& _MSID2Priority, unordered_map<int, int>& _MSID2MCS, vector<MSID>& _UnAvaliableMSID);
/**********************************************************************
* 函数名称：TimeOutProcess
* 功能描述：将待重传的调度信息中packet超时的包删除
* 输入参数：_vMSSet：待调度的用户集合
          _MSTxBuffer：用户的下行状态信息缓存
          _HARQTxState：下行重传信息缓存
* 输出参数：_mMSID2HARQStateList:用户各个HARQ进程状态
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void TimeOutProcess(vector<MSID>& _vActiveSet, std::unordered_map<int, MSTxBufferBTS>& _MSTxBuffer, HARQTxStateBTS& _HARQTxState, std::unordered_map<int, vector<bool> >& _MSID2HARQStateList);
/**********************************************************************
* 函数名称：HARQ
* 功能描述：重传操作
* 输入参数：_vMSSet：待调度的用户集合
          _HARQTxState：下行重传信息缓存
          _vRBUsedFlag：下行资源位图
* 输出参数：_MSID2HARQID：用户分配的HARQID
          _ReTxMSID：重传用户集合
          _ReTxRBID2vMSID：RB到重传用户的映射
          _qScheduleMesQueue:存储调度信息的队列
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void HARQ(vector<MSID>& _vMSSet, HARQTxStateBTS& _HARQTxState,
              std::unordered_map<int, int>& _MSID2HARQID, vector<MSID>& _ReTxMSID,
              std::unordered_map<int, vector<MSID> >& _ReTxRBID2vMSID,
              SafeDeque<std::shared_ptr<SchedulingMessageDL> >& _qScheduleMesQueue, vector<vector<bool>>& _vRBUsedFlag);
/**********************************************************************
* 函数名称：GetUnAvaliableMS_CQI
* 功能描述：删除CQI消息为空的用户
* 输入参数：_vMSSet：待调度的用户集合
          _mMSTxBuffer：用户的下行状态信息缓存
* 输出参数：_UnAvaliableMSID:CQI消息为空的用户
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void GetUnAvaliableMS_CQI(vector<MSID>& _vMSSet, vector<MSID>& UnAvaliableMSID, std::unordered_map<int, MSTxBufferBTS>& _mMSTxBuffer);
/**********************************************************************
* 函数名称：GetUnAvaliableMS_DRX
* 功能描述：删除DRX非激活状态的用户
* 输入参数：_vMSSet：待调度的用户集合
* 输出参数：_UnAvaliableMSID:DRX非激活用户
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void GetUnAvaliableMS_DRX(vector<MSID>& _vMSSet, vector<MSID>& UnAvaliableMSID);
/**********************************************************************
* 函数名称：AllocateHARQID
* 功能描述：给每个用户分配HARQ进程，去除分不到的用户
* 输入参数：_vMSSet：待调度的用户集合
          _mMSID2HARQStateList:用户各个HARQ进程状态
* 输出参数：_UnAvaliableMSID:分不到HARQ进程的用户
          _mMSID2AvaiHARQID：用户分配到的HARQID
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void AllocateHARQID(vector<MSID>& _vMSSet, vector<MSID>& _UnAvaliableMSID, std::unordered_map<int, vector<bool> >& _mMSID2HARQStateList, std::unordered_map<int, int>& _mMSID2AvaiHARQID);
/**********************************************************************
* 函数名称：MUPairing
* 功能描述：多用户配对
* 输入参数：_vMSSet：待调度的用户集合
          _mMSTxBuffer:用户的下行状态信息缓存
* 输出参数：_vCompetors:配对后的“用户对”集合
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void MUPairing(vector<MSID>& _vMSSet, vector<NewCompetor>& _vCompetors, std::unordered_map<int, MSTxBufferBTS>& _mMSTxBuffer);
/**********************************************************************
* 函数名称：PDSCH_RBNum_Allocator
* 功能描述：计算用户需要的RB数
* 输入参数：_vCompetors:待调度的竞争体
          _mMSTxBuffer:用户的下行状态信息缓存
          _MSID2MCS:用户本次调度的MCS

* 输出参数：用户需要的RB数，存在_vCompetors中
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    //PDSCH数量分配
    void PDSCH_RBNum_Allocator(vector<NewCompetor>& _vCompetors, std::unordered_map<int, MSTxBufferBTS>& _mMSTxBuffer, unordered_map<int, int>& _MSID2MCS);
    //PDCCH
/**********************************************************************
* 函数名称：PDCCH_Allocate
* 功能描述：给每个用户分配PDCCH CCE,分不到的用户会被删除
* 输入参数：_vCompetors:待调度的竞争体
          ResourceGrid:下行资源位图
          _MSID2CCELevel：用户的CCE等级
* 输出参数： _MSID2CCERB：各用户的CCE对应的RB位置
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void PDCCH_Allocate(vector<NewCompetor>& _vCompetors, vector<vector<bool>>& ResourceGrid, unordered_map<int, int>& _MSID2CCELevel, unordered_map<int, vector<int>>& _MSID2CCERB);
    //PUCCH 保留
    void PUCCH_Allocator();
/**********************************************************************
* 函数名称：PDSCH_RBPos_Allocator
* 功能描述：给每个用户分配RB位置
* 输入参数：_vCompetors:待调度的竞争体
          _vSBSet:小区总资源集合
          ResourceGrid:下行资源位图
* 输出参数：分配RB的结果，存在_vCompetors中
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void PDSCH_RBPos_Allocator(vector<NewCompetor>& _vCompetors, vector<SBID>& _vSBSet, vector<vector<bool>>& ResourceGrid);
/**********************************************************************
* 函数名称：GenerateSchedulingMessage
* 功能描述：生成新传的调度信息
* 输入参数：_vCompetors:待调度的竞争体
          _mMSTxBuffer:用户的下行状态信息缓存
          mMSID2AvaiHARQID:用户分配到的HARQID
          MSID2MCS：用户本次调度的MCS
* 输出参数：_qScheduleMesQueue：调度信息队列
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void GenerateSchedulingMessage(vector<NewCompetor>& _vCompetors, std::unordered_map<int, MSTxBufferBTS>& _mMSTxBuffer, std::unordered_map<int, int>& mMSID2AvaiHARQID, unordered_map<int, int>& MSID2MCS, SafeDeque<std::shared_ptr<SchedulingMessageDL> >& _qScheduleMesQueue);
/**********************************************************************
* 函数名称：UpdateHistoryThrouthput
* 功能描述：更新用户的历史吞吐量
* 输入参数：_vActiveSet:本基站下的用户集合
* 输出参数：_mMSTxBuffer:用户的下行状态信息缓存
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void UpdateHistoryThrouthput(vector<MSID>&_vActiveSet, std::unordered_map<int, MSTxBufferBTS>&_mMSTxBuffer);
public:
    //@threads
    void Scheduling(BTSTxer* _pBTSTxer, vector<MSID>& _vActiveSet, HARQTxStateBTS& _HARQTxState, std::unordered_map<int, MSTxBufferBTS>& _mMSTxBuffer, SafeDeque<std::shared_ptr<SchedulingMessageDL> >& _qSchM, vector<vector<bool>>& _vSBUsedFlag);
public:
    explicit Mixed_DL_Scheduler(const BTSID& _MainServBTSID);
    ~Mixed_DL_Scheduler() override = default;
};

