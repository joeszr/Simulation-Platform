///@file CodeBook.h
///@brief  预编码码本类声明
///@author wangxiaozhou
#pragma once
#include "../Utility/Include.h"
#include "itpp/itbase.h"

using namespace std;
using namespace itpp;

/// @brief 码本基类
///
/// 若干的码字可以组合成为一个码本，码本中的码字依靠预编码索引（Precoding Matrix Index，PMI）和
/// 信道的秩（rank）来查找。根据rank可以将码本中的码字分为1流码字，2流码字，...，N流码字。

class CodeBook {
public:
    /// @brief 根据PMI和Rank获得预编码码字
    /// @param _iPMI 预编码索引
    /// @param _iRank 预编码的秩
    virtual cmat GetCodeWord(int _iPMI, int _iRank)  = 0;
//    /// @brief 根据信道相关阵、PMI和Rank获得Transformed预编码码字
//    /// @param _mR 信道相关阵
//    /// @param _iPMI 预编码索引
//    /// @param _iRank 预编码的秩
//    virtual cmat GetCodeWord(cmat _mCovMat, int _iPMI, int _iRank) = 0;
    /// @brief 获得码本支持的最大Rank数
    virtual int GetMaxRank()  = 0;
    /// @brief 获得码本相应Rank包含的码字数
    virtual int GetCodeNum(int _iRank)  = 0;

public:
    /// @brief 构造函数
    CodeBook() = default;
    /// @brief 析构函数
    virtual ~CodeBook()  = default;
};
