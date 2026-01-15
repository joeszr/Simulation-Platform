/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file functions.cpp
///@brief 定义了ChannelModel中用到的一些函数
///
///@author wangfei
#include "P.h"
#include "functions.h"

#include "Random.h"


///使角度在-PI到PI之间
///@param _dAngleRAD 角度值

double cm::ConvergeAngle(double _dAngleRAD) {
    double result = _dAngleRAD;
    while (result > M_PI) {
        result -= 2 * M_PI;
    }
    while (result <= -1 * M_PI) {
        result += 2 * M_PI;
    }
    return result;
}

/// from (-pi, pi) --> (0, 2*pi) --> (0, pi)

double cm::ConvergeAngle_to_0_PI(double _dAngleRAD) {
    double result = cm::ConvergeAngle(_dAngleRAD); // -pi : pi

    if (result < 0) {
        result += 2 * M_PI; // 0 : 2 * pi
    }

    if (result > M_PI) {
        result = 2 * M_PI - result;
    }

    return result; // 0 : pi
}

///Extract the real part of a complex matrix.
///@param _cmat is any complex matrix.
///@return the real part of the input complex matrix.

itpp::mat cm::real(itpp::cmat& _cmat) {
    int row = _cmat.rows();
    int col = _cmat.cols();
    if (row == 0 || col == 0) {
        return itpp::mat(0, 0);
    }
    itpp::mat result(row, col);
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            result(i, j) = _cmat(i, j).real();
        }
    }
    return result;
}

///Extract the real part of a complex vector.
///@param _cvec is any complex matrix.
///@return the real part of the input complex vector.

itpp::vec cm::real(itpp::cvec& _cvec) {
    int iSize = _cvec.size();
    itpp::vec result(iSize);
    for (int i = 0; i < iSize; ++i) {
        result[i] = _cvec[i].real();
    }
    return result;
}

///Extract the image part of a complex matrix.
///@param _cmat is any complex matrix.
///@return the image part of the input complex matrix.

itpp::mat cm::imag(itpp::cmat& _cmat) {
    int row = _cmat.rows();
    int col = _cmat.cols();
    if (row == 0 || col == 0) {
        return itpp::mat(0, 0);
    }

    itpp::mat result(row, col);
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            result(i, j) = _cmat(i, j).imag();
        }
    }
    return result;
}

///Extract the image part of a complex vector.
///@param _cvec is any complex matrix.
///@return the image part of the input complex vector.

itpp::vec cm::imag(itpp::cvec& _cvec) {
    int iSize = _cvec.size();
    itpp::vec result(iSize);
    for (int i = 0; i < iSize; ++i) {
        result[i] = _cvec[i].imag();
    }
    return result;
}

///将线性值转化为DB值
///@param _Linear 线性值
///@return DB值

double cm::L2DB(double _Linear) {
    return 10 * log10(_Linear);
}

///将DB值转化为线性值
///@param _dB dB值
///@return 线性值

double cm::DB2L(double _dB) {
    return pow(10.0, _dB / 10.0);
}

///度转化为弧度

double cm::DEG2RAD(double _dDegree) {
    return M_PI * _dDegree / 180.0;
}

///弧度转化为度

double cm::RAD2DEG(double _dRAD) {
    return 180.0 * _dRAD / M_PI;
}

///将线性值矩阵转化为dB值的矩阵
///@param _Linear 线性值矩阵
///@return DB值矩阵

itpp::mat cm::L2DB(itpp::mat _Linear) {
    itpp::mat result = _Linear;
    for (int i = 0; i < _Linear.rows(); ++i) {
        for (int j = 0; j < _Linear.cols(); ++j) {
            result(i, j) = L2DB(_Linear(i, j));
        }
    }
    return result;
}

///将dB值矩阵转化为线性值矩阵
///@param _dB dB值矩阵
///@return 线性值矩阵

itpp::mat cm::DB2L(itpp::mat _dB) {
    itpp::mat result = _dB;
    for (int i = 0; i < _dB.rows(); ++i) {
        for (int j = 0; j < _dB.cols(); ++j) {
            result(i, j) = DB2L(_dB(i, j));
        }
    }
    return result;
}

itpp::cmat cm::CalPanelWeight(double _detiltRAD, double _descanRAD,
        int _iN_H, int _iN_V, double _DHAntSpace, double _DVAntSpace) {
    ///注意：该函数中_detiltRAD=0代表垂直于Z轴方向；_descanRAD代表和水平维主瓣方向的角度差值
    // mMatrix_old = [h0v0, h0v1, h0v2, ..., h1v0, h1v1, h1v2, ...], 标准
    //    itpp::cmat mMatrix_old = itpp::zeros_c(1 , _iN_H*_iN_V );
    //    
    //    // mMatrix_new = [v0h0, v0h1, v0h2, ..., v1h0, v1h1, v1h2, ...]，非标
    //    itpp::cmat mMatrix_new = itpp::zeros_c(1 , _iN_H*_iN_V );

    // mMatrix_old = [h0v0, h0v1, h0v2, ..., h1v0, h1v1, h1v2, ...], 标准
    itpp::cmat mMatrix = itpp::zeros_c(1, _iN_H * _iN_V);
    
    double d_temp_a = _DVAntSpace * sin(_detiltRAD);
    double d_temp_b = _DHAntSpace * cos(_detiltRAD) * sin(_descanRAD);
            

    for (int h = 0; h < _iN_H; ++h) {
        for (int v = 0; v < _iN_V; ++v) {
            complex<double> mtemp = 1 / sqrt(_iN_H * _iN_V) * exp(2 * M_PI * (v * d_temp_a
                    - h * d_temp_b) * M_J);
            //             mMatrix(0,m*_iN_V+n)=mtemp;

            // 20171129
            //            mMatrix_old(0,h*_iN_V+v)=mtemp;
            //            mMatrix_new(0,h+v*_iN_H)=mtemp;   

            mMatrix(0, GetPanelWeightIndex(h, v, _iN_H, _iN_V)) = mtemp;
        }
    }
    return mMatrix;
}

int cm::GetPanelWeightIndex(int h, int v, int _iN_H, int _iN_V) {
    return h * _iN_V + v;
}

double cm::CalcPossLOS_for_UMA_4G(double _dDisM, double _h_UT) {
    //待定，为d和UE高度的函数
    double G, C;
    if (_dDisM > 18) {
        G = 1.25 * pow(10, -6) * pow(_dDisM, 3) * std::exp(-1 * _dDisM / 150);
    } else {
        G = 0;
    }
    if (_h_UT >= 13 && _h_UT <= 23) {
        C = pow((_h_UT - 13) / 10, 1.5) * G;
    } else if (_h_UT < 13) {
        C = 0;
    }
    //dPossLOS = std::min(18.0 / _dDisM, 1.0) * (1 - std::exp(-1 * _dDisM / 63)) + std::exp(-1 * _dDisM / 63) * (1 + C);
    double dPossLOS = (std::min(18.0 / _dDisM, 1.0)
            * (1 - std::exp(-1 * _dDisM / 63)) + std::exp(-1 * _dDisM / 63))* (1 + C);
    return dPossLOS;
}

double cm::CalcPossLOS_for_UMA_5G_ITU(double _dDisM, double _h_UT) {
    //待定，为d和UE高度的函数
    double C;
    if (_h_UT >= 13 && _h_UT <= 23) {
        C = pow((_h_UT - 13) / 10, 1.5);
    } else if (_h_UT < 13) {
        C = 0;
    }

    double A = 18.0 / _dDisM
            + std::exp(-1.0 * _dDisM / 63.0) * (1 - 18.0 / _dDisM);
    double B = 1
            + C * 1.25 * pow(10, -6) * pow(_dDisM, 3) * std::exp(-1 * _dDisM / 150);

    double dPossLOS;
    if (_dDisM <= 18) {
        dPossLOS = 1.0;
    } else {
        dPossLOS = A * B;
    }

    return dPossLOS;
}
//
//void cm::test_for_ProLos_UMA(){
//    double _h_UT = 1.5;
//    Observer::Print("ProLos_UMA") 
//            << "UE_height = " << _h_UT << "m" << endl;
//    
//    Observer::Print("ProLos_UMA") << endl;
//    
//    Observer::Print("ProLos_UMA") << "2D_Dis(m)" 
//            << setw(20) << "LOS_Prob_UMA_4G" 
//            << setw(20) << "LOS_Prob_UMA_5G" 
//            << endl;
//    for(double _dDisM = 0.0; _dDisM < 300; _dDisM += 5.0) {
//        double ProLos_4G = cm::CalcPossLOS_for_UMA_4G(_dDisM, _h_UT);
//        double ProLos_5G = cm::CalcPossLOS_for_UMA_5G_ITU(_dDisM, _h_UT);
//        
//        Observer::Print("ProLos_UMA") << _dDisM 
//            << setw(20) << ProLos_4G 
//            << setw(20) << ProLos_5G
//            << endl;
//    }
//
//}

//20260115
bool cm::bigger(std::pair<std::pair<int,int>,double> a, std::pair<std::pair<int,int>,double> b){
    return a.second > b.second;
}
//未实现
bool cm::smaller(std::pair<std::pair<int,int>,double> a, std::pair<std::pair<int,int>,double> b){
    return a.second < b.second;
}

