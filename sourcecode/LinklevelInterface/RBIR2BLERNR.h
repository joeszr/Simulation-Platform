/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RBIR2BLERNR.h
 * Author: cyh
 *
 * Created on 2020年6月17日, 下午11:13
 */

///@brief 定义NR链路级RBIR—FER表
#pragma once
#include "FERTable.h"

///自定义NR的键值类型<MCS,TBS>
typedef pair<int, int> keyNR;
//typedef int MCS;

///RBIR2BLERNR类，初始化NR的RBIR-BLER的链路级表

class RBIR2BLERNR final : public FERTABLE<keyNR> {
private:
    double Interpolation_RBIR2BLER(vector<double> & _vRBIR, vector<double> & _vBLER, const double& _dRBIR_dB);

    void ReadTable(const char * _pFileName);

    //    keyNR ReadKEY(char _cBuffer[500]);
    int ReadMCS(char _cBuffer[500]);
    int ReadTBS(char _cBuffer[500]);
    //    int MCS;
    //    int TBS;
    //    map<int, CURVE> m_FERTableTemp;
    unordered_map<int, map<int, CURVE> > m_FERTableNR;


public:
    //    double RBIR2BLERNR::FER(const keyNR & _rKey, const double _dRBIR_dB);
    double FER(const int& _MCS, const int& _iTBS, const double& _dRBIR_dB);
    ///RBIR2BLERNR构造函数
    RBIR2BLERNR();
    //ctytry
    explicit RBIR2BLERNR(int tableindex);
    ///RBIR2BLERNR析构函数
    ~RBIR2BLERNR() override = default;
};
