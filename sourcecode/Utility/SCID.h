///@file  SCID.h
///@brief SCID类的声明
///
///包括构造函数，析构函数，获取子载波归属RB的ID，以及获取该子载波归属的SB的ID
///@author
#pragma once
#include "./IDType.h"
class RBID;
class SBID;

/// 子载波的ID类型

class SCID : public IDType {
public:
    /// 取该子载波归属的RB的ID
    RBID GetRBID();
    /// 取该子载波归属的SB的ID
    SBID GetSBID();

    static SCID Begin();

    static SCID End();
public:
    /// 构造函数
    explicit SCID(int _iSCID);
    /// 析构函数
    ~SCID() override = default;
};
bool operator==(const SCID & __x, const SCID & __y);
