///@file CQIMessage.h
///@brief  CQIMessage类声明
///@author wangsen
#pragma once
#include "../Utility/Include.h"
#include "../Utility/SBID.h"
class SBID;

/// @brief CQI消息类

class CQIMessage {
private:
    /// 记录各个subband上的MCS信息
    std::unordered_map<int, imat> m_mSubBandMCS;
    /// 全带宽MCS信息
    imat m_mWideBandMCS;
    /// 记录各个subband上的PMI信息
    std::unordered_map<int, int> m_mSubBandPMI;
    /// 全带宽PMI信息
    int m_iWideBandPMI;
    /// 全带宽信道的秩
    int m_iRank;
    /// 保存CQIMessage的生成时间
    int m_iBornTime;
    /// 保存Transformed CodeWord
    cmat m_mCovMat;

public:
    ///@brief 声明返回给定子带上MCS的函数接口
    ///@param _SubBandID 用于指定子带序号
    ///@return  返回一个指定子带上的MCS矩阵，考虑了有可能是双流的情况
    imat GetSubBandMCS(SBID _SubBandID);
    ///@brief 声明全带宽上MCS的函数接口
    ///@param void
    ///@return  返回全带宽上的MCS矩阵，考虑了有可能是双流的情况
    imat GetWideBandMCS();
    ///@brief 声明返回子带上PMI的函数接口
    ///@param _SubBandID 用于指定子带序号
    ///@return  返回一个指定子带上使用的PMI
    int GetSubBandPMI(const SBID& _SubBandID);
    ///@brief 声明返回全带宽PMI的函数接口
    ///@param void
    ///@return  返回全带宽上使用的PMI
    int GetWideBandPMI() const;
    ///@brief 声明返回RI的函数接口
    ///@param void
    ///@return  返回RI
    int GetRank() const;
    ///@brief 声明返回CQIMessage的生成时间的函数接口
    ///@param void
    ///@return  返回CQIMessage的生成时间
    int GetBornTime() const;

    ///@brief 声明设置子带上MCS的函数接口
    ///@param _SubBandID 用于指定子带序号
    ///@param _mMCS 指定子带序号上的MCS
    ///@return  void
    void SetSubBandMCS(const SBID& _SubBandID, imat _mMCS);
    ///@brief 声明设置全带宽上MCS的函数接口
    ///@param _mMCS 用于指定全带宽上的MCS
    ///@return  void
    void SetWideBandMCS(imat _mMCS);
    ///@brief 声明设置子带上使用的PMI的函数接口
    ///@param _SubBandID 用于指定子带序号
    ///@param _iPMI 用于指定子带序号上的PMI
    ///@return  void
    void SetSubBandPMI(const SBID& _SubBandID, int _iPMI);
    ///@brief 声明全带宽上使用的PMI的函数接口
    ///@param _iPMI 用于指定全带宽上的PMI
    ///@return  void
    void SetWideBandPMI(int _iPMI);
    ///@brief 声明设置RI的函数接口
    ///@param _iRI 传递RI
    ///@return  void
    void SetRank(int _iRI);
    ///@brief 声明设置m_mTransformR的函数接口
    ///@param _mTransformR 传递m_mTransformR
    ///@return  void
    void SetCovMat(cmat _mTransformR);
    ///@brief 声明获取m_mTransformR的函数接口
    ///@return  void
    cmat GetCovMat();

public:
    /// @brief 构造函数
    CQIMessage();
    /// @brief 析构函数
    ~CQIMessage() = default;
};
