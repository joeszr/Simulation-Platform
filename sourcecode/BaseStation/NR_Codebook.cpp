#include "itpp/itbase.h"
#include <complex>
#include "../Parameters/Parameters.h"
#include "NR_CodeBook.h"

NR_CodeBook::NR_CodeBook(int _iTxPortNum) : m_iTxPortNum(_iTxPortNum)
{
    switch (m_iTxPortNum) {
        case 1:
            BuildCodeBooks_for_1Tx();
            break;
        case 2:
            BuildCodeBooks_for_2Tx();
            break;
        case 4:
            BuildCodeBooks_for_4Tx();
            break;
        case 8:
            BuildCodeBooks_for_8Tx();
            break;
        case 32:
            BuildCodeBooks_for_32Tx();
            break;
        default:
            break;

    }
}

void NR_CodeBook::BuildCodeBooks_for_1Tx() {
	m_vCodebooks[0] = vector<cmat>{ itpp::ones_c(1,1) };
}

void NR_CodeBook::BuildCodeBooks_for_2Tx() {

    m_iMaxNumberOfLayers = 2;

    int _iRank = 0;

    // NumberOfLayers == 1
    complex<double> comTemp1, comTemp2;
    cmat codeword1;
    codeword1.set_size(2, 1);
    vector<cmat> temp1;

    comTemp1 = complex<double>(1 / sqrt(2.0), 0);
    comTemp2 = complex<double>(1 / sqrt(2.0), 0);
    codeword1(0, 0) = comTemp1;
    codeword1(1, 0) = comTemp2;
    temp1.push_back(codeword1);

    //m_vCodebookRank0.push_back(codeword1);
    comTemp1 = complex<double>(1 / sqrt(2.0), 0);
    comTemp2 = complex<double>(0, 1 / sqrt(2.0));
    codeword1(0, 0) = comTemp1;
    codeword1(1, 0) = comTemp2;
    temp1.push_back(codeword1);

    //m_vCodebookRank0.push_back(codeword1);
    comTemp1 = complex<double>(1 / sqrt(2.0), 0);
    comTemp2 = complex<double>(-1 / sqrt(2.0), 0);
    codeword1(0, 0) = comTemp1;
    codeword1(1, 0) = comTemp2;
    temp1.push_back(codeword1);


    //m_vCodebookRank0.push_back(codeword1);
    comTemp1 = complex<double>(1 / sqrt(2.0), 0);
    comTemp2 = complex<double>(0, -1 / sqrt(2.0));
    codeword1(0, 0) = comTemp1;
    codeword1(1, 0) = comTemp2;
    temp1.push_back(codeword1);
    //m_vCodebookRank0.push_back(codeword1);
    /*
        m_vCodebooks.insert(pair<int, vector < cmat > > (0, temp1));
     */

    //  m_vCodebooks(_iRank)[_iPMI]
    // std::map<int,  std::vector<cmat> > m_vCodebooks;
    m_vCodebooks[_iRank] = temp1;

    //return m_vCodebooks;

    // NumberOfLayers == 2
    _iRank = 1;
    cmat codeword2;
    codeword2.set_size(2, 2);
    vector<cmat> temp2;
    codeword2(0, 0) = complex<double>(1.0 / sqrt(2.0), 0);
    codeword2(0, 1) = complex<double>(1.0 / sqrt(2.0), 0);
    codeword2(1, 0) = complex<double>(1.0 / sqrt(2.0), 0);
    codeword2(1, 1) = complex<double>(-1.0 / sqrt(2.0), 0);
    temp2.push_back(codeword2);

    codeword2(0, 0) = complex<double>(1.0 / sqrt(2.0), 0);
    codeword2(0, 1) = complex<double>(1.0 / sqrt(2.0), 0);
    codeword2(1, 0) = complex<double>(0, 1.0 / sqrt(2.0));
    codeword2(1, 1) = complex<double>(0, -1.0 / sqrt(2.0));
    temp2.push_back(codeword2);
    m_vCodebooks[_iRank] = temp2;

}

void NR_CodeBook::BuildCodeBooks_for_4Tx() {
    m_iMaxNumberOfLayers = 4;
    /*
    
        int iN1 = 2;
        int iN2 = 1;
        int iO1 = 4;
        int iO2 = 1;
    
        // NumberOfLayers == 1
        for(int i_1_1 = 0; i_1_1 < iN1 * iO1; i_1_1++) {
            for(int i_1_2 = 0; i_1_2 < iN2 * iO2; i_1_2++) {
                for(int i_2 = 0; i_2 < 4; i_2++) {

                   // int PMI = index2PMI(i_1_1, i_1_2, i_2, iN1, iN2, iO1, iO2);

                    // W_l,m,n
                }
            }
        }
     */
    // NumberOfLayers == 2, 3, 4
    vector<cmat> m_vCodebookRank0;
    /// LTE4天线发送双流码字
    vector<cmat> m_vCodebookRank1;
    /// LTE4天线发送三流码字
//    vector<cmat> m_vCodebookRank2;
//    /// LTE4天线发送四流码字
//    vector<cmat> m_vCodebookRank3;

    
     //4TR
    double real;
    double imag;
    const char* pathreal = "./inputfiles/TR4_layer_1_real.txt";
    const char* pathImag = "./inputfiles/TR4_layer_1_imag.txt";
    ifstream fcodebookreal(pathreal);
    ifstream fcodebookimag(pathImag);
    assert(fcodebookreal);
    assert(fcodebookimag);
    itpp::cmat codebook4_layer1(4,32);//单流的码本向量，32个
    vector<double> vCodebookreal;
    vector<double> vCodebookimag;
    //vCodebookreal.resize(32*1024,0.0);
    //vCodebookimag.resize(32*1024,0.0);
    //copy(istream_iterator<double>(fcodebookreal), istream_iterator<double>(), std::back_inserter(vCodebookreal));
    //copy(istream_iterator<double>(fcodebookimag), istream_iterator<double>(), std::back_inserter(vCodebookimag));
    for (int i = 0; i < 4; ++i)
       for (int j = 0; j < 32; ++j){
           fcodebookreal>>real;
           fcodebookimag>>imag;
           codebook4_layer1(i, j).real(real);
           codebook4_layer1(i, j).imag(imag);
            //codebook32_layer1(i, j).real() = vCodebookreal[i * 1024 + j];
            //codebook32_layer1(i, j).imag() = vCodebookimag[i * 1024 + j];
       }
    for(int i=0;i<32;i++){
     m_vCodebookRank0.emplace_back(codebook4_layer1.get_col(i));
    }
    fcodebookreal.close();
    fcodebookimag.close();
    
    
    const char* pathreal1 = "./inputfiles/TR4_layer_2_real.txt";
    const char* pathImag1 = "./inputfiles/TR4_layer_2_imag.txt";
    ifstream fcodebookreal1(pathreal1);
    ifstream fcodebookimag1(pathImag1);
    assert(fcodebookreal1);
    assert(fcodebookimag1);
    std::vector<itpp::cmat> codebook4_layer2(64,itpp::zeros_c(4,2));//二流的码本向量，64个
    vCodebookreal.clear();
    vCodebookimag.clear();
    //copy(istream_iterator<double>(fcodebookreal1), istream_iterator<double>(), std::back_inserter(vCodebookreal));
    //copy(istream_iterator<double>(fcodebookimag1), istream_iterator<double>(), std::back_inserter(vCodebookimag));
    //j表示2048个码本，i表示32TR，k表示双流
        for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 64; ++j)
        for (int k = 0; k < 2; ++k){
            fcodebookreal1>>real;
            fcodebookimag1>>imag;
            codebook4_layer2[j](i, k).real(real);
            codebook4_layer2[j](i, k).imag(imag);
        }
    m_vCodebookRank1.insert(m_vCodebookRank1.end(),codebook4_layer2.begin(),codebook4_layer2.begin()+64);
    fcodebookreal1.close();
    fcodebookimag1.close();
//    double M_SQRT1to2 = 1.0 / sqrt(2.0);
//    double U_Real[4][16] = {
//        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
//        {-1, 0, 1, 0, -M_SQRT1to2, M_SQRT1to2, M_SQRT1to2, -M_SQRT1to2, -1, 0, 1, 0, -1, -1, 1, 1},
//        {-1, 1, -1, 1, 0, 0, 0, 0, 1, -1, 1, -1, -1, 1, -1, 1},
//        {-1, 0, 1, 0, M_SQRT1to2, -M_SQRT1to2, -M_SQRT1to2, M_SQRT1to2, 1, 0, -1, 0, 1, -1, -1, 1}
//    };
//    double U_Imag[4][16] = {
//        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//        {0, -1, 0, 1, -M_SQRT1to2, -M_SQRT1to2, M_SQRT1to2, M_SQRT1to2, 0, -1, 0, 1, 0, 0, 0, 0},
//        {0, 0, 0, 0, -1, 1, -1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
//        {0, 1, 0, -1, -M_SQRT1to2, -M_SQRT1to2, M_SQRT1to2, M_SQRT1to2, 0, -1, 0, 1, 0, 0, 0, 0}
//    };
//    for (int i = 0; i < 16; i++) {
//        ///U矩阵
//        cmat U_Precoding;
//        U_Precoding.set_size(4, 1);
//        for (int j = 0; j < 4; j++) {
//            U_Precoding(j, 0) = complex<double> (U_Real[j][i], U_Imag[j][i]);
//        }
//        ///W矩阵
//        cmat W_Precoding;
//        W_Precoding.set_size(4, 4);
//        cmat I = eye_c(4);
//        double temp = 1.0 / (pow(abs(U_Precoding(0, 0)), 2) + pow(abs(U_Precoding(1, 0)), 2) + pow(abs(U_Precoding(2, 0)), 2) + pow(abs(U_Precoding(3, 0)), 2));
//        W_Precoding = I - 2 * temp * U_Precoding * U_Precoding.H();
//
//        ///Codebook_SU4TxLayer1赋值开始取W矩阵首列
//        m_vCodebookRank0.push_back(W_Precoding.get_col(0));
//        ///Codebook_SU4TxLayer1赋值完成
//
//        ///Codebook_SU4TxLayer2赋值开始
//        cmat mW12, mW13, mW14;
//        mW12.set_size(4, 2);
//        mW13.set_size(4, 2);
//        mW14.set_size(4, 2);
//        mW12.zeros();
//        mW13.zeros();
//        mW14.zeros();
//        switch (i) {
//            case 1:
//            case 2:
//            case 3:
//            case 8:
//            case 12:
//            case 15:
//                //W12,Codebook_SU4TxLayer2
//                mW12.set_submatrix(0, 3, 0, 0, W_Precoding.get_col(0));
//                mW12.set_submatrix(0, 3, 1, 1, W_Precoding.get_col(1));
//                m_vCodebookRank1.push_back(mW12);
//                break;
//            case 6:
//            case 7:
//            case 10:
//            case 11:
//            case 13:
//            case 14:
//                //W13,Codebook_SU4TxLayer2
//                mW13.set_submatrix(0, 3, 0, 0, W_Precoding.get_col(0));
//                mW13.set_submatrix(0, 3, 1, 1, W_Precoding.get_col(2));
//                m_vCodebookRank1.push_back(mW13);
//                break;
//            case 0:
//            case 4:
//            case 5:
//            case 9:
//                //W14,Codebook_SU4TxLayer2
//                mW14.set_submatrix(0, 3, 0, 0, W_Precoding.get_col(0));
//                mW14.set_submatrix(0, 3, 1, 1, W_Precoding.get_col(3));
//                m_vCodebookRank1.push_back(mW14);
//                break;
//            default:
//                assert(false);
//                break;
//                ///Codebook_SU4TxLayer2赋值完毕
//        }
//        //添加4码本3流
//        cmat mW123, mW124, mW134;
//        mW123.set_size(4, 3);
//        mW124.set_size(4, 3);
//        mW134.set_size(4, 3);
//        mW123.zeros();
//        mW124.zeros();
//        mW134.zeros();
//        switch (i) {
//            case 1:
//            case 2:
//            case 3:
//            case 10:
//            case 12:
//            case 13:
//            case 14:
//            case 15:
//                //W123,Codebook_SU4TxLayer3
//                mW123.set_submatrix(0, 3, 0, 0, W_Precoding.get_col(0));
//                mW123.set_submatrix(0, 3, 1, 1, W_Precoding.get_col(1));
//                mW123.set_submatrix(0, 3, 2, 2, W_Precoding.get_col(2));
//                m_vCodebookRank2.push_back(mW123);
//                break;
//            case 0:
//            case 4:
//            case 5:
//            case 8:
//                //W124,Codebook_SU4TxLayer3
//                mW124.set_submatrix(0, 3, 0, 0, W_Precoding.get_col(0));
//                mW124.set_submatrix(0, 3, 1, 1, W_Precoding.get_col(1));
//                mW124.set_submatrix(0, 3, 2, 2, W_Precoding.get_col(3));
//                m_vCodebookRank2.push_back(mW124);
//                break;
//            case 6:
//            case 7:
//            case 9:
//            case 11:
//                //W134,Codebook_SU4TxLayer3
//                mW134.set_submatrix(0, 3, 0, 0, W_Precoding.get_col(0));
//                mW134.set_submatrix(0, 3, 1, 1, W_Precoding.get_col(2));
//                mW134.set_submatrix(0, 3, 2, 2, W_Precoding.get_col(3));
//                m_vCodebookRank2.push_back(mW134);
//                break;
//            default:
//                assert(false);
//                break;
//                ///Codebook_SU4TxLayer3赋值完毕
//        }
//        //添加4码本4流
//        switch (i) {
//            case 0:
//            case 1:
//            case 2:
//            case 3:
//            case 4:
//            case 5:
//            case 6:
//            case 7:
//            case 8:
//            case 9:
//            case 10:
//            case 11:
//            case 12:
//            case 13:
//            case 14:
//            case 15:
//                m_vCodebookRank3.push_back(W_Precoding);
//                break;
//            default:
//                assert(false);
//                break;
//                ///Codebook_SU4TxLayer4赋值完毕
//        }
//    }
    //m_vCodebooks[iRank]
    m_vCodebooks[0] = m_vCodebookRank0;
    m_vCodebooks[1] = m_vCodebookRank1;
//    m_vCodebooks[2] = m_vCodebookRank2;
//    m_vCodebooks[3] = m_vCodebookRank3;

}
void NR_CodeBook::BuildCodeBooks_for_8Tx() {
    m_iMaxNumberOfLayers = 8;
    vector<cmat> m_vCodebookRank0;
    /// LTE4天线发送双流码字
    vector<cmat> m_vCodebookRank1;
    /// LTE4天线发送三流码字
    vector<cmat> m_vCodebookRank2;
    /// LTE4天线发送四流码字
    vector<cmat> m_vCodebookRank3;
    int Size0 = 256; 
    int Size1 = 512;
    int Size2 = 384;
    int Size3 = 384;
    
    
    int iSpace0 = 4;
    int iSpace1 = 4;
    int iSpace2 = 3;
    int iSpace3 = 3;
    //8TR
    double real;
    double imag;
    const char* pathreal = "./inputfiles/TR8_layer_1_real.txt";
    const char* pathImag = "./inputfiles/TR8_layer_1_imag.txt";
    ifstream fcodebookreal(pathreal);
    ifstream fcodebookimag(pathImag);
    assert(fcodebookreal);
    assert(fcodebookimag);
    
    itpp::cmat codebook8_layer1(8,Size0);//单流的码本向量，256个
    vector<double> vCodebookreal;
    vector<double> vCodebookimag;
    //vCodebookreal.resize(32*1024,0.0);
    //vCodebookimag.resize(32*1024,0.0);
    //copy(istream_iterator<double>(fcodebookreal), istream_iterator<double>(), std::back_inserter(vCodebookreal));
    //copy(istream_iterator<double>(fcodebookimag), istream_iterator<double>(), std::back_inserter(vCodebookimag));
    for (int i = 0; i < 8; ++i)
       for (int j = 0; j < Size0; ++j){
           fcodebookreal>>real;
           fcodebookimag>>imag;
           codebook8_layer1(i, j).real(real);
           codebook8_layer1(i, j).imag(imag);
            //codebook32_layer1(i, j).real() = vCodebookreal[i * 1024 + j];
            //codebook32_layer1(i, j).imag() = vCodebookimag[i * 1024 + j];
       }
    for(int i=0;i<Size0;i+=iSpace0){
     m_vCodebookRank0.emplace_back(codebook8_layer1.get_col(i));
    }
    fcodebookreal.close();
    fcodebookimag.close();
    
    
    const char* pathreal1 = "./inputfiles/TR8_layer_2_real.txt";
    const char* pathImag1 = "./inputfiles/TR8_layer_2_imag.txt";
    ifstream fcodebookreal1(pathreal1);
    ifstream fcodebookimag1(pathImag1);
    assert(fcodebookreal1);
    assert(fcodebookimag1);
    
    std::vector<itpp::cmat> codebook8_layer2(Size1,itpp::zeros_c(8,2));//二流的码本向量，512个
    vCodebookreal.clear();
    vCodebookimag.clear();
    //copy(istream_iterator<double>(fcodebookreal1), istream_iterator<double>(), std::back_inserter(vCodebookreal));
    //copy(istream_iterator<double>(fcodebookimag1), istream_iterator<double>(), std::back_inserter(vCodebookimag));
    //j表示2048个码本，i表示32TR，k表示双流
        for (int i = 0; i < 8; ++i)
        for (int j = 0; j < Size1; ++j)
        for (int k = 0; k < 2; ++k){
            fcodebookreal1>>real;
            fcodebookimag1>>imag;
            codebook8_layer2[j](i, k).real(real);
            codebook8_layer2[j](i, k).imag(imag);
        }
    for(int i=0;i<Size1;i+=iSpace1){
     m_vCodebookRank1.push_back(codebook8_layer2[i]);
    }
    //m_vCodebookRank1.insert(m_vCodebookRank1.end(),codebook8_layer2.begin(),codebook8_layer2.begin()+Size1);
    fcodebookreal1.close();
    fcodebookimag1.close();
    
    const char* pathreal2 = "./inputfiles/TR8_layer_3_real.txt";
    const char* pathImag2 = "./inputfiles/TR8_layer_3_imag.txt";
    ifstream fcodebookreal2(pathreal2);
    ifstream fcodebookimag2(pathImag2);
    assert(fcodebookreal2);
    assert(fcodebookimag2);
    
    std::vector<itpp::cmat> codebook8_layer3(Size2,itpp::zeros_c(8,3));//3流的码本向量，384个
    vCodebookreal.clear();
    vCodebookimag.clear();
    //copy(istream_iterator<double>(fcodebookreal1), istream_iterator<double>(), std::back_inserter(vCodebookreal));
    //copy(istream_iterator<double>(fcodebookimag1), istream_iterator<double>(), std::back_inserter(vCodebookimag));
    //j表示2048个码本，i表示32TR，k表示双流
        for (int i = 0; i < 8; ++i)
        for (int j = 0; j < Size2; ++j)
        for (int k = 0; k < 3; ++k){
            fcodebookreal2>>real;
            fcodebookimag2>>imag;
            codebook8_layer3[j](i, k).real(real);
            codebook8_layer3[j](i, k).imag(imag);
        }
    for(int i=0;i<Size2;i+=iSpace2){
     m_vCodebookRank2.push_back(codebook8_layer3[i]);
    }
//    m_vCodebookRank2.insert(m_vCodebookRank2.end(),codebook8_layer3.begin(),codebook8_layer3.begin()+Size2);
    fcodebookreal2.close();
    fcodebookimag2.close();
    
    
    const char* pathreal3 = "./inputfiles/TR8_layer_4_real.txt";
    const char* pathImag3 = "./inputfiles/TR8_layer_4_imag.txt";
    ifstream fcodebookreal3(pathreal3);
    ifstream fcodebookimag3(pathImag3);
    assert(fcodebookreal3);
    assert(fcodebookimag3);
    
    std::vector<itpp::cmat> codebook8_layer4(Size3,itpp::zeros_c(8,4));//四流的码本向量，384个
    vCodebookreal.clear();
    vCodebookimag.clear();
    copy(istream_iterator<double>(fcodebookreal3), istream_iterator<double>(), std::back_inserter(vCodebookreal));
    copy(istream_iterator<double>(fcodebookimag3), istream_iterator<double>(), std::back_inserter(vCodebookimag));
    //j表示2048个码本，i表示32TR，k表示双流
        for (int i = 0; i < 8; ++i)
        for (int j = 0; j < Size3; ++j)
        for (int k = 0; k < 4; ++k){
            fcodebookreal3>>real;
            fcodebookimag3>>imag;
            codebook8_layer4[j](i, k).real(real);
            codebook8_layer4[j](i, k).imag(imag);
        }
    
    for(int i=0;i<Size3;i+=iSpace3){
     m_vCodebookRank3.push_back(codebook8_layer4[i]);
    }
//    m_vCodebookRank3.insert(m_vCodebookRank3.end(),codebook8_layer4.begin(),codebook8_layer4.begin()+Size3);
    
    fcodebookreal3.close();
    fcodebookimag3.close();
    //以上为码本

    
    m_vCodebooks[0] = m_vCodebookRank0;
    m_vCodebooks[1] = m_vCodebookRank1;
    m_vCodebooks[2] = m_vCodebookRank2;
    m_vCodebooks[3] = m_vCodebookRank3;
//    m_vCodebooks[4] = m_vCodebookRank0;
//    m_vCodebooks[5] = m_vCodebookRank0;
//    m_vCodebooks[6] = m_vCodebookRank0;
//    m_vCodebooks[7] = m_vCodebookRank0;
}
void NR_CodeBook::BuildCodeBooks_for_32Tx() {
//    m_iMaxNumberOfLayers = 32;
//    vector<cmat> m_vCodebookRank0;
//    //8TR
//    cmat W_Precoding;
//    W_Precoding.set_size(32,32);
//    complex<double>Mj(0.0,1.0);
//    for(int m = 0; m < 32; m++){
//        for(int n = 0; n < 32; n++){
//            W_Precoding(m,n) = 1/sqrt(32)*exp(Mj*2*M_PI/32*m*n);
//        }
//    }
//    /*m_vCodebookRank7.push_back(W_Precoding);
//        
//    m_vCodebooks[7] = m_vCodebookRank7;*/
//    m_vCodebookRank0.push_back(W_Precoding.get_col(0));
//    m_vCodebooks[0] = m_vCodebookRank0;
    /*m_vCodebooks[1] = m_vCodebookRank0;
    m_vCodebooks[2] = m_vCodebookRank0;
    m_vCodebooks[3] = m_vCodebookRank0;
    m_vCodebooks[4] = m_vCodebookRank0;
    m_vCodebooks[5] = m_vCodebookRank0;
    m_vCodebooks[6] = m_vCodebookRank0;
    m_vCodebooks[7] = m_vCodebookRank0;*/
    vector<cmat> m_vCodebookRank0;
    /// LTE4天线发送双流码字
    vector<cmat> m_vCodebookRank1;
    /// LTE4天线发送三流码字
    vector<cmat> m_vCodebookRank2;
    /// LTE4天线发送四流码字
    vector<cmat> m_vCodebookRank3;
    
    int Size0 = 1024; 
    int Size1 = 2048;
    int Size2 = 1024;
    int Size3 = 1024;
    
    
    int iSpace0 = 8;
    int iSpace1 = 16;
    int iSpace2 = 8;
    int iSpace3 = 8;
    
    double real;
    double imag;
    const char* pathreal = "./inputfiles/TR3211024Real.txt";
    const char* pathImag = "./inputfiles/TR3211024Imag.txt";
    ifstream fcodebookreal(pathreal);
    ifstream fcodebookimag(pathImag);
    assert(fcodebookreal);
    assert(fcodebookimag);
    itpp::cmat codebook32_layer1(32,Size0);//单流的码本向量，1024个
    vector<double> vCodebookreal;
    vector<double> vCodebookimag;
    //vCodebookreal.resize(32*1024,0.0);
    //vCodebookimag.resize(32*1024,0.0);
    //copy(istream_iterator<double>(fcodebookreal), istream_iterator<double>(), std::back_inserter(vCodebookreal));
    //copy(istream_iterator<double>(fcodebookimag), istream_iterator<double>(), std::back_inserter(vCodebookimag));
    for (int i = 0; i < 32; ++i)
       for (int j = 0; j < Size0; ++j){
           fcodebookreal>>real;
           fcodebookimag>>imag;
           codebook32_layer1(i, j).real(real);
           codebook32_layer1(i, j).imag(imag);
            //codebook32_layer1(i, j).real() = vCodebookreal[i * 1024 + j];
            //codebook32_layer1(i, j).imag() = vCodebookimag[i * 1024 + j];
       }
    for(int i=0;i<Size0;i+=iSpace0){
     m_vCodebookRank0.emplace_back(codebook32_layer1.get_col(i));
    }
    fcodebookreal.close();
    fcodebookimag.close();
    
    
    const char* pathreal1 = "./inputfiles/TR3222048Real.txt";
    const char* pathImag1 = "./inputfiles/TR3222048Imag.txt";
    ifstream fcodebookreal1(pathreal1);
    ifstream fcodebookimag1(pathImag1);
    assert(fcodebookreal1);
    assert(fcodebookimag1);
    std::vector<itpp::cmat> codebook32_layer2(Size1,itpp::zeros_c(32,2));//二流的码本向量，2048个
    vCodebookreal.clear();
    vCodebookimag.clear();
    //copy(istream_iterator<double>(fcodebookreal1), istream_iterator<double>(), std::back_inserter(vCodebookreal));
    //copy(istream_iterator<double>(fcodebookimag1), istream_iterator<double>(), std::back_inserter(vCodebookimag));
    //j表示2048个码本，i表示32TR，k表示双流
        for (int i = 0; i < 32; ++i)
        for (int j = 0; j < Size1; ++j)
        for (int k = 0; k < 2; ++k){
            fcodebookreal1>>real;
            fcodebookimag1>>imag;
            codebook32_layer2[j](i, k).real(real);
            codebook32_layer2[j](i, k).imag(imag);
        }
    for(int i=0;i<Size1;i+=iSpace1){
     m_vCodebookRank1.push_back(codebook32_layer2[i]);
    }
    fcodebookreal1.close();
    fcodebookimag1.close();
    
    
    const char* pathreal2 = "./inputfiles/TR32_layer_3_real.txt";
    const char* pathImag2 = "./inputfiles/TR32_layer_3_imag.txt";
    ifstream fcodebookreal2(pathreal2);
    ifstream fcodebookimag2(pathImag2);
    assert(fcodebookreal2);
    assert(fcodebookimag2);
    std::vector<itpp::cmat> codebook32_layer3(Size2,itpp::zeros_c(32,3));//3流的码本向量，1024个
    vCodebookreal.clear();
    vCodebookimag.clear();
    //copy(istream_iterator<double>(fcodebookreal1), istream_iterator<double>(), std::back_inserter(vCodebookreal));
    //copy(istream_iterator<double>(fcodebookimag1), istream_iterator<double>(), std::back_inserter(vCodebookimag));
    //j表示2048个码本，i表示32TR，k表示双流
        for (int i = 0; i < 32; ++i)
        for (int j = 0; j < Size2; ++j)
        for (int k = 0; k < 3; ++k){
            fcodebookreal2>>real;
            fcodebookimag2>>imag;
            codebook32_layer3[j](i, k).real(real);
            codebook32_layer3[j](i, k).imag(imag);
        }
    for(int i=0;i<Size2;i+=iSpace2){
     m_vCodebookRank2.push_back(codebook32_layer3[i]);
    }
    fcodebookreal2.close();
    fcodebookimag2.close();
    
    const char* pathreal3 = "./inputfiles/Layer4Real.txt";
    const char* pathImag3 = "./inputfiles/Layer4Imag.txt";
    ifstream fcodebookreal3(pathreal3);
    ifstream fcodebookimag3(pathImag3);
    assert(fcodebookreal3);
    assert(fcodebookimag3);
    std::vector<itpp::cmat> codebook32_layer4(Size3,itpp::zeros_c(32,4));//四流的码本向量，1024个
    vCodebookreal.clear();
    vCodebookimag.clear();
    copy(istream_iterator<double>(fcodebookreal3), istream_iterator<double>(), std::back_inserter(vCodebookreal));
    copy(istream_iterator<double>(fcodebookimag3), istream_iterator<double>(), std::back_inserter(vCodebookimag));
    //j表示2048个码本，i表示32TR，k表示双流
        for (int i = 0; i < 32; ++i)
        for (int j = 0; j < Size3; ++j)
        for (int k = 0; k < 4; ++k){
            fcodebookreal3>>real;
            fcodebookimag3>>imag;
            codebook32_layer4[j](i, k).real(real);
            codebook32_layer4[j](i, k).imag(imag);
        }
    
    for(int i=0;i<Size3;i+=iSpace3){
     m_vCodebookRank3.push_back(codebook32_layer4[i]);
    }
    
    fcodebookreal3.close();
    fcodebookimag3.close();
    //以上为码本
    
    m_vCodebooks[0] = m_vCodebookRank0;
    m_vCodebooks[1] = m_vCodebookRank1;
    m_vCodebooks[2] = m_vCodebookRank2;
    m_vCodebooks[3] = m_vCodebookRank3;
}
/*
int  Index2PMI_for_layer1(int i_1_1, int i_1_2, int i_2, int iN1, int iN2, int iO1, int iO2) {
    int PMI = i_1_1 + i_1_2 * iN1 * iO1 + i_2 * iN1 * iO1 * iN2 * iO2;
    return PMI;
}

void  PMI2Index_for_layer1(int PMI, int& i_1_1, int& i_1_2, int& i_2, int iN1, int iN2, int iO1, int iO2) {
    i_2 = PMI / (iN1 * iO1 * iN2 * iO2);
    
    i_1_2 = ( PMI / (iN1 * iO1) ) % (iN2 * iO2);
    
    i_1_1 = PMI % (iN1 * iO1);
    

    int PMI = i_1_1 + i_1_2 * iN1 * iO1 + i_2 * iN1 * iO1 * iN2 * iO2;

    return PMI;
}
 */
