///@file LinkLevelInterface.h
///@brief 链路级接口函数
///@author dushaofeng

#pragma once
#include "../Utility/Include.h"
#include <unordered_map>
#include "../Utility/Random.h"
///链路级接口类LinkLevelInterface

class LinkLevelInterface {
protected:
    Random random;
    ///<moulation,SINRSIQPSK/SINR16QAM/SINR64QAM>
    static unordered_map<int, vector<double> > m_SINR2SI;
    ///存储SINR值
    static vector<double> m_vMIESMSINR;

    static vector<double> m_vMIESMSINR_256QAM;
public:
    //上行添加：
    ///根据RBIR查询链路级数据得到BLER
    //    virtual double BLER_UL(const vector<double>& _vSINR, int _iMCSIndex, int _iEESMorMIESM);
    ///测试用，用于查看用户的SINR值,线性值
    //    virtual double SINRCombineUL(const vector<double>& _vSINR);

    ///返回各种MCS对应的平均到每个PRB（12*7）的编码块大小
    //    virtual double MCS2TBSizeKbit_UL(int _iMCSIndex, int _iRBNum);
    //    double MCS2TBSizeKbit_UL(itpp::imat _mMCS, int _iRBNum);

    //    static double RBIR2SINRdB_UL(double _dRBIRtarget, int _iModulation);

    ///返回各种MCS对应的平均到每个PRB（12*7）的编码块大小
    //    virtual double MCS2TBSKBit(int _iMCS, int _iRBNum, int _iFlag);

    //    virtual double MCS2TBSKBit_LTE(int _iMCS, int _iRBNum, int _iFlag = 0);

    //    double MCS2TBSKBit_LTE(itpp::imat _mMCS, int _iRBNum, int _iFlag = 0);

    double MCS2TBSKBit(itpp::imat _mMCS, int _iRENum_AllSB);
    virtual double SINRCombineUL(const vector<double>& _vSINR);
    virtual double MCS2TBSKBit(int _iMCS, int _iRENum_AllSB);
    ///根据预测出的SINR返回最优的MCS
    
    virtual int GetRENum(int _iRBnum_SingleSB, int _iLayerNum);
    virtual int GetRENumInRange(int RBbegin, int RBend);
    
    virtual imat SINR2MCS(const vector<mat>& _vSINR) = 0;
    ///根据预测的MCS，映射到预测的信噪比
    virtual double MCS2SINR(int _iMCS, int _iRBNum);

    virtual mat MCS2SINR(const imat& _iMCS, const int& _iRBNum);
    ///给出从信噪比到互信息的映射
    static double SINRDB2SI(double _dSINRdB, int _iModLevel);
    ///给出互信息到等效信噪比的映射
    static double SI2SINRDB(double _dSI, int _iModLevel);
    ///从SI到RBIR的映射
    static double SI2RBIR(double _dSI, int _iModLevel);
    ///
    virtual double ComputePostSINRDB(const vector<double>& _vSINR, int _iMCS) = 0;

    //NR
    virtual double BLER(const vector<double>& _vSINR, int _iMCSIndex, double _dTBSizeKbit) = 0;
    
    virtual double BLERmat(const vector<mat>& _vSINR, int _iMCSIndex, double _dTBSizeKbit) = 0;
    
    virtual double SINRCombineUL2(const vector< mat >& _vSINR);

    virtual double MCS2SINR(int _iMCSIndex, int _iRBNum, double _dTBSizeKbit) = 0;

    virtual int MCS2CQI(int _iMCSIndex) = 0;

    virtual mat MCS2SINR(imat _iMCSIndex, int _iRBNum, double _dTBSizeKbit) = 0;

    virtual double GetTBSizeKBit_woDMRS(imat _mMCS, int _iRBNum) = 0;

    virtual double GetTBSizeKBit_woDMRS(int _iMCS, int _iRBNum) = 0;
    
    virtual double MCS2TBSKBit2(int _iMCS, int _iRENum_AllSB,int v)=0;
    virtual int SINR2MCS2(const vector<double>& _vSINR,int v) = 0;
    virtual double GetTBSizeKBit_woDMRS2(imat _mMCS, int _iRBNum, int v)=0;

    virtual double GetTBSizeKBit_woDMRS2(int _iMCS, int _iRBNum, int v)=0;
   // virtual double GetTBSizeKBit_woDMRS(int _iMCS, int _iRBNum, MSID msid);

    virtual int SINR2MCS_simplified(const vector<double>& VSINR) = 0;
    virtual int SINR2MCS_simplified(double dSINR) = 0;
    virtual double MCS2SINR_simplified(const int& _MCS) = 0;

    virtual int GetRBNum_ByTable(const int layer, const int RENumPerRB, const int MCS, const double TBSize) = 0;

protected:
    ///对子载波对应的SINR进行MIESM合并
    double MIESM(const vector<double>& _vSINR, int _iModulation);
    double MIESM(const vector<mat>& _vSINR, int _iModulation);
    ///对子载波对应的SINR进行EESM合并
    double EESM(const vector<double>& _vSINR, double _b);


protected:
    static LinkLevelInterface* m_pLinkLevelInterface_DL;
    static LinkLevelInterface* m_pLinkLevelInterface_UL;

public:
    ///返回LinkLevelInterface对象的引用
    static LinkLevelInterface& Instance_DL();

    static LinkLevelInterface& Instance_UL();

protected:
    ///LinkLevelInterface的构造函数
    LinkLevelInterface();
    ///LinkLevelInterface的析构函数
    virtual ~LinkLevelInterface() = default;
};
