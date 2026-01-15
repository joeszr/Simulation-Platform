///@file functions.h
///@brief  DB值与线性值之间的转换和线性插值函数的定义
///@author wangsen

#pragma once
#include <cmath>
//#include <boost/random.hpp>
//#include "../Utility/Include.h"
#include "../ChannelModel/functions.h"
#include "../ChannelModel/P.h"
#include <itpp/itbase.h>
#include "../Parameters/Parameters.h"
//#include"../MobileStation/MSID.h"

using namespace boost;
using namespace itpp;

/// 线性值到DB值的转换

double L2DB(double _dLinear);
mat L2DB(mat _mLinear);

/// DB值到现性值的转换

double DB2L(double _dB);
mat DB2L(mat _mdB);


///@brief 线性插值算法的函数声明
///@param _v 输入的插值向量，向量的元素可以是double,mat和cmat类型
///@param _iSpace 输入插值向量的有效数据之间的间隔，相邻两个元素之间的间隔为1，例如：1、0、3...则1和3之间的_iSpace=2
///@param _iOffset 输入插值向量的偏移，即第一个有效元素的位置
///@return void,但是该函数可以改变输入插值向量的值，插值后的结果仍然保存在输入参数_v中
template <typename T>
void Interpolate(vector<T>& _v, int _iSpace, int _iOffset)
{
    assert(_iOffset >= _iSpace || 0 == _iOffset);
    int iSize = static_cast<int> (_v.size());
    assert(_iSpace < iSize && _iOffset < iSize);
    ///偏移量为0时的情况
    if (_iOffset == 0)
    {
        int j = 0;
        for (int i = _iSpace; i < iSize; i = (i + _iSpace))
        {
            for (int k = 0; k < _iSpace; ++k)
            {
                T slope = (_v[i] - _v[i - _iSpace]);
                T temp = (static_cast<double> (k) / _iSpace) * slope + _v[i - _iSpace];
                _v[i - _iSpace + k] = temp;
            }
            ///记录循环条件结束时变量i的值，以便进行外插时使用
            j = i;
        }
        ///补充插值向量最后一个元素
        if (j == (iSize - 1))
        {
            _v[j] = _v[iSize - 1];
        }///根据循环条件结束时保存的i值，从第(j+1)个元素进行外插
        else {
            for (int k = 0; k < (iSize - j); ++k) {
                T slope = (_v[j] - _v[j - _iSpace]);
                T temp = (static_cast<double> (k) / _iSpace) * slope + _v[j];
                _v[j + k] = temp;
            }
        }
    }        ///偏移量大于0时的情况，偏移量不可能小于0
    else if (_iOffset > 0) {
        int j = 0;
        ///由于此时偏移量大于0，所以首先对第0个元素到第(_iOffset-1)个元素进行向左外插
        for (int k = _iOffset; k > 0; --k) {
            T slope = (_v[0 + _iOffset + _iSpace] - _v[0 + _iOffset]);
            T temp = (-1 * static_cast<double> (k) / _iSpace) * slope + _v[_iOffset];
            _v[_iOffset - k] = temp;
        }
        ///对第_iOffset个元素开始进行内插
        for (int i = (_iSpace + _iOffset); i < iSize; i = (i + _iSpace)) {
            for (int k = 0; k < _iSpace; ++k) {
                T slope = (_v[i] - _v[i - _iSpace]);
                T temp = (static_cast<double> (k) / _iSpace) * slope + _v[i - _iSpace];
                _v[i - _iSpace + k] = temp;
            }
            ///记录循环条件结束时变量i的值，以便进行外插时使用
            j = i;
        }
        ///补充插值向量最后一个元素
        if (j == (iSize - 1)) {
            _v[j] = _v[iSize - 1];
        }            ///根据循环结束时保存的变量i的值，从第(j+1)个元素进行向右外插
        else {
            for (int k = 0; k < (iSize - j); ++k) {
                T slope = (_v[j] - _v[j - _iSpace]);
                T temp = (static_cast<double> (k) / _iSpace) * slope + _v[j];
                _v[j + k] = temp;
            }
        }
    }
}

void Interpolate(vector<double>::iterator const _ITBegin, vector<double>::iterator const _ITEnd, int _iSpace, int _iOffset);

void Interpolate(vector<mat>::iterator const _ITBegin, vector<mat>::iterator const _ITEnd, int _iSpace, int _iOffset);

void Interpolate(vector<cmat>::iterator const _ITBegin, vector<cmat>::iterator const _ITEnd, int _iSpace, int _iOffset);

void InterpolateLog(vector<double>& _v, int _iSpace, int _iOffset);

void InterpolateLog(vector<mat>& _v, int _iSpace, int _iOffset);

void InterpolateLog(vector<double>::iterator const _ITBegin, vector<double>::iterator const _ITEnd, int _iSpace, int _iOffset);

void InterpolateLog(vector<mat>::iterator const _ITBegin, vector<mat>::iterator const _ITEnd, int _iSpace, int _iOffset);
///产生一个均匀分布的伪随机数
double xUniform(double _dmin = 0, double _dmax = 1);
double xUniform_channel(double _dmin = 0.0, double _dmax = 1.0);
/// 产生一个均匀分布的随机整数
/// @param _imin 随机整数的下界
/// @param _imax 随机整数的上界
int xUniformInt(const int _imin = 0, const int _imax = 1);
double xUniform_distributems(double _dmin = 0, double _dmax = 1);
double xUniform_msconstruct(double _dmin = 0, double _dmax = 1);
/// @brief 判断输入参数指定的时隙是下行时隙或上行时隙
/// @return 0---Downlink, 1---Uplink
int DownOrUpLink(int _iTime);

/// @brief 计算给定周期内有多少个上行slot
int GetULSlotNum(int Period);

// @brief Block Diagolization
void BD(const pair<cmat, cmat>& _rChannelMat, pair<cmat, cmat>& _rCodeWord, pair<double, double>& _rSLR, pair<double,double>& _rSD);
void BDR(const pair<cmat, cmat>& _rCovMat, pair<cmat, cmat>& _rCodeWord, pair<double, double>& _rSLR, pair<double,double>& _rSD);
// @brief BD for MUBF
void BD(const vector<cmat>& _vChannelMat, vector<cmat>& _vCodeWord);
void BDR(const vector<cmat>& _vChannelMat, vector<cmat>& _vCodeWord);
void BDR_RankA(const vector<cmat>& _vChannelMat, vector<cmat>& _vCodeWord, vector<int>& _vRank);
void BD_RankA(const vector<cmat>& _vChannelMat, vector<cmat>& _vCodeWord, vector<int>& _vRank);
void MET(const pair<cmat, cmat>& _rChannelMat, pair<cmat, cmat>& _rCodeWord, pair<double, double>& _rSLR, pair<double,double>& _rSD);

//20180720
void CalcL_for_MET_RankA(
        itpp::cmat& mH1_MET, int iRank, itpp::cmat& _mL1_MET, bool _bIsCovR);
void CalcL_for_MET_RankA(
        itpp::cmat& mV1_MET, itpp::vec vS1_MET ,int iRank, itpp::cmat& _mL1_MET, bool _bIsCovR);
void MET_RankA(
        const vector<cmat>& _vH, 
        vector<cmat>& _vCodeWord, 
        vector<int> _vRank, bool _bIsCovR = true);
//wxd:for FDD(2022.8.15)
void MET_RankA(
        const vector<cmat>& _vV,vector<itpp::vec> _vS,
        vector<cmat>& _vCodeWord,
        vector<int> _vRank, bool _bIsCovR = true) ;
void MET_Rank0(
        const vector<cmat>& _vH, 
        vector<cmat>& _vCodeWord, bool _bIsCovR = true);

//20171206
//cmat ProductmPPortleftsub();
cmat ProductmPPortleftsub_0();
cmat ProductPrecodeMat(const cmat& mCovRe);
cmat ChangeMatFull2Half(const cmat& InMatrix);
//20171206
//cmat ProductmPPortleftsub(const double _detiltRAD,const double _descanRAD,const int _iN_H_perPanel, const int _iN_V_perPanel, const int _iN_H_Total, const int _iN_V_Total, int PolarNum = 2);
cmat ProductmPPortleftsub(const vector<pair<double,double> >& vPairEtiltRADAndEscanRAD, const int _iN_H_perPanel, const int _iN_V_perPanel, const int _iN_H_Total, const int _iN_V_Total);
//cmat ProductmPPortleftsub(MSID _msid,map<MSID,pair<int,vector<int> > >_MSID2PanelAndvBeamIndex, int _iN_H_perPanel, int _iN_V_perPanel, int _iN_H_Total, int _iN_V_Total);
cmat ProductmPPortleftsub(const pair<int, vector<int> >& _PanelAndvBeamIndex, const int _iN_H_perPanel, const int _iN_V_perPanel, const int _iN_H_Total, const int _iN_V_Total);
cmat ProductmPPortleftsub(const vector<int>& vSelectedBeamIndex, const int _iN_H_perPanel, const int _iN_V_perPanel, const int _iN_H_Total, const int _iN_V_Total);
double BSBeamIndex2EtiltRAD(int _iBSBeamIndex);
double BSBeamIndex2EscanRAD(int _iBSBeamIndex);
double UEBeamIndex2EtiltRAD(int _iUEBeamIndex);
double UEBeamIndex2EscanRAD(int _iUEBeamIndex);
cmat ProductmPPortleft_UE(double dUEetiltRAD,double dUEescanRAD);

///20171129
//cmat ProductmPPortleftsub_old(const double _detiltRAD, const double _descanRAD, const int _iN_H_perPanel, const int _iN_V_perPanel, const int _iN_H_Total, const int _iN_V_Total) ;
//void ProductmPPortleftsub_test();
//void ProductmPPortleftsub_test_inner(const double _detiltRAD, const double _descanRAD, const int _iN_H_perPanel, const int _iN_V_perPanel, const int _iN_H_Total, const int _iN_V_Total, int PolarNum);

//20171206
//cmat ProductmPPortleftsub_with_details(const double _detiltRAD, const double _descanRAD, const int _iN_H_perPanel, const int _iN_V_perPanel, const int _iN_H_Total, const int _iN_V_Total, int PolarNum);
//void testformPPortleft_Of_BS();

//20171206
cmat ProductmPPortleftsub_for_TXRU(
    const double _detiltRAD, const double _descanRAD, 
    const int _H_AntNumPerTXRU, const int _V_AntNumPerTXRU, 
    const int _H_TXRU_Num, const int _V_TXRU_Num, 
    int _PolarNum = 2,
    double _DHAntSpace = cm::P::s().MacroTX.DHAntSpace, 
    double _DVAntSpace = cm::P::s().MacroTX.DVAntSpace);
//phi:AOD_GCS,theta:EOD_GCS
//inline double ALoSRAD_GCS2LCS(double phi,double theta,double alfa,double beta,double gamma = 0);
//
//inline double ELoSRAD_GCS2LCS(double phi,double theta,double alfa,double beta,double gamma = 0);
//
//inline double GetPhiRAD_GCS(double phi,double theta,double alfa,double beta,double gamma);
inline double ALoSRAD_GCS2LCS(double phi,double theta,double alfa,double beta,double gamma){
    double A_LCS = std::arg(std::complex <double>(cos(beta)*sin(theta)*cos(phi-alfa)-sin(beta)*cos(theta),
            cos(beta)*sin(gamma)*cos(theta)+(sin(beta)*sin(gamma)*cos(phi-alfa)+cos(gamma)*sin(phi-alfa))*sin(theta)));
    return A_LCS;
}//Calculate AOD\AOA\phi

inline double ELoSRAD_GCS2LCS(double phi,double theta,double alfa,double beta,double gamma){
    double E_LCS= acos(cos(beta)*cos(gamma)*cos(theta)+(sin(beta)*cos(gamma)*cos(phi-alfa)-sin(gamma)*sin(phi-alfa))*sin(theta));
    return E_LCS ;
}//Calculate EOD\EOA\theta

inline double GetPhiRAD_GCS(double phi,double theta,double alfa,double beta,double gamma){
    double dPhiRAD = std::arg(std::complex <double>(sin(gamma)*cos(theta)*sin(phi - alfa)
        +cos(gamma)*(cos(beta)*sin(theta)-sin(beta)*cos(theta)*cos(phi - alfa))
        ,sin(gamma)*cos(phi -alfa)+sin(beta)*cos(gamma)*sin(phi - alfa)));
    return dPhiRAD;
}

//cmat operator*(const cmat &m1, const cmat &m2);

void test_4CalcL_for_MET_RankA();
void test_4for_MET_RankA();