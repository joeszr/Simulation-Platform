///@file CodeBookLTE4TX.h
///@brief  LTE4天线系统发送预编码类声明，继承于CodeBook类
///@author wangxiaozhou
#pragma once
#include "../Utility/Include.h"
#include "CodeBook.h"

using namespace itpp;
using namespace std;

/// @brief LTE的4天线码本类

class CodeBookLTE4TX : public CodeBook {
public:
    /// LTE4天线发送单流码字
    vector<cmat> m_vCodebookRank0;
    /// LTE4天线发送双流码字
    vector<cmat> m_vCodebookRank1;
    /// LTE4天线发送三流码字
    vector<cmat> m_vCodebookRank2;
    /// LTE4天线发送四流码字
    vector<cmat> m_vCodebookRank3;

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
    CodeBookLTE4TX();
    /// @brief 析构函数
    ~CodeBookLTE4TX();
};