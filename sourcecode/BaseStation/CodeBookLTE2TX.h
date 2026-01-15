///@file CodeBookLTE2TX.h
///@brief  LTE2天线系统发送预编码类声明，继承于CodeBook类
///@author wangxiaozhou

#pragma once
#include "../Utility/Include.h"
#include "CodeBook.h"

using namespace itpp;
using namespace std;

/// @brief LTE的2天线码本

class CodeBookLTE2TX : public CodeBook {
public:
    /// LTE2天线发送单流码字
    vector<cmat> m_vCodebookRank0;
    /// LTE2天线发送双流码字
    vector<cmat> m_vCodebookRank1;

public:
    /// @see CodeBook::GetCodeWord
    cmat GetCodeWord(int _iPMI, int _iRank);
    /// @see CodeBook::GetTransformedCodeWord
    cmat GetCodeWord(cmat _mCovMat, int _iPMI, int _iRank);
    /// @see CodeBook::GetMaxRank
    int GetMaxRank();
    /// @see CodeBook::GetCodeNum
    int GetCodeNum(int _iRank);

public:
    /// @brief 构造函数
    CodeBookLTE2TX();
    /// @brief 析构函数
    ~CodeBookLTE2TX();
};