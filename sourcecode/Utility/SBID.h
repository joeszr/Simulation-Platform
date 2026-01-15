///@file  SBID.h
///@brief SBID类的声明
///@author
#pragma once
#include "./IDType.h"

class IDType;
class RBID;
class SCID;

/// 子带的ID类型

class SBID : public IDType {
protected:
    int m_iFirstRBIndex;
    int m_iRBNum;
public:
    /// 取该子带的第一个RB的ID
    RBID GetFirstRBID() const;
    /// 取该子带的最后一个RB的ID
    RBID GetLastRBID() const;
    /// 取该子带的第一个子载波的ID
    SCID GetFirstSCID() const;
    /// 取该子带的最后一个子载波的ID
    SCID GetLastSCID() const;

    static SBID Begin();

    static SBID End();
public:
    /// 构造函数
    explicit SBID(int _iSBID);
    /// 构造函数

    explicit SBID(int _iSBID, int _iFirstRBIndex, int _iRBNum);
    /// 析构函数
    ~SBID() override =default;
};
bool operator==(const SBID & __x, const SBID & __y);
size_t SBIDHash(const SBID & p1);
bool operator==(const std::pair<SBID, int> & __x, const std::pair<SBID, int> & __y);
size_t pair_int_SBIDHash(const std::pair<SBID, int> & p1);