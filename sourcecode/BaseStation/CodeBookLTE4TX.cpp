///@file CodeBookLTE4TX.cpp
///@brief  LTE4天线系统发送预编码类声明，继承于CodeBook类
///@author wangxiaozhou

#include "itpp/itbase.h"
#include <complex>
#include "../Parameters/Parameters.h"
#include "CodeBookLTE4TX.h"
using namespace std;
///构造函数，在构造函数中对4天线码本初始化

CodeBookLTE4TX::CodeBookLTE4TX() {
    double M_SQRT1to2 = 1.0 / sqrt(2.0);
    double U_Real[4][16] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {-1, 0, 1, 0, -M_SQRT1to2, M_SQRT1to2, M_SQRT1to2, -M_SQRT1to2, -1, 0, 1, 0, -1, -1, 1, 1},
        {-1, 1, -1, 1, 0, 0, 0, 0, 1, -1, 1, -1, -1, 1, -1, 1},
        {-1, 0, 1, 0, M_SQRT1to2, -M_SQRT1to2, -M_SQRT1to2, M_SQRT1to2, 1, 0, -1, 0, 1, -1, -1, 1}
    };
    double U_Imag[4][16] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, -1, 0, 1, -M_SQRT1to2, -M_SQRT1to2, M_SQRT1to2, M_SQRT1to2, 0, -1, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, -1, 1, -1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, -1, -M_SQRT1to2, -M_SQRT1to2, M_SQRT1to2, M_SQRT1to2, 0, -1, 0, 1, 0, 0, 0, 0}
    };
    for (int i = 0; i < 16; i++) {
        ///U矩阵
        cmat U_Precoding;
        U_Precoding.set_size(4, 1);
        for (int j = 0; j < 4; j++) {
            U_Precoding(j, 0) = complex<double> (U_Real[j][i], U_Imag[j][i]);
        }
        ///W矩阵
        cmat W_Precoding;
        W_Precoding.set_size(4, 4);
        cmat I = eye_c(4);
        double temp = 1.0 / (pow(abs(U_Precoding(0, 0)), 2) + pow(abs(U_Precoding(1, 0)), 2) + pow(abs(U_Precoding(2, 0)), 2) + pow(abs(U_Precoding(3, 0)), 2));
        W_Precoding = I - 2 * temp * U_Precoding * U_Precoding.H();

        ///Codebook_SU4TxLayer1赋值开始取W矩阵首列
        m_vCodebookRank0.push_back(W_Precoding.get_col(0));
        ///Codebook_SU4TxLayer2赋值开始
        cmat mW12, mW13, mW14;
        mW12.set_size(4, 2);
        mW13.set_size(4, 2);
        mW14.set_size(4, 2);
        mW12.zeros();
        mW13.zeros();
        mW14.zeros();
        switch (i) {
            case 1:
            case 2:
            case 3:
            case 8:
            case 12:
            case 15:
                //W12,Codebook_SU4TxLayer2
                mW12.set_submatrix(0, 3, 0, 0, W_Precoding.get_col(0));
                mW12.set_submatrix(0, 3, 1, 1, W_Precoding.get_col(1));
                m_vCodebookRank1.push_back(mW12);
                break;
            case 6:
            case 7:
            case 10:
            case 11:
            case 13:
            case 14:
                //W13,Codebook_SU4TxLayer2
                mW13.set_submatrix(0, 3, 0, 0, W_Precoding.get_col(0));
                mW13.set_submatrix(0, 3, 1, 1, W_Precoding.get_col(2));
                m_vCodebookRank1.push_back(mW13);
                break;
            case 0:
            case 4:
            case 5:
            case 9:
                //W14,Codebook_SU4TxLayer2
                mW14.set_submatrix(0, 3, 0, 0, W_Precoding.get_col(0));
                mW14.set_submatrix(0, 3, 1, 1, W_Precoding.get_col(3));
                m_vCodebookRank1.push_back(mW14);
                break;
            default:
                assert(false);
                ///Codebook_SU4TxLayer2赋值完毕
        }
        //添加4码本3流
        cmat mW123, mW124, mW134;
        mW123.set_size(4, 3);
        mW124.set_size(4, 3);
        mW134.set_size(4, 3);
        mW123.zeros();
        mW124.zeros();
        mW134.zeros();
        switch (i) {
            case 1:
            case 2:
            case 3:
            case 10:
            case 12:
            case 13:
            case 14:
            case 15:
                //W123,Codebook_SU4TxLayer3
                mW123.set_submatrix(0, 3, 0, 0, W_Precoding.get_col(0));
                mW123.set_submatrix(0, 3, 1, 1, W_Precoding.get_col(1));
                mW123.set_submatrix(0, 3, 2, 2, W_Precoding.get_col(2));
                m_vCodebookRank2.push_back(mW123);
                break;
            case 0:
            case 4:
            case 5:
            case 8:
                //W124,Codebook_SU4TxLayer3
                mW124.set_submatrix(0, 3, 0, 0, W_Precoding.get_col(0));
                mW124.set_submatrix(0, 3, 1, 1, W_Precoding.get_col(1));
                mW124.set_submatrix(0, 3, 2, 2, W_Precoding.get_col(3));
                m_vCodebookRank2.push_back(mW124);
                break;
            case 6:
            case 7:
            case 9:
            case 11:
                //W134,Codebook_SU4TxLayer3
                mW134.set_submatrix(0, 3, 0, 0, W_Precoding.get_col(0));
                mW134.set_submatrix(0, 3, 1, 1, W_Precoding.get_col(2));
                mW134.set_submatrix(0, 3, 2, 2, W_Precoding.get_col(3));
                m_vCodebookRank2.push_back(mW134);
                break;
            default:
                assert(false);
                ///Codebook_SU4TxLayer3赋值完毕
        }
        //添加4码本4流
        switch (i) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                m_vCodebookRank3.push_back(W_Precoding);
                break;
            default:
                assert(false);
                ///Codebook_SU4TxLayer4赋值完毕
        }
    }
}
///析构函数

CodeBookLTE4TX::~CodeBookLTE4TX() {

}
///获得码本相应Rank包含的码字数

int CodeBookLTE4TX::GetCodeNum(int _iRank) {
    int iret;
    switch (_iRank) {
        case 0:
            iret= static_cast<int> (m_vCodebookRank0.size());
            break;
        case 1:
            iret= static_cast<int> (m_vCodebookRank1.size());
            break;
        case 2:
            iret= static_cast<int> (m_vCodebookRank2.size());
            break;
        case 3:
            iret= static_cast<int> (m_vCodebookRank3.size());
            break;
        default:
            assert(false);
    }
    return iret;
}
///获得Precoding码字

cmat CodeBookLTE4TX::GetCodeWord(int _iPMI, int _iRank) {
    cmat cmatret;
    switch (_iRank) {
        case 0:
            cmatret= m_vCodebookRank0[_iPMI];
            break;
        case 1:
            cmatret= m_vCodebookRank1[_iPMI];
            break;
        case 2:
            cmatret= m_vCodebookRank2[_iPMI];
            break;
        case 3:
            cmatret= m_vCodebookRank3[_iPMI];
            break;
        default:
            assert(false);
    }
    return cmatret;
}

///根据信道相关阵、PMI和Rank获得Transformed预编码码字

cmat CodeBookLTE4TX::GetCodeWord(cmat _mCovMat, int _iPMI, int _iRank) {
    assert(_mCovMat.rows() == Parameters::Instance().Macro.ITotalAntNum && _mCovMat.cols() == Parameters::Instance().Macro.ITotalAntNum);
    cmat mCodeWord = this->GetCodeWord(_iPMI, _iRank);
    if (_iRank == 0) {
        assert(mCodeWord.cols() == 1);
        mCodeWord = _mCovMat * mCodeWord;
        mCodeWord = mCodeWord / sqrt((mCodeWord.H() * mCodeWord)(0, 0));
    }
    return mCodeWord;
}
///获得码本支持的最大Rank数

int CodeBookLTE4TX::GetMaxRank() {
    return 4;
}