/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LinkLevelNR.h
 * Author: lijie
 *
 * Created on 2020年6月17日, 下午5:05
 */
#pragma once
#include "LinkLevelInterface.h"
#include "RBIR2BLERNR.h"

///NR的链路级接口类LinkLevelNR

class LinkLevelNR : public LinkLevelInterface {
private:
    ///下行RBIR到BLER的数据表
    RBIR2BLERNR m_RBIR2BLERDL;
    vector<double>MCS2SINRTable;
    vector<vector<itpp::imat>>Layer_RE_MCS_RBNum2TBSize;
    char* filename;
    vector<int> RENumTable;
    int MaxLayNum;
    int RELevelNum;
    int MaxMCSLevel;
    int MaxRBNum;

    vector<int> vMaxCBSizeBit;     //链路级曲线的最大CBSize
public:
    ///根据输入的SINR数组和MCS等级，查询链路级数据得到BLER
    ///@param _vSINR 记录每个子载波上的SINR
    ///@param _iMCSIndex 记录传输数据的MCS等级
    double BLER(const vector<double>& _vSINR, int _iMCSIndex, double _dTBSizeKbit) override;
    
    double BLERmat(const vector<mat>& _vSINR, int _iMCSIndex, double _dTBSizeKbit) override;
    ///返回各种MCS对应的编码块大小
//    int GetRENum_SingleSB(int _iusedRBnum);

    int GetRENum(int _iRBnum_SingleSB, int _iLayerNum) override;
    int GetRENumInRange(int RBbegin, int RBend) override;


    double MCS2TBSKBit(int _iMCSIndex, int _iRENum_AllSB) override;

    ///计算SINR向量能够支持的MCS等级
    int SINR2MCS(const vector<double>& _vSINR);

    imat SINR2MCS(const vector<mat>& _vSINR) override;
    ///计算正确解调MCS所需要的SINR值（假设每个子载波上的SINR相同）
    double MCS2SINR(int _iMCSIndex, int _iRBNum, double _dTBSizeKbit) override;
    
    double SINRCombineUL2(const vector< mat >& _vSINR) override;

    mat MCS2SINR(imat _iMCSIndex, int _iRBNum, double _dTBSizeKbit) override;
    ///MCS到调制阶数的映射
    static int MCS2ModLevel(int _iMCSIndex, int _iDLorUL = 1);
    ///MCS到Target Code Rate的映射
    static double MCS2Rate(int _iMCSIndex);
    ///计算后处理等效SINR，DB
    double ComputePostSINRDB(const vector<double>& _vSINR, int _iMCS) override;

    double GetTBSizeKBit_woDMRS(imat _mMCS, int _iRBNum) override;

    double GetTBSizeKBit_woDMRS(int _iMCS, int _iRBNum) override;
    double SINRCombineUL(const vector<double>& _vSINR) override;
    
    double MCS2TBSKBit2(int _iMCSIndex, int _iRENum_AllSB,int v) override;
    int SINR2MCS2(const vector<double>& _vSINR,int v) override;
    double GetTBSizeKBit_woDMRS2(imat _mMCS, int _iRBNum,int v) override;

    double GetTBSizeKBit_woDMRS2(int _iMCS, int _iRBNum,int v) override;
    double MCS2TBS(int _iMCSIndex, int _iRENum_AllSB,int v);

    int SINR2MCS_simplified(const vector<double>& vSINR);
    int SINR2MCS_simplified(double dSINR);
    double MCS2SINR_simplified(const int& _MCS);

    int GetRBNum_ByTable(const int layer, const int RENumPerRB, const int MCS, const double TBSize);

    int MCS2CQI(int _iMCSIndex);
private:
    ///根据MCS等级和RB数计算SINR门限
    double CalculateMCS2SINR(int _iMCSIndex, int _iRBNum, double _dTBSizeKbit);
public:
    static string ModulationOrder(int _iMCSIndex, int _iDLorUL = 1);

public:
    ///LinkLevelNR的构造函数
    LinkLevelNR() = default;
    explicit LinkLevelNR(int tableindex);
    ///LinkLevelNR的析构函数
    ~LinkLevelNR() override = default;
};
