//
// Created by AAA on 2023/3/7.
//
#include "../Utility/Include.h"
#include "../MobileStation/MSID.h"
#include "../BaseStation/BTSID.h"
#ifndef CHTY_PDCCH_ALLOCATOR_H
#define CHTY_PDCCH_ALLOCATOR_H


class PDCCH_Allocator {
private:
    BTSID m_BTSID;
public:
/**********************************************************************
* 函数名称：Allocate
* 功能描述：给用户分配PDCCH CCE
* 输入参数：msid:待分配的用户id
          ResourceGrid：资源位图
          DLorUL：上行或下行的指示
          MSID2CCELevel:用户的CCE等级
* 输出参数：MSID2CCERB：用户分到的CCE RB
* 返 回 值：false:分配失败；true：分配成功
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    bool Allocate(MSID msid, vector<vector<bool>>& ResourceGrid, unordered_map<int, int>& MSID2CCELevel, unordered_map<int, vector<int>>& MSID2CCERB, int DLorUL); //DLorUL:表示是上行调度分配还是下行调度分配
public:
    PDCCH_Allocator() = default;
    PDCCH_Allocator(const BTSID& btsid)
    {
        m_BTSID = btsid;
    }
    ~PDCCH_Allocator() = default;
};


#endif //CHTY_PDCCH_ALLOCATOR_H
