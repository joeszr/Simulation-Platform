/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "RBIR2BLERNR.h"
#include "LinkLevelInterface.h"
#include "LinkLevelNR.h"

///@brief RBIR2BLERNR构造函数实现
///
///初始化RBIR-BLER表，从读入的SINR-BLER表经过计算转化为RBIR-BLER表

RBIR2BLERNR::RBIR2BLERNR() {
    switch(Parameters::Instance().BASIC.ISINR2BLERTable){
        case 0:
            if(Parameters::Instance().DaHuaWu.bDl256QamSwitch == true)
            {
                m_pFileName = const_cast<char*> ("./inputfiles/NR_SINR2BLER_DL.txt");
            }
            else
            {
                m_pFileName = const_cast<char*> ("./inputfiles/LTE_SINR2BLER_DL.txt");
            }
            break;
        case 1:
            m_pFileName = const_cast<char*>("./inputfiles/NR_SINR2BLER_UL_Table1.txt");
            break;
        case 2:
            m_pFileName = const_cast<char*>("./inputfiles/NR_SINR2BLER_UL_Table2.txt");
            break;
    }
    
    ReadTable(m_pFileName);

    for (auto& it_MCS : m_FERTableNR) {
        for (auto it_TBS = it_MCS.second.begin(); it_TBS != it_MCS.second.end(); ++it_TBS) {
            int iModulation = LinkLevelNR::MCS2ModLevel(it_MCS.first, Parameters::Instance().BASIC.IDLORUL);
            ///SINR序列的第一个
            auto start = it_TBS->second.first.begin();
            ///
            auto end = it_TBS->second.first.end();

            while (start != end) {
                double dSI = LinkLevelInterface::SINRDB2SI(*start, iModulation);
                *start = LinkLevelInterface::SI2RBIR(dSI, iModulation);
                start++;
            }
        }
    }
}
RBIR2BLERNR::RBIR2BLERNR(int tableindex){
    switch(tableindex){
        case 0:
            if(Parameters::Instance().DaHuaWu.bDl256QamSwitch == true)
            {
                m_pFileName = const_cast<char*> ("./inputfiles/NR_SINR2BLER_DL.txt");
            }
            else
            {
                m_pFileName = const_cast<char*> ("./inputfiles/LTE_SINR2BLER_DL.txt");
            }
            break;
        case 1:
            m_pFileName = const_cast<char*>("./inputfiles/NR_SINR2BLER_UL_Table1.txt");
            break;
        case 2:
            m_pFileName = const_cast<char*>("./inputfiles/NR_SINR2BLER_UL_Table2.txt");
            break;
        default:
            assert(false);
    }
    
    ReadTable(m_pFileName);

    for (auto& it_MCS : m_FERTableNR) {
        for (auto it_TBS = it_MCS.second.begin(); it_TBS != it_MCS.second.end(); ++it_TBS) {
            int iModulation = LinkLevelNR::MCS2ModLevel(it_MCS.first, Parameters::Instance().BASIC.IDLORUL);
            ///SINR序列的第一个
            auto start = it_TBS->second.first.begin();
            ///
            auto end = it_TBS->second.first.end();

            while (start != end) {
                double dSI = LinkLevelInterface::SINRDB2SI(*start, iModulation);
                *start = LinkLevelInterface::SI2RBIR(dSI, iModulation);
                start++;
            }
        }
    }
}

void RBIR2BLERNR::ReadTable(const char * _pFileName) {
    assert(_pFileName);
    ifstream BLER_FILE(_pFileName);
    assert(BLER_FILE);
    char buffer[2000];

    string temp;
    int iMCS, iTBS;
    cout<<_pFileName<<endl;
    while (!BLER_FILE.eof()) {
        BLER_FILE >> temp;
        ///找到键key值标志“KEY”字符串，并且读取键值
        if (temp == "KEY") {
            ///执行完本行后buffer是包含键值的字符串
            BLER_FILE.getline(buffer, 2000);
            ///（虚函数）从字符串读取键值。（模板方法设计模式）
            //            key = ReadKEY(buffer);
            iMCS = ReadMCS(buffer);
            iTBS = ReadTBS(buffer);
        } else continue;
        /////在FER表中插入该键值代表的链路BLER曲线（现在曲线还是空的）
        //        m_FERTableTemp.insert(make_pair(iMCS, CURVE()));
        //        m_FERTableNR.insert(make_pair(iMCS, m_FERTableTemp));
        m_FERTableNR[iMCS].insert(make_pair(iTBS, CURVE()));
        ///将下一行作为整个字符串读入
        BLER_FILE.getline(buffer, 2000);
        ///用字符串构造一个字符串输入流
        istringstream isstr1(buffer);
        istream_iterator<double> start, end;
        while (temp != "SINR") isstr1 >> temp;
        ///找到数据起始的位置
        while (temp != "(") isstr1 >> temp;
        start = istream_iterator<double>(isstr1);
        ///使用STL的copy算法读入SINR序列
        copy(start, end, back_inserter(m_FERTableNR[iMCS][iTBS].first));

        ///将下一行作为整个字符串读入
        BLER_FILE.getline(buffer, 2000);
        ///用字符串构造一个字符串输入流
        istringstream isstr2(buffer);
        while (temp != "BLER") isstr2 >> temp;
        ///找到数据起始的位置
        while (temp != "(") isstr2 >> temp;
        ///流对像去初始化一个流迭代器
        start = istream_iterator<double>(isstr2);
        ///使用STL的copy算法读入FER序列
        //        copy(start, end, back_inserter(m_FERTableNR[key].second));
        copy(start, end, back_inserter(m_FERTableNR[iMCS][iTBS].second));
        int sinrnum=m_FERTableNR[iMCS][iTBS].first.size(),blernum=m_FERTableNR[iMCS][iTBS].second.size();
        if(sinrnum!=blernum){
            if(sinrnum==1){
                cout<<" "<<endl;
            }
            cout<<iMCS<<" "<<iTBS<<" "<<sinrnum<<" "<<blernum<<endl;
        }
    }
}

//keyNR RBIR2BLERNR::ReadKEY(char _cBuffer[500]) {
//    keyNR result;
//    istringstream isstr(_cBuffer);
//    string temp;
//    ///找到并读取MCS值
//    while (temp != "MCS") isstr >> temp;
//    isstr >> temp >> result.first;
//    ///找到并读取TBS值
//    while (temp != "TBS") isstr >> temp;
//    isstr >> temp >> result.second;
//
//    return result;
//}

int RBIR2BLERNR::ReadMCS(char _cBuffer[500]) {
    int result;
    istringstream isstr(_cBuffer);
    string temp;
    ///找到并读取MCS值
    while (temp != "MCS") isstr >> temp;
    isstr >> temp >> result;
    ///找到并读取TBS值
    return result;
}

int RBIR2BLERNR::ReadTBS(char _cBuffer[500]) {
    int result;
    istringstream isstr(_cBuffer);
    string temp;
    ///找到并读取MCS值
    while (temp != "TBS") isstr >> temp;
    isstr >> temp >> result;
    ///找到并读取TBS值
    return result;
}

//double RBIR2BLERNR::FER(const keyNR & _rKey, const double _dRBIR_dB) {
//    //Two Dimension Interpolation
////    typedef map<MCS,map<keyNR, CURVE> >::iterator FERTable_POS;
//    for (map<MCS, map < keyNR, CURVE> >::iterator it_MCS = m_FERTableNR.begin(); it_MCS != m_FERTableNR.end(); ++it_MCS) {
////        typedef map<keyNR, CURVE>::iterator FERTable_POS=it->second.begin();
//        for (map <keyNR, CURVE>::iterator it_TBS = it_MCS->second.begin(); it_TBS != it_MCS->second.end(); ++it_TBS) {
//        FERTable_POS pTable_right = m_FERTableNR.lower_bound(_rKey);
//
//        if (pTable_right == m_FERTableNR.end()) {
//            FERTable_POS pTable = m_FERTableNR.end();
//            pTable--;
//            ///取RBIR序列(已排序，递增)
//            vector<double> & vRBIR = pTable->second.first;
//            ///取BLER序列
//            vector<double> & vBLER = pTable->second.second;
//
//            return Interpolation_RBIR2BLER(vRBIR, vBLER, _dRBIR_dB);
//        }
//
//        if (pTable_right == m_FERTableNR.begin()) {
//            FERTable_POS pTable = m_FERTableNR.begin();
//            ///取RBIR序列(已排序，递增)
//            vector<double> & vRBIR = pTable->second;
//            ///取BLER序列
//            vector<double> & vBLER = pTable->second.second;
//
//            return Interpolation_RBIR2BLER(vRBIR, vBLER, _dRBIR_dB);
//        }
//
//
//        FERTable_POS pTable_left = pTable_right;
//        pTable_left--;
//
//        double dBLER_left = Interpolation_RBIR2BLER(pTable_left->second.first, pTable_left->second.second, _dRBIR_dB);
//        double dBLER_right = Interpolation_RBIR2BLER(pTable_right->second.first, pTable_right->second.second, _dRBIR_dB);
//
//        double k = (dBLER_right - dBLER_left) / (pTable_right->first.second - pTable_left->first.second);
//        double dBLER = dBLER_left + k * (_rKey.second - pTable_left->first.second);
//
//        return dBLER;
//        }
//    }
//}

double RBIR2BLERNR::FER(const int& _MCS, const int& _iTBS, const double& _dRBIR_dB) {
    //    for (map < int, CURVE>::iterator it_MCS = m_FERTableNR[_MCS].begin(); it_MCS != m_FERTableNR[_MCS].end(); ++it_MCS) {
    auto pTable_right = m_FERTableNR[_MCS].lower_bound(_iTBS);
    if (pTable_right == m_FERTableNR[_MCS].end()) {
        auto pTable = m_FERTableNR[_MCS].end();
        pTable--;
        ///取RBIR序列(已排序，递增)
        vector<double> & vRBIR = pTable->second.first;
        ///取BLER序列
        vector<double> & vBLER = pTable->second.second;

        return Interpolation_RBIR2BLER(vRBIR, vBLER, _dRBIR_dB);
    }

    if (pTable_right == m_FERTableNR[_MCS].begin()) {
        auto pTable = m_FERTableNR[_MCS].begin();
        ///取RBIR序列(已排序，递增)
        vector<double> & vRBIR = pTable->second.first;
        ///取BLER序列
        vector<double> & vBLER = pTable->second.second;

        return Interpolation_RBIR2BLER(vRBIR, vBLER, _dRBIR_dB);
    }


    auto pTable_left = pTable_right;
    pTable_left--;

    double dBLER_left = Interpolation_RBIR2BLER(pTable_left->second.first, pTable_left->second.second, _dRBIR_dB);
    double dBLER_right = Interpolation_RBIR2BLER(pTable_right->second.first, pTable_right->second.second, _dRBIR_dB);

    double k = (dBLER_right - dBLER_left) / (pTable_right->first - pTable_left->first);
    double dBLER = dBLER_left + k * (_iTBS - pTable_left->first);

    return dBLER;
}
//    }
//}

double RBIR2BLERNR::Interpolation_RBIR2BLER(vector<double> & _vRBIR, vector<double> & _vBLER,const double& _dRBIR_dB) {
    typedef vector<double>::iterator POS;
    ///left代表输入RBIR的左侧的位置，right代表右侧的位置
    POS left, right;
    right = find_if(_vRBIR.begin(), _vRBIR.end(), bind2nd(greater<double>(), _dRBIR_dB));

    ///如果要查的值不在序列范围内，则返回边缘值
    if (right == _vRBIR.begin())
        return _vBLER[0];
    if (right == _vRBIR.end())
        return 0;
    left = right - 1;
    int ileft = static_cast<int> (left - _vRBIR.begin()); //将pos类型转换成int表示
    int iright = ileft + 1;
    ///如果右侧点的BLER等于0，则返回0。
    if(iright>=_vBLER.size()){
        return 0;
    }
    if (_vBLER[iright] == 0)
        return 0;
    double dRBIR1 = _vRBIR[ileft];
    ///将BLER值取对数（BLER曲线在对数域是直线）
    double dBLER1_dB = log10(_vBLER[ileft]);
    double dRBIR2 = _vRBIR[iright];
    double dBLER2_dB = log10(_vBLER[iright]);
    ///计算直线的斜率
    double k = (dBLER2_dB - dBLER1_dB) / (dRBIR2 - dRBIR1);
    ///计算要查的RBIR对应的对数域BLER
    double result = dBLER1_dB + k * (_dRBIR_dB - dRBIR1);
    ///将该BLER转换到线性域
    result = pow(10.0, result);
    return result;
}
