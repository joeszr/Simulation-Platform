  //
// Created by ChTY on 2022/9/21.
//
#include "Parameters.h"
#include <boost/random.hpp>
#ifndef CHTY_RANDOM_H
#define CHTY_RANDOM_H
//static int SeedBegin = Parameters::Instance().BASIC.ISeed;
static int SeedBegin = 0;
class Random {
private:
    static unsigned pseudo_seed; ///伪随机种子
    static bool _on;
public:
    Random(){
        pseudo_seed = SeedBegin++;
//        _on = !Parameters::Instance().BASIC.IsPseudoRandom;
        _on = (Parameters::Instance().BASIC.ISeed != 0);
    }
public :
    double xUniform(double _dmin = 0, double _dmax = 1);
    static double xUniform_channel(double _dmin = 0, double _dmax = 1);
    static double xUniform_distributems(double _dmin = 0, double _dmax = 1);
    double xUniform_detection(double _dmin = 0, double _dmax = 1);

    double xUniform_AntCalibrationError(double _dmin = 0, double _dmax = 1);

    double xUniform_distributepico(double _dmin = 0, double _dmax = 1);
    double xUniform_msconstruct(double _dmin = 0, double _dmax = 1);

//UL_begin
    double xUniform_Detection(double _dmin = 0, double _dmax = 1);
    double xUniform_DiffLoad(double _dmin = 0, double _dmax = 1);
    double xNormal_error(double _dave = 0, double _dstd = 1);
//UL_begin

/// 产生一个正态分布的随机数
/// @param _dave 随机数的均值
/// @param _dmax 随机数的标准差
    double xNormal(double _dave = 0, double _dstd = 1);
    double xNormal_channel(double _dave = 0, double _dstd = 1);

    double xNormal_SRSError(double _dave = 0, double _dstd = 1);
    double xNormal_DMRSError(double _dave = 0, double _dstd = 1);
    double xNormal_AntCalibrationError(double _dave = 0, double _dstd = 1);
    double xNormal_Other(double _dave = 0, double _dstd = 1);

    double xNormal_msconstruct(double _dave = 0, double _dstd = 1);
/// 产生一个均匀分布的随机整数
/// @param _imin 随机整数的下界
/// @param _imax 随机整数的上界
    static int  xUniformInt(int _imin = 0, int _imax = 1);

/// 产生一个指数分布的随机整数
/// @param _dave 随机数的均值
    double xExponent(double _dlambda);

/// 产生一个泊松分布的随机整数
    int xPossion_DL(double _ave = 1);


};


#endif //CHTY_RANDOM_H
