//
// Created by 31048 on 2023/1/11.
//
///@file CalcRENumperRB.h
///@brief  计算一个PRB中可用的RE数
///@author ljq

#pragma once

class CalcRENum{

private:
    /*
    N_RE = N_SCperRB * N_SymbolperSH - N_DMRSperRB - N_OHperPRB
    N_SCperRB is the number of subcarriers in a physical resource block
    N_SymbolperSH is the number of symbols of the PDSCH allocation within the slot
    N_DMRSperRB is the number of REs for DM-RS per PRB in the scheduled duration including the overhead of the DM-RS CDM groups without data
    N_OHperPRB is the overhead configured by higher layer parameter xOverhead in PDSCH-ServingCellConfig.
    If the xOverhead in PDSCH-ServingCellconfig is not configured (a value from 6, 12, or 18), the   is set to 0.
    */
    int N_SCperRB;          // 每个PRB中的子载波数目，12
    int N_SymbolperSH;      // PDSCH占用的符号数目
    int N_DMRSperRB;        // 每个PRB中DMRE占据的RE数
    int N_OHperPRB;         // 每个PRB中信道开销，可配置为 0、6、12、18

    int N_RE = 0;

private:

    //DMRS 采用 Type 1/2
    int N_DMRSType;
    //DMRS采用双符号还是单符号
    int N_DMRSSymbol;

    static const int DMRS_SingleSymbol = 1;
    static const int DMRS_DoubleSymbol = 2;

public:
    ///@brief 构造函数
    CalcRENum();

    void GetRRCConfig(int SymbolperSH, int DMRSType, int DMRSSymbol, int OHperPRB);

    void GetDMRSperRB();

    int CalculateRENum(bool IsConfig);

    static const CalcRENum& Instance();

    ~CalcRENum();
};
