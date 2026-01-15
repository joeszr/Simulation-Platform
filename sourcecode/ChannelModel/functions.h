/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file functions.h
///@brief ChannelModel中用到的一些函数的声明
///
///@author wangfei
#pragma once
#include <cmath>
#include <vector>
#include <numeric>
#include <itpp/itbase.h>
using namespace std;

namespace cm {
    ///将线性值转化为DB值
    double L2DB(double );
    ///将线性值矩阵转化为dB值的矩阵
    itpp::mat L2DB(itpp::mat );

    ///将DB值转化为线性值
    double DB2L(double _dB);
    ///将dB值矩阵转化为线性值矩阵
    itpp::mat DB2L(itpp::mat _dB);

    ///度转化为弧度
    double DEG2RAD(double _dDegree);

    ///弧度转化为度
    double RAD2DEG(double _dRAD);

    ///使角度在0到PI之间
    double ConvergeAngle(double _dAngleRAD);
    
    ///使垂直方向角度在0到pi之间 171128
    double ConvergeAngle_to_0_PI(double _dAngleRAD);

    ///Extract the real part of a complex matrix.
    itpp::mat real(itpp::cmat& _cmat);

    ///Extract the real part of a complex vector.
    itpp::vec real(itpp::cvec& _cvec);

    ///Extract the image part of a complex matrix.
    itpp::mat imag(itpp::cmat& _cmat);

   ///Extract the image part of a complex vector.
    itpp::vec imag(itpp::cvec& _cvec);
    itpp::cmat CalPanelWeight (double _detiltRAD, double _descanRAD, int _iN_H, int _iN_V,double _DHAntSpace, double _DVAntSpace);

    int GetPanelWeightIndex(int h, int v, int _iN_H, int _iN_V);
    
    
    double CalcPossLOS_for_UMA_4G (double _dDisM, double _h_UT);
    double CalcPossLOS_for_UMA_5G_ITU (double _dDisM, double _h_UT);
//    void test_for_ProLos_UMA();
    //对map根据value从高到低排序
    bool bigger(std::pair<std::pair<int,int>,double> a, std::pair<std::pair<int,int>,double> b);
    //对map根据value从低到高排序
    bool smaller(std::pair<std::pair<int,int>,double> a, std::pair<std::pair<int,int>,double> b);
}

