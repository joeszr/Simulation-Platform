//
// Created by ChTY on 2022/10/10.
//

#ifndef CHTY_MAC_BTS_H
#define CHTY_MAC_BTS_H
#include <unordered_map>
#include "MobileStation/MAC/LCG.h"
#include <memory>
using namespace std;
class MAC_BTS {
private:
    std::shared_ptr<std::mutex> addmslock;//多个用户同时上报BSR，保证LCGbuffer线程安全
    unordered_map<int,unordered_map<int,double>> LCGbuffer;//记录MSID-LCGID-BUFFER
public:

    //UE在不同LCG上的历史吞吐量,应该维护在BTS端
    unordered_map<int,unordered_map<int,double>> LCG2MSID_HistoryThroughput;

    unordered_map<int,unordered_map<int,double>>& GetHistoryThroughput();

    void AccumulateSuccessRxKbit_LCG(const int& msid,  int LCGID, double _dKbit);

    // add by zhaoyan
/**********************************************************************
* 函数名称：checkLCGBuffer
* 功能描述：检查某个用户的所有LCG是否有缓存
* 输入参数：iUserId:用户id
* 输出参数：dBufferSize:缓存大小
* 返 回 值：int值，是否有缓存的标识,有返回0，没有返回-1
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    int  checkLCGBuffer(const int& iUserId, double& dBufferSize);
    // add by zhaoyan
/**********************************************************************
* 函数名称：RecieveBSR
* 功能描述：接收来自用户的BSR
* 输入参数：bsr:BSR消息
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void RecieveBSR(const std::shared_ptr<BSR>& bsr);
/**********************************************************************
* 函数名称：BSRIndexConvertion5bits
* 功能描述：将5bit的BSR 等级映射为具体的buffer大小
* 输入参数：index:BSR 等级
* 输出参数：无
* 返 回 值：double值，标识转换后的buffer大小
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    double BSRIndexConvertion5bits(const int& index);
/**********************************************************************
* 函数名称：GetSumBuffer
* 功能描述：获取用户的buffer总和
* 输入参数：msid:用户id
* 输出参数：无
* 返 回 值：double值，buffer大小
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    double GetSumBuffer(const int& msid);
/**********************************************************************
* 函数名称：GetBufferOfHighestPriority
* 功能描述：获取用户优先级最高且buffer>0的逻辑信道组的buffer
* 输入参数：msid:用户id
* 输出参数：无
* 返 回 值：double值，buffer大小
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    double GetBufferOfHighestPriority(const int& msid);
/**********************************************************************
* 函数名称：HaveAnyData
* 功能描述：用户是否有buffer
* 输入参数：msid:用户id
* 输出参数：无
* 返 回 值：bool值，是否有buffer的标识
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    bool HaveAnyData(const int& msid);
/**********************************************************************
* 函数名称：GetLCGIDOfHighestPriority
* 功能描述：获取用户优先级最高且buffer>0的逻辑信道组的ID
* 输入参数：msid:用户id
* 输出参数：无
* 返 回 值：int，LCG ID
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    int GetLCGIDOfHighestPriority(const int& msid);
/**********************************************************************
* 函数名称：ReduceBuffer
* 功能描述：减少用户的缓存
* 输入参数：msid:用户id
          datasize：减少的缓存大小
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void ReduceBuffer(const int& msid, double datasize);
    unordered_map<int,unordered_map<int,double>>& GetLGCbuffer();

    MAC_BTS();
};


#endif //CHTY_MAC_BTS_H
