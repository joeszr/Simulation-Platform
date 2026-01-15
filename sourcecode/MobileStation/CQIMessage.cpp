///@file CQIMessage.cpp
///@brief CQIMessage类的实现
///@author wangsen
#include "../Utility/IDType.h"
#include "../Utility/SBID.h"
#include "../NetworkDrive/Clock.h"
#include "CQIMessage.h"

CQIMessage::CQIMessage():m_mSubBandMCS(Parameters::Instance().BASIC.ISBNum) ,m_mSubBandPMI(Parameters::Instance().BASIC.ISBNum){
    m_iBornTime = Clock::Instance().GetTimeSlot();
}

///@brief 定义返回给定子带上MCS的函数接口
///
///@param _SubBandID 用于指定子带序号
///@return  返回一个指定子带上的MCS矩阵，考虑了有可能是双流的情况

imat CQIMessage::GetSubBandMCS(SBID _SubBandID) {
    return m_mSubBandMCS[_SubBandID.ToInt()];
}

///@brief 定义返回全带宽上MCS的函数接口
///
///@param void
///@return  返回全带宽上的MCS矩阵，考虑了有可能是双流的情况

imat CQIMessage::GetWideBandMCS() {
    return m_mWideBandMCS;
}

///@brief 定义返回子带PMI的函数接口
///
///@param _SubBandID 用于指定子带序号
///@return  返回给定子带上使用的PMI

int CQIMessage::GetSubBandPMI(const SBID& _SubBandID) {
    return m_mSubBandPMI[_SubBandID.ToInt()];
}

///@brief 定义返回全带宽PMI的函数接口
///
///@param void
///@return  返回全带宽上使用的PMI

int CQIMessage::GetWideBandPMI() const{
    return m_iWideBandPMI;
}

///@brief 定义返回RI的函数接口
///
///@param void
///@return  返回RI

int CQIMessage::GetRank() const{
    return m_iRank;
}

///@brief 定义返回CQIMessage的生成时间的函数接口
///
///@param void
///@return  返回CQIMessage的生成时间

int CQIMessage::GetBornTime() const{
    return m_iBornTime;
}

///@brief 定义设置子带上MCS的函数接口
///
///@param _SubBandID 用于指定子带序号
///@param _mMCS 指定子带序号上的MCS
///@return  void

void CQIMessage::SetSubBandMCS(const SBID& _SubBandID, imat _mMCS) {
    m_mSubBandMCS[_SubBandID.ToInt()] = _mMCS;
}

///@brief 定义设置全带宽上MCS的函数接口
///
///@param _mMCS 用于指定全带宽上的MCS
///@return  void

void CQIMessage::SetWideBandMCS(imat _mMCS) {
    m_mWideBandMCS = _mMCS;
}

///@brief 定义设置子带上使用的PMI的函数接口
///
///@param _SubBandID 用于指定子带序号
///@param _iPMI 用于指定子带序号上的PMI
///@return  void

void CQIMessage::SetSubBandPMI(const SBID& _SubBandID, int _iPMI) {
    m_mSubBandPMI[_SubBandID.ToInt()] = _iPMI;
}

void CQIMessage::SetWideBandPMI(int _iPMI) {
    m_iWideBandPMI = _iPMI;
}

///@brief 定义设置RI的函数接口
///
///@param _iRI 传递RI
///@return  void

void CQIMessage::SetRank(int _iRI) {
    m_iRank = _iRI;
}

///@brief 声明设置m_mTransformR的函数接口
///@param _mTransformR 传递m_mTransformR
///@return  void

void CQIMessage::SetCovMat(cmat _mTransformR) {
    assert(_mTransformR.rows() == Parameters::Instance().Macro.ITotalAntNum && _mTransformR.cols() == Parameters::Instance().Macro.ITotalAntNum);
    m_mCovMat = _mTransformR;
}

///@brief 声明获取m_mTransformR的函数接口
///@return  void
cmat CQIMessage::GetCovMat(){
    assert(m_mCovMat.rows() == Parameters::Instance().Macro.ITotalAntNum && m_mCovMat.cols() == Parameters::Instance().Macro.ITotalAntNum);
    return m_mCovMat;
}