/*
* Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/*
 * File:   RIS.h
 * Author: oumali
 *
 * Created on 2023年7月11日, 下午4:57
 */

#pragma once
//#include "../ChannelModel/Point.h"
#include "../ChannelModel/Antenna.h"
#include "../ChannelModel/AntennaOrientGain.h"
#include "RISID.h"
#include "../BaseStation/BTS.h"
#include "../MobileStation/MS.h"
#include "../ChannelModel/RISTx.h"
#include "../ChannelModel/RISRx.h"

class RIS : public cm::Point {
public:
    RISID m_ID;
    //    std::shared_ptr<cm::TxNode> ptx;
    //    std::shared_ptr<cm::RxNode> prx;
    static int riscounter;
    //入射角 存储的是BTS到本小区RIS的入射角
    double dEOARAD_BTSRIS_GCS;
    double dAOARAD_BTSRIS_GCS;
private:
    /// RIS服务的用户激活集
    vector<MSID> m_ActiveSet;

public:
    /// @brief 取发射器的ID序号
    RISID GetRISID();
    cm::TxNode& GetTx();
    cm::RxNode& GetRx();
    /// @brief 返回发射器的总对象个数
    static int CountRIS();
    void SetOrientRAD(double _angle);
    void SetTxRxHeightM(double _dHeightM);
    void SetTxRxAntNum(int _iAntNum);
    void SetTxRxAntGainDB(double _AntGainDB);
    void SetTxRxAOG();
    double PhaseAdjust();
    std::shared_ptr<cm::Antenna> m_pSelfAntenna;
    std::shared_ptr<cm::RISTxNode> m_pTxNode;
    std::shared_ptr<cm::RISRxNode> m_pRxNode;
    /// @brief 反馈ActiveSet集合
    vector<MSID> GetActiveSet();
    void ConnectMS(MSID _imsid);

protected:


public:
    std::shared_ptr<cm::Antenna> GetAntennaPointer() {
        return m_pSelfAntenna;
    }

protected:
    void Build_RIS_Antenna();
    void SetTxRx();

public:
    //    RIS();
    /// @brief 构造函数
    RIS(BTSID _btsid, int _iID);
    /// @brief 析构函数
    ~RIS(void);
};

