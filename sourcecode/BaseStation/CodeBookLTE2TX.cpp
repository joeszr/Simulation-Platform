///@file CodeBookLTE2TX.cpp
///@brief  LTE2天线系统发送预编码类声明，继承于CodeBook类
///@author wangxiaozhou

#include "itpp/itbase.h"
#include <complex>
#include "../Parameters/Parameters.h"
#include "CodeBookLTE2TX.h"
using namespace std;

CodeBookLTE2TX::CodeBookLTE2TX() {
    complex<double> comTemp1, comTemp2;
    cmat codeword1;
    codeword1.set_size(2, 1);
    ///Codebook_SU2TxLayer1赋值开始
//    comTemp1 = complex<double>(1, 0);
//    comTemp2 = complex<double>(0, 0);
//    codeword1(0, 0) = comTemp1;
//    codeword1(1, 0) = comTemp2;
//    m_vCodebookRank0.push_back(codeword1);
//    comTemp1 = complex<double>(0, 0);
//    comTemp2 = complex<double>(1, 0);
//    codeword1(0, 0) = comTemp1;
//    codeword1(1, 0) = comTemp2;
//    m_vCodebookRank0.push_back(codeword1);
    comTemp1 = complex<double>(1 / sqrt(2.0), 0);
    comTemp2 = complex<double>(1 / sqrt(2.0), 0);
    codeword1(0, 0) = comTemp1;
    codeword1(1, 0) = comTemp2;
    m_vCodebookRank0.push_back(codeword1);
    comTemp1 = complex<double>(1 / sqrt(2.0), 0);
    comTemp2 = complex<double>(-1 / sqrt(2.0), 0);
    codeword1(0, 0) = comTemp1;
    codeword1(1, 0) = comTemp2;
    m_vCodebookRank0.push_back(codeword1);
    comTemp1 = complex<double>(1 / sqrt(2.0), 0);
    comTemp2 = complex<double>(0, 1 / sqrt(2.0));
    codeword1(0, 0) = comTemp1;
    codeword1(1, 0) = comTemp2;
    m_vCodebookRank0.push_back(codeword1);
    comTemp1 = complex<double>(1 / sqrt(2.0), 0);
    comTemp2 = complex<double>(0, -1 / sqrt(2.0));
    codeword1(0, 0) = comTemp1;
    codeword1(1, 0) = comTemp2;
    m_vCodebookRank0.push_back(codeword1);
    ///Codebook_SU2TxLayer1赋值完毕

    cmat codeword2;
    codeword2.set_size(2, 2);
    ///Codebook_SU2TxLayer2赋值开始
//    codeword2(0, 0) = complex<double>(1.0, 0);
//    codeword2(0, 1) = complex<double>(0, 0);
//    codeword2(1, 0) = complex<double>(0, 0);
//    codeword2(1, 1) = complex<double>(1.0, 0);
//    m_vCodebookRank1.push_back(codeword2);

    codeword2(0, 0) = complex<double>(1.0 / sqrt(2.0), 0);
    codeword2(0, 1) = complex<double>(1.0 / sqrt(2.0), 0);
    codeword2(1, 0) = complex<double>(1.0 / sqrt(2.0), 0);
    codeword2(1, 1) = complex<double>(-1.0 / sqrt(2.0), 0);
    m_vCodebookRank1.push_back(codeword2);

    codeword2(0, 0) = complex<double>(1.0 / sqrt(2.0), 0);
    codeword2(0, 1) = complex<double>(1.0 / sqrt(2.0), 0);
    codeword2(1, 0) = complex<double>(0, 1.0 / sqrt(2.0));
    codeword2(1, 1) = complex<double>(0, -1.0 / sqrt(2.0));
    m_vCodebookRank1.push_back(codeword2);
    ///Codebook_SU2TxLayer1赋值完毕
}
///析构函数

CodeBookLTE2TX::~CodeBookLTE2TX() {

}
///获得码本相应Rank包含的码字数

int CodeBookLTE2TX::GetCodeNum(int _iRank) {
    switch (_iRank) {
        case 0:
            return static_cast<int> (m_vCodebookRank0.size());
        case 1:
            return static_cast<int> (m_vCodebookRank1.size());
        default:
            assert(false);
    }
}
///获得Precoding码字

cmat CodeBookLTE2TX::GetCodeWord(int _iPMI, int _iRank) {
    cmat cmatret;
    switch (_iRank) {
        case 0:
            cmatret= m_vCodebookRank0[_iPMI];
            break;
        case 1:
            cmatret= m_vCodebookRank1[_iPMI];
            break;
        default:
            assert(false);
    }
    return cmatret;
}

///根据信道相关阵、PMI和Rank获得Transformed预编码码字

cmat CodeBookLTE2TX::GetCodeWord(cmat _mCovMat, int _iPMI, int _iRank) {
    assert(_mCovMat.rows() == Parameters::Instance().Macro.ITotalAntNum && _mCovMat.cols() == Parameters::Instance().Macro.ITotalAntNum);
    cmat mCodeWord = this->GetCodeWord(_iPMI, _iRank);
    if (_iRank == 0) {
        assert(mCodeWord.cols() == 1);
        mCodeWord = _mCovMat * mCodeWord;
        mCodeWord = mCodeWord / sqrt((mCodeWord.H() * mCodeWord)(0,0));
    } 
    return mCodeWord;
}
///获得码本支持的最大Rank数

int CodeBookLTE2TX::GetMaxRank() {
    return 2;
}