///@file  RBID.h
///@brief RBID类的声明
///
///包括构造函数，析构函数，返回本RB归属的SB的ID，返回本RB的第一个和最后一个SC的ID
///@author
#pragma once
class IDType;
class SBID;
class SCID;

/// Resource Block 的ID类型

class RBID : public IDType {
public:
    /// 返回本RB归属的Sub Band ID
    SBID GetSBID();
    /// 返回本RB的第一个子载波的ID
    SCID GetFirstSCID() const;
    /// 返回本RB的最后一个子载波的ID
    SCID GetLastSCID() const;

    static RBID Begin();

    static RBID End();
public:
    /// 构造函数
    explicit RBID(int _iRBID);
    /// 析构函数
    ~RBID() = default;
};
bool operator==(const RBID & , const RBID &);
