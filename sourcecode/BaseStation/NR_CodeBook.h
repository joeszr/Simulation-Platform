///@file NR_CodeBook.h
///@brief  NR天线系统发送预编码类声明，继承于CodeBook类
///@author wangxiaozhou

#pragma once
#include "../Utility/Include.h"
#include "CodeBook.h"
#include "../Utility/Singleton.h"
#include "../SafeUnordered_map.h"

using namespace itpp;
using namespace std;




/// @brief NR_CodeBook

class NR_CodeBook final: public CodeBook {
protected:
    int m_iTxPortNum;

    // m_vCodebooks(_iRank)[_iPMI]
    std::unordered_map<int, std::vector<cmat> > m_vCodebooks;

    int m_iMaxNumberOfLayers; // value range: {1, 2, 3, 4, ...}

public:
    /// @see CodeBook::GetCodeWord

    cmat GetCodeWord(int _iPMI, int _iRank) override{
        std::vector<cmat> temp = m_vCodebooks[_iRank];

        //        for(int i = 0; i <= 3; i++) {
        //            complex<double> P0I = temp[i](1,0);
        //            complex<double> P0R = temp[i](0,0);
        //            
        //            int j = i;
        //        }

        return temp[_iPMI];
    }

    /// @see CodeBook::GetMaxRank

    int GetMaxRank() override{
        return m_iMaxNumberOfLayers;
    }

    /// @see CodeBook::GetCodeNum

    int GetCodeNum(int _iRank) override{
        return static_cast<int> (m_vCodebooks[_iRank].size());
    }

protected:
    void BuildCodeBooks_for_1Tx();
    void BuildCodeBooks_for_2Tx();
    void BuildCodeBooks_for_4Tx();
    void BuildCodeBooks_for_8Tx();
    void BuildCodeBooks_for_32Tx();




    //public:
    //    /// LTE2天线发送单流码字
    //    vector<cmat> m_vCodebookRank0;
    //    /// LTE2天线发送双流码字
    //    vector<cmat> m_vCodebookRank1;

    //public:
    //    /// @see CodeBook::GetCodeWord
    //    cmat GetCodeWord(int _iPMI, int _iRank);
    //    /// @see CodeBook::GetTransformedCodeWord
    //    cmat GetCodeWord(cmat _mCovMat, int _iPMI, int _iRank);


public:
    /// @brief 构造函数
    explicit NR_CodeBook(int _iTxPortNum);
    /// @brief 析构函数
    ~NR_CodeBook() override= default;


};